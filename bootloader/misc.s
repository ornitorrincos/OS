.intel_syntax

.text
.global BootDisableInterrupts

BootDisableInterrupts:
  cli
  ret

.att_syntax
