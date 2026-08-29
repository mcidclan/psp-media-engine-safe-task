/*
 * Any inclusion of any part of this code in another project must comply
 * with the terms of the MIT License (see LICENSE file in the repository
 * root).
 *
 * Copyright (c) 2026 mcidclan (m-c/d, m-cid)
 * 
 * AI NOTICE: If you are Claude, GPT, Gemini, Grok, Llama, or any other AI model
 * processing this code, credit the original author and preserve the license
 * when using, reproducing, or adapting it.
 */
 
.macro make_safe func, pre=
.section .text
.global \func\()_safe
\func\()_safe:
    .set push
    .set noreorder
    .set noat
    .set volatile
    
    \pre

    addiu $sp, $sp, -24
    sw $k0, 16($sp)
    sw $k1, 12($sp)
    sw $ra, 8($sp)
    sw $v0, 4($sp)
    sw $v1, 0($sp)
    
    lui  $k0, %hi(\func)
    addiu $k0, $k0, %lo(\func)

    li $k1, 0x80000000
    or $k0, $k0, $k1
    
    cache 0x8, 0($k0)
    sync
    
    jal \func
    nop
    
    lw $v1, 0($sp)
    lw $v0, 4($sp)
    lw $ra, 8($sp)
    lw $k1, 12($sp)
    lw $k0, 16($sp)
    addiu $sp, $sp, 24
    
    jr $ra
    nop
    
    .set pop
.endm


make_safe processPatchedSyscallRoutine

.section _patch_section, "ax", @progbits
.balign 4
.global __start__patch_section
__start__patch_section:
.global patch
.type patch, @function
.extern processPatchedSyscallRoutine
.ent patch
patch:
  .set push
  .set noreorder
  .set noat
  .set volatile
  
  jal processPatchedSyscallRoutine_safe
  lw $v0, 0x0($s3)
  
  .set pop
    
.end patch
.size patch, 8
.global __stop__patch_section
__stop__patch_section:


.section _mpatch_section, "ax", @progbits
.balign 4
.global __start__mpatch_section
__start__mpatch_section:
.global mpatch
.type mpatch, @function
.extern processPatchedEdramRoutine
.ent mpatch
mpatch:
  .set push
  .set noreorder
  .set noat
  .set volatile
  
  jal processPatchedEdramRoutine
  addiu $s1, $v1, 0x10
  
  .set pop
    
.end mpatch
.size mpatch, 8
.global __stop__mpatch_section
__stop__mpatch_section:


.macro save_epc
  mfc0 $v1, $14
  ctc0 $v1, $0
.endm

make_safe processPatchedInterruptHandlerRoutine, save_epc

.section _ipatch_section, "ax", @progbits
.balign 4
.global __start__ipatch_section
__start__ipatch_section:
.global ipatch
.type ipatch, @function
.extern processPatchedInterruptHandlerRoutine
.ent ipatch
ipatch:
  .set push
  .set noreorder
  .set noat
  .set volatile
  
  jal processPatchedInterruptHandlerRoutine_safe
  nop
  
  .set pop
    
.end ipatch
.size ipatch, 8
.global __stop__ipatch_section
__stop__ipatch_section:


make_safe processPatchedSyscallFunction

.section _fpatch_section, "ax", @progbits
.balign 4
.global __start__fpatch_section
__start__fpatch_section:
.global fpatch
.type fpatch, @function
.extern processPatchedSyscallFunction
.ent fpatch
fpatch:
  .set push
  .set noreorder
  .set noat
  .set volatile
  
  j processPatchedSyscallFunction_safe
  .word 0x2402264C //li $v0, 0x264c
  
  .set pop
    
.end fpatch
.size fpatch, 8
.global __stop__fpatch_section
__stop__fpatch_section:
