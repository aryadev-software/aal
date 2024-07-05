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

extern "C"
{
#include <lib/inst.h>
}

#include <algorithm>
#include <sstream>

#include <src/lexer.hpp>

static_assert(NUMBER_OF_OPCODES == 99, "ERROR: Lexer is out of date");

using std::string, std::string_view;

namespace Lexer
{
  constexpr auto VALID_SYMBOL =
                     "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUV"
                     "WXYZ0123456789-_.:%#$",
                 VALID_DIGIT = "0123456789",
                 VALID_HEX   = "0123456789abcdefABCDEF";

  bool is_char_in_s(char c, const char *s)
  {
    return string_view(s).find(c) != string::npos;
  }

  bool initial_match(string_view src, string_view match)
  {
    return (src.size() > match.size() && src.substr(0, match.size()) == match);
  }

  Err tokenise_unsigned_type(const string_view &symbol,
                             Token::OperandType &type, size_t column,
                             size_t line)
  {
    if (symbol == "BYTE")
    {
      type = Token::OperandType::BYTE;
      return Err{};
    }
    else if (symbol == "HWORD")
    {
      type = Token::OperandType::HWORD;
      return Err{};
    }
    else if (symbol == "WORD")
    {
      type = Token::OperandType::WORD;
      return Err{};
    }
    return Err{Err::Type::EXPECTED_UNSIGNED_TYPE_SUFFIX, column, line};
  }

  Err tokenise_signed_type(const string_view &symbol, Token::OperandType &type,
                           size_t column, size_t line)
  {
    if (symbol == "BYTE")
    {
      type = Token::OperandType::BYTE;
      return Err{};
    }
    else if (symbol == "CHAR")
    {
      type = Token::OperandType::CHAR;
      return Err{};
    }
    else if (symbol == "HWORD")
    {
      type = Token::OperandType::HWORD;
      return Err{};
    }
    else if (symbol == "INT")
    {
      type = Token::OperandType::INT;
      return Err{};
    }
    else if (symbol == "WORD")
    {
      type = Token::OperandType::WORD;
      return Err{};
    }
    else if (symbol == "LONG")
    {
      type = Token::OperandType::LONG;
      return Err{};
    }
    return Err{Err::Type::EXPECTED_TYPE_SUFFIX, column, line};
  }

