test_case="crash0"

server_file="../cmake/build/servertemppath/"$test_case
persistent_file="~/cache_temp/"$test_case
temp_file="../cmake/build/temp8/fuse/"$test_case
test_program=$test_case".c"

touch $server_file
echo $test_case > $server_file
g++ -o $test_case $test_program
./$test_case