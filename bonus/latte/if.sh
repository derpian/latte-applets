#!/bin/sh

yiffs=$(/sbin/ifconfig -a |grep -e ^[a-z] | awk '{ print $1}'| sed 's/ *$//;s/.$//')

echo "iconsize = 24"

for s in $yiffs; do
yiff=$(ifconfig|grep "$s")
echo -n "Submenu = "
if [ "$s" == "$1" ]; then
	echo "$1" | sed 's/./_&/g;s/^ //'
else
	echo "$s"
fi
if [ -z "$yiff" ]; then
echo "	icon = /usr/share/icons/stop.png"
else
echo "	icon = /usr/share/icons/start.png"
fi
cat <<EOF
	item = _Up
	cmd = ifconfig $s up
	icon = /usr/share/icons/start.png

	item = _Down
	cmd = ifconfig $s down
	icon = /usr/share/icons/stop.png

	SEPARATOR

	item = D_hcp
	cmd = pump -i $s
	icon = /usr/share/icons/stop.png

	item = _Static
	cmd = /usr/share/pancake/static.sh $s
	icon = /usr/share/icons/stop.png
EOF

done

#cat /root/1/my/TestMenu.txt

