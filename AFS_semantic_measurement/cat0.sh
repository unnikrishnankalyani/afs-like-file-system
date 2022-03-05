test_case="test0"

server_file="../cmake/build/servertemppath/"$test_case
persistent_file="~/cache_temp/"$test_case
temp_file="../cmake/build/temp8/fuse/"$test_case
test_program=$test_case".c"

touch $server_file
echo $test_case > $server_file
g++ -o $test_case $test_program
./$test_case

sleep 1
echo "Server File: "
if [ ! -f "$server_file" ];then
 echo "The file does not exist!"
 else
 cat $server_file
fi

echo "Client Temp Cache: "
if [ ! -f $temp_file ];then
 echo "The file does not exist!"
 else
 cat $temp_file
fi


echo "Client Persistent Storage: "
if [ ! -f $persistent_file ];then
 echo "The file does not exist!"
 else
 cat $persistent_file
fi

rm ./$test_case