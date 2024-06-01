/* Copyright (C) 2024 Aryadev Chavali

 * You may distribute and modify this code under the terms of the
 * GPLv2 license.  You should have received a copy of the GPLv2
 * license with this file.  If not, please write to:
 * aryadev@aryadevchavali.com.

 * Created: 2024-04-14
 * Author: Aryadev Chavali
 * Description: Lexer for assembly language
 */

#ifndef LEXER_HPP
#define LEXER_HPP

#include <ostream>
#include <string>
#include <tuple>
#include <vector>

enum class token_type_t
{
  PP_CONST,     // %const(<symbol>)...
  PP_USE,       // %use <string>
  PP_END,       // %end
  PP_REFERENCE, // $<symbol>
  GLOBAL,
  STAR,
  LITERAL_NUMBER,
  LITERAL_CHAR,
  LITERAL_STRING,
  NOOP,
  HALT,
  PUSH,
  POP,
  PUSH_REG,
  MOV,
  DUP,
  MALLOC,
  MALLOC_STACK,
  MSET,
  MSET_STACK,
  MGET,
  MGET_STACK,
  MDELETE,
  MSIZE,
  NOT,
  OR,
  AND,
  XOR,
  EQ,
  LT,
  LTE,
  GT,
  GTE,
  PLUS,
  SUB,
  MULT,
  PRINT,
  JUMP_ABS,
  JUMP_STACK,
  JUMP_IF,
  CALL,
  CALL_STACK,
  RET,
  SYMBOL,
};

const char *token_type_as_cstr(token_type_t type);

struct token_t
{
  token_type_t type;
  size_t column, line;
  std::string content;

  token_t();
  token_t(token_type_t, std::string, size_t col = 0, size_t line = 0);
};

std::ostream &operator<<(std::ostream &, token_t &);

enum class lerr_type_t
{
  OK = 0,
  INVALID_CHAR_LITERAL,
  INVALID_CHAR_LITERAL_ESCAPE_SEQUENCE,
  INVALID_STRING_LITERAL,
  INVALID_NUMBER_LITERAL,
  INVALID_PREPROCESSOR_DIRECTIVE,
  UNKNOWN_LEXEME,
};

struct lerr_t
{
  size_t col, line;
  lerr_type_t type;

  lerr_t(lerr_type_t type = lerr_type_t::OK, size_t col = 0, size_t line = 0);
};

std::ostream &operator<<(std::ostream &, lerr_t &);

lerr_t tokenise_buffer(std::string_view, std::vector<token_t *> &);

#endif
