#!/bin/sh
#
# hack up a basic .swu file for SWUpdate, which uses "libconfig"
# https://sbabic.github.io/swupdate/sw-description.html

version=""
decription=""
kernel=""
rescue=""
outfile=""
err=0

while [ "$1" ]; do
	case "$1" in
	"--version")
		version="$2"
		shift
		shift
		continue
		;;
	"--description")
		description="$2"
		shift
		shift
		continue
		;;
	"--kernel")
		kernel="$2"
		shift
		shift
		continue
		;;
	"--rescue")
		rescue="$2"
		shift
		shift
		continue
		;;
	*)
		if [ ! "$outfile" ]; then
			outfile=$1
			shift
			continue
		fi
		;;
	esac
done

# FIXME: $(IMAGE_ROOTFS) seems to be messed up when artifacts are created?
rootfs="$(dirname $kernel)/root.squashfs"
if [ -z "$version" -o -z "$description" -o ! -r "$kernel" -o ! -r "$rootfs" -o ! -r "$rescue" -o ! "$outfile" ]; then
	echo "syntax: $0 --version version --description description --kernel kernelimage --rescue rescue out"
	exit 1
fi

# taken from sysupgrade-tar.sh
tmpdir="$( mktemp -d 2> /dev/null )"
if [ -z "$tmpdir" ]; then
	# try OSX signature
	tmpdir="$( mktemp -t 'ubitmp' -d )"
fi
if [ -z "$tmpdir" ]; then
	exit 1
fi

cp "$rootfs" "$tmpdir/root"
cp "$kernel" "$tmpdir/kernel"
cp "$rescue" "$tmpdir/rescue"

# The OEM U-Boot validates both "nominal" and "rescue" images by
# looking at the rootfs magic.  So we need a dummy filesystem even for
# an initramfs kernel.  And this has to be a valid filesystem image to
# be accepted by swupdate
echo "" | mksquashfs4 - "$tmpdir/dummy" -cpiostyle -nopad -noappend -root-owned

# Some odd looking things in the below sw-description:
#  - there is no standard way to create a max sized UBI volume, so
#    we create an aribtrary sized "rootfs_data" in the hope that it
#    will fit.  This will fixed by the first sysupgrade in OpenWrt
#  - the OEM firmware will delete rootfs_data on boot, so we need
#    to prevent it from ever booting after OpenWrt is installed.
#    Therefore we install an OpenWrt initramfs as "rescue" system.
#  - the boot loader verifies that the selected rootfs volume contains
#    a squashfs file system, even when booting a RAM root from tftp or
#    an initramfs.  Writing an empty dummy squashfs to the rescue rootfs
#    to work around this
#  - writing OpenWrt to "oem-kernel" and "oem-rootfs" and then swapping
#    these volumes with the real "kernel" and "rootfs" to avoid writing
#    directly to the running system, gracefully handle partial updates,
#    and save a copy of the OEM firmware for backup purposes
cat <<EOT >"$tmpdir/sw-description"
software =
{
    version = "$version";
    hardware-compatibility = [ "1.0" ];
    description = "$description";
    partitions = (
	{
	    name = "rootfs_data";
	    device = "ubi";
	    size = 209715200;
	}
    );
    images = (
	{
	    filename = "kernel";
	    volume = "oem-kernel";
            device = "ubi";
	    sha256 = "$($MKHASH sha256 $tmpdir/kernel)";
	    installed-directly = True;
	    properties = { auto-resize = "true"; };
	},
	{
	    filename = "root";
	    volume = "oem-rootfs";
            device = "ubi";
	    sha256 = "$($MKHASH sha256 $tmpdir/root)";
	    installed-directly = True;
	    properties = { auto-resize = "true"; };
	},
	{
	    filename = "rescue";
	    volume = "kernel-rescue";
            device = "ubi";
	    sha256 = "$($MKHASH sha256 $tmpdir/rescue)";
	    installed-directly = True;
	    properties = { auto-resize = "true"; };
	},
	{
	    filename = "dummy";
	    volume = "rootfs-rescue";
            device = "ubi";
	    sha256 = "$($MKHASH sha256 $tmpdir/dummy)";
	    installed-directly = True;
	    properties = { auto-resize = "true"; };
	}
    );
    scripts = (
	{

	    type = "ubiswap";
	    properties = {
		swap-0 = [ "kernel", "oem-kernel" ];
		swap-1 = [ "rootfs", "oem-rootfs" ];
	    };
	}
    );
 };
EOT

if [ -n "$SOURCE_DATE_EPOCH" ]; then
	touch -hcd "@$SOURCE_DATE_EPOCH" $tmpdir/*
fi

(cd "$tmpdir"; cat <<EOT | cpio -ov -H crc -R +0:+0 > "$outfile")
sw-description
kernel
root
rescue
dummy
EOT

err="$?"
rm -rf "$tmpdir"
exit $err
