from os.path import join
import sys
Import("env")

# Returns TARGET_MCU, MCU_PACKAGE and TARGET_MCU_LD values
# In the same way that the Makefile would do.
def get_ld_defines(chip_name: str):
    target_mcu: str = ""
    mcu_package: int = 0
    target_mcu_ld: int = 0
    if chip_name.startswith("ch32v003"):
        target_mcu = "CH32V003"
        target_mcu_ld = 0
    else:
        mcu_package = 1
        if chip_name.startswith("ch32v10"):
            target_mcu = chip_name.upper()[0:len("ch32v10x")]
            if "r8" in chip_name:
                mcu_package = 1
            elif "c8" in chip_name:
                mcu_package = 1
            elif "c6" in chip_name:
                mcu_package = 2
            target_mcu_ld = 1
        elif chip_name.startswith("ch32x03"):
            target_mcu = chip_name.upper()[0:len("ch32x035")]
            if "f8" in chip_name:
                mcu_package = 1
            elif "r8" in chip_name:
                mcu_package = 1
            elif "k8" in chip_name:
                mcu_package = 1
            elif "c8" in chip_name:
                mcu_package = 1
            elif "g8" in chip_name:
                mcu_package = 1
            elif "g6" in chip_name:
                mcu_package = 1
            elif "f7" in chip_name:
                mcu_package = 1
            target_mcu_ld = 4
        elif chip_name.startswith("ch32l103"):
            target_mcu = chip_name.upper()[0:len("ch32l103")]
            if "f8" in chip_name:
                mcu_package = 1
            elif "k8" in chip_name:
                mcu_package = 1
            elif "c8" in chip_name:
                mcu_package = 1
            elif "g8" in chip_name:
                mcu_package = 1
            target_mcu_ld = 4
        elif chip_name.startswith("ch32v20"):
            target_mcu = chip_name.upper()[0:len("ch32v20x")]
            if "f8" in chip_name:
                mcu_package = 1
            elif "g8" in chip_name:
                mcu_package = 1
            elif "k8" in chip_name:
                mcu_package = 1
            elif "c8" in chip_name:
                mcu_package = 1
            elif "f6" in chip_name:
                mcu_package = 2
            elif "k6" in chip_name:
                mcu_package = 2
            elif "c6" in chip_name:
                mcu_package = 2
            elif "rb" in chip_name:
                mcu_package = 3
            elif "gb" in chip_name:
                mcu_package = 3
            elif "cb" in chip_name:
                mcu_package = 3
            elif "wb" in chip_name:
                mcu_package = 3
            target_mcu_ld = 2
        elif chip_name.startswith("ch32v30"):
            target_mcu = chip_name.upper()[0:len("ch32v30x")]
            if "rc" in chip_name:
                mcu_package = 1
            elif "vc" in chip_name:
                mcu_package = 1
            elif "wc" in chip_name:
                mcu_package = 1
            elif "cb" in chip_name:
                mcu_package = 2
            elif "fb" in chip_name:
                mcu_package = 2
            elif "rb" in chip_name:
                mcu_package = 2
            target_mcu_ld = 3
        else:
            sys.stdout.write("Unkonwn MCU %s\n" % chip_name)
            env.Exit(-1)
    return (target_mcu, mcu_package, target_mcu_ld)

# Retrieve MCU name from selected board info
board = env.BoardConfig()
chip_name = str(board.get("build.mcu", "")).lower()
# retrieve needed macro values
target_mcu, mcu_package, target_mcu_ld = get_ld_defines(chip_name)

# Let the LD script be generated right before the .elf is built
env.AddPreAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(" ".join([
        "$CC",
        "-E",
        "-P",
        "-x",
        "c",
        "-DTARGET_MCU=%s" % target_mcu,
        "-DMCU_PACKAGE=%d" % mcu_package,
        "-DTARGET_MCU_LD=%d" % target_mcu_ld,
        join("ch32fun", "ch32fun.ld"),
        ">",
        join("$BUILD_DIR", "ldscript.ld")
    ]), "Building %s" % join("$BUILD_DIR", "ldscript.ld"))
)
# Already put in the right path for the to-be-generated file
env.Replace(LDSCRIPT_PATH=join("$BUILD_DIR", "ldscript.ld"))
