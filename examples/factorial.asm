;;; factorial.asm: A program that generates the factorials of each
;;;  number from 1 to 20.  Using the registers to store `n` and `n!`.

  ;; Constants
  ;; Choice of 20 was not arbitrary; log(20!) ~= 61 while log(21!) ~=
  ;; 65 which means that past 20! results are truncated and therefore
  ;; the program produces inaccurate factorials.

  %const limit 20 %end

  ;; Setup entrypoint
  global main
main:
  ;; $I -> W[0] = 1, $J -> W[1] = 1
  push.word 1
  mov.word 0
  push.word 1
  mov.word 1

  ;; Print `$I: $J`
loopback:
  push.byte '\t'
  print.char
  push.reg.word 0
  print.word
  push.byte ':'
  print.char
  push.byte ' '
  print.char
  push.reg.word 1
  print.word
  push.byte '\n'
  print.char

  ;;  $I += 1
  push.reg.word 0
  push.word 1
  plus.word
  mov.word 0

  ;;  $J *= $I
  push.reg.word 0
  push.reg.word 1
  mult.word
  mov.word 1

  ;; IF $I >= $LIMIT ...
  push.word $limit
  push.reg.word 0
  gte.word
  ;; THEN jump to `loopback`
  jump.if.byte loopback
  ;; ELSE halt
  halt
