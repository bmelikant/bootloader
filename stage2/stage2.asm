[org 0x0]
[bits 16]

_entryPt:

    mov ax,0x0050
    mov ds,ax
    mov es,ax

    mov si,_testStr
    call _bootldrPrintLine

    cli
    jmp $

%include "print.inc"

_testStr db 'Hello, Stage 2 World!',0