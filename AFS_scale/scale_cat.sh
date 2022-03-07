
for i in {0..9}
do
    persistent_file="~/storage"$i"/scale"
    echo $persistent_file
    cat $persistent_file
    echo "Client"$i" Persistent Storage: "
    if [ ! -f $persistent_file ];then
    echo "The file does not exist!"
    else
    cat $persistent_file
    fi
done

