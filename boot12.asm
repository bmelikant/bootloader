;-----------------------------------------------------------------------------------
; boot12.asm: Boot an operating system from a 3 1/2" floppy disk formatted to FAT12
; Ben Melikant, 11/6/2018 (why on earth am I doing this in 2018???)
;-----------------------------------------------------------------------------------

[org 0x7c00]
[bits 16]

_entryPt:

	jmp short _bootCode       ; jump immediately over the bios parameter block
	nop

;---------------------------------------
; FAT12 BIOS parameter block
;---------------------------------------
_biosParameterBlock:

	_OEM_NAME 				db 'MISSY OS'			; oem name, 8 byte string
	_BYTES_PER_SECTOR		dw 512					; the number of bytes per sector for this floppy
	_SECTORS_PER_CLUSTER	db 1					; the number of sectors per cluster for this floppy
	_RESERVED_SECTORS		dw 1					; the number of reserved sectors including the boot sector
	_FAT_COPIES				db 2					; how many copies of the FAT are present?
	_ROOT_DIRECTORY_ENTRIES	dw 224					; the maximum number of root directory entries
	_TOTAL_SECTORS_SMALL	dw 2880					; the total number of sectors on the disk
	_MEDIA_DESCRIPTOR		db 0xF0					; the type of media (0xF0 is 1.44MB floppy disk)
	_SECTORS_PER_FAT		dw 9					; how many sectors are in each FAT?
	_SECTORS_PER_TRACK		dw 18					; number of sectors on each track
	_NUMBER_OF_HEADS		dw 2					; number of heads on the disk
	_HIDDEN_SECTORS			dd 0					; how many sectors are hidden from the system?
	_TOTAL_SECTORS_LARGE	dd 0					; if _TOTAL_SECTORS_SMALL is zero, this is the total number of sectors

_extendedBiosParameterBlock:

	_DRIVE_NUMBER		db 0
	_RESERVED_FIELD1	db 0
	_BOOT_SIGNATURE		db 0x29
	_VOLUME_ID			dd 0x01020304				; volume id, 4 bytes
	_VOLUME_LABEL		db '           '			; volume label, 11 bytes
	_SYSTEM_ID			db 'FAT12   '

_biosParameterBlockEnd:

;-----------------------------------------------------------
; BIOS parameter block string verification macros
;-----------------------------------------------------------

%if (_BYTES_PER_SECTOR - _OEM_NAME) != 8
%error "Format error: OEM name must be 8 bytes to conform to FAT12 spec"
%endif

%if (_SYSTEM_ID - _VOLUME_LABEL) != 11
%error "Format error: The volume label must be 11 bytes to conform to FAT12 spec"
%endif

%if (_biosParameterBlockEnd - _SYSTEM_ID) != 8
%error "Format error: The system id field must be 8 bytes to conform to FAT12 spec"
%endif

_bootCode:

	; set up the segment registers to point to segment 0x00
	xor ax,ax
	mov ds,ax
	mov es,ax

	mov ss,ax
	mov sp,0x9000       ; locate the stack above boot code

_loadOsImage:

	; start loading the file stage2.sys
	mov si,_searchingFileStr
	call _bootldrPrintLine

;-----------------------------------------------------
; loading and printing routines are below this line
;-----------------------------------------------------

;-------------------------------------------------------------------------------------------
; @fn _bootldrPrintLine: this function will print a line followed by a carriage return
; using the BIOS interrupts
;
; @param reg SI contains a pointer to the C-style (null-terminated) string to display
; 
; @note this function is NOT register safe! Registers will not be preserved through calls
;-------------------------------------------------------------------------------------------
_bootldrPrintLine:

	mov ah,0x0e     	; interrupt 0x10, function 0x0a print single character
	mov cx,0x01     	; print a single character at a time

.printLoop:

	lodsb               ; load a byte from the string
	or al,al            ; compare it to zero
	jz .printDone       ; finish if we hit the null terminator

	int 0x10            ; call the interrupt, duh!
	jmp .printLoop      ; and print the next one!

.printDone:

	mov al,0x0a			; carriage return
	int 0x10			; print it
	ret					; return to caller

;--------------------------------------------------------------------------------------------
; @fn _lbaToChs: this function converts values in linear block addressing (LBA) format to
; cylinder-head-sector (CHS) values.
;--------------------------------------------------------------------------------------------
;--------------------------------------------------------
; data section: strings, filenames, scratch area, etc.
;--------------------------------------------------------

_searchingFileStr	db 'Looking for stage2.sys...',0
_fileFoundStr		db 'Found stage2.sys, booting...',0
_imageFileNameStr 	db 'STAGE2  SYS'

; zero fill and boot signature
times 510-($-$$) db 0
dw 0xaa55