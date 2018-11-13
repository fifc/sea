#!/usr/bin/zsh

get_pid() {
	pids=`pidof $1`
	for p in `echo $pids`; do
		tmp=`ps -oargs -p $p | grep 'mosquitto.conf'`
		if [ -n "$tmp" ]; then
			return $p
		fi
	done
	return -1
}

stop_proc() {
	while (true) do
		get_pid $1
		pid=$?
		if (( $pid < 0 )) then
			return 0
		fi
		echo -n "stopping $pid ... "
		kill $pid
		i=0
		for (( ; i < $2 * 10; ++i)); do
			get_pid $1
			if (( $? < 0 )) then
				echo "ok"
				break
			fi
			sleep 0.1
		done
		if (( $i >= $2 * 10 )) then
			echo "fail!"
			break
		fi
	done

	return 1
}

server_status() {
	get_pid $1
	pid=$?
	if (( $pid < 0 )) then
		echo "$1 is not running ..."
	else
		stime=`ps -olstart -p $pid|grep -v STARTED`
		echo "$1($pid) is running since $stime ..."
	fi
}

check_proc() {
	i=0
	for (( ; i < 5; ++i)); do
		sleep 0.2
		get_pid $1
		pid=$?
		if (( $pid < 0 )) then
			echo "fail"
			return 0
		fi
	done

	echo "ok"
	return 1
}

if [ "$1" = "status" ]; then
	server_status mosquitto
	exit 0
fi

if [[ "$0" =~ '.*\.status$' ]]; then
	server_status mosquitto
	exit 0
fi

if ! ( stop_proc mosquitto 5 ) then
	echo "error stop process!"
	exit 1
fi

if [[ "$0" =~ '.*\.stop$' ]]; then
	exit 0
fi

if [ "$1" = "stop" ]; then
	exit 0
fi

ulimit -c unlimited
echo -n "starting mosquitto ..."
/g/pkg/sbin/mosquitto -c /g/pkg/etc/mosquitto.conf -d > m.log 2>&1

check_proc mosquitto
