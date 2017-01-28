#!/bin/sh
pass=$(yad --center --title "Mount internets" --form --field protocol:CB "ssh!nfs!ftp!smb" --image="dialog-password" --field=host "" --field=username "root" --field=password:H "" --field read_only:chk "" --field=dir "/")

set -- "$pass" 
IFS="|"; declare -a Array=($*) 

if [ "${Array[1]}" == "" ]; then exit; fi
PX=`echo "${Array[5]}"|tr / _`
if [ "$PX" == "_" ]; then PX=""; fi
PT=/media/"${Array[1]}$PX"

mkdir $PT

case "${Array[0]}" in
"ssh" )
	OP="password_stdin"; U=""
	if [ "${Array[4]}" == "TRUE" ]; then OP="password_stdin,ro"; U="-o ro"; fi

	SX=`cat ~/.ssh/known_hosts|cut -f1 -d' '|grep ${Array[1]}`
	if [ -z "$SX" ]; then
		##S1=`echo "yes\n""${Array[3]}"|sshfs -o $OP ${Array[2]}@${Array[1]}:${Array[5]} $PT` || (gxmessage -center "mount ${Array[1]} fail:$S1" -default "okay"; rmdir $PT)
		##rmdir $PT; gxmessage -center "mount ${Array[1]} fail:fingerprint not accepted!!!" -default "okay"
		#echo "sshfs $U ${Array[2]}@${Array[1]}:${Array[5]} $PT"
		##exec xterm -e "sshfs $U ${Array[2]}@${Array[1]}:${Array[5]} $PT; echo -n Press ENTER to exit... ; read"
		sakura -e "sshfs $U ${Array[2]}@${Array[1]}:${Array[5]} $PT"
		umount $PT
		S1=`echo "${Array[3]}"|sshfs -o $OP ${Array[2]}@${Array[1]}:${Array[5]} $PT` || (gxmessage -center "mount ${Array[1]} fail:$S1" -default "okay"; rmdir $PT)
	else
		S1=`echo "${Array[3]}"|sshfs -o $OP ${Array[2]}@${Array[1]}:${Array[5]} $PT` || (gxmessage -center "mount ${Array[1]} fail:$S1" -default "okay"; rmdir $PT)
	fi
	;;
"nfs" )
	OP="";
	if [ "${Array[4]}" == "TRUE" ]; then OP="-r"; else OP=""; fi
	S1=`mount.nfs4 ${Array[1]}:${Array[5]} $PT $OP` || (gxmessage -center "mount ${Array[1]} fail:$S1" -default "okay"; rmdir $PT)
	;;
"ftp" )
	OP=""; U="${Array[2]}"; P="${Array[3]}"
	if [ -z "$U" ]; then echo; else U+="@"; fi
	if [ -z "$P" ]; then echo; else P+=":"; fi
	if [ "${Array[4]}" == "TRUE" ]; then OP="-o ro"; fi
	S1=`curlftpfs ftp://$P$U${Array[1]}${Array[5]} $PT $OP` || (gxmessage -center "mount ${Array[1]} fail:$S1" -default "okay"; rmdir $PT)
	;;
"smb" )
	OP=""
	if [ "${Array[4]}" == "TRUE" ]; then OP="ro"; else OP="rw"; fi
	if [ -z "${Array[2]}" ]; then echo; else OP+=",user=${Array[2]}"; fi
	if [ -z "${Array[3]}" ]; then OP+=",pass="; else OP+=",pass=${Array[3]}"; fi
	S1=`mount.cifs ${Array[1]}:${Array[5]} $PT -o $OP` || (gxmessage -center "mount ${Array[1]} fail:$S1" -default "okay"; rmdir $PT)
	;;
esac