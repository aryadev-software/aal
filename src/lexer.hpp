/* Copyright (C) 2024 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

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
      // Preprocessor and other parse time constants
      PP_CONST,     // %const(<symbol>)...
      PP_USE,       // %use <string>
      PP_END,       // %end
      PP_REFERENCE, // $<symbol>
      GLOBAL,
      STAR,
      // Literals
      LITERAL_NUMBER,
      LITERAL_CHAR,
      LITERAL_STRING,
      SYMBOL,

      // Instruction
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
    } type;
    enum class OperandType
    {
      NIL,
      BYTE,
      CHAR,
      HWORD,
      INT,
      WORD,
      LONG
    } operand_type;

    size_t column, line;
    std::string source_name, content;

    Token();
    Token(Token::Type, std::string_view content = "", size_t col = 0,
          size_t line = 0, OperandType type = OperandType::NIL);
  };

  struct Err
  {
    std::string_view source_name;
    size_t col, line;
    enum class Type
    {
      OK = 0,
      INVALID_CHAR_LITERAL,
      INVALID_CHAR_LITERAL_ESCAPE_SEQUENCE,
      INVALID_STRING_LITERAL,
      INVALID_NUMBER_LITERAL,
      INVALID_PREPROCESSOR_DIRECTIVE,
      EXPECTED_TYPE_SUFFIX,
      EXPECTED_UNSIGNED_TYPE_SUFFIX,
      UNKNOWN_LEXEME,
    } type;

    Err();

    Err(Type type, size_t col, size_t line, std::string_view source_name = "");
  };

  Err tokenise_buffer(std::string_view source_name, std::string_view content,
                      std::vector<Token *> &vec);

  std::string to_string(const Token::Type &);
  std::string to_string(const Token::OperandType &);
  std::string to_string(const Token &);
  std::string to_string(const Err::Type &);
  std::string to_string(const Err &);

  std::ostream &operator<<(std::ostream &, const Token &);
  std::ostream &operator<<(std::ostream &, const Err &);
} // namespace Lexer

#endif