  Err tokenise_symbol(string_view &source, size_t &column, size_t line,
                      Token &token)
  {
    auto end = source.find_first_not_of(VALID_SYMBOL);
    if (end == string::npos)
      end = source.size() - 1;
    string sym{source.substr(0, end)};
    source.remove_prefix(end);
    std::transform(sym.begin(), sym.end(), sym.begin(), ::toupper);

    Token t{};

    if (sym == "%CONST")
    {
      token.type = Token::Type::PP_CONST;
    }
    else if (sym == "%USE")
    {
      token.type = Token::Type::PP_USE;
    }
    else if (sym == "%END")
    {
      token.type = Token::Type::PP_END;
    }
    else if (sym[0] == '%')
    {
      return Err(Err::Type::INVALID_PREPROCESSOR_DIRECTIVE, column, line);
    }
    else if (sym.size() > 1 && sym[0] == '$')
    {
      token = Token{Token::Type::PP_REFERENCE, sym.substr(1)};
    }
    else if (sym == "NOOP")
    {
      token.type = Token::Type::NOOP;
    }
    else if (sym == "HALT")
    {
      token.type = Token::Type::HALT;
    }
    else if (initial_match(sym, "PUSH.REG."))
    {
      token.type = Token::Type::PUSH_REG;
      Err err    = tokenise_unsigned_type(std::string_view{sym}.substr(9),
                                          token.operand_type, column + 9, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "PUSH."))
    {
      token.type = Token::Type::PUSH;
      Err err    = tokenise_unsigned_type(std::string_view{sym}.substr(5),
                                          token.operand_type, column + 5, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "POP."))
    {
      token.type = Token::Type::POP;
      Err err    = tokenise_unsigned_type(std::string_view{sym}.substr(4),
                                          token.operand_type, column + 4, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "MOV."))
    {
      token.type = Token::Type::MOV;
      Err err    = tokenise_unsigned_type(std::string_view{sym}.substr(4),
                                          token.operand_type, column + 4, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "DUP."))
    {
      token.type = Token::Type::DUP;
      Err err    = tokenise_unsigned_type(std::string_view{sym}.substr(4),
                                          token.operand_type, column + 4, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "MALLOC.STACK."))
    {
      token.type = Token::Type::MALLOC_STACK;
      Err err    = tokenise_unsigned_type(std::string_view{sym}.substr(13),
                                          token.operand_type, column + 13, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "MALLOC."))
    {
      token.type = Token::Type::MALLOC;
      Err err    = tokenise_unsigned_type(std::string_view{sym}.substr(7),
                                          token.operand_type, column + 7, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "MSET.STACK."))
    {
      token.type = Token::Type::MSET_STACK;
      Err err    = tokenise_unsigned_type(std::string_view{sym}.substr(11),
                                          token.operand_type, column + 11, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "MSET."))
    {
      token.type = Token::Type::MSET;
      Err err    = tokenise_unsigned_type(std::string_view{sym}.substr(5),
                                          token.operand_type, column + 5, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "MGET.STACK."))
    {
      token.type = Token::Type::MGET_STACK;
      Err err    = tokenise_unsigned_type(std::string_view{sym}.substr(11),
                                          token.operand_type, column + 11, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "MGET."))
    {
      token.type = Token::Type::MGET;
      Err err    = tokenise_unsigned_type(std::string_view{sym}.substr(5),
                                          token.operand_type, column + 5, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (sym == "MDELETE")
    {
      token.type = Token::Type::MDELETE;
    }
    else if (sym == "MSIZE")
    {
      token.type = Token::Type::MSIZE;
    }
    else if (initial_match(sym, "NOT."))
    {
      token.type = Token::Type::NOT;
      Err err    = tokenise_unsigned_type(std::string_view{sym}.substr(4),
                                          token.operand_type, column + 4, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "OR."))
    {
      token.type = Token::Type::OR;
      Err err    = tokenise_unsigned_type(std::string_view{sym}.substr(3),
                                          token.operand_type, column + 3, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "AND."))
    {
      token.type = Token::Type::AND;
      Err err    = tokenise_unsigned_type(std::string_view{sym}.substr(4),
                                          token.operand_type, column + 4, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "XOR."))
    {
      token.type = Token::Type::XOR;
      Err err    = tokenise_unsigned_type(std::string_view{sym}.substr(4),
                                          token.operand_type, column + 4, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "EQ."))
    {
      token.type = Token::Type::EQ;
      Err err    = tokenise_unsigned_type(std::string_view{sym}.substr(3),
                                          token.operand_type, column + 3, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "LTE."))
    {
      token.type = Token::Type::LTE;
      Err err    = tokenise_signed_type(std::string_view{sym}.substr(4),
                                        token.operand_type, column + 4, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "LT."))
    {
      token.type = Token::Type::LT;
      Err err    = tokenise_signed_type(std::string_view{sym}.substr(3),
                                        token.operand_type, column + 3, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "GTE."))
    {
      token.type = Token::Type::GTE;
      Err err    = tokenise_signed_type(std::string_view{sym}.substr(4),
                                        token.operand_type, column + 4, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "GT."))
    {
      token.type = Token::Type::GT;
      Err err    = tokenise_signed_type(std::string_view{sym}.substr(3),
                                        token.operand_type, column + 3, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "SUB."))
    {
      token.type = Token::Type::SUB;
      Err err    = tokenise_signed_type(std::string_view{sym}.substr(4),
                                        token.operand_type, column + 4, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "PLUS."))
    {
      token.type = Token::Type::PLUS;
      Err err    = tokenise_signed_type(std::string_view{sym}.substr(5),
                                        token.operand_type, column + 5, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "MULT."))
    {
      token.type = Token::Type::MULT;
      Err err    = tokenise_signed_type(std::string_view{sym}.substr(5),
                                        token.operand_type, column + 5, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (initial_match(sym, "PRINT."))
    {
      token.type = Token::Type::PRINT;
      Err err    = tokenise_signed_type(std::string_view{sym}.substr(6),
                                        token.operand_type, column + 6, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (sym == "JUMP.ABS")
    {
      token.type = Token::Type::JUMP_ABS;
    }
    else if (sym == "JUMP.STACK")
    {
      token.type = Token::Type::JUMP_STACK;
    }
    else if (initial_match(sym, "JUMP.IF."))
    {
      token.type = Token::Type::JUMP_IF;
      Err err    = tokenise_unsigned_type(std::string_view{sym}.substr(8),
                                          token.operand_type, column + 8, line);
      if (err.type != Err::Type::OK)
        return err;
    }
    else if (sym == "CALL.STACK")
    {
      token.type = Token::Type::CALL_STACK;
    }
    else if (sym == "CALL")
    {
      token.type = Token::Type::CALL;
    }
    else if (sym == "RET")
    {
      token.type = Token::Type::RET;
    }
    else if (sym == "GLOBAL")
    {
      token.type = Token::Type::GLOBAL;
    }
    else
    {
      token.type    = Token::Type::SYMBOL;
      token.content = sym;
    }

    token.column = column;
    column += sym.size() - 1;
    return Err();
  }

  Token tokenise_literal_number(string_view &source, size_t &column)
  {
    bool is_negative = false;
    if (source[0] == '-')
    {
      is_negative = true;
      source.remove_prefix(1);
    }

    auto end = source.find_first_not_of(VALID_DIGIT);
    if (end == string::npos)
      end = source.size() - 1;
    string digits{source.substr(0, end)};
    source.remove_prefix(end);

    Token t{Token::Type::LITERAL_NUMBER, (is_negative ? "-" : "") + digits,
            column};

    column += digits.size() + (is_negative ? 1 : 0);

    return t;
  }

  Token tokenise_literal_hex(string_view &source, size_t &column)
  {
    // Remove x char from source
    source.remove_prefix(1);
    auto end = source.find_first_not_of(VALID_HEX);
    if (end == string::npos)
      end = source.size() - 1;
    string digits{source.substr(0, end)};
    source.remove_prefix(end);

    Token t = {Token::Type::LITERAL_NUMBER, "0x" + digits, column};

    column += digits.size() + 1;
    return t;
  }

  Err tokenise_literal_char(string_view &source, size_t &column, size_t &line,
                            Token &t)
  {
    auto end = source.find('\'', 1);
    if (source.size() < 3 || end == 1 || end > 3)
      return Err(Err::Type::INVALID_CHAR_LITERAL, column, line);
    else if (source[1] == '\\')
    {
      // Escape sequence
      char escape = '\0';
      if (source.size() < 4 || source[3] != '\'')
        return Err(Err::Type::INVALID_CHAR_LITERAL_ESCAPE_SEQUENCE, column,
                   line);
      switch (source[2])
      {
      case 'n':
        escape = '\n';
        break;
      case 't':
        escape = '\t';
        break;
      case 'r':
        escape = '\r';
        break;
      case '\\':
        escape = '\\';
        break;
      default:
        column += 2;
        return Err(Err::Type::INVALID_CHAR_LITERAL_ESCAPE_SEQUENCE, column,
                   line);
        break;
      }
      t = Token{Token::Type::LITERAL_CHAR, std::to_string(escape), column};
      column += 4;
      source.remove_prefix(4);
    }
    else
    {
      t = Token{Token::Type::LITERAL_CHAR, std::to_string(source[1])};
      column += 3;
      source.remove_prefix(3);
    }
    return Err();
  }

  Token tokenise_literal_string(string_view &source, size_t &column, size_t end)
  {
    source.remove_prefix(1);
    Token token{Token::Type::LITERAL_STRING, string(source.substr(0, end - 1)),
                column};
    source.remove_prefix(end);
    column += end + 1;
    return token;
  }

  Err tokenise_buffer(string_view source_name, string_view source,
                      std::vector<Token *> &tokens)
  {
    size_t column = 0, line = 1;
    while (source.size() > 0)
    {
      bool is_token = true;
      char first    = source[0];
      Token t{};
      if (isspace(first) || first == '\0')
      {
        size_t i;
        for (i = 0;
             i < source.size() && (isspace(source[i]) || source[i] == '\0');
             ++i)
        {
          ++column;
          if (source[i] == '\n')
          {
            column = 0;
            ++line;
          }
        }
        ++column;
        source.remove_prefix(i);
        is_token = false;
      }
      else if (first == ';')
      {
        size_t i;
        for (i = 0; i < source.size() && source[i] != '\n'; ++i)
          continue;
        column = 0;
        ++line;
        source.remove_prefix(i + 1);
        is_token = false;
      }
      else if (first == '*')
      {
        t = Token{Token::Type::STAR, "", column};
        source.remove_prefix(1);
      }
      else if (first == '\"')
      {
        auto end = source.find('\"', 1);
        if (end == string::npos)
          return Err(Err::Type::INVALID_STRING_LITERAL, column, line);
        t = tokenise_literal_string(source, column, end);
      }
      else if (first == '\'')
      {
        Err lerr = tokenise_literal_char(source, column, line, t);
        if (lerr.type != Err::Type::OK)
          return lerr;
      }
      else if (isdigit(first) ||
               (source.size() > 1 && first == '-' && isdigit(source[1])))
      {
        auto end = source.find_first_not_of(VALID_DIGIT, first == '-' ? 1 : 0);
        if (end == string::npos)
          end = source.size() - 1;
        else if (end != string::npos && !(isspace(source[end])))
          return Err(Err::Type::INVALID_NUMBER_LITERAL, column, line);
        t = tokenise_literal_number(source, column);
      }
      else if (first == '0' && source.size() > 2 && source[1] == 'x' &&
               is_char_in_s(source[2], VALID_HEX))
      {
        auto end = source.find_first_not_of(VALID_HEX);
        if (end == string::npos)
          end = source.size() - 1;
        else if (end != string::npos && !(isspace(source[end])))
          return Err(Err::Type::INVALID_NUMBER_LITERAL, column, line);
        t = tokenise_literal_hex(source, column);
      }
      else if (is_char_in_s(first, VALID_SYMBOL))
      {
        Err lerr;
        lerr = tokenise_symbol(source, column, line, t);
        if (lerr.type != Err::Type::OK)
          return lerr;
      }
      else
      {
        ++column;
        return Err{Err::Type::UNKNOWN_LEXEME, column, line};
      }

      if (is_token)
      {
        t.source_name = source_name;
        t.line        = line;
        Token *acc    = new Token{t};
        tokens.push_back(acc);
      }
    }
    return Err{};
  }

  Token::Token()
  {
  }

  Token::Token(Token::Type type, string_view content, size_t col, size_t line,
               OperandType optype)
      : type{type}, operand_type{optype}, column{col}, line{line},
        content{content}
  {
  }

  Err::Err(Err::Type type, size_t col, size_t line)
      : col{col}, line{line}, type{type}
  {
  }

  std::string to_string(const Token::Type &type)
  {
    switch (type)
    {
    case Token::Type::PP_USE:
      return "PP_USE";
    case Token::Type::PP_CONST:
      return "PP_CONST";
    case Token::Type::PP_END:
      return "PP_END";
    case Token::Type::PP_REFERENCE:
      return "PP_REFERENCE";
    case Token::Type::GLOBAL:
      return "GLOBAL";
    case Token::Type::STAR:
      return "STAR";
    case Token::Type::LITERAL_STRING:
      return "LITERAL_STRING";
    case Token::Type::LITERAL_NUMBER:
      return "LITERAL_NUMBER";
    case Token::Type::LITERAL_CHAR:
      return "LITERAL_CHAR";
    case Token::Type::NOOP:
      return "NOOP";
    case Token::Type::HALT:
      return "HALT";
    case Token::Type::PUSH:
      return "PUSH";
    case Token::Type::POP:
      return "POP";
    case Token::Type::PUSH_REG:
      return "PUSH_REG";
    case Token::Type::MOV:
      return "MOV";
    case Token::Type::DUP:
      return "DUP";
    case Token::Type::MALLOC:
      return "MALLOC";
    case Token::Type::MALLOC_STACK:
      return "MALLOC_STACK";
    case Token::Type::MSET:
      return "MSET";
    case Token::Type::MSET_STACK:
      return "MSET_STACK";
    case Token::Type::MGET:
      return "MGET";
    case Token::Type::MGET_STACK:
      return "MGET_STACK";
    case Token::Type::MDELETE:
      return "MDELETE";
    case Token::Type::MSIZE:
      return "MSIZE";
    case Token::Type::NOT:
      return "NOT";
    case Token::Type::OR:
      return "OR";
    case Token::Type::AND:
      return "AND";
    case Token::Type::XOR:
      return "XOR";
    case Token::Type::EQ:
      return "EQ";
    case Token::Type::LT:
      return "LT";
    case Token::Type::LTE:
      return "LTE";
    case Token::Type::GT:
      return "GT";
    case Token::Type::GTE:
      return "GTE";
    case Token::Type::PLUS:
      return "PLUS";
    case Token::Type::SUB:
      return "SUB";
    case Token::Type::MULT:
      return "MULT";
    case Token::Type::PRINT:
      return "PRINT";
    case Token::Type::JUMP_ABS:
      return "JUMP_ABS";
    case Token::Type::JUMP_STACK:
      return "JUMP_STACK";
    case Token::Type::JUMP_IF:
      return "JUMP_IF";
    case Token::Type::CALL:
      return "CALL";
    case Token::Type::CALL_STACK:
      return "CALL_STACK";
    case Token::Type::RET:
      return "RET";
    case Token::Type::SYMBOL:
      return "SYMBOL";
    }
    return "";
  }

  std::string to_string(const Token::OperandType &type)
  {
    switch (type)
    {
    case Token::OperandType::NIL:
      return "NIL";
    case Token::OperandType::BYTE:
      return "BYTE";
    case Token::OperandType::CHAR:
      return "CHAR";
    case Token::OperandType::HWORD:
      return "HWORD";
    case Token::OperandType::INT:
      return "INT";
    case Token::OperandType::WORD:
      return "WORD";
    case Token::OperandType::LONG:
      return "LONG";
    }
    return "";
  }

  std::string to_string(const Token &t)
  {
    std::stringstream stream;
    stream << to_string(t.type);

    if (t.operand_type != Token::OperandType::NIL)
      stream << "[" << to_string(t.operand_type) << "]";
    if (t.content != "")
      stream << "(`" << t.content << "`)";
    stream << "@" << t.line << ", " << t.column;
    return stream.str();
  }

  std::string to_string(const Err::Type &type)
  {
    switch (type)
    {
    case Err::Type::OK:
      return "OK";
    case Err::Type::INVALID_CHAR_LITERAL:
      return "INVALID_CHAR_LITERAL";
    case Err::Type::INVALID_CHAR_LITERAL_ESCAPE_SEQUENCE:
      return "INVALID_CHAR_LITERAL_ESCAPE_SEQUENCE";
    case Err::Type::INVALID_STRING_LITERAL:
      return "INVALID_STRING_LITERAL";
    case Err::Type::INVALID_NUMBER_LITERAL:
      return "INVALID_NUMBER_LITERAL";
    case Err::Type::INVALID_PREPROCESSOR_DIRECTIVE:
      return "INVALID_PREPROCESSOR_DIRECTIVE";
    case Err::Type::EXPECTED_TYPE_SUFFIX:
      return "EXPECTED_TYPE_SUFFIX";
    case Err::Type::EXPECTED_UNSIGNED_TYPE_SUFFIX:
      return "EXPECTED_UNSIGNED_TYPE_SUFFIX";
    case Err::Type::UNKNOWN_LEXEME:
      return "UNKNOWN_LEXEME";
    default:
      return "";
    }
  }

  std::string to_string(const Err &err)
  {
    std::stringstream stream;
    stream << err.line << ":" << err.col << ": ";
    stream << to_string(err.type);
    return stream.str();
  }

  std::ostream &operator<<(std::ostream &os, const Token &t)
  {
    return os << to_string(t);
  }

  std::ostream &operator<<(std::ostream &os, const Err &err)
  {
    return os << to_string(err);
  }
} // namespace Lexer
