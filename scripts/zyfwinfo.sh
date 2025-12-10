#!/bin/sh
# add a zyfwinfo file to an existing sysupgrade-tar file, ignoring any fwtool data!

_chksum() { hexdump -v ${1:+-n$1} -e '1/1 "%u\n"' 2>/dev/null | awk '{ s+=$1; } END { print and(and(s,0xffff)+rshift(s,16),0xffff); }'; }
_htole32() { printf "%08x" $(($1)) | sed -E 's/^(..)(..)(..)(..)$/\\x\4\\x\3\\x\2\\x\1/'; }
_htole16() { printf "%04x" $(($1)) | sed -E 's/^(..)(..)$/\\x\2\\x\1/'; }
_hexbytes() { echo -n "$1" | sed -E 's/(..)/\\x\1/g'; }

zyfwinfo_create() {
	local outfile="$1"
	local product="$2"
	local feature="$3"
	local kernel="$4"
	local rootfs="$5"
	local version="$6"
	local timestamp="$7"

	local kernelsum=$(_chksum < "$kernel")
	local kernellen=$(stat --format=%s "$kernel" 2>/dev/null)
	local rootfssum=$(_chksum < "$rootfs")
	local rootfslen=$(stat --format=%s "$rootfs" 2>/dev/null)

	# zloader insists on 256, regardless of rootfs.  Not sure this is correct?
	local bs=256
	if [ "$(hexdump -v -n 4 -e '1/1 "%02x"' "$dir/$board_dir/root" 2>/dev/null)" = "68737173" ]; then
		bs=$(($(hexdump -v -n 4 -s 12 -e '1/1 "%02x"' "$dir/$board_dir/root" | sed -E 's/^(..)(..)(..)(..)$/0x\4\3\2\1/')/1024))
	fi

	(
		/bin/echo -ne "EXYZ\x2\x0\x0\x0"	# magic + version + sequence
		/bin/echo -ne $(_htole16 ${bs})	# blocksize in kB
		/bin/echo -ne $(date -ud@${timestamp} +"\x1\x0\x0\x%m\x%d\x%C\x%y\x%H\x%M\x%S") # 01 00 00 mm dd yy/100 yy%100 HH MM SS (in "BCD" format)
		/bin/echo -ne "${product:-\x0}" | dd bs=32 count=1 conv=sync 2>/dev/null
		/bin/echo -ne "${version:-\x0}" | dd bs=30 count=1 conv=sync 2>/dev/null
		/bin/echo -ne "${version:-\x0}" | dd bs=30 count=1 conv=sync 2>/dev/null
		/bin/echo -ne $(_htole16 ${kernelsum})
		/bin/echo -ne $(_htole16 ${rootfssum})
		/bin/echo -ne $(_hexbytes "$feature") | dd bs=4 count=1 conv=sync 2>/dev/null
		/bin/echo -ne $(_htole32 ${kernellen})
		/bin/echo -ne $(_htole32 ${rootfslen})
		dd if=/dev/zero bs=126 count=1 conv=sync 2>/dev/null
	) >"$outfile"

	# append checksum
	/bin/echo -ne $(_htole16 $(_chksum 128 <"$outfile")) >>"$outfile"
}

tarfile="$1"
product="$2"
feature="$(echo "$3" | sed -nE 's/^([0-9A-F])([0-9A-F])([0-9A-F])([0-9A-F])$/0\10\20\30\4/ip')"
version="${4:-OpenWrt}"

dir=$(mktemp -d)
board_dir="$(tar -C "$dir" -xvf "$tarfile" | grep '^sysupgrade-.*/$' | head -1)"
board_dir="${board_dir%/}"

[ -f "$dir/$board_dir/kernel" ] || { echo "$board_dir/kernel is required"; return 1; }
[ -f "$dir/$board_dir/root" ] || {
	echo "adding 1 byte dummy $board_dir/root"
	dd if=/dev/zero of="$dir/$board_dir/root" bs=1 count=1 2>/dev/null
}

timestamp="$(date +%s -ur "$dir/$board_dir/kernel")"

zyfwinfo_create "$dir/$board_dir/zyfwinfo" "$product" "$feature" "$dir/$board_dir/kernel" "$dir/$board_dir/root" "$version" "$timestamp"
tar -C "$dir" --sort=name --owner=0 --group=0 --numeric-owner --mtime=@$timestamp -cvf "$dir/tmp.tar" "$board_dir"
err="$?"

[ -e "$dir/tmp.tar" ] && cp "$dir/tmp.tar" "$tarfile"
rm -rf "$dir"

exit $err
