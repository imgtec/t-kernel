
ARCH := arm

# MACH := mach-em1d
# MACH := mach-asm9
# MACH := mach-acd2
# MACH := mach-mx28

MACH := mach-mx28
GPL := -gpl

include Makefile.src

CROSS_COMPILE ?= arm-rtems4.12-

HOST ?= windows

ifeq ($(HOST),windows)
TFTPBOOT := "D:\\DATA\\APP\\tftpd\\"
RM := del
COPY := copy
else
TFTPBOOT := "/var/lib/tftpboot/"
RM := rm -f
COPY := cp
endif

GNUCC := $(CROSS_COMPILE)gcc
ARMCC := armcc
CC := $(GNUCC)

ifeq ($(MACH),mach-em1d)
LDS := "hwdepend/arm/lib/monitor.lnk"
else
LDS := "hwdepend/arm/lib/monitor.ld"
endif

INCLUDE = \
	-Ihwdepend/arm/$(MACH)/include \
	-Ihwdepend/arm/include \
	-Iinclude \
	-Ihwdepend/arm/$(MACH) \
	-I../sysdepend/arm/include \
	-I../include \
	-Icmdsvc

CFLAGS := -msoft-float -mthumb-interwork -mstructure-size-boundary=8 -ffreestanding -Wno-pointer-sign -D_TEF_EM1D_ -nostdlib -g

-include hwdepend/$(ARCH)/Makefile

ifeq ($(ARCH),arm)
CFLAGS += -D__ARM__
endif
ifeq ($(ARCH),mips)
CFLAGS += -D__MIPS__
endif
ifeq ($(MACH),mach-em1d)
CFLAGS += -mcpu=arm1176jzf-s
CFLAGS += -D__TRON_ARM_ARCH=6
CFLAGS += -DCONFIG_MACH_EM1D
endif

ifeq ($(MACH),mach-acd2)
CFLAGS += -mcpu=cortex-r4
CFLAGS += -D__TRON_ARM_ARCH=7
CFLAGS += -DCONFIG_MACH_ACD2
endif

ifeq ($(MACH),mach-asm9) 
CFLAGS += -mcpu=arm926ej-s
CFLAGS += -D__TRON_ARM_ARCH=5
CFLAGS += -DCONFIG_MACH_ASM9
CFLAGS += -DCONFIG_INIT_SP=0x40002000
endif

ifeq ($(MACH),mach-sam9) 
CFLAGS += -mcpu=arm926ej-s
CFLAGS += -D__TMONITOR__
CFLAGS += -DCONFIG_MACH_SAM9
CFLAGS += -D__TRON_ARM_ARCH=5
CFLAGS += -DCONFIG_SKIP_LOWLEVEL

# memory
CFLAGS += -DCONFIG_TM_INIT_SP=0x00010000 # 64KiB
CFLAGS += -DCONFIG_TM_STACK_TOP=0x72300000
CFLAGS += -DCONFIG_TM_STACK_SIZE=0x00010000
CFLAGS += -DCONFIG_TM_HEAP_TOP=0
CFLAGS += -DCONFIG_TM_HEAP_SIZE=0

# drivers
CFLAGS += -DCONFIG_TM_SIO
CFLAGS += -DCONFIG_TM_DISK
CFLAGS += -DCONFIG_TM_FLASHROM
CFLAGS += -DCONFIG_TM_BAUD_RATE=115200
CFLAGS += -DCONFIG_TM_EARLY_PRINTK

# t-monitor
CFLAGS += -DCONFIG_TM_WRKBUF_SZ="(1<<10)"
endif

ifeq ($(MACH),mach-mx28) 
CFLAGS += -mcpu=arm926ej-s
CFLAGS += -D__TMONITOR__
CFLAGS += -DCONFIG_MACH_MX28
CFLAGS += -DCONFIG_TM_MONKEY
CFLAGS += -D__TRON_ARM_ARCH=5
CFLAGS += -DCONFIG_SKIP_LOWLEVEL
CFLAGS += -DCONFIG_TM_DELAY64US=256

# memory
# 128K
CFLAGS += -DCONFIG_TM_INIT_SP="(0x00020000-0x10)"
CFLAGS += -DCONFIG_TM_STACK_TOP=0x44000000
CFLAGS += -DCONFIG_TM_STACK_SIZE=0x00080000
CFLAGS += -DCONFIG_TM_HEAP_TOP=0
CFLAGS += -DCONFIG_TM_HEAP_SIZE=0

# drivers
CFLAGS += -DCONFIG_TM_SIO
CFLAGS += -DCONFIG_TM_DISK
CFLAGS += -DCONFIG_TM_FLASHROM
CFLAGS += -DCONFIG_TM_BAUD_RATE=115200
CFLAGS += -DCONFIG_TM_EARLY_PRINTK

# t-monitor
CFLAGS += -DCONFIG_TM_WRKBUF_SZ="(1<<10)"

# t-kernel & t-monitor shared
# CFLAGS += -DCONFIG_EIT_VECTBL=0
endif

.PHONY: all
all: tmonitor


hwdepend/arm/cpu/monhdr.o: hwdepend/arm/cpu/monhdr.S
	@$(CC) $(INCLUDE) -mcpu=arm1176jzf-s $(CFLAGS) -c $< -o $@ -D_in_asm_source_

%.o: %.S
	@echo	"     AS\t"	$@
	@$(CC) $(INCLUDE) $(CFLAGS) -c $< -o $@ -D_in_asm_source_

%.o: %.c
	@echo	"     CC\t"	$@
	@$(CC) $(INCLUDE) $(CFLAGS) -c $< -o $@

tmonitor: $(OBJ)
	@$(CROSS_COMPILE)gcc $^ -o $@ $(INCLUDE) -T$(LDS) -nostdlib -lgcc
	@echo	"     LD\t"	$@
	@$(CROSS_COMPILE)objdump -DS $@ >tm.S
	@echo	"    DIS\t"	tm.S
	@$(CROSS_COMPILE)objcopy -O binary $@ tm.bin
	@echo	"OBJDUMP\t"	tm.bin
	@$(COPY) tm.bin $(TFTPBOOT)
	@$(CROSS_COMPILE)nm $@ >tm.map
	@echo   " SYSMAP\t"	tm.map

clean:
ifeq ($(HOST),windows)
	del /s *.o
	del tmonitor tm.S tm.bin tm.map
else
	@$(RM) $(OBJ) tmonitor tm.S tm.bin tm.map
	@echo	"  CLEAN"
endif
prepare:
	@sed 's/^SRC/OBJ/g' Makefile.src | sed 's/[cS]$$/o/g' >Makefile.obj
	@echo	"   GEN\tMakefile: " $(MACH)
