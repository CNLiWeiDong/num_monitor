#!/bin/bash

function secret
{
    local secret_variable="$1"
    local PASS=""
    while : ;do
        char=`
            stty cbreak -echo
            dd if=/dev/tty bs=1 count=1 2>/dev/null
            stty -cbreak echo
        `
        if [ "$char" = "" ];then
            echo #这里的echo只是为换行
            break
        fi
        PASS="$PASS$char"
        echo -n "*"
    done
    eval "$secret_variable=$PASS"
}

function read_secret
{
    local hint="$1"
    local secret_variable="$2"

    while [ 1 = 1 ]
    do
	echo "$hint"
	echo -n 'first input secret:'
	# read -s secret_1st_time
    secret secret_1st_time
	echo -n 'confirm the secret:'
	# read -s secret_2nd_time
    secret secret_2nd_time

	if [ "$secret_1st_time" != "$secret_2nd_time" ]
	then
	    echo 'secret mismatch, please retry.' >&2
	    continue
	fi

	if [ -z "$secret_1st_time" ]
	then
	    echo 'empty secret, please retry.' >&2
	    continue
	fi

	eval "$secret_variable=$secret_1st_time"
	break
    done
}

function create_secret
{
    read_secret '****************** IPFS_PROJECT_ID ******************' IPFS_PROJECT_ID
    read_secret '****************** IPFS_PROJECT_SECRET ******************' IPFS_PROJECT_SECRET
    read_secret '****************** HTTP_SECURITY_RSA_PRIVATE_PASS ******************' HTTP_SECURITY_RSA_PRIVATE_PASS
    
    export IPFS_PROJECT_ID=$IPFS_PROJECT_ID
    export IPFS_PROJECT_SECRET=$IPFS_PROJECT_SECRET
    export HTTP_SECURITY_RSA_PRIVATE_PASS=$HTTP_SECURITY_RSA_PRIVATE_PASS
}
create_secret
./num_monitor
# ./build/hbapp/num_monitor/num_monitor
# 必需挂起启动，否则无法清楚环境变量
# nohup ./build/hbapp/num_monitor/num_monitor&
IPFS_PROJECT_ID=""
IPFS_PROJECT_SECRET=""
HTTP_SECURITY_RSA_PRIVATE_PASS=""
export IPFS_PROJECT_ID=""
export IPFS_PROJECT_SECRET=""
export HTTP_SECURITY_RSA_PRIVATE_PASS=""