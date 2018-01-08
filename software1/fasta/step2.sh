for file in `ls $1`;
do
    echo -e \>\>$file"\n"
    #cat $file
    count=1
    for line in `cat $file`;
    do
        echo \>sequence$count
        echo -e $line"\n"
        count=$[$count+1]
    done
done
