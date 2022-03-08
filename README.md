# wi-AFS
AFS-like Client/Server File System

To test :   
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

  The following POSIX APIs are supported:  
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

AfsClient.h
Contains implementations of all client side functions

client.cc
Defines fuse operations supported by the client

server.cc
Defines the server interface

commonheaders.h
1. Contains implementation of the persistent hashmap we use to store modified time and modified flags

    * Hashmap:
        * Key: filename
        * Value: serverfile.getattr.mtime OR “modified” flag

    * Modified times returned by the gRPC calls (Fetch, Store, Create) are stored in the hashmap.
    * File modifications - 
        * If the file is modified, the persistent hashmap stores a “modified” flag (negative arbitrary value) for the file
        * This is updated by the serverfile.getattr.mtime when flushed to the server on close

2. Contains other helper functions




