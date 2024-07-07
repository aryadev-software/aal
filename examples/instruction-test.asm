;;; instruction-test.asm: A file that contains all possible opcodes in
;;; order, with proper calling convention.  Used to test lexer and
;;; parser but isn't a semantically correct program, but may be run as
;;; first instruction is halt (so program will stop immediately).

  ;; setup entrypoint
  global main
main:
  halt
  push.byte 1
  push.hword 2
  push.word 3
  pop.byte
  pop.hword
  pop.word
  push.reg.byte 1
  push.reg.hword 2
  push.reg.word 3
  mov.byte 1
  mov.hword 2
  mov.word 3
  dup.byte 1
  dup.hword 2
  dup.word 3
  malloc.byte
  malloc.hword
  malloc.word
  mset.byte
  mset.hword
  mset.word
  mget.byte
  mget.hword
  mget.word
  not.byte
  not.hword
  not.word
  or.byte
  or.hword
  or.word
  and.byte
  and.hword
  and.word
  xor.byte
  xor.hword
  xor.word
  eq.byte
  eq.hword
  eq.word
  plus.byte
  plus.hword
  plus.word
  sub.byte
  sub.hword
  sub.word
  print.char
  print.byte
  print.int
  print.hword
  print.long
  print.word
  jump.abs 1
  jump.stack
  jump.if.byte 1
  jump.if.hword 2
  jump.if.word 3

  ;; Testing if overflows work correctly
  ;; Format is:
  ;;     -1        All bits are turned on
  ;;     UINT_MAX  All bits are turned on
  ;;     INT_MAX   All bits but the most significant are on
  ;;     INT_MIN   Only the most significant bit is on
  push.byte -1
  push.byte 255
  push.byte 127
  push.byte -128

  push.hword -1
  push.hword 4294967295
  push.hword 2147483647
  push.hword -2147483648

  push.word -1
  push.word 18446744073709551615
  push.word 9223372036854775807
  push.word -9223372036854775808
