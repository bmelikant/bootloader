;------------------------------------------------------------------------------------------------------------
; @file fat.inc: This file defines the FAT12 BIOS parameter block. This BPB is also the base block for
; FAT16 and FAT32 file systems.
;------------------------------------------------------------------------------------------------------------

%ifndef _FAT_INC
%define _FAT_INC

; if it isn't already defined, define the segment and offset for the OS
%ifndef _OS_LOAD_SEGMENT
%define _OS_LOAD_SEGMENT 0x0050
%endif

%ifndef _OS_LOAD_OFFSET
%define _OS_LOAD_OFFSET 0x0000
%endif

;----------------------------
; FAT interaction routines
;----------------------------

%if (FILESYSTEM_TYPE == FAT12)

;-----------------------------------------------------------------------
; @fn _clusterToLBA: Convert a cluster value to a linear block address
;-----------------------------------------------------------------------
_clusterToLBA:

	sub ax,0x02								; make the cluster zero-based
	xor cx,cx								; clear cx for operations
	mov cl,byte [_BPB_SECTORS_PER_CLUSTER]	; store the sectors per cluster in cl
	mul cx									; multiply the cluster number by the sectors per cluster
	add ax,word [_firstDataSector]			; and add the starting sector offset
	ret

;----------------------------------------------------------------------------------
; @fn _fetchRootDirectory: this function is used to read the FAT12 root directory
; ES:BX -> data buffer for root directory sector
;----------------------------------------------------------------------------------
_fetchRootDirectory:

	; compute the size of the root directory and store in cx
	xor cx,cx
	xor dx,dx
	mov ax,_ROOT_ENTRY_SIZE
	mul word [_BPB_ROOT_DIRECTORY_ENTRIES]
	div word [_BPB_BYTES_PER_SECTOR]
	xchg ax,cx

	; compute the location of the root directory
	mov al,byte [_BPB_FAT_COPIES]
	mul word [_BPB_SECTORS_PER_FAT]
	add ax,word [_BPB_RESERVED_SECTORS]

	; store the end of the root directory
	mov word [_firstDataSector],ax
	add word [_firstDataSector],cx

	call _readSectors
	ret

;----------------------------------------------------------------------------------
; @fn _parseRootDirectory: this function will parse the root directory for the
; file by the name indicated in DS:SI. The filename must be in MS-DOS 8.3 format
; DS:SI -> file name pointer
; ES:DI -> root directory structure
; CX -> root directory size
;----------------------------------------------------------------------------------
_parseRootDirectory:

	mov cx,word [_BPB_ROOT_DIRECTORY_ENTRIES]

.parseLoop:

	; save important registers
	push si
	push di
	push cx

	; search for the filename
	mov cx,_FILENAME_LEN
	rep cmpsb

	; restore the registers before the next attempt
	pop cx
	pop di
	pop si

	je .found		; filename was found!

	; check the next entry while there are entries left
	add di,_ROOT_ENTRY_SIZE
	loop .parseLoop

	; no more entries; go to error condition return
	stc
	jmp .done

.found:

	clc								; the file was found, clear the error status
	mov dx,word [di+0x1a]			; this is our starting sector
	mov word [_currentCluster],dx	; store the starting sector

.done:

	ret		; return to caller

;--------------------------------------------------------------
; @fn _readStage2Image: Read the second stage into memory
; This function expects the following conditions:
; ES:BX => destination buffer for the FAT
; _currentCluster => starting cluster number of the file
;--------------------------------------------------------------
_readStage2Image:

	; compute the size of the FAT
%ifdef _LOAD_BOTH_FAT_COPIES
	xor ax,ax
	mov al,byte [_BPB_FAT_COPIES]
	mul word [_BPB_SECTORS_PER_FAT]
	xchg cx,ax
%else
	mov cx,word [_BPB_SECTORS_PER_FAT]
%endif

	; compute the location of the FAT (just above the reserved sectors)
	mov ax,word [_BPB_RESERVED_SECTORS]

	; read the FAT into memory at ES:BX
	mov word [_fatBuffer],bx
	call _readSectors

	; set up ES and BX to receive the image file
	mov ax,_OS_LOAD_SEGMENT
	mov es,ax
	mov bx,_OS_LOAD_OFFSET
	push bx

.readFileLoop:

	mov ax,word [_currentCluster]
	pop bx								; restore our buffer
	call _clusterToLBA

	xor cx,cx
	mov cl,byte [_BPB_SECTORS_PER_CLUSTER]
	call _readSectors
	jc .done
	push bx

	; this is the part where we find the next cluster!
	mov ax,word [_currentCluster]
	mov cx,ax
	mov dx,ax
	shr dx,0x0001
	add cx,dx
	mov bx,[_fatBuffer]
	add bx,cx
	mov dx,word [bx]
	test ax,0x0001
	jz .evenCluster

.oddCluster:

	shr dx,0x04
	jmp .clusterDone

.evenCluster:

	and dx,0x0fff

.clusterDone:

	mov word [_currentCluster],dx
	cmp dx,0xff8
	jb .readFileLoop
	clc

.done:

	ret

;-----------------------------------------------------------------------------------------
; current sector to read. Used to determine where we are on the disk at any given time
;-----------------------------------------------------------------------------------------
_fatBuffer			dw 0x00
_firstDataSector	dw 0x00
_currentCluster 	dw 0x00

%endif

%endif