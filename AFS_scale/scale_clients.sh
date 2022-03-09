#!/bin/bash

for i in {0..49}
do
	command1=../cmake/build/scale${i}/fuse
        command2=~/storage${i}
	echo $command1
	echo $command2
	./client -d ${command1} ${command2} &
done

echo "Run Scalable Clients!"
