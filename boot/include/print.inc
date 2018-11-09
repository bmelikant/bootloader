%ifndef _PRINT_INC
%define _PRINT_INC

;--------------------------------------------------------------------------------------------------------
; @file print.inc: this file defines a simple function to print early strings to the teletype display
; It calls the BIOS from real mode to do it's work
;--------------------------------------------------------------------------------------------------------

; this directive should be overridden after this file if 32 bit code is needed
[bits 16]

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
	mov al,0x0d
	int 0x10
	ret					; return to caller

; _PRINT_INC
%endif