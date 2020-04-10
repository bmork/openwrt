append DRIVERS "quantenna"

# This protocol depends on a configured qv840-utils package
qcsapi_target_ip() {
	[ -z "$QCSAPI_TARGET_IP" ] && [ -r "/etc/qcsapi_target_ip.conf" ] &&  [ -x "$(which qcsapi_sockrpc)" ] && QCSAPI_TARGET_IP=$(cat /etc/qcsapi_target_ip.conf)
	echo "$QCSAPI_TARGET_IP"
}

qcsapi_call_complete() {
    local ret=$(qcsapi_sockrpc $@)
    [ "$ret" = "complete" ] && return 0
    return 1
}

qcsapi_call_ret() {
    local ret=$(qcsapi_sockrpc $@)
    echo $ret
    return 0
}

qcsapi_call_is() {
    local expected=$1; shift
    [ "$(qcsapi_sockrpc $@)" = "$expected" ] && return 0
    return 1
}

qcsapi_error() {
    local code=$(echo $@ | sed -ne "s/^QCS API error \([0-9]*\):.*/\1/p")
    [ -z "$code" ] && code=0
    echo $code
    return $code
}

run_startprod() {
    qcsapi_sockrpc startprod
}


# wait until module is ready to accept commands
wait_for_ready() {
	local i=0

	while [ $i -le 20 ]; do
		local mode=$(qcsapi_sockrpc get_mode wifi0)
		if [ "$mode" = "Access point" -o "$mode" = "Station" ]; then
			ready=1
			break;
		fi
		sleep 1
		i=$(($i + 1))
	done
}

quanteanna_setup_hw() {

	
	local ctry=$(echo $country | tr 'A-Z' 'a-z')
	qt_call $confidx update_persistent_param region "$ctry"
	[ "$bf" = "" ] && bf=1
	qt_call $confidx update_persistent_param bf "$bf"
	qt_call $confidx set_wifi_macaddr "$macaddr"
	local original_mode= `qcsapi_sockrpc --host $control_device_ip get_mode wifi$confidx`
	[ "$mode" = "ap" -a "$original_mode" = "Access point" ] || [ "$mode" = "sta" -a "$original_mode" = "Station" ] || qt_call $confidx reload_in_mode "$mode"
	qt_call $confidx rfenable "$((disabled^1))"

	[ "$hwmode" = "" ] && hwmode="auto"
	[ "$htmode" = "" ] && htmode="auto"

	local bw=
	local vht=
	case "$hwmode,$htmode" in
		*na,HT20)
			bw=20
			vht=0
			;;
		*na,HT40*)
			bw=40
			vht=0
			;;
		*ac,HT20)
			bw=20
			vht=1
			;;
		*ac,HT40*)
			bw=40
			vht=1
			;;
		*ac,HT80)
			bw=80
			vht=1
			;;
		*ac,*)
			bw=80
			vht=1
			;;
		*)
			bw=80
			vht=1
			;;
	esac

	qt_call $confidx update_persistent_param vht $vht
	qt_call $confidx set_bw $bw
	qt_call $confidx update_persistent_param bw $bw
	qt_call $confidx set_bb_param 1

	[ "$mode" = "sta" ] && return

	[ "$channel" = "auto" ] && channel=0
	qt_call $confidx set_channel $channel
	qt_call $confidx update_persistent_param channel $channel
	qt_call $confidx enable_scs 0
	qt_call $confidx update_persistent_param scs 0
	local reg_region=`qcsapi_sockrpc --host $control_device_ip get_regulatory_region wifi$confidx`
	local reg_chnls=
	if [ "$reg_region" != "none" ]; then
		reg_chnls=`qcsapi_sockrpc --host $control_device_ip get_list_regulatory_channels $reg_region`
		reg_chnls=$(echo $reg_chnls | sed  -e 's/,/ /g')
	else
		return
	fi
	for chnl in $reg_chnls
	do
		local max_power=`qcsapi_sockrpc --host $control_device_ip get_configured_tx_power wifi$confidx $chnl $reg_region`
		local txpwr=$(($txpower<$max_power?$txpower:$max_power))
		echo "max: $max_power requested: $txpower result: $txpwr" >> $QT_LOG_FILE
		qt_call $confidx set_tx_power $chnl $txpwr
	done
}

drv_qtpcie_teardown() {
	json_select config
	json_get_var control_device_ip control_device_ip
	json_select ..
	for_each_interface "sta ap adhoc" qtpcie_interface_cleanup
	qt_call 0 rfenable 0
	qcsapi_sockrpc --host $control_device_ip set_ip br0 netmask 255.255.255.248
	qcsapi_sockrpc --host $control_device_ip set_ip br0 broadcast 1.1.1.7
	qcsapi_sockrpc --host $control_device_ip set_ip br0 ipaddr 1.1.1.2
	initialized=0
}

