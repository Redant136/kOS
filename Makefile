CCPATH:=$(HOME)/opt/cross/bin
export PATH:=$(CCPATH):$(PATH)

# ix86 x64 arm64 arm32
ARCH?=ix86
ifeq ($(ARCH),i386)
ARCH:=ix86
else ifeq ($(ARCH),x86)
ARCH:=ix86
else ifeq ($(ARCH),i686)
ARCH:=ix86

else ifeq ($(ARCH),x86_64)
ARCH:=x64
else ifeq ($(ARCH), x86-64)
ARCH:=x64
else ifeq ($(ARCH),AMD64)
ARCH:=x64
else ifeq ($(ARCH),INTEL64)
ARCH:=x64

else ifeq ($(ARCH),arm_32)
ARCH:=arm32
else ifeq ($(ARCH),arm-32)
ARCH:=arm32

else ifeq ($(ARCH),arm)
ARCH:=arm64
else ifeq ($(ARCH),arm_64)
ARCH:=arm64
else ifeq ($(ARCH),arm-64)
ARCH:=arm64
endif


CFLAGS?=-O2 -g 
CFLAGS:=$(CFLAGS) -Wall -Wextra -ffreestanding -fno-exceptions -Wno-unused-function
LFLAGS:=-ffreestanding -O2 -nostdlib -lgcc -z max-page-size=0x1000
DEFINES:=

ifeq ($(ARCH), x64)
CC:=x86_64-elf-gcc
CFLAGS+=-mcmodel=large
DEFINES+=-D__x86_64__ -D__x64__ -Dkernel_vadr=0xC00000000000
else ifeq ($(ARCH),ix86)
CC:=i686-elf-gcc
DEFINES+=-D__i386__ -D__i686__ -D__ix86__ -Dkernel_vadr=0xC0000000
else ifeq ($(ARCH), arm32)
CFLAGS+=
DEFINES+=-D__aarch32__
else ifeq ($(ARCH),arm64)
DEFINES+=-D__aarch64__
endif
CFLAGS+=$(DEFINES)

buildDIR:=build
sysroot:=$(buildDIR)/sysroot
LIBC_INCLUDEDIR:=usr/include
LIBC_SOURCEDIR:=usr/src

KERNEL_FOLDER:=kernel/kernel-$(ARCH)
LIBK_INCLUDEDIR:=$(KERNEL_FOLDER)/libkernel/include
LIBK_SOURCEDIR:=$(KERNEL_FOLDER)/libkernel/src

LIBKCommon_INCLUDEDIR:=kernel/libkernel/include
LIBKCommon_SOURCEDIR:=kernel/libkernel/src

ASM_INCLUDEDIR:=$(KERNEL_FOLDER)/asmInclude

LIBCList:= $(shell grep -rl ".c" $(LIBC_SOURCEDIR) | sed "s,$(LIBC_SOURCEDIR)/,,g" | sed "s,\.c,,g")
LIBCHeaders:= $(shell grep -rl ".h" $(LIBC_INCLUDEDIR) | sed "s,$(LIBC_INCLUDEDIR)/,,g")
LIBKList:= $(shell grep -rl ".c" $(LIBK_SOURCEDIR) | sed "s,$(LIBK_SOURCEDIR)/,,g" | sed "s,\.c,,g")
LIBKHeaders:= $(shell grep -rl ".h" $(LIBK_INCLUDEDIR) | sed "s,$(LIBK_INCLUDEDIR)/,,g")
LIBKListCommon:= $(shell grep -rl ".c" $(LIBKCommon_SOURCEDIR) | sed "s,$(LIBKCommon_SOURCEDIR)/,,g" | sed "s,\.c,,g")
LIBKHeadersCommon:= $(shell grep -rl ".h" $(LIBKCommon_INCLUDEDIR) | sed "s,$(LIBKCommon_INCLUDEDIR)/,,g")

CRTBEGIN_OBJ:=$(shell $(CCPATH)/$(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CCPATH)/$(CC) $(CFLAGS) -print-file-name=crtend.o)

IFLAGS:= $(addprefix -I,$(LIBC_INCLUDEDIR) $(LIBK_INCLUDEDIR) $(LIBKCommon_INCLUDEDIR) $(ASM_INCLUDEDIR))

