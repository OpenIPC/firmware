#!/bin/bash
shopt -s nullglob

toolchain_by_config() {
	CF=$1
	FMT=$2

	BR2_VER=$(grep BR2_DEFAULT_KERNEL_VERSION $CF)
	if [ -n "$BR2_VER" ]; then
		VENDOR=$(echo $CF | cut -d - -f 4 | cut -d / -f 1 | sed -r 's/\<./\U&/g')
		KVER=$(echo $BR2_VER | cut -d \" -f 2 | awk -F . '{printf "%s_%s", $1, $2}')
		GCC_VER=$(sed -rn "s/^BR2_GCC_VERSION_([0-9]*)_X=y/\1/p" $CF)
		ARCH=$(sed -rn "s/^BR2_(arm[a-z0-9_]+)=y/\1/p" $CF)
		if [ -z "$ARCH" ]; then
			ARCH=$(sed -rn "s/^BR2_(cortex_[a-z0-9_]+)=y/\1/p" $CF)
		fi
		if [ -z "$ARCH" ]; then
			ARCH=$(sed -rn "s/^BR2_(arm)=y/\1/p" $CF)
		else
			# Here we have something from ARM modern family
			HF=$(sed -rn "s/^BR2_ARM_EABI(HF)=y/\1/p" $CF | tr '[:upper:]' '[:lower:]')
		fi

		if [ -z "$ARCH" ]; then
			ARCH=$(sed -rn "s/^BR2_(mips_[a-z]*)=y/\1/p" $CF)
		else
			THUMB=$(sed -rn "s/^BR2_ARM_INSTRUCTIONS_(THUMB2)=y/\1/p" $CF)
			if [ -n "$THUMB" ]; then
				ARCH="${ARCH}_${THUMB,,}"
			fi
			if [ -n "$HF" ]; then
				ARCH="${ARCH}_${HF}"
			fi
		fi

		LIBC=$(sed -rn "s/^BR2_TOOLCHAIN_BUILDROOT_LIBC=\"(.*)\"/\1/p" $CF)

		SOC=$(echo $CF | cut -d _ -f 3)
		case $FMT in
			list)
				echo $ARCH $GCC_VER $LIBC $KVER $VENDOR $CF
				;;
			uniq)
				echo $ARCH $GCC_VER $LIBC $KVER
				;;
			*)
				if [ -n "$2" ]; then
					BR_DIR=buildroot-$2
					GCC_VER=$(sed -rn \
						"s/^\s+default\s+\"([0-9.]+)\"\s+if BR2_GCC_VERSION_${GCC_VER}_X/\1/p" \
							$BR_DIR/package/gcc/Config.in.host)
					case $LIBC in
						musl)
							VER=$(sed -rn "s/^MUSL_VERSION\s*=\s*([0-9.]+)/\1/p" \
								$BR_DIR/package/musl/musl.mk)
							;;
						uclibc)
							VER=$(sed -rn "s/^UCLIBC_VERSION\s*=\s*([0-9.]+)/\1/p" \
								$BR_DIR/package/uclibc/uclibc.mk)
							;;
						glibc)
							VER=$(sed -rn "s/^GLIBC_VERSION\s*=\s*([0-9.]+).*/\1/p" \
							$BR_DIR/package/glibc/glibc.mk	| tail -1)
							;;
						esac
					HASH=$(echo $VER | sha1sum | cut -c 1-8)
					echo ${ARCH}-gcc${GCC_VER}-${LIBC}-${KVER}-${HASH}
				else
					echo ${ARCH}-gcc${GCC_VER}-${LIBC}-${KVER}
				fi
				;;
		esac
	fi
}

if [ $# -eq 0 ]; then
	$0 list | sort
else
	if [ "$1" != "list" ] && [ "$1" != "uniq" ]; then
		toolchain_by_config $1 $2
	else
		for D in br-ext-chip-*; do
			for CF in $D/configs/*; do
				toolchain_by_config $CF $1
			done
		done
	fi
fi
