%ifndef _DISK_INC
%define _DISK_INC

%include "print.inc"
%include "fat.inc"

;--------------------------------------------------------------------
; @file disk.inc: Disk interaction utilities for FAT bootloaders
;--------------------------------------------------------------------

%ifndef _USE_INT13_EXTENSIONS

;----------------------------------------------------------------------------------
; @fn _lbaToChs: Convert a linear base addres to cylinder-head-sector notation
; AX should contain the LBA of the current sector. Registers are not preserved
; through calls, so they should be saved beforehand if needed
; This code was taken from Brokenthorn's OS development series
;----------------------------------------------------------------------------------
_lbaToChs:

	; clear dx for operations
	xor dx,dx

	; first calcuation is sector: sector = (lba % SECTORS_PER_TRACK + 1)
	div word [_BPB_SECTORS_PER_TRACK]		; divide the LBA by SECTORS PER TRACK value
	inc dl									; increment the modulus of division stored in dl
	mov byte [_sector],dl					; and store the result in memory

	; clear dx again
	xor dx,dx

	; second calculation is head: head = (lba % (SECTORS_PER_TRACK * 2)) / SECTORS_PER_TRACK
	div word [_BPB_NUMBER_OF_HEADS]
	mov byte [_head],dl

	; we already have the sector value from the above calculation: track = (lba / (SECTORS_PER_TRACK * 2))
	mov byte [_cylinder],al
	ret

;------------------------------------------------------------------------------
; @fn _readSectors: read a series of sectors from the floppy disk
; CX should contain the number of sectors to read, AX is the starting sector
; The buffer for the read sector is ES:BX
; If the carry flag is set at the end of this operation, an error has
; occurred
;------------------------------------------------------------------------------
_readSectors:

	mov di,0x5			; we will retry five times on error

.sectorLoop:

	; we need to preserve these between reads
	push ax
	push bx
	push cx

	call _lbaToChs
	mov ah,0x02
	mov al,0x01				; ah = bios function 2, al = read single sector
	mov ch,byte [_cylinder]
	mov cl,byte [_sector]
	mov dh,byte [_head]
	mov dl,byte [_BPB_DRIVE_NUMBER]
	int 0x13

	jnc .readSuccess

	; try to reset the disk for retry
	xor ax,ax
	int 0x13
	dec di

	; restore for the next attempt
	pop cx
	pop bx
	pop ax

	jnz .sectorLoop
	stc
	jmp .readComplete

.readSuccess:

%ifdef _PRINT_LOADING_PROGRESS
	mov si,_loadProgressString
	call _bootldrPrintLine
%endif

	; restore to move on to the next sector
	pop cx
	pop bx
	pop ax

	add bx,word [_BPB_BYTES_PER_SECTOR]
	inc ax
	loop _readSectors
	clc

.readComplete:

	ret
	
;----------------------------------------
; data section for floppy operations
;----------------------------------------

_cylinder   	db 0x00
_head       	db 0x00
_sector     	db 0x00
_currentLBA		dw 0x00

%ifdef _PRINT_LOADING_PROGRESS
_loadProgressString db '.',0
%endif

; !(_USE_INT13_EXTENSIONS)
%endif

; _DISK_INC
%endif