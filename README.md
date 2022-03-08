# wi-AFS
AFS-like Client/Server File System

To test :   
  Run (after setting $MY_INSTALL_DIR (to the directory where gRPC is installed) and $PATH appropriately):  
      export MY_INSTALL_DIR=$HOME/.local
      export PATH=/opt/homebrew/bin  
      PATH=/bin:/usr/bin:/usr/local/bin:${PATH}                     
      export PATH     
            
      mkdir -p cmake/build  
      cd cmake/build  
      cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..  
      make afs.grpc.pb.o  
      make  
      
  on client :    
    ./client [fuse mount path] [cache path]  
	on server :   
	./server [server fs path]

  The following POSIX APIs are supported:  
   open()  
   creat()  
   mkdir()  
   read()  
   pread()  
   stat()  
   close()     
  unlink()   
  rmdir()   
  write()   
  pwrite()   
  fsync()    

The server interface consists of the following functions:     
Create - 
Creates a new file if the file doesn’t exist and sends a reply with modified time.   
Store - 
Stores the data of the specified file and sends a reply with modified time.   
GetAttr - 
Get the attributes of the file.   
Remove - 
Deletes specified file.   
Release - 
Locally cached data is written onto the server if modified.   
Mkdir - 
Creates a new directory if it doesn’t exist.   
Rmdir - 
Deletes the specified directory (which must be empty).   
Fetch - 
Returns the data of the specified file or directory and the modified time.   




