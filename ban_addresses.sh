!/bin/bash

if [ "$1" = "" ]; then
	echo "$(basename $0) (--yesterday|--all)

Bans repeated ip attempts updating from yesterday or from all known records"
	exit -1
fi

[ "$1" = "--yesterday" ] && arg="--since=yesterday"
[ "$1" = "--all" ] && arg=""


echo ""
echo "========================================================="
echo "=============$(date)================"

fold="address_processing"
mkdir -p $fold

evad=evil_addresses
evtxt=$evad".txt"
evtab=$fold/$evad".table"
banlist=$fold/$evad".ban"


echo -n "1: Getting journal ($arg)"
journalctl -u sshd $arg | grep "Failed" | pv > $evtxt
echo -e "\tX"

echo -n "2: Parsing for unique addresses"
./evilAddressParser $evtxt > $evtab
mv $evtxt $fold/
echo -e "\tX"

echo -n "3: Sorting table"
cat $evtab | sort -n -r -k 2 -n -r -k 3 | egrep -v "(#|Address|=|\s0$)" > $evtab".sorted"
awk '{print $1}' < $evtab."sorted" > $banlist
echo -e "\tX"


echo "4: Checking already banned ips:"
iptables -nvL | awk '{print $8}' | egrep "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+" | sort > $fold/old.names
cat $banlist | egrep "[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+" | sort > $fold/new.names
diff -b $fold/old.names $fold/new.names | egrep "^>" | sed 's/> //g' > $fold/to_update.txt
#diff -b $fold/old.names $fold/new.names | egrep "^>" | sed 's/>//g' 

echo "5: Banning new ips:"
while read address; do
	echo "--new: "$address
	iptables -A INPUT -s $address -j DROP
done < $fold/to_update.txt
echo "~~done~~"

#rm -rf $fold


systemctl reload iptables
