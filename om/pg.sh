#!/usr/bin/zsh

action=start
if [[ "$0" =~ '.*\.stop$' || "$1" = "stop" ]]; then
	action=stop
elif [[ "$0" =~ '.*\.status$' || "$1" = "status" ]]; then
	action=status
fi

pg_ctl -D pg -l pg.log ${action}
