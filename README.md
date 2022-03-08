# wi-AFS
AFS-like Client/Server File System

To test :   
 &emsp;  Run (after setting $MY_INSTALL_DIR (to the directory where gRPC is installed) and $PATH appropriately):  
     &emsp; &emsp;  export MY_INSTALL_DIR=$HOME/.local
     &emsp; &emsp;  export PATH=/opt/homebrew/bin  
      &emsp; &emsp; PATH=/bin:/usr/bin:/usr/local/bin:${PATH}                     
      &emsp; &emsp; export PATH     
            
      &emsp; &emsp; mkdir -p cmake/build  
      &emsp; &emsp; cd cmake/build  
      &emsp; &emsp; cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..  
     &emsp; &emsp;  make afs.grpc.pb.o  
      &emsp; &emsp; make  
      
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