detect_quantenna() { # - discover the radio hardware and produce suitable default configuration
#        Invoked by /sbin/wifi config through hotplug
#        The function should be idempotent, if discovered hardware already is present in the config, no further configuration should be emitted.
#        Receives no arguments.

	# figure out the first available "radioX" device name
	# - from package/kernel/mac80211/files/lib/wifi/mac80211.sh
	devidx=0
	config_load wireless
	while :; do
		config_get type "radio$devidx" type
		[ -n "$type" ] || break
		# the qcsapi does not support more than one module in a system
		[ "$type" = "quantenna" ] && return;
		devidx=$(($devidx + 1))
	done

	# find "all" - i.e. then single supported - Quantenna devices.
	# there is no reliable way to do this, as it can be virtually
	# any ethernet device
	qcsapi_device=$(ip route get $(qcsapi_target_ip) | sed -e 's,.* dev ,,' -e 's, .*,,' | tr -d \\n)

	uci -q batch <<-EOF
		set wireless.wifi${i}=wifi-device
		set wireless.wifi${i}.type=quantenna
		set wireless.wifi${i}.channel=${channel:-36}
		set wireless.wifi${i}.hwmode=11ac
		set wireless.wifi${i}.phy=${qcsapi_device}
		set wireless.wifi${i}.disabled=1

		set wireless.default_wifi${i}=wifi-iface
		set wireless.default_wifi${i}.device=wifi${i}
		set wireless.default_wifi${i}.network=lan
		set wireless.default_wifi${i}.mode=ap
		set wireless.default_wifi${i}.ssid=OpenWrt
		set wireless.default_wifi${i}.encryption=none
EOF
	uci -q commit wireless
}		

scan_quantenna() { # - preparation work for enable_xxx() and disable_xxx()
#        A bit misleadingly named; its purpose is to prepare state information and query hardware details in preparation to the subsequent call of disable_xxx() or enable_xxx()
#        Called by /sbin/wifi up [radioname], /sbin/wifi down [radioname], /sbin/wifi reload [radioname] which is usually invoked automatically by netifd.
#        If you do not need any context specific preparation, supply a no-op shell procedure
#        Receives the config wifi-device section name (as produced by detect_xxx()) as sole argument.


	local device="$1"
	local vif vifs wds
	local adhoc sta apmode mon disabled
	local adhoc_if sta_if ap_if mon_if

	config_get vifs "$device" vifs
        for vif in $vifs; do
		config_get_bool disabled "$vif" disabled 0
		[ $disabled -eq 0 ] || continue

		local mode
		config_get mode "$vif" mode
                case "$mode" in
                        adhoc|sta|wds|monitor)
				echo "$device($vif): unsupported mode 'mode'"
				continue
				;;
                        ap)
                                apmode=1
                                ap_if="${ap_if:+$ap_if }$vif"
                        ;;
                        *) echo "$device($vif): Invalid mode";;
                esac
        done

        local _c=
        for vif in ${adhoc_if:-$sta_if $ap_if $mon_if}; do
                config_set "$vif" ifname "${device}${_c:+-$_c}"
                _c=$((${_c:-0} + 1))
        done
        config_set "$device" vifs "${adhoc_if:-$sta_if $ap_if $mon_if}"

        ap=1
        infra=1
        if [ "$_c" -gt 1 ]; then
                mssid=1
        else
                mssid=
        fi

}

enable_quantenna() { # - translate the /etc/config/wireless settings into specific hardware configuration and bring up all configured SSIDs
#        Receives the config wifi-device section name (as produced by detect_xxx()) as sole argument
#        Invoked by /sbin/wifi up [radioname] and /sbin/wifi reload [radioname]
#        This is the central place where /etc/config/wireless should be read and turned into driver specific settings

	# 1. run start_prod - unless done
	# 2. wait for ready
	# 3. enable VLANs
	# 4. preconfigure N vifs with VLANs
	# 5. configure the vifs

	
}
  
disable_quantenna() { # - deconfigure all SSIDs, stop beaconing and shut down the radio
#       Receives the config wifi-device section name (as produced by detect_xxx()) as sole argument
#        Invoked by /sbin/wifi down [radioname] and /sbin/wifi reload [radioname]
#        This function shall properly deconfigure the radio so that a subsequent enable_xxx() call can work off a clean state


	# 1. disable all vfs
	# 2. destroy all VLANs
	# 3. reset wifi0 to default state
	# 4. ?? - make sure the state is good to go for enable...

}