LIBCFILES:=$(addprefix $(sysroot)/usr/lib/, $(addsuffix .o,$(LIBCList)))
LIBCHeaderFILES:=$(addprefix $(sysroot)/usr/include/, $(LIBCHeaders))
LIBKFILES:=$(addprefix $(sysroot)/lib/kernel/, $(addsuffix .o,$(LIBKList)))
LIBKHeaderFILES:=$(addprefix $(sysroot)/lib/kernel/include/, $(LIBKHeaders))
LIBKCommonFILES:=$(addprefix $(sysroot)/lib/kernel/, $(addsuffix .o,$(LIBKListCommon)))
LIBKCommonHeaderFILES:=$(addprefix $(sysroot)/lib/kernel/include/, $(LIBKHeadersCommon))


.PHONY: all

all: kernel libc libk kOS.iso Makefile

clean:
	rm -rf build

run: $(buildDIR)/kOS.iso Makefile
	qemu-system-i386 -cdrom $(buildDIR)/kOS.iso

kOS.iso: $(buildDIR)/kOS.iso
kernel: $(buildDIR)/kOS.bin
libc: $(LIBCFILES)
libk: $(LIBKFILES) 


# ---------------- iso ------------------------

$(buildDIR)/kOS.iso: $(sysroot)/boot/kOS.bin $(sysroot)/boot/grub/grub.cfg
	grub2-mkrescue -o $(buildDIR)/kOS.iso $(sysroot)

# ---------------- libc/libk -------------------

$(LIBCFILES): $(sysroot)/usr/lib/%.o: $(LIBC_SOURCEDIR)/%.c $(LIBC_INCLUDEDIR)/%.h $(LIBC_INCLUDEDIR)/**/*.h
	@ mkdir -p $(sysroot)/usr/lib/$(*D)
	@ mkdir -p $(sysroot)/usr/src/$(*D)
	@ mkdir -p $(sysroot)/usr/include/$(*D)
	cp $(LIBC_SOURCEDIR)/$*.c $(sysroot)/usr/src/$*.c
	cp $(LIBC_INCLUDEDIR)/$*.h $(sysroot)/usr/include/$*.h
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

$(LIBCHeaderFILES): $(sysroot)/usr/include/%.h: $(LIBC_INCLUDEDIR)/%.h
	@ mkdir -p $(sysroot)/usr/include/$(*D)
	cp $(LIBC_INCLUDEDIR)/$*.h $(sysroot)/usr/include/$*.h

$(LIBKFILES): $(sysroot)/lib/kernel/%.o: $(LIBK_SOURCEDIR)/%.c $(LIBK_INCLUDEDIR)/%.h $(LIBK_INCLUDEDIR)/**/*.h $(LIBKCommon_INCLUDEDIR)/**/*.h
	@ mkdir -p $(sysroot)/lib/kernel/$(*D)
	@ mkdir -p $(sysroot)/lib/kernel/include/$(*D)
	cp $(LIBK_INCLUDEDIR)/$*.h $(sysroot)/lib/kernel/include/$*.h
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

$(LIBKHeaderFILES): $(sysroot)/lib/kernel/include/%.h: $(LIBK_INCLUDEDIR)/%.h
	@ mkdir -p $(sysroot)/lib/kernel/include/$(*D)
	cp $(LIBK_INCLUDEDIR)/$*.h $(sysroot)/lib/kernel/include/$*.h

$(LIBKListCommon): $(sysroot)/lib/kernel/%.o: $(LIBKCommon_SOURCEDIR)/%.c $(LIBKCommon_INCLUDEDIR)/%.h $(LIBK_INCLUDEDIR)/**/*.h $(LIBKCommon_INCLUDEDIR)/**/*.h
	@ mkdir -p $(sysroot)/lib/kernel/$(*D)
	@ mkdir -p $(sysroot)/lib/kernel/include/$(*D)
	cp $(LIBKCommon_INCLUDEDIR)/$*.h $(sysroot)/lib/kernel/include/$*.h
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

$(LIBKCommonHeaderFILES): $(sysroot)/lib/kernel/include/%.h: $(LIBKCommon_INCLUDEDIR)/%.h
	@ mkdir -p $(sysroot)/lib/kernel/include/$(*D)
	cp $(LIBKCommon_INCLUDEDIR)/$*.h $(sysroot)/lib/kernel/include/$*.h

# --------------- kernel --------------------

CRT_OBJ_DIR:=$(sysroot)/lib/kernel
crt_obj:=$(CRT_OBJ_DIR)/crti.o $(CRTBEGIN_OBJ) $(CRTEND_OBJ) $(CRT_OBJ_DIR)/crtn.o

# $(buildDIR)/bootstrap_loader.bin: kernel/bootstrap_linker.ld $(buildDIR)/bootstrap_loader.o $(LIBCFILES) $(LIBKFILES)
# 	$(CC) -T $(KERNEL_FOLDER)/bootstrap_linker.ld -o $(buildDIR)/kOS.bin $(LFLAGS)\
# 			$(buildDIR)/boot.o $(buildDIR)/kernel.o $(LIBCFILES) $(LIBKFILES)\
# 			$(CRTEND_OBJ) $(CRT_OBJ_DIR)/crtn.o $(DEFINES)

# $(buildDIR)/bootstrap_loader.o: $(KERNEL_FOLDER)/bootstrap_loader.c
# 	$(CC) $(CFLAGS) $(IFLAGS) -c $(KERNEL_FOLDER)/bootstrap_loader.c -o $(buildDIR)/bootstrap_loader.o



# $(buildDIR)/kOS.bin: $(KERNEL_FOLDER)/linker.ld $(buildDIR)/boot.o $(buildDIR)/kernel.o $(LIBCFILES) $(LIBKFILES)\
#  		$(crt_obj)
# 	$(CC) -T $(KERNEL_FOLDER)/linker.ld -o $(buildDIR)/kOS.bin $(LFLAGS)\
# 		$(CRT_OBJ_DIR)/crti.o $(CRTBEGIN_OBJ)\
# 		$(buildDIR)/boot.o $(buildDIR)/kernel.o $(LIBCFILES) $(LIBKFILES)\
# 		$(CRTEND_OBJ) $(CRT_OBJ_DIR)/crtn.o $(DEFINES) -mcmodel=kernel
$(buildDIR)/kOS.bin: $(KERNEL_FOLDER)/linker.ld $(buildDIR)/boot.o $(buildDIR)/kernel.o $(LIBCFILES) $(LIBKFILES)\
 		$(crt_obj)
	$(CC) -T $(KERNEL_FOLDER)/linker.ld -o $(buildDIR)/kOS.bin $(LFLAGS)\
		$(buildDIR)/boot.o $(buildDIR)/kernel.o $(LIBCFILES) $(LIBKFILES)\
		$(CFLAGS)

$(CRT_OBJ_DIR)/crti.o: kernel/crti.S $(buildDIR)/boot.o
	@ mkdir -p $(CRT_OBJ_DIR)
	$(CC) $(CFLAGS) $(IFLAGS) -c kernel/crti.S -o $(CRT_OBJ_DIR)/crti.o

$(CRT_OBJ_DIR)/crtn.o: kernel/crtn.S $(buildDIR)/boot.o
	@ mkdir -p $(CRT_OBJ_DIR)
	$(CC) $(CFLAGS) $(IFLAGS) -c kernel/crtn.S -o $(CRT_OBJ_DIR)/crtn.o

BootASM:=boot_$(ARCH).S
# BootASM:=_boot.S
$(buildDIR)/boot.o: kernel/$(BootASM) $(ASM_INCLUDEDIR)/*.S
	@ mkdir -p build
	$(CC) $(CFLAGS) $(IFLAGS) -c kernel/$(BootASM) -o $(buildDIR)/boot.o

$(buildDIR)/kernel.o: kernel/kernel.c
	@ mkdir -p build
	$(CC) $(CFLAGS) $(IFLAGS) -c kernel/kernel.c -o $(buildDIR)/kernel.o

# ------------- copy ------------

$(sysroot)/boot/kOS.bin: $(buildDIR)/kOS.bin
	@ mkdir -p $(sysroot)/boot/grub
	cp $(buildDIR)/kOS.bin $(sysroot)/boot/kOS.bin

$(sysroot)/boot/grub/grub.cfg: kernel/grub.cfg
	@ mkdir -p $(sysroot)/boot/grub
	cp kernel/grub.cfg $(sysroot)/boot/grub/grub.cfg

