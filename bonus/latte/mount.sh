#!/bin/sh
S1=`gxmessage mount "$1" 2>&1` || gxmessage -center "mount $1 fail: $S1" -default "okay"
