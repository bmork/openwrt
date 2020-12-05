#!/bin/sh
#
# Copyright (C) 2020 OpenWrt.org

myname=$(basename $0)
sys=${myname#fw_print}
[ "$sys" ] && [ -r "/etc/fw_$sys.config" ] && exec /usr/sbin/fw_printenv -c "/etc/fw_$sys.config" "$@"
sys=${myname#fw_set}
[ "$sys" ] && [ -r "/etc/fw_$sys.config" ] && exec /usr/sbin/fw_setenv -c "/etc/fw_$sys.config" "$@"
echo "Called as invalid name '$0'"
exit 1
