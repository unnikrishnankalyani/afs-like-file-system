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
    ./client [server ip address, default - localhost]   
	on server :   
	./server
