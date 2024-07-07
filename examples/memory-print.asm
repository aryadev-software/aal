;;; memory-print: An example program that features a subroutine for
;;; printing a memory buffer, of any length, as characters.

  ;; Setup label for entrypoint
  global main
main:
  ;; Allocate a buffer of 3 characters
  push.word 3
  malloc.byte
  mov.word 0
  ;; Setup the buffer to be equivalent to "abc"
  push.reg.word 0
  push.byte 'a'
  push.word 0
  ;; mset 0 'a'
  mset.byte
  push.reg.word 0
  push.byte 'b'
  push.word 1
  ;; mset 1 'b'
  mset.byte
  push.reg.word 0
  push.byte 'c'
  push.word 2
  ;; mset 2 'c'
  mset.byte

  ;; Save buffer to W[8] because the first 8 registers should be
  ;; reserved for library routines as it may be overwritten
  push.reg.word 0
  mov.word 8
  ;; Call the routine
  call print_cptr

  ;; Delete allocated buffer
  push.reg.word 8
  mdelete

  halt

;;; print_cptr: Prints pointer to a buffer of characters.  Pointer
;;; should be on the stack as a word.
print_cptr:
  ;; iterator I -> W[1]
  push.word 0
  mov.word 1
  ;; (W[0])[W[1]] -> P[I]
loopback:
  push.reg.word 0
  push.reg.word 1
  mget.byte
  print.char

  ;; I += 1
  push.reg.word 1
  push.word 1
  plus.word
  mov.word 1

  ;; if I != |P| ...
  push.reg.word 1
  push.reg.word 0
  msize
  eq.word
  not.byte
  ;; then go to `loopback`
  jump.if.byte loopback
  ;; else print a newline
  push.byte '\n'
  print.char
  ;; return back to the caller
  ret
