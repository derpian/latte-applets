#!/bin/sh
S1=`echo $1 | awk -F / '{print $NF}'`
umount $1 && gxmessage -center "$1 unmounted" -default "okay" || exit
if [ "$1" == "/media/$S1" ]; then
	rmdir "/media/$S1"
fi
