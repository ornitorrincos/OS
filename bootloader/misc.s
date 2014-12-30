.text
.global BootDisableInterrupts

BootDisableInterrupts:
  pushq %rbp
  movq %rsp, %rbp
  cli
  popq %rbp
  ret

