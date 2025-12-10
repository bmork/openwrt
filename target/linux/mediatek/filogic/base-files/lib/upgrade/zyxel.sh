# zyfwinfo_create() - create Zyxel "zyfwinfo" metadata
#
# Minimum requirements for booting are
# - metadata checksum must be correct
# - kernel and rootfs checksums must be correct over the given length,
#   which can be 0
zyfwinfo_create() {
	local product="$1"
	local feature="$(echo $2 | sed -E 's/(..)/\\x\1/g')"
	local version="$3"

	(
		printf "EXYZ\x2\x0\x0\x0\x0\x1"
		printf $(date -u +"\x1\x0\x0\x%m\x%d\x%C\x%y\x%H\x%M\x%S")
		printf "${product:-\x0}" | dd bs=32 count=1 conv=sync 2>/dev/null
		printf "${version:-\x0}" | dd bs=30 count=1 conv=sync 2>/dev/null
		printf "${version:-\x0}" | dd bs=30 count=1 conv=sync 2>/dev/null
		printf "\x0\x0\x0\x0"
		printf "${feature:-\x0}" | dd bs=4 count=1 conv=sync 2>/dev/null
		dd if=/dev/zero bs=134 count=1 conv=sync 2>/dev/null
	) >/tmp/zyfwinfo

	local chksum=$(hexdump -v -n 128 -e '1/1 "%u\n"' /tmp/zyfwinfo | awk '{ s+=$1; } END { print and(and(s,0xffff)+rshift(s,16),0xffff); }')
	printf $(printf "%04x" "$chksum" | sed -E 's/^(..)(..)$/\\x\2\\x\1/') >>/tmp/zyfwinfo
}

zyfwinfo_version() {
	. /usr/share/libubox/jshn.sh
	json_load "$(fwtool -q -i - "$1")"
	json_select "version" >/dev/null
	json_get_vars "dist" "version"
	[ -n "$dist" ] && [ -n "$version" ] && echo "$dist $version" || echo "OpenWrt"
}

# zyfwinfo_do_check() - Verify sysupgrade-tar image
#
# Accepts both OpenWrt images without any Zyxel metadata and OEM
# images with "zyfwinfo" and "zydefault" files included
#
# Side effects:
# 1. saves zyfwinfo metadata to /tmp
# 2. configures system to boot from 1st kernel partition/volume
zyfwinfo_do_check() {
	local file="$1"
	local cmd="$(identify_if_gzip "$file")cat"

	# the zyfwinfo data must match the this system
	local product=$(fw_printenv -n ProductName)
	local feature=$(fw_printenv -n FeatureBits)
	feature=${feature:0:8}
	[ -n "$product" ] && [ -n "$feature" ] || {
		v "ProductName or FeatureBits missing in environment"
		return 1
	}

	local board_dir="$( $cmd < "$file" | tar tf - | grep -m 1 '^sysupgrade-.*/$')"
	board_dir="${board_dir%/}"
	[ -n "$board_dir" ] || return 1

	# save vendor metadata files for later
	$cmd < "$file" | tar xOf - "$board_dir/zyfwinfo" 2>/dev/null >/tmp/zyfwinfo
	$cmd < "$file" | tar xOf - "$board_dir/zydefault" 2>/dev/null >/tmp/zydefault

	# create new zyfwinfo if required
	[ -s /tmp/zyfwinfo ] ||	zyfwinfo_create "$product" "$feature" "$(zyfwinfo_version "$file")"

	# Verify vendor image
	if [ -s /tmp/zydefault ]; then
		[ "$product" = "$(dd if=/tmp/zyfwinfo bs=1 skip=20 count=32 2>/dev/null)" ] &&
		[ "$feature" = "$(hexdump -v -n4 -s116 -e '1/1 "%02X"' /tmp/zyfwinfo)" ] || {
			v "image is not intended for $product"
			return 1
		}
	fi

	[ $(fw_printenv -n dual_boot.current_slot) -eq 0 ] || {
		v "Switching from 2nd to 1st firmware image"
		fw_setenv dual_boot.current_slot 0
		[ $(fw_printenv -n dual_boot.slot_0_invalid) -eq 0 ] || fw_setenv dual_boot.slot_0_invalid 0
	}

	return 0
}

zywinfo_ubi_meta() {
	local ubidev="$1"
	local volname="$2"
	local file="$3"

	[ -s "$file" ] || return
	local ubivol="$(nand_find_volume $ubidev "$volname")"
	[ -n "$ubivol" ] || {
		ubimkvol /dev/$ubidev -N "$volname" -S 1
		ubivol="$(nand_find_volume $ubidev "$volname")"
	}
	[ -n "$ubivol" ] && ubiupdatevol /dev/$ubivol "$file"
}

zyfwinfo_nand() {
	local ubidev="$(nand_attach_ubi "$CI_UBIPART")"
	[ -n "$ubidev" ] || nand_do_upgrade_failed

	zywinfo_ubi_meta "$ubidev" "$CI_ZYFWINFO" /tmp/zyfwinfo
	zywinfo_ubi_meta "$ubidev" "$CI_ZYDEFAULT" /tmp/zydefault
	nand_do_upgrade "$1"
}

zywinfo_emmc_meta() {
	local partname="$1"
	local file="$2"

	[ -s "$file" ] || return
	local mmcpart="$(find_mmc_part "$partname")"
	[ -n "$mmcpart" ] && dd if="$file" of="$mmcpart" bs=512 conv=sync
}

zyfwinfo_emmc() {
	zywinfo_emmc_meta "$CI_ZYFWINFO" /tmp/zyfwinfo
	zywinfo_emmc_meta "$CI_ZYDEFAULT" /tmp/zydefault
	emmc_do_upgrade "$1"
}

zyfwinfo_do_upgrade() {
	local file="$1"
	local root="$(cmdline_get_var root)"
	CI_ZYFWINFO="zyfwinfo"
	CI_ZYDEFAULT="zydefault"

	case "$root" in
	/dev/mmc*)
		# vendor image must use rootfs
		[ -s /tmp/zydefault ] || EMMC_ROOT_DEV="$root"
		zyfwinfo_emmc "$file"
		;;
	*)
		zyfwinfo_nand "$file"
		;;
	esac
}
