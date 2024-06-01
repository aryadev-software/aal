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
#include <vector>

namespace Lexer
{
  struct Token
  {
    enum class Type
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
    } type;
    size_t column, line;
    std::string content;

    Token();
    Token(Token::Type, std::string, size_t col = 0, size_t line = 0);
  };

  std::ostream &operator<<(std::ostream &, const Token &);
  std::string to_string(const Token::Type &);
  std::string to_string(const Token &);

  struct Err
  {
    size_t col, line;
    enum class Type
    {
      OK = 0,
      INVALID_CHAR_LITERAL,
      INVALID_CHAR_LITERAL_ESCAPE_SEQUENCE,
      INVALID_STRING_LITERAL,
      INVALID_NUMBER_LITERAL,
      INVALID_PREPROCESSOR_DIRECTIVE,
      UNKNOWN_LEXEME,
    } type;

    Err(Type type = Type::OK, size_t col = 0, size_t line = 0);
  };

  std::ostream &operator<<(std::ostream &, const Err &);
  std::string to_string(const Err::Type &);
  std::string to_string(const Err &);

  Err tokenise_buffer(std::string_view, std::vector<Token *> &);
} // namespace Lexer

#endif
