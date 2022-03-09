# wi-AFS
## AFS-like Client/Server File System

### To test :
 &emsp;  Run (after setting $MY_INSTALL_DIR (to the directory where gRPC is installed) and $PATH appropriately):
     &emsp; &emsp;  export MY_INSTALL_DIR=$HOME/.local
     &emsp; &emsp;  export PATH=/opt/homebrew/bin
      &emsp; &emsp; PATH=/bin:/usr/bin:/usr/local/bin:${PATH}
      &emsp; &emsp; export PATH

      mkdir -p cmake/build
      cd cmake/build
      cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..
      make afs.grpc.pb.o
      make

  on client :
   &emsp;  ./client [fuse mount path] [cache path]
	on server :
	&emsp; ./server [server fs path]
   #### Test performance:
   &emsp; Go to the AFS_performance folder &emsp;
   *  read_performance: test read performance
   *  write_performance: test write performance
   *  protocol_perf-part1 & part2: test consistency performance, run protocol_perf-part1 in client 1, then run protocol_perf-part2 in client2, and run protocol_perf-part1 in client 1 again.
   * Scalable read/write performance: 50 AFS clients run in the same machine (with different temporary caches and persistent storage) and communicate with the AFS server (in another machine). All clients read/write the same file (And the file does not exist in persistent storage). Go to AFS_scale folder, and check the README to run scalability test.

## Code:
### client.cc
Defines fuse operations supported by the client.   The following POSIX APIs are supported:
   &emsp; open()
  &emsp;  creat()
  &emsp;  mkdir()
   &emsp; read()
  &emsp;  pread()
  &emsp;  stat()
  &emsp;  close()
 &emsp;  unlink()
  &emsp; rmdir()
  &emsp; write()
  &emsp; pwrite()
  &emsp; fsync()

### AfsClient.h
Contains implementations of all client side functions.

### server.cc
Defines the server interface
The server interface consists of the following functions:
&emsp;Create -
Creates a new file if the file doesn’t exist and sends a reply with modified time.
&emsp; Store -
Stores the data of the specified file and sends a reply with modified time.
&emsp; GetAttr -
Get the attributes of the file.
&emsp; Remove -
Deletes specified file.
&emsp; Release -
Locally cached data is written onto the server if modified.
&emsp; Mkdir -
Creates a new directory if it doesn’t exist.
&emsp; Rmdir -
Deletes the specified directory (which must be empty).
&emsp; Fetch -
Returns the data of the specified file or directory and the modified time.

### commonheaders.h
1. Contains implementation of the persistent hashmap we use to store modified time and modified flags

    * Hashmap:
        * Key: filename
        * Value: serverfile.getattr.mtime OR “modified” flag

    * Modified times returned by the gRPC calls (Fetch, Store, Create) are stored in the hashmap.
    * File modifications -
        * If the file is modified, the persistent hashmap stores a “modified” flag (negative arbitrary value) for the file
        * This is updated by the serverfile.getattr.mtime when flushed to the server on close

2. Contains other helper functions




