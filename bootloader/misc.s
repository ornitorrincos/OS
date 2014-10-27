.intel_syntax

.text
.global BootDisableInterrupts
.global KernelJump

BootDisableInterrupts:
  cli
  ret


.att_syntax

