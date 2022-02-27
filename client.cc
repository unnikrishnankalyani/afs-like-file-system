#include <string>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <iostream>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

#include <grpcpp/grpcpp.h>
#include "afs.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using afs::AFS;
using afs::CreateReq;
using afs::CreateRes;

static struct options {	
	AfsClient* afsclient;
	int show_help;
} options;

#define MAX_PATH_LENGTH 1000
char fs_path[MAX_PATH_LENGTH];

#define OPTION(t, p)                           \
    { t, offsetof(struct options, p), 1 }
static const struct fuse_opt option_spec[] = {
	OPTION("-h", show_help),
	OPTION("--help", show_help),
	FUSE_OPT_END
};

static int client_create(std::string path)
{
    return options.afsclient->afs_CREATE(path);
}

static int client_write(const char *path, const char *buffer, size_t size, off_t offset,
                      struct fuse_file_info *file_info)
{
    return options.afsclient->afs_WRITE(path, buffer, size, offset, file_info);
}


static int client_read(const char *path, char *buffer, size_t size, off_t offset,
		      struct fuse_file_info *file_info)
{
    return options.afsclient->afs_READ(path, buffer, size, offset, file_info);
}

struct client_fuse_operations:fuse_operations
{
    client_fuse_operations ()
    {
        create     = client_create;
        write      = client_write;
        read       = client_read;

        //uncomment the below as and when the corresponding implementation is done.
        
        // getattr    = client_getattr;
        // readdir    = client_readdir;
        // open       = client_open;
        
        // release    = client_release;
    }
} client_oper;


void RunAfsClient(std::string ipadd) {
    std::string address(ipadd+":5000");
    AfsClient client(
        grpc::CreateChannel(
            address,
            grpc::InsecureChannelCredentials()
        )
    );

    int response;

    std::string msg = "testmsg: hi!";
    response = client.afs_CREATE(msg);
            
    
    std::cout << "Success: " << response << std::endl;
}

// int main(int argc, char* argv[]){

//     std::string ipadd = "0.0.0.0";
//     if (argc >1){
//             ipadd = argv[1];
//         }

//     RunAfsClient(ipadd);

//     return 0;
// }

int main(int argc, char* argv[]){

	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	options.nfsclient = new AfsClient(grpc::CreateChannel(
  "0.0.0.0:50051", grpc::InsecureChannelCredentials()));


    // if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1)
	// return 1;

    // if (options.show_help) {
    //     show_help(argv[0]);
    //     assert(fuse_opt_add_arg(&args, "--help") == 0);
    //     args.argv[0] = (char*) "";
    // }


    // return fuse_main(argc, argv, &client_oper, &options);

    //cache path and actual path
    strncpy(fs_path, realpath(argv[argc-1], NULL), MAX_PATH_LENGTH);
    strncat(fs_path, "/", MAX_PATH_LENGTH);
    argv[argc-1] = NULL;
    argc--;
    printf("FS PATH: %s\n", fs_path);


    return fuse_main(argc, argv, &client_oper, NULL);
}
