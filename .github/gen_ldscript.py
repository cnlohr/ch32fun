from os.path import join
import sys
Import("env")

# Returns TARGET_MCU, MCU_PACKAGE and TARGET_MCU_LD values
# In the same way that the Makefile would do.
def get_ld_defines(chip_name: str):
    target_mcu: str = ""
    mcu_package: int = 0
    target_mcu_ld: int = 0
    if chip_name.startswith("ch32v00"):
        target_mcu = chip_name.upper()[0:len("ch32v00x")]
        mcu_package = 1
        if "003" in chip_name:
            target_mcu_ld = 0
        elif "002" in chip_name:
            target_mcu_ld = 5
        elif "004" in chip_name or "005" in chip_name:
            target_mcu_ld = 6
        elif "006" in chip_name or "007" in chip_name:
            target_mcu_ld = 7
        else:
            sys.stdout.write("Unknown CH32V00x variant %s\n" % chip_name)
            env.Exit(-1)
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
        elif chip_name.startswith("ch57"):
            # CH570 / CH571 / CH572 / CH573
            target_mcu = "CH57X"
            if "570" in chip_name:
                mcu_package = 0
            elif "571" in chip_name:
                mcu_package = 1
            elif "572" in chip_name:
                mcu_package = 2
            elif "573" in chip_name:
                mcu_package = 3
            else:
                sys.stdout.write("Unknown CH57x variant %s\n" % chip_name)
                env.Exit(-1)
            target_mcu_ld = 10
        elif chip_name.startswith("ch58"):
            # CH582 / CH583 / CH584 / CH585
            target_mcu = "CH58X"
            if "582" in chip_name:
                mcu_package = 2
            elif "583" in chip_name:
                mcu_package = 3
            elif "584" in chip_name:
                mcu_package = 4
            elif "585" in chip_name:
                mcu_package = 5
            else:
                sys.stdout.write("Unknown CH58x variant %s\n" % chip_name)
                env.Exit(-1)
            target_mcu_ld = 8
        elif chip_name.startswith("ch59"):
            # CH591 / CH592
            target_mcu = "CH59X"
            if "591" in chip_name:
                mcu_package = 1
            elif "592" in chip_name:
                mcu_package = 2
            else:
                sys.stdout.write("Unknown CH59x variant %s\n" % chip_name)
                env.Exit(-1)
            target_mcu_ld = 9
        elif chip_name.startswith("ch32h41"):
            target_mcu = chip_name.upper()[0:len("ch32h41x")]
            mcu_package = 1
            if "416" in chip_name:
                mcu_package = 2
            elif "415" in chip_name:
                mcu_package = 3
            target_mcu_ld = 11
        else:
            sys.stdout.write("Unknown MCU %s\n" % chip_name)
            env.Exit(-1)
    return (target_mcu, mcu_package, target_mcu_ld)

# Retrieve MCU name from selected board info
board = env.BoardConfig()
chip_name = str(board.get("build.mcu", "")).lower()
# retrieve needed macro values
target_mcu, mcu_package, target_mcu_ld = get_ld_defines(chip_name)

# some header files also use these macros, so inject them
env.Append(
    CPPDEFINES=[
        ("TARGET_MCU", target_mcu),
        ("MCU_PACKAGE", mcu_package),
        ("TARGET_MCU_LD", target_mcu_ld)
    ]
)

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
