kill -9 $(ps -ef | grep "cmake/build/client" | grep -v grep | awk '{print $2}')

# create temprory cache and persistent storage for each client
# delete possible existing file
for i in {0..49}
do
	temp1=scale${i}/fuse
	temp2=~/storage${i}/10G
	fusermount -u $temp1
	rm $temp2
done

# run this script in ../cmake/build