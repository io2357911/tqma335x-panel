#!/bin/sh

case "$1" in
  start)
	export QT_QPA_EVDEV_TOUCHSCREEN_PARAMETERS=/dev/input/event0:inverty
	if [ -f /opt/panel ]; then
		/opt/panel > /dev/null 2>&1 &
	fi
	;;
  stop)
	killall -9 panel
	;;
  restart|reload)
	;;
  *)
	echo "Usage: $0 {start|stop|restart}"
	exit 1
esac

