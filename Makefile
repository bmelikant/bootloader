ASSEMBLER:=nasm

# stage one (boot) definitions
FILESYSTEM:=fat12
ASM_FLAGS:=-dFILESYSTEM=$(FILESYSTEM) -Iboot/include/ -Istage2/include/
ASM_STAGE2_FLAGS:=-Iboot/include/ -Istage2/include/

# bootloader target options
BTLDR_TARGET:=boot.bin
BTLDR_SRC=boot/boot.asm

BTLDR_HEADERS:=\
boot/include/disk.inc \
boot/include/fat.inc \
boot/include/fat12.inc \
boot/include/print.inc

# stage2 target options
STAGE2_TARGET:=stage2.sys
STAGE2_SRC=stage2/stage2.asm

# for running image target
DD:=dd
DEV_ZERO:=/dev/zero
FLOPPY_IMG:=boot.img
LOSETUP:=losetup
MOUNT=mount
UMOUNT=umount

# for running make bochs target
BOCHS:=bochs
A_DRIVE:='floppya: 1_44=$(FLOPPY_IMG),status=inserted'

all: $(BTLDR_TARGET) $(BTLDR_HEADERS)

clean:
	rm -f $(BTLDR_TARGET)
	rm -f $(FLOPPY_IMG)

bochs_floppy: floppy_image
	$(BOCHS) $(A_DRIVE)

floppy_image: $(BTLDR_TARGET) $(STAGE2_TARGET)
	$(DD) if=$(DEV_ZERO) of=$(FLOPPY_IMG) bs=512 count=2880
	$(DD) if=$(BTLDR_TARGET) of=$(FLOPPY_IMG) bs=512 conv=notrunc
	$(LOSETUP) /dev/loop0 $(FLOPPY_IMG)
	$(MOUNT) /dev/loop0 /mnt
	cp $(STAGE2_TARGET) /mnt
	$(UMOUNT) /mnt
	$(LOSETUP) -d /dev/loop0

$(BTLDR_TARGET): $(BTLDR_SRC)
	$(ASSEMBLER) $< -o $(BTLDR_TARGET) $(ASM_FLAGS)

$(STAGE2_TARGET): $(STAGE2_SRC)
	$(ASSEMBLER) $< -o $(STAGE2_TARGET) $(ASM_STAGE2_FLAGS)