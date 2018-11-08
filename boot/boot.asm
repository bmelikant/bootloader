;-----------------------------------------------------------------------------------
; boot12.asm: Boot an operating system from a 3 1/2" floppy disk formatted to FAT12
; Ben Melikant, 11/6/2018 (why on earth am I doing this in 2018???)
;-----------------------------------------------------------------------------------

%define _DISK_BUFFER 0x200
%define _PRINT_LOADING_PROGRESS

[org 0x0]
[bits 16]

_entryPt:

	jmp short _bootCode       ; jump immediately over the bios parameter block
	nop

; this is where the BIOS parameter block gets loaded in
%include "fat.inc"

_bootCode:

	; set up the segment registers to point to segment 0x07c0
	cli
	mov ax,0x07c0
	mov ds,ax
	mov es,ax

	xor ax,ax
	mov ss,ax
	mov sp,0x9000       ; locate the stack above boot code
	sti

_loadOsImage:

	; start loading the file stage2.sys
	mov si,_searchingFileStr
	call _bootldrPrintLine

	; read and parse the root directory. If CF=1 after any of these calls, jump to error
	mov bx,_DISK_BUFFER
	call _fetchRootDirectory
	jc .readError

	mov si,_imageFileNameStr
	mov di,_DISK_BUFFER
	call _parseRootDirectory
	jc .findError

	; for now just print the success status
	mov si,_fileFoundStr
	call _bootldrPrintLine
	jmp .halt

.readError:

	mov si,_diskReadErrorStr
	call _bootldrPrintLine
	jmp .halt

.findError:

	mov si,_rootDirSearchFailStr
	call _bootldrPrintLine

.halt:

	cli
	jmp $

;------------------------------
; included function files
;------------------------------

%include "print.inc"
%include "disk.inc"

;--------------------------------------------------------------------------------------------
; @fn _lbaToChs: this function converts values in linear block addressing (LBA) format to
; cylinder-head-sector (CHS) values.
;--------------------------------------------------------------------------------------------
;--------------------------------------------------------
; data section: strings, filenames, scratch area, etc.
;--------------------------------------------------------

_searchingFileStr		db 'FIND STAGE2.SYS',0
_fileFoundStr			db 'STAGE2 FOUND',0
_diskReadErrorStr		db 'DISK ERR',0
_rootDirSearchFailStr 	db 'NOT FOUND',0

_imageFileNameStr 	db 'STAGE2  SYS'

; zero fill and boot signature
times 510-($-$$) db 0
dw 0xaa55