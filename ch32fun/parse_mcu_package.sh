#!/bin/sh
TARGET_MCU_PACKAGE=${1:-empty}
TARGET_MCU=""
MEMORY_SPLIT=${2:-empty}
ENABLE_FPU=$3
MCU_REMAINING=${TARGET_MCU_PACKAGE}
DEFINES=""
ARCH="rv32imac"
ABI=""
FLASH_SIZE_KB=0
RAM_SIZE_KB=0
EXT_ORIGIN=""
EXT_SIZE_KB=0

# Use colors if stderr is a terminal
if [ -t 2 ]; then
    RED='\033[41m'
    RESET='\033[0m'
else
    RED=''
    RESET=''
fi

unknown_mcu() {
    printf "Unknown MCU: %s%b%s%b\n" "${TARGET_MCU_PACKAGE%"${MCU_REMAINING}"}" "$RED" "$MCU_REMAINING" "$RESET" 1>&2
    exit 1
}

advance() {
    i=$1
    while [ "$i" -gt 0 ]; do
        MCU_REMAINING=${MCU_REMAINING#?}
        i=$((i-1))
    done
}

set_target_mcu() {
    TARGET_MCU="${TARGET_MCU_PACKAGE%"${MCU_REMAINING}"}"
}

case $TARGET_MCU_PACKAGE in
    CH32V00*)
        DEFINES="CH32V00x"
        ARCH="rv32ec_zmmul"
        advance 7
        case ${MCU_REMAINING} in
            3*)
                ARCH="rv32ec"
                FLASH_SIZE_KB=16
                RAM_SIZE_KB=2
                ;;
            2*)
                FLASH_SIZE_KB=16
                RAM_SIZE_KB=4
                ;;
            [45]*)
                FLASH_SIZE_KB=32
                RAM_SIZE_KB=6
                ;;
            [67]*)
                FLASH_SIZE_KB=64
                RAM_SIZE_KB=8
                ;;
            *)
                unknown_mcu
                ;;
        esac
        advance 1
        set_target_mcu
        ;;
    CH32V10*)
        DEFINES="CH32V10x"
        advance 8
        case ${MCU_REMAINING} in
            [RC]8*)
                FLASH_SIZE_KB=64
                RAM_SIZE_KB=20
                ;;
            C6*)
                FLASH_SIZE_KB=32
                RAM_SIZE_KB=10
                ;;
            *)
                unknown_mcu
                ;;
        esac
        advance 2
        set_target_mcu
        ;;
    CH32X03[35]*)
        DEFINES="CH32X03x"
        FLASH_SIZE_KB=62
        RAM_SIZE_KB=20
        advance 8
        set_target_mcu
        ;;
    CH32L103*)
        DEFINES="CH32L103"
        FLASH_SIZE_KB=62
        RAM_SIZE_KB=20
        advance 8
        set_target_mcu
        ;;
    CH32V20?[CFGKRW]*)
        DEFINES="CH32V20x"
        advance 7
        case ${MCU_REMAINING} in
            3RB*)
                DEFINES="${DEFINES} CH32V20x_D8"
                ;;
            8*)
                DEFINES="${DEFINES} CH32V20x_D8W"
                ;;
            3*)
                DEFINES="${DEFINES} CH32V20x_D6"
                ;;
            *)
                unknown_mcu
                ;;
        esac
        advance 1
        set_target_mcu
        advance 1
        case ${MCU_REMAINING} in
            6*)
                FLASH_SIZE_KB=32
                RAM_SIZE_KB=10
                EXT_ORIGIN="0x08008000"
                EXT_SIZE_KB=192
                ;;
            8*)
                FLASH_SIZE_KB=64
                RAM_SIZE_KB=20
                EXT_ORIGIN="0x08010000"
                EXT_SIZE_KB=160
                ;;
            B*)
                FLASH_SIZE_KB=128
                RAM_SIZE_KB=32
                EXT_ORIGIN="0x08020000"
                EXT_SIZE_KB=352
                ;;
            *)
                unknown_mcu
                ;;
        esac
        ;;
    CH32V30?[CFRVW]*)
        ARCH="rv32imafc"
        DEFINES="CH32V30x"
        advance 7
        case ${MCU_REMAINING} in
            3*)
                DEFINES="${DEFINES} CH32V30x_D8"
                ;;
            [567]*)
                DEFINES="${DEFINES} CH32V30x_D8C"
                ;;
            *)
                unknown_mcu
                ;;
        esac
        advance 1
        set_target_mcu
        advance 1
        case ${MCU_REMAINING} in
            C*)
                case $MEMORY_SPLIT in
                    0)
                        FLASH_SIZE_KB=192
                        RAM_SIZE_KB=128
                        ;;
                    1)
                        FLASH_SIZE_KB=224
                        RAM_SIZE_KB=96
                        ;;
                    2)
                        FLASH_SIZE_KB=256
                        RAM_SIZE_KB=64
                        ;;
                    3)
                        FLASH_SIZE_KB=288
                        RAM_SIZE_KB=32
                        ;;
                    *)
                        printf "Unknown memory split: %s%b%s%b\n" "$MEMORY_SPLIT" "$RED" "$MEMORY_SPLIT" "$RESET" 1>&2
                        exit 1
                        ;;
                esac
                ;;
            B*)
                FLASH_SIZE_KB=128
                RAM_SIZE_KB=32
                ;;
            *)
                unknown_mcu
                ;;
        esac
        ;;
    CH5*)
        DEFINES="CH5xx HAS_HIGHCODE"
        advance 3
        case ${MCU_REMAINING} in
            7*)
                DEFINES="${DEFINES} CH57x"
                advance 1
                case ${MCU_REMAINING} in
                    [02]*)
                        FLASH_SIZE_KB=240
                        RAM_SIZE_KB=12
                        DEFINES="${DEFINES} CH570_CH572"
                        ;;
                    [13]*)
                        RAM_SIZE_KB=18
                        DEFINES="${DEFINES} CH571_CH573 HAS_DMA_QUIRK"
                        case ${MCU_REMAINING} in
                            1*)
                                FLASH_SIZE_KB=192
                                ;;
                            3*)
                                FLASH_SIZE_KB=448
                                ;;
                        esac
                        ;;
                    *)
                        unknown_mcu
                        ;;
                esac
                ;;
            8*)
                DEFINES="${DEFINES} CH58x"
                FLASH_SIZE_KB=448
                advance 1
                case ${MCU_REMAINING} in
                    [23]*)
                        RAM_SIZE_KB=32
                        DEFINES="${DEFINES} CH582_CH583"
                        ;;
                    [45]*)
                        DEFINES="${DEFINES} CH584_CH585"
                        case ${MCU_REMAINING} in
                            4*)
                                FLASH_SIZE_KB=96
                                ;;
                            5*)
                                FLASH_SIZE_KB=128
                                ;;
                        esac
                        ;;
                    *)
                        unknown_mcu
                        ;;
                esac
                ;;
            9*)
                DEFINES="${DEFINES} CH59x CH591_CH592"
                RAM_SIZE_KB=26
                advance 1
                case ${MCU_REMAINING} in
                    1*)
                        FLASH_SIZE_KB=192
                        ;;
                    2*)
                        FLASH_SIZE_KB=448
                        ;;
                    *)
                        unknown_mcu
                        ;;
                esac
                ;;
            *)
                unknown_mcu
                ;;
        esac
        advance 1
        set_target_mcu
        ;;
    CH32H41*)
        DEFINES="CH32H4x HAS_COUPLED_RAM"
        ARCH="rv32imafc"
        RAM_SIZE_KB=512
        advance 7
        case ${MCU_REMAINING} in
            [57]*)
                FLASH_SIZE_KB=960
                ;;
            6*)
                FLASH_SIZE_KB=480
                ;;
            *)
                unknown_mcu
                ;;
        esac
        advance 1
        set_target_mcu
        ;;
    *)
        unknown_mcu
        ;;
esac

case $ENABLE_FPU in
    0)
        case $ARCH in
            rv32imafc)
                ARCH="rv32imac"
                DEFINES="${DEFINES} DISABLED_FLOAT"
                ;;
        esac
        ;;
esac

case $ARCH in
    rv32ec*)
        ABI="ilp32e"
        ;;
    rv32imac)
        ABI="ilp32"
        ;;
    rv32imafc)
        ABI="ilp32f"
        ;;
esac

printf "TARGET_MCU=%s\n" "$TARGET_MCU"
printf "ARCH=%s\n" "$ARCH"
printf "ABI=%s\n" "$ABI"
printf "DEFINES=%s\n" "$DEFINES"
printf "FLASH_SIZE_KB=%s\n" "$FLASH_SIZE_KB"
printf "RAM_SIZE_KB=%s\n" "$RAM_SIZE_KB"
if [ -n "$EXT_ORIGIN" ]; then
    printf "EXT_ORIGIN=%s\n" "$EXT_ORIGIN"
    printf "EXT_SIZE_KB=%s\n" "$EXT_SIZE_KB"
fi
