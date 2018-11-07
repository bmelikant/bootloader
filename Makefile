ASSEMBLER:=nasm

# stage one (boot) definitions
FILESYSTEM:=fat12
ASM_FLAGS:=-dFILESYSTEM=$(FILESYSTEM) -Iboot/include/

# bootloader target options
BTLDR_TARGET:=boot.bin
BTLDR_SRC=boot/boot.asm

# for running image target
DD:=dd
DEV_ZERO:=/dev/zero
FLOPPY_IMG:=boot.img

# for running make bochs target
BOCHS:=bochs
A_DRIVE:='floppya: 1_44=$(FLOPPY_IMG),status=inserted'

all: $(BTLDR_TARGET)

clean:
	rm -f $(BTLDR_TARGET)
	rm -f $(FLOPPY_IMG)

bochs_floppy: floppy_image
	$(BOCHS) $(A_DRIVE)

floppy_image: $(BTLDR_TARGET)
	$(DD) if=$(DEV_ZERO) of=$(FLOPPY_IMG) bs=512 count=2880
	$(DD) if=$(BTLDR_TARGET) of=$(FLOPPY_IMG) bs=512 conv=notrunc

$(BTLDR_TARGET): $(BTLDR_SRC)
	$(ASSEMBLER) $< -o $(BTLDR_TARGET) $(ASM_FLAGS)