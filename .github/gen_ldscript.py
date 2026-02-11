from os.path import join
import sys
Import("env")

# Retrieve MCU name from selected board info
board = env.BoardConfig()
chip_name = str(board.get("build.mcu", "")).lower()
# retrieve needed macro values using the helper script
config_vars = env.Execute(" ".join([
    "sh",
    join("ch32fun", "ch32fun", "parse_mcu_package.sh"),
    chip_name
])).strip().splitlines()
config_dict = dict(var.split("=", 1) for var in config_vars)

defines_list = [
    (config_dict["TARGET_MCU"], 1),
]
for key in ("TARGET_MCU", "FLASH_SIZE_KB", "RAM_SIZE_KB", "EXT_ORIGIN", "EXT_SIZE_KB"):
    if key in config_dict:
        defines_list.append((key, config_dict[key]))
for define in config_dict.get("DEFINES", "").split():
    if define:
        defines_list.append((define, 1))

# some header files also use these macros, so inject them
env.Append(
    CPPDEFINES=defines_list
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
        " ".join("-D%s=%s" % (key, value) for key, value in defines_list),
        join("ch32fun", "ch32fun.ld"),
        ">",
        join("$BUILD_DIR", "ldscript.ld")
    ]), "Building %s" % join("$BUILD_DIR", "ldscript.ld"))
)
# Already put in the right path for the to-be-generated file
env.Replace(LDSCRIPT_PATH=join("$BUILD_DIR", "ldscript.ld"))
