#!/bin/sh
rmdir "/media/$1"
t=`blkid -o value -s TYPE /dev/$1`
o=''
if [ "$t" == 'vfat' ]; then o='-t vfat -o rw,noexec,iocharset=utf8,codepage=866'; fi
if [ "$t" == 'ntfs' ]; then o='-t ntfs -o noatime,users,rw,fmask=111,dmask=000,locale=ru_RU.UTF-8'; fi
mkdir "/media/$1" && S1=`mount $o "/dev/$1" "/media/$1" 2>&1` || (gxmessage -center "mount $1 fail:$S1" -default "okay"; rmdir "/media/$1")
