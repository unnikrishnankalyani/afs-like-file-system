!/bin/bash
g++ -o test scale_test_write.c
for i in {0..49}
do
    op_file=../cmake/build/scale${i}/fuse/scale
    ./test $op_file &
done