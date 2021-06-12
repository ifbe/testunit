if [ $# != 2 ] ; then
	echo "$0 eth0 pi4b_eth0"
	exit 1;
fi
ipv6=`ifconfig $1|grep 240e|cut -f 10-11 -d " "|cut -f 5-8 -d ":"`
echo ipv6=$ipv6
ssh root@192.168.5.1 "./firewall.sh ::${ipv6} $2"
