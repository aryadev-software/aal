;;; fib.asm: A program that generates the fibonacci numbers up to a
;;; very large bound (~UINT64_MAX).  Using the registers to store the
;;; pairs of fibonacci numbers, we ensure only a finite amount of
;;; memory is necessary for this program to function, unlike a pure
;;; stack version.

  ;; Constants
  %const limit 93 %end

  %const increment_i
  push.reg.word 2
  push.word 1
  plus.word
  mov.word 2
  %end

  %const print_i
  push.reg.word 2
  print.word
  %end

  %const print_reg_0
  push.reg.word 0
  print.word
  %end

  %const print_reg_1
  push.reg.word 1
  print.word
  %end

  ;; Setup entrypoint
  global main
main:
  ;; Setup iterator I
  push.word 1
  mov.word 2
  ;; Setup initial A -> W[0] = 1 and B -> W[1] = 1
  push.word 1
  mov.word 0
  push.word 1
  mov.word 1

  ;; Print "$I: $A" and "($I + 1): $B"
loopback:
  call print_pair

  ;;  $A += $B
  push.reg.word 0
  push.reg.word 1
  plus.word
  mov.word 0

  ;;  $B += $A
  push.reg.word 0
  push.reg.word 1
  plus.word
  mov.word 1

  ;; IF $I < $LIMIT ...
  push.reg.word 2
  push.word $limit
  lt.word
  ;; THEN jump to `loopback`
  jump.if.byte loopback
  ;; ELSE halt
  halt

print_pair:
  push.byte '\t'
  print.char
  $print_i
  push.byte ':'
  print.char
  push.byte ' '
  print.char
  $print_reg_0
  push.byte '\n'
  print.char
  $increment_i
  push.byte '\t'
  print.char
  $print_i
  push.byte ':'
  print.char
  push.byte ' '
  print.char
  $print_reg_1
  push.byte '\n'
  print.char
  $increment_i
  ret
