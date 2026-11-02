/*
 * PROJECT:     LiberNT NT Layer DLL
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     AMD64 Control Flow Guard check and dispatch targets
 * COPYRIGHT:   Copyright 2026 NiSeullent <coolfriendsbz@gmail.com>
 */

#include <asm.inc>
#include <ksamd64.inc>

EXTERN LdrpValidateUserCallTarget:PROC

.code

PUBLIC LdrpGuardCheckIcall
.PROC LdrpGuardCheckIcall
    sub rsp, HEX(B8)
    .allocstack HEX(B8)
    .endprolog

    mov [rsp + HEX(80)], rax
    mov [rsp + HEX(88)], rcx
    mov [rsp + HEX(90)], rdx
    mov [rsp + HEX(98)], r8
    mov [rsp + HEX(A0)], r9
    mov [rsp + HEX(A8)], r10
    mov [rsp + HEX(B0)], r11
    movaps [rsp + HEX(20)], xmm0
    movaps [rsp + HEX(30)], xmm1
    movaps [rsp + HEX(40)], xmm2
    movaps [rsp + HEX(50)], xmm3
    movaps [rsp + HEX(60)], xmm4
    movaps [rsp + HEX(70)], xmm5

    call LdrpValidateUserCallTarget
    test al, al
    jnz LdrpGuardCheckIcallValid
    mov ecx, FAST_FAIL_GUARD_ICALL_CHECK_FAILURE
    int HEX(29)

LdrpGuardCheckIcallValid:
    movaps xmm0, [rsp + HEX(20)]
    movaps xmm1, [rsp + HEX(30)]
    movaps xmm2, [rsp + HEX(40)]
    movaps xmm3, [rsp + HEX(50)]
    movaps xmm4, [rsp + HEX(60)]
    movaps xmm5, [rsp + HEX(70)]
    mov rax, [rsp + HEX(80)]
    mov rcx, [rsp + HEX(88)]
    mov rdx, [rsp + HEX(90)]
    mov r8, [rsp + HEX(98)]
    mov r9, [rsp + HEX(A0)]
    mov r10, [rsp + HEX(A8)]
    mov r11, [rsp + HEX(B0)]
    add rsp, HEX(B8)
    ret
.ENDP

PUBLIC LdrpGuardDispatchIcall
.PROC LdrpGuardDispatchIcall
    push rcx
    .pushreg rcx
    .endprolog

    mov rcx, rax
    call LdrpGuardCheckIcall
    pop rcx
    jmp rax
.ENDP

END
