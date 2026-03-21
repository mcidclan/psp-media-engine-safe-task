.macro make_safe func, pre=
.section .text
.global \func\()_safe
\func\()_safe:
    .set push
    .set noreorder
    .set noat
    .set volatile
    
    \pre

    addiu $sp, $sp, -12
    sw $ra, 8($sp)
    sw $v0, 4($sp)
    sw $v1, 0($sp)
    
    jal \func
    nop
    
    lw $v1, 0($sp)
    lw $v0, 4($sp)
    lw $ra, 8($sp)
    addiu $sp, $sp, 12
    
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
