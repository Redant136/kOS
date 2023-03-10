CCPATH:=$(HOME)/opt/cross/bin
export PATH:=$(CCPATH):$(PATH)

# i386 x64 arm-64
KERNEL_ARCH?=i386

ASMC:=i686-elf-as
CC:=i686-elf-gcc
CFLAGS?=-O2 -g 
CFLAGS:=$(CFLAGS) -Wall -Wextra -ffreestanding -fno-exceptions -Wno-unused-function

buildDIR:=build
sysroot:=$(buildDIR)/sysroot
LIBC_INCLUDEDIR:=usr/include
LIBC_SOURCEDIR:=usr/src

ifeq ($(KERNEL_ARCH),i386)
	KERNEL_FOLDER:=kernel/kernel-i386
else ifeq ($(KERNEL_ARCH),x64)
	KERNEL_FOLDER:=kernel/kernel-x86_64
else ifeq ($(KERNEL_ARCH),arm)
	KERNEL_FOLDER:=
endif
LIBK_INCLUDEDIR:=$(KERNEL_FOLDER)/libkernel/include
LIBK_SOURCEDIR:=$(KERNEL_FOLDER)/libkernel/src

LIBCList:=stdlib stdio
LIBCHeaders:= sys/cdefs.h string.h ctype.h
LIBKList:=VGA_TTY mem/paging mem/virtual_mem_manager mem/physical_mem_manager
LIBKHeaders:= mem_manager.h kmacro.h


CRTBEGIN_OBJ:=$(shell $(CCPATH)/$(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CCPATH)/$(CC) $(CFLAGS) -print-file-name=crtend.o)

IFLAGS:=$(addprefix -I,$(LIBC_INCLUDEDIR) $(LIBK_INCLUDEDIR))

LIBCFILES:=$(addprefix $(sysroot)/usr/lib/, $(addsuffix .o,$(LIBCList)))
LIBCHeaderFILES:=$(addprefix $(sysroot)/usr/include/, $(LIBCHeaders))
LIBKFILES:=$(addprefix $(sysroot)/lib/kernel/, $(addsuffix .o,$(LIBKList)))
LIBKHeaderFILES:=$(addprefix $(sysroot)/lib/kernel/include/, $(LIBKHeaders))


.PHONY: all

all: kernel libc libk kOS.iso

clean:
	rm -rf build

run: $(buildDIR)/kOS.iso
	qemu-system-i386 -cdrom $(buildDIR)/kOS.iso

kOS.iso: $(buildDIR)/kOS.iso
kernel: $(buildDIR)/kOS.bin
libc: $(LIBCFILES)
libk: $(LIBKFILES) 


# ---------------- iso ------------------------

$(buildDIR)/kOS.iso: $(sysroot)/boot/kOS.bin $(sysroot)/boot/grub/grub.cfg
	grub2-file --is-x86-multiboot $(buildDIR)/kOS.bin && grub2-mkrescue -o $(buildDIR)/kOS.iso $(sysroot)

# ---------------- libc/libk -------------------

$(LIBCFILES): $(sysroot)/usr/lib/%.o: $(LIBC_SOURCEDIR)/%.c $(LIBC_INCLUDEDIR)/%.h $(LIBC_INCLUDEDIR)/**/*.h $(LIBCHeaderFILES)
	@ mkdir -p $(sysroot)/usr/lib/$(*D)
	@ mkdir -p $(sysroot)/usr/src/$(*D)
	@ mkdir -p $(sysroot)/usr/include/$(*D)
	cp $(LIBC_SOURCEDIR)/$*.c $(sysroot)/usr/src/$*.c
	cp $(LIBC_INCLUDEDIR)/$*.h $(sysroot)/usr/include/$*.h
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

$(LIBCHeaderFILES): $(sysroot)/usr/include/%.h: $(LIBC_INCLUDEDIR)/%.h $(LIBC_INCLUDEDIR)/**/*.h 
	@ mkdir -p $(sysroot)/usr/include/$(*D)
	cp $(LIBC_INCLUDEDIR)/$*.h $(sysroot)/usr/include/$*.h

$(LIBKFILES): $(sysroot)/lib/kernel/%.o: $(LIBK_SOURCEDIR)/%.c $(LIBK_INCLUDEDIR)/%.h $(LIBK_INCLUDEDIR)/**/*.h $(LIBKHeaderFILES)
	@ mkdir -p $(sysroot)/lib/kernel/$(*D)
	@ mkdir -p $(sysroot)/lib/kernel/include/$(*D)
	cp $(LIBK_INCLUDEDIR)/$*.h $(sysroot)/lib/kernel/include/$*.h
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@

$(LIBKHeaderFILES): $(sysroot)/lib/kernel/include/%.h: $(LIBK_INCLUDEDIR)/%.h $(LIBK_INCLUDEDIR)/**/*.h
	@ mkdir -p $(sysroot)/lib/kernel/include/$(*D)
	cp $(LIBK_INCLUDEDIR)/$*.h $(sysroot)/lib/kernel/include/$*.h

# --------------- kernel --------------------

CRT_OBJ_DIR:=$(sysroot)/lib/kernel
crt_obj:=$(CRT_OBJ_DIR)/crti.o $(CRTBEGIN_OBJ) $(CRTEND_OBJ) $(CRT_OBJ_DIR)/crtn.o

$(buildDIR)/kOS.bin: $(KERNEL_FOLDER)/linker.ld $(buildDIR)/boot.o $(buildDIR)/kernel.o $(LIBCFILES) $(LIBKFILES)\
 		$(crt_obj)
	$(CC) -T $(KERNEL_FOLDER)/linker.ld -o $(buildDIR)/kOS.bin -ffreestanding -O2 -nostdlib -lgcc\
		$(CRT_OBJ_DIR)/crti.o $(CRTBEGIN_OBJ)\
		$(buildDIR)/boot.o $(buildDIR)/kernel.o $(LIBCFILES) $(LIBKFILES)\
		$(CRTEND_OBJ) $(CRT_OBJ_DIR)/crtn.o

$(CRT_OBJ_DIR)/crti.o: kernel/crti.s $(buildDIR)/boot.o
	@ mkdir -p $(CRT_OBJ_DIR)
	$(ASMC) kernel/crti.s -o $(CRT_OBJ_DIR)/crti.o

$(CRT_OBJ_DIR)/crtn.o: kernel/crtn.s $(buildDIR)/boot.o
	@ mkdir -p $(CRT_OBJ_DIR)
	$(ASMC) kernel/crtn.s -o $(CRT_OBJ_DIR)/crtn.o

$(buildDIR)/boot.o: $(KERNEL_FOLDER)/boot.s
	@ mkdir -p build
	$(ASMC) $(KERNEL_FOLDER)/boot.s -o $(buildDIR)/boot.o

$(buildDIR)/kernel.o: kernel/kernel.c $(LIBC_INCLUDEDIR)/**/*.h $(LIBK_INCLUDEDIR)/**/*.h
	@ mkdir -p build
	$(CC) $(CFLAGS) $(IFLAGS) -c kernel/kernel.c -o $(buildDIR)/kernel.o

# ------------- copy ------------

$(sysroot)/boot/kOS.bin: $(buildDIR)/kOS.bin
	@ mkdir -p $(sysroot)/boot/grub
	cp $(buildDIR)/kOS.bin $(sysroot)/boot/kOS.bin

$(sysroot)/boot/grub/grub.cfg: kernel/grub.cfg
	@ mkdir -p $(sysroot)/boot/grub
	cp kernel/grub.cfg $(sysroot)/boot/grub/grub.cfg

