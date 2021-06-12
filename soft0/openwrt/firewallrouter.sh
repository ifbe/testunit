if [ $# != 2 ] ; then
echo "$0 ::your:ip:last:64bit pi4b_eth0"
exit 1;
fi

str=`uci show firewall|grep -A4 $2|grep dest_ip`
echo $str

aaa=`echo $str|cut -f 1 -d "'"`
bbb=`echo $str|cut -f 2 -d "'"`
echo \"$aaa\" \"$bbb\"

old=`echo $bbb|cut -f 1 -d /|tr -d '\n'`
echo old=$old
echo new=$1

ccc=`echo $bbb|sed "s/$old/$1/"|tr -d '\n'`
echo -e $aaa$ccc

uci set $aaa$ccc
uci commit firewall
#service firewall restart
/etc/init.d/firewall restart
