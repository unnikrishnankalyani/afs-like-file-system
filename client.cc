#include <string>
#include <time.h>
#include <sys/time.h>
#include <grpcpp/grpcpp.h>
#include "timetest.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using timetest::StrTest;
using timetest::StrRequest;
using timetest::StrReply;

const int REPEAT_EXP = 1;
const int TEST_RUNS = 5000;
const int MAX_MSG = 4000000;
const int IGNORE_FIRST_X_MSGS = 50;
const int one_ns = 1e9;
const int one_us = 1e6;

void get_time(struct timespec* ts)
{
    clock_gettime(CLOCK_MONOTONIC, ts);
}

double get_time_diff(struct timespec* before, struct timespec* after)
{
    double delta_s = after->tv_sec - before->tv_sec;
    double delta_ns = after->tv_nsec - before->tv_nsec;

    return delta_s + (delta_ns * 1e-9);
}

class StrTestClient {
    public:
        StrTestClient(std::shared_ptr<Channel> channel) : stub_(StrTest::NewStub(channel)) {}

    int sendRequest(grpc::string a) {
        StrRequest request;

        struct timespec t1,t2;
        double tdiff;

        request.set_a(a);

        StrReply reply;

        ClientContext context;

        Status status = stub_->sendRequest(&context, request, &reply);

        if(status.ok()){
            return reply.ack();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return -1;
        }
    }

    private:
        std::unique_ptr<StrTest::Stub> stub_;
};

void RunStrClient(std::string ipadd) {
    std::string address(ipadd+":5000");
    StrTestClient client(
        grpc::CreateChannel(
            address,
            grpc::InsecureChannelCredentials()
        )
    );

    int response;


    // Timer & tracker Variables
    struct timespec start;
    struct timespec end;
    double best_send = 0, rtt_time = 0, sum_rtt = 0, first_time, second_time;
    int msg_size;
    std::string msg;

    // Repeat exp REPEAT times
    for (int j = 0; j < REPEAT_EXP; j++){                 
        msg_size = 1;
        // Run different message sizes           
        //for (int x = 0; x < MAX_MSG; x++){
        while(msg_size < MAX_MSG){
        msg = "";

        // Build string
        for (int y = 0; y < msg_size; y++){
            msg += 'a';
        }

        // Send a message
        sum_rtt = 0;
        for (int i = 0; i < TEST_RUNS; i++){

            get_time(&start);
            response = client.sendRequest(msg);
            get_time(&end);
            rtt_time = get_time_diff(&start, &end);
            if (i==0 || rtt_time < best_send) {best_send=rtt_time;}
            if (i==0) {first_time = rtt_time;}
            if (i==1) {second_time = rtt_time;}
            if (i>IGNORE_FIRST_X_MSGS) {sum_rtt += rtt_time;}
        }  
        double avg_rtt  = (sum_rtt/(TEST_RUNS - IGNORE_FIRST_X_MSGS))*one_us; // in us
        //std::cout << "Avg RTT = " << avg_rtt[y] << " sum_rtt[y] = " << sum_rtt[y] << std::endl;
        double throughput= (double) (msg_size / (best_send*one_us)); //in Mb/S

        //double throughput = 
        std::cout << "Message size: "<<msg_size << ", " << j << ", Test runs: " << TEST_RUNS << ", Best_us: " 
                    << best_send*one_us << ", First: " << first_time*one_us << ", Second: " << second_time*one_us
                    <<" Throughput: "<< throughput << ",\n";      
        
        //msg_size=msg_size<<1; 
        if (msg_size < 1000)
            msg_size += 100;
        else if (msg_size < 65000)
            msg_size += 1000;
        else
            msg_size += 100000;                    
        }           
    }  

    
    std::cout << "Success: " << response << std::endl;
}

int main(int argc, char* argv[]){

    std::string ipadd = "0.0.0.0";
    if (argc >1){
            ipadd = argv[1];
        }

    RunStrClient(ipadd);

    return 0;
}
