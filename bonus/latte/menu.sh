#!/bin/sh

mysql=$(pidof tor)
ii=$(pidof i2pd)
ssh=$(pidof sshd)
echo "iconsize = 24"

echo "Submenu = _tor"
if [ -z "$mysql" ]; then
echo "	icon = /usr/share/icons/stop.png"
else
echo "	icon = /usr/share/icons/start.png"
fi
cat <<EOF
	item = _Start
	cmd = /usr/bin/tor --runasdaemon 1
	icon = /usr/share/icons/start.png

	item = S_top
	cmd = /usr/bin/killall tor
	icon = /usr/share/icons/stop.png
EOF

echo "Submenu = _i2p"
if [ -z "$ii" ]; then
echo "	icon = /usr/share/icons/stop.png"
else
echo "	icon = /usr/share/icons/start.png"
fi
cat <<EOF
	item = _Start
	cmd = /usr/bin/i2pd --daemon
	icon = /usr/share/icons/start.png

	item = S_top
	cmd = /usr/bin/killall i2pd
	icon = /usr/share/icons/stop.png
EOF

echo "SEPARATOR"

echo "Submenu = _ssh"
if [ -z "$ssh" ]; then
echo "	icon = /usr/share/icons/stop.png"
else
echo "	icon = /usr/share/icons/start.png"
fi
cat <<EOF
	item = _Start
	cmd = /usr/bin/systemctl start sshd
	icon = /usr/share/icons/start.png

	item = S_top
	cmd = /usr/bin/systemctl stop sshd
	icon = /usr/share/icons/stop.png
EOF

#cat /root/1/my/TestMenu.txt

