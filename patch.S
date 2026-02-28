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
    
    jal processPatchedSyscallRoutine
    lw $v0, 0x0($s3)
    
    #nop
    
    #mtic $0, $0
    #cfc0 $a1, $11
    
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
.type patch, @function
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
