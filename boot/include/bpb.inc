%ifndef _BPB_INC
%define _BPB_INC

%define FAT12 1
%define FAT16 2
%define FAT32 3

%define _ROOT_ENTRY_SIZE 	32
%define _FILENAME_LEN		11

; default file system for build is FAT12
%ifndef FILESYSTEM_TYPE
%define FILESYSTEM_TYPE FAT12
%endif

; define the OEM name of the operating system (currently MISSY OS, can be overridden)
%ifndef _OEM_NAME
%define _OEM_NAME 'MISSY OS'
%endif

%if (FILESYSTEM_TYPE == FAT12)
%include "fat12.inc"
%endif

;-----------------------------------------------------------------
; FAT12 BIOS parameter block and base block for FAT16 / FAT32
;-----------------------------------------------------------------
_biosParameterBlock:

	_BPB_OEM_NAME 				db _OEM_NAME				; oem name, 8 byte string
	_BPB_BYTES_PER_SECTOR		dw _BYTES_PER_SECTOR		; the number of bytes per sector for this floppy
	_BPB_SECTORS_PER_CLUSTER	db _SECTORS_PER_CLUSTER		; the number of sectors per cluster for this floppy
	_BPB_RESERVED_SECTORS		dw _RESERVED_SECTORS		; the number of reserved sectors including the boot sector
	_BPB_FAT_COPIES				db _FAT_COPIES				; how many copies of the FAT are present?
	_BPB_ROOT_DIRECTORY_ENTRIES	dw _ROOT_ENTRIES			; the maximum number of root directory entries
	_BPB_TOTAL_SECTORS_SMALL	dw _TOTAL_SECTORS_SMALL		; the total number of sectors on the disk
	_BPB_MEDIA_DESCRIPTOR		db _MEDIA_DESCRIPTOR		; the type of media (0xF0 is 1.44MB floppy disk)
	_BPB_SECTORS_PER_FAT		dw _SECTORS_PER_FAT			; how many sectors are in each FAT?
	_BPB_SECTORS_PER_TRACK		dw _SECTORS_PER_TRACK		; number of sectors on each track
	_BPB_NUMBER_OF_HEADS		dw _NUMBER_OF_HEADS			; number of heads on the disk
	_BPB_HIDDEN_SECTORS			dd _HIDDEN_SECTORS			; how many sectors are hidden from the system?
	_BPB_TOTAL_SECTORS_LARGE	dd _TOTAL_SECTORS_LARGE		; if _TOTAL_SECTORS_SMALL is zero, this is the total number of sectors

_extendedBiosParameterBlock:

	_BPB_DRIVE_NUMBER		db _DRIVE_NUMBER
	_BPB_RESERVED_FIELD1	db 0
	_BPB_BOOT_SIGNATURE		db _BOOT_SIGNATURE
	_BPB_VOLUME_ID			dd _VOLUME_ID				; volume id, 4 bytes
	_BPB_VOLUME_LABEL		db _VOLUME_LABEL			; volume label, 11 bytes
	_BPB_SYSTEM_ID			db _SYSTEM_ID

_biosParameterBlockFAT12End:

;-----------------------------------------------------------
; BIOS parameter block string verification macros
;-----------------------------------------------------------

%if (_BPB_BYTES_PER_SECTOR - _BPB_OEM_NAME) != 8
%error "Format error: OEM name must be 8 bytes to conform to FAT12 spec"
%endif

%if (_BPB_SYSTEM_ID - _BPB_VOLUME_LABEL) != 11
%error "Format error: The volume label must be 11 bytes to conform to FAT12 spec"
%endif

%if (_biosParameterBlockFAT12End - _BPB_SYSTEM_ID) != 8
%error "Format error: The system id field must be 8 bytes to conform to FAT12 spec"
%endif

%endif