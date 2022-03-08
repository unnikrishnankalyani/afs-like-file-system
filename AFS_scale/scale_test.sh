g++ -o test scale_test.c
../cmake/build/client -d ../cmake/build/scale0/fuse ~/storage0 &
../cmake/build/client -d ../cmake/build/scale1/fuse ~/storage1 &
../cmake/build/client -d ../cmake/build/scale2/fuse ~/storage2 &
../cmake/build/client -d ../cmake/build/scale3/fuse ~/storage3 &
../cmake/build/client -d ../cmake/build/scale4/fuse ~/storage4 &
../cmake/build/client -d ../cmake/build/scale5/fuse ~/storage5 &
../cmake/build/client -d ../cmake/build/scale6/fuse ~/storage6 &
../cmake/build/client -d ../cmake/build/scale7/fuse ~/storage7 &
../cmake/build/client -d ../cmake/build/scale8/fuse ~/storage8 &
../cmake/build/client -d ../cmake/build/scale9/fuse ~/storage9 &

# sleep(10)
# ./test ../cmake/build/scale0/fuse/scale &
# ./test ../cmake/build/scale1/fuse/scale &
# ./test ../cmake/build/scale2/fuse/scale &
# ./test ../cmake/build/scale3/fuse/scale &
# ./test ../cmake/build/scale4/fuse/scale &
# ./test ../cmake/build/scale5/fuse/scale &
# ./test ../cmake/build/scale6/fuse/scale &
# ./test ../cmake/build/scale7/fuse/scale &
# ./test ../cmake/build/scale8/fuse/scale &
# ./test ../cmake/build/scale9/fuse/scale &

# wait
echo "Scalability testing has been done!"
rm test