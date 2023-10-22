/* Copyright (C) 2023 Aryadev Chavali

 * You may distribute and modify this code under the terms of the
 * GPLv2 license.  You should have received a copy of the GPLv2
 * license with this file.  If not, please write to:
 * aryadev@aryadevchavali.com.

 * Created: 2023-10-15
 * Author: Aryadev Chavali
 * Description: Instructions and opcodes
 */

#ifndef INST_H
#define INST_H

#include <stdio.h>
#include <stdlib.h>

#include "./base.h"
#include "./darr.h"

typedef enum
{
  OP_NOOP = 0,

  // Dealing with data and registers
  OP_PUSH_BYTE,
  OP_PUSH_HWORD,
  OP_PUSH_WORD,

  OP_POP_BYTE,
  OP_POP_HWORD,
  OP_POP_WORD,

  OP_PUSH_REGISTER_BYTE,
  OP_PUSH_REGISTER_HWORD,
  OP_PUSH_REGISTER_WORD,

  OP_MOV_BYTE,
  OP_MOV_HWORD,
  OP_MOV_WORD,

  OP_DUP_BYTE,
  OP_DUP_HWORD,
  OP_DUP_WORD,

  // Boolean operations
  OP_NOT_BYTE,
  OP_NOT_HWORD,
  OP_NOT_WORD,

  OP_OR_BYTE,
  OP_OR_HWORD,
  OP_OR_WORD,

  OP_AND_BYTE,
  OP_AND_HWORD,
  OP_AND_WORD,

  OP_XOR_BYTE,
  OP_XOR_HWORD,
  OP_XOR_WORD,

  OP_EQ_BYTE,
  OP_EQ_HWORD,
  OP_EQ_WORD,

  // Mathematical operations
  OP_PLUS_BYTE,
  OP_PLUS_HWORD,
  OP_PLUS_WORD,

  // Simple I/O
  OP_PRINT_CHAR,
  OP_PRINT_BYTE,
  OP_PRINT_INT,
  OP_PRINT_HWORD,
  OP_PRINT_LONG,
  OP_PRINT_WORD,

  // Program control flow
  OP_JUMP_ABS,
  OP_JUMP_STACK,
  OP_JUMP_REGISTER,

  // Should not be an opcode
  NUMBER_OF_OPCODES,
  OP_HALT = 0b11111111, // top of the byte is a HALT
} opcode_t;

const char *opcode_as_cstr(opcode_t);

#define OPCODE_IS_TYPE(OPCODE, OP_TYPE) \
  (((OPCODE) >= OP_TYPE##_BYTE) && ((OPCODE) <= OP_TYPE##_WORD))

#define OPCODE_DATA_TYPE(OPCODE, OP_TYPE)            \
  ((OPCODE) == OP_TYPE##_BYTE      ? DATA_TYPE_BYTE  \
   : ((OPCODE) == OP_TYPE##_HWORD) ? DATA_TYPE_HWORD \
                                   : DATA_TYPE_WORD)

typedef struct
{
  opcode_t opcode;
  data_t operand;
} inst_t;

void inst_print(inst_t, FILE *);

size_t inst_bytecode_size(inst_t);
void inst_write_bytecode(inst_t, darr_t *);
void insts_write_bytecode(inst_t *, size_t, darr_t *);
// Here the dynamic array is a preloaded buffer of bytes, where
// darr.available is the number of overall bytes and used is the
// cursor (where we are in the buffer).
inst_t inst_read_bytecode(darr_t *);
inst_t *insts_read_bytecode(darr_t *, size_t *);

void insts_write_bytecode_file(inst_t *, size_t, FILE *);
inst_t *insts_read_bytecode_file(FILE *, size_t *);

#define INST_NOOP ((inst_t){0})
#define INST_HALT ((inst_t){.opcode = OP_HALT})

#define INST_PUSH(TYPE, OP) \
  ((inst_t){.opcode = OP_PUSH_##TYPE, .operand = D##TYPE(OP)})

#define INST_MOV(TYPE, OP) \
  ((inst_t){.opcode = OP_MOV_##TYPE, .operand = D##TYPE(OP)})

#define INST_POP(TYPE) ((inst_t){.opcode = OP_POP_##TYPE})

#define INST_PUSH_REG(TYPE, REG) \
  ((inst_t){.opcode = OP_PUSH_REGISTER_##TYPE, .operand = D##TYPE(REG)})

#define INST_DUP(TYPE, OP) \
  ((inst_t){.opcode = OP_DUP_##TYPE, .operand = DWORD(OP)})

#define INST_NOT(TYPE)  ((inst_t){.opcode = OP_NOT_##TYPE})
#define INST_OR(TYPE)   ((inst_t){.opcode = OP_OR_##TYPE})
#define INST_AND(TYPE)  ((inst_t){.opcode = OP_AND_##TYPE})
#define INST_XOR(TYPE)  ((inst_t){.opcode = OP_XOR_##TYPE})
#define INST_EQ(TYPE)   ((inst_t){.opcode = OP_EQ_##TYPE})
#define INST_PLUS(TYPE) ((inst_t){.opcode = OP_PLUS_##TYPE})

#define INST_JUMP_ABS(OP) \
  ((inst_t){.opcode = OP_JUMP_ABS, .operand = DWORD(OP)})
#define INST_JUMP_STACK    ((inst_t){.opcode = OP_JUMP_STACK})
#define INST_JUMP_REGISTER ((inst_t){.opcode = OP_JUMP_REGISTER})

#define INST_PRINT(TYPE) ((inst_t){.opcode = OP_PRINT_##TYPE})
#endif
