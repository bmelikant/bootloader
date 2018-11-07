;-----------------------------------------------------------------------------------
; boot12.asm: Boot an operating system from a 3 1/2" floppy disk formatted to FAT12
; Ben Melikant, 11/6/2018 (why on earth am I doing this in 2018???)
;-----------------------------------------------------------------------------------

[org 0x7c00]
[bits 16]

_entryPt:

	jmp short _bootCode       ; jump immediately over the bios parameter block
	nop

%include "fat.inc"

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

; included routines
%include "print.inc"

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