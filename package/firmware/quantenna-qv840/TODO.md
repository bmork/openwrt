# TODO #

## Add a new wireless driver for uci ##

* /lib/wifi/quantenna.sh


## must/should implement ##

 *   detect_xxx() - discover the radio hardware and produce suitable default configuration
 
        Invoked by /sbin/wifi config through hotplug
        The function should be idempotent, if discovered hardware already is present in the config, no further configuration should be emitted.
        Receives no arguments.
		
  *  scan_xxx() - preparation work for enable_xxx() and disable_xxx()
  
        A bit misleadingly named; its purpose is to prepare state information and query hardware details in preparation to the subsequent call of disable_xxx() or enable_xxx()
        Called by /sbin/wifi up [radioname], /sbin/wifi down [radioname], /sbin/wifi reload [radioname] which is usually invoked automatically by netifd.
        If you do not need any context specific preparation, supply a no-op shell procedure
        Receives the config wifi-device section name (as produced by detect_xxx()) as sole argument.
		
  *  enable_xxx() - translate the /etc/config/wireless settings into specific hardware configuration and bring up all configured SSIDs
  
        Receives the config wifi-device section name (as produced by detect_xxx()) as sole argument
        Invoked by /sbin/wifi up [radioname] and /sbin/wifi reload [radioname]
        This is the central place where /etc/config/wireless should be read and turned into driver specific settings
		
   * disable_xxx() - deconfigure all SSIDs, stop beaconing and shut down the radio
   
        Receives the config wifi-device section name (as produced by detect_xxx()) as sole argument
        Invoked by /sbin/wifi down [radioname] and /sbin/wifi reload [radioname]
        This function shall properly deconfigure the radio so that a subsequent enable_xxx() call can work off a clean state



## examples ##

* atheros
* madwifi
* broadcom-wl
* mac80211


## references ##

https://forum.openwrt.org/t/netifd-interaction-with-various-driver/12522
