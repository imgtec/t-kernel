#CFLAGS += -D__LINUX_ARM_ARCH__=4
#CFLAGS += -O2
#CFLAGS += -Uarm
#

CFLAGS += -Wno-format-security

#CFLAGS += -Wstrict-prototypes
#CFLAGS += -Werror=strict-prototypes

CFLAGS += -Wall
CFLAGS += -Wundef
CFLAGS += -Werror-implicit-function-declaration
CFLAGS += -Wno-pointer-sign
CFLAGS += -Werror=implicit-int
CFLAGS += -Wdeclaration-after-statement
CFLAGS += -Wno-trigraphs
CFLAGS += -Wframe-larger-than=1024

#
CFLAGS += -fconserve-stack
CFLAGS += -fno-common
CFLAGS += -fno-delete-null-pointer-checks
CFLAGS += -fno-dwarf2-cfi-asm
CFLAGS += -fno-stack-protector
CFLAGS += -fno-strict-aliasing
CFLAGS += -fno-strict-overflow
CFLAGS += -fomit-frame-pointer
CFLAGS += -funwind-tables
CFLAGS += -g
#CFLAGS += -mabi=aapcs-linux
#CFLAGS += -march=armv4t
CFLAGS += -marm
CFLAGS += -mfpu=vfp
CFLAGS += -mno-thumb-interwork
CFLAGS += -msoft-float
#CFLAGS += -mtune=arm9tdmi
CFLAGS += -std=gnu89


ASFLAGS += -D__ASSEMBLY__
ASFLAGS += -mabi=aapcs-linux
ASFLAGS += -mno-thumb-interwork
ASFLAGS += -mfpu=vfp
ASFLAGS += -funwind-tables
ASFLAGS += -marm
ASFLAGS += -march=armv6k
ASFLAGS += -mtune=arm1136j-s
ASFLAGS += -msoft-float
ASFLAGS += -Wa,-gdwarf-2

