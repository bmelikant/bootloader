;-------------------------------------------------------------------------------------------
; @file fat12.inc: Defines the default BIOS parameter block settings for a FAT12 diskette
;-------------------------------------------------------------------------------------------

; define the defaults for the FAT12 bios parameter block
%if (FILESYSTEM_TYPE == FAT12)

%ifdef _USE_INT13_EXTENSIONS
%undef _USE_INT13_EXTENSIONS
%endif

; these properties are always for FAT12
%define _BYTES_PER_SECTOR 512
%define _SECTORS_PER_CLUSTER 1
%define _FAT_COPIES 2
%define _ROOT_ENTRIES 224
%define _TOTAL_SECTORS_SMALL 2880
%define _MEDIA_DESCRIPTOR 0xF0
%define _SECTORS_PER_FAT 9
%define _SECTORS_PER_TRACK 18
%define _NUMBER_OF_HEADS 2
%define _HIDDEN_SECTORS 0
%define _TOTAL_SECTORS_LARGE 0
%define _SYSTEM_ID 'FAT12   '

; these are changeable via configuration in boot.asm or command line
%ifndef _RESERVED_SECTORS
%define _RESERVED_SECTORS 1
%endif
%ifndef _DRIVE_NUMBER
%define _DRIVE_NUMBER 0
%endif
%ifndef _BOOT_SIGNATURE
%define _BOOT_SIGNATURE 0x29
%endif
%ifndef _VOLUME_ID
%define _VOLUME_ID 0x12345678
%endif
%ifndef _VOLUME_LABEL
%define _VOLUME_LABEL 'MISS-FLOPPY'
%endif
%endif