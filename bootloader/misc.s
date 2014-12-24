.text
.global BootDisableInterrupts
.global GetVMCPUID

BootDisableInterrupts:
  pushq %rbp
  movq %rsp, %rbp
  cli
  popq %rbp
  ret

GetVMCPUID:
  pushq %rbp
  movq %rsp, %rbp
  movq 0x80000008, %rax
  cpuid
  popq %rbp
  ret

