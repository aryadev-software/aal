/* Copyright (C) 2024 Aryadev Chavali

 * You may distribute and modify this code under the terms of the
 * GPLv2 license.  You should have received a copy of the GPLv2
 * license with this file.  If not, please write to:
 * aryadev@aryadevchavali.com.

 * Created: 2024-04-14
 * Author: Aryadev Chavali
 * Description: Lexer for assembly language
 */

extern "C"
{
#include <lib/inst.h>
}

#include <algorithm>
#include <tuple>

#include "./lexer.hpp"

static_assert(NUMBER_OF_OPCODES == 99, "ERROR: Lexer is out of date");

using std::string, std::string_view, std::pair, std::make_pair;

const auto VALID_SYMBOL = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUV"
                          "WXYZ0123456789-_.:%#$",
           VALID_DIGIT = "0123456789", VALID_HEX = "0123456789abcdefABCDEF";

bool is_char_in_s(char c, const char *s)
{
  return string_view(s).find(c) != string::npos;
}

bool initial_match(string_view src, string_view match)
{
  return (src.size() > match.size() && src.substr(0, match.size()) == match);
}

pair<Token, lerr_t> tokenise_symbol(string_view &source, size_t &column,
                                    size_t line)
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
    t.type = token_type_t::PP_CONST;
  }
  else if (sym == "%USE")
  {
    t.type = token_type_t::PP_USE;
  }
  else if (sym == "%END")
  {
    t.type = token_type_t::PP_END;
  }
  else if (sym[0] == '%')
  {
    return make_pair(
        t, lerr_t(lerr_type_t::INVALID_PREPROCESSOR_DIRECTIVE, column, line));
  }
  else if (sym.size() > 1 && sym[0] == '$')
  {
    t = Token(token_type_t::PP_REFERENCE, sym.substr(1));
  }
  else if (sym == "NOOP")
  {
    t.type = token_type_t::NOOP;
  }
  else if (sym == "HALT")
  {
    t.type = token_type_t::HALT;
  }
  else if (initial_match(sym, "PUSH.REG."))
  {
    t = Token(token_type_t::PUSH_REG, sym.substr(9));
  }
  else if (initial_match(sym, "PUSH."))
  {
    t = Token(token_type_t::PUSH, sym.substr(5));
  }
  else if (initial_match(sym, "POP."))
  {
    t = Token(token_type_t::POP, sym.substr(4));
  }
  else if (initial_match(sym, "MOV."))
  {
    t = Token(token_type_t::MOV, sym.substr(4));
  }
  else if (initial_match(sym, "DUP."))
  {
    t = Token(token_type_t::DUP, sym.substr(4));
  }
  else if (initial_match(sym, "MALLOC.STACK."))
  {
    t = Token(token_type_t::MALLOC_STACK, sym.substr(13));
  }
  else if (initial_match(sym, "MALLOC."))
  {
    t = Token(token_type_t::MALLOC, sym.substr(7));
  }
  else if (initial_match(sym, "MSET.STACK."))
  {
    t = Token(token_type_t::MSET_STACK, sym.substr(11));
  }
  else if (initial_match(sym, "MSET."))
  {
    t = Token(token_type_t::MSET, sym.substr(5));
  }
  else if (initial_match(sym, "MGET.STACK."))
  {
    t = Token(token_type_t::MGET_STACK, sym.substr(11));
  }
  else if (initial_match(sym, "MGET."))
  {
    t = Token(token_type_t::MGET, sym.substr(5));
  }
  else if (sym == "MDELETE")
  {
    t.type = token_type_t::MDELETE;
  }
  else if (sym == "MSIZE")
  {
    t.type = token_type_t::MSIZE;
  }
  else if (initial_match(sym, "NOT."))
  {
    t = Token(token_type_t::NOT, sym.substr(4));
  }
  else if (initial_match(sym, "OR."))
  {
    t = Token(token_type_t::OR, sym.substr(3));
  }
  else if (initial_match(sym, "AND."))
  {
    t = Token(token_type_t::AND, sym.substr(4));
  }
  else if (initial_match(sym, "XOR."))
  {
    t = Token(token_type_t::XOR, sym.substr(4));
  }
  else if (initial_match(sym, "EQ."))
  {
    t = Token(token_type_t::EQ, sym.substr(3));
  }
  else if (initial_match(sym, "LTE."))
  {
    t = Token(token_type_t::LTE, sym.substr(4));
  }
  else if (initial_match(sym, "LT."))
  {
    t = Token(token_type_t::LT, sym.substr(3));
  }
  else if (initial_match(sym, "GTE."))
  {
    t = Token(token_type_t::GTE, sym.substr(4));
  }
  else if (initial_match(sym, "GT."))
  {
    t = Token(token_type_t::GT, sym.substr(3));
  }
  else if (initial_match(sym, "SUB."))
  {
    t = Token(token_type_t::SUB, sym.substr(4));
  }
  else if (initial_match(sym, "PLUS."))
  {
    t = Token(token_type_t::PLUS, sym.substr(5));
  }
  else if (initial_match(sym, "MULT."))
  {
    t = Token(token_type_t::MULT, sym.substr(5));
  }
  else if (initial_match(sym, "PRINT."))
  {
    t = Token(token_type_t::PRINT, sym.substr(6));
  }
  else if (sym == "JUMP.ABS")
  {
    t.type = token_type_t::JUMP_ABS;
  }
  else if (sym == "JUMP.STACK")
  {
    t.type = token_type_t::JUMP_STACK;
  }
  else if (initial_match(sym, "JUMP.IF."))
  {
    t = Token(token_type_t::JUMP_IF, sym.substr(8));
  }
  else if (sym == "CALL.STACK")
  {
    t.type = token_type_t::CALL_STACK;
  }
  else if (sym == "CALL")
  {
    t.type = token_type_t::CALL;
  }
  else if (sym == "RET")
  {
    t.type = token_type_t::RET;
  }
  else if (sym == "GLOBAL")
  {
    t.type = token_type_t::GLOBAL;
  }
  else
  {
    t.type = token_type_t::SYMBOL;
  }

  if (t.content == "")
    t.content = sym;
  t.column = column;
  column += sym.size() - 1;
  return make_pair(t, lerr_t());
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

  Token t{token_type_t::LITERAL_NUMBER, (is_negative ? "-" : "") + digits,
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

  Token t = {token_type_t::LITERAL_NUMBER, "0x" + digits, column};

  column += digits.size() + 1;
  return t;
}

pair<Token, lerr_t> tokenise_literal_char(string_view &source, size_t &column,
                                          size_t &line)
{
  Token t{};
  auto end = source.find('\'', 1);
  if (source.size() < 3 || end == 1 || end > 3)
    return make_pair(t,
                     lerr_t(lerr_type_t::INVALID_CHAR_LITERAL, column, line));
  else if (source[1] == '\\')
  {
    // Escape sequence
    char escape = '\0';
    if (source.size() < 4 || source[3] != '\'')
      return make_pair(t,
                       lerr_t(lerr_type_t::INVALID_CHAR_LITERAL_ESCAPE_SEQUENCE,
                              column, line));
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
      return make_pair(t,
                       lerr_t(lerr_type_t::INVALID_CHAR_LITERAL_ESCAPE_SEQUENCE,
                              column, line));
      break;
    }
    t = Token{token_type_t::LITERAL_CHAR, std::to_string(escape), column};
    column += 4;
    source.remove_prefix(4);
  }
  else
  {
    t = Token(token_type_t::LITERAL_CHAR, std::to_string(source[1]));
    column += 3;
    source.remove_prefix(3);
  }
  return make_pair(t, lerr_t());
}

Token tokenise_literal_string(string_view &source, size_t &column, size_t end)
{
  source.remove_prefix(1);
  Token token{token_type_t::LITERAL_STRING, string(source.substr(0, end - 1)),
              column};
  source.remove_prefix(end);
  column += end + 1;
  return token;
}

lerr_t tokenise_buffer(string_view source, std::vector<Token *> &tokens)
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
           i < source.size() && (isspace(source[i]) || source[i] == '\0'); ++i)
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
      t = Token(token_type_t::STAR, "", column);
      source.remove_prefix(1);
    }
    else if (first == '\"')
    {
      auto end = source.find('\"', 1);
      if (end == string::npos)
        return lerr_t(lerr_type_t::INVALID_STRING_LITERAL, column, line);
      t = tokenise_literal_string(source, column, end);
    }
    else if (first == '\'')
    {
      lerr_t lerr;
      std::tie(t, lerr) = tokenise_literal_char(source, column, line);
      if (lerr.type != lerr_type_t::OK)
        return lerr;
    }
    else if (isdigit(first) ||
             (source.size() > 1 && first == '-' && isdigit(source[1])))
    {
      auto end = source.find_first_not_of(VALID_DIGIT, first == '-' ? 1 : 0);
      if (end == string::npos)
        end = source.size() - 1;
      else if (end != string::npos && !(isspace(source[end])))
        return lerr_t(lerr_type_t::INVALID_NUMBER_LITERAL, column, line);
      t = tokenise_literal_number(source, column);
    }
    else if (first == '0' && source.size() > 2 && source[1] == 'x' &&
             is_char_in_s(source[2], VALID_HEX))
    {
      auto end = source.find_first_not_of(VALID_HEX);
      if (end == string::npos)
        end = source.size() - 1;
      else if (end != string::npos && !(isspace(source[end])))
        return lerr_t(lerr_type_t::INVALID_NUMBER_LITERAL, column, line);
      t = tokenise_literal_hex(source, column);
    }
    else if (is_char_in_s(first, VALID_SYMBOL))
    {
      lerr_t lerr;
      std::tie(t, lerr) = tokenise_symbol(source, column, line);
      if (lerr.type != lerr_type_t::OK)
        return lerr;
    }
    else
    {
      ++column;
      return lerr_t{lerr_type_t::UNKNOWN_LEXEME, column, line};
    }

    if (is_token)
    {
      t.line     = line;
      Token *acc = new Token(t);
      tokens.push_back(acc);
    }
  }
  return lerr_t{};
}

std::ostream &operator<<(std::ostream &os, Token &t)
{
  return os << token_type_as_cstr(t.type) << "(`" << t.content << "`)@"
            << t.line << ", " << t.column;
}

Token::Token()
{}

Token::Token(token_type_t type, string content, size_t col, size_t line)
    : type{type}, column{col}, line{line}, content{content}
{}

const char *token_type_as_cstr(token_type_t type)
{
  switch (type)
  {
  case token_type_t::PP_USE:
    return "PP_USE";
  case token_type_t::PP_CONST:
    return "PP_CONST";
  case token_type_t::PP_END:
    return "PP_END";
  case token_type_t::PP_REFERENCE:
    return "PP_REFERENCE";
  case token_type_t::GLOBAL:
    return "GLOBAL";
  case token_type_t::STAR:
    return "STAR";
  case token_type_t::LITERAL_STRING:
    return "LITERAL_STRING";
  case token_type_t::LITERAL_NUMBER:
    return "LITERAL_NUMBER";
  case token_type_t::LITERAL_CHAR:
    return "LITERAL_CHAR";
  case token_type_t::NOOP:
    return "NOOP";
  case token_type_t::HALT:
    return "HALT";
  case token_type_t::PUSH:
    return "PUSH";
  case token_type_t::POP:
    return "POP";
  case token_type_t::PUSH_REG:
    return "PUSH_REG";
  case token_type_t::MOV:
    return "MOV";
  case token_type_t::DUP:
    return "DUP";
  case token_type_t::MALLOC:
    return "MALLOC";
  case token_type_t::MALLOC_STACK:
    return "MALLOC_STACK";
  case token_type_t::MSET:
    return "MSET";
  case token_type_t::MSET_STACK:
    return "MSET_STACK";
  case token_type_t::MGET:
    return "MGET";
  case token_type_t::MGET_STACK:
    return "MGET_STACK";
  case token_type_t::MDELETE:
    return "MDELETE";
  case token_type_t::MSIZE:
    return "MSIZE";
  case token_type_t::NOT:
    return "NOT";
  case token_type_t::OR:
    return "OR";
  case token_type_t::AND:
    return "AND";
  case token_type_t::XOR:
    return "XOR";
  case token_type_t::EQ:
    return "EQ";
  case token_type_t::LT:
    return "LT";
  case token_type_t::LTE:
    return "LTE";
  case token_type_t::GT:
    return "GT";
  case token_type_t::GTE:
    return "GTE";
  case token_type_t::PLUS:
    return "PLUS";
  case token_type_t::SUB:
    return "SUB";
  case token_type_t::MULT:
    return "MULT";
  case token_type_t::PRINT:
    return "PRINT";
  case token_type_t::JUMP_ABS:
    return "JUMP_ABS";
  case token_type_t::JUMP_STACK:
    return "JUMP_STACK";
  case token_type_t::JUMP_IF:
    return "JUMP_IF";
  case token_type_t::CALL:
    return "CALL";
  case token_type_t::CALL_STACK:
    return "CALL_STACK";
  case token_type_t::RET:
    return "RET";
  case token_type_t::SYMBOL:
    return "SYMBOL";
  }
  return "";
}

std::ostream &operator<<(std::ostream &os, lerr_t &lerr)
{
  os << lerr.line << ":" << lerr.col << ": ";
  switch (lerr.type)
  {
  case lerr_type_t::OK:
    os << "OK";
    break;
  case lerr_type_t::INVALID_CHAR_LITERAL:
    os << "INVALID_CHAR_LITERAL";
    break;
  case lerr_type_t::INVALID_CHAR_LITERAL_ESCAPE_SEQUENCE:
    os << "INVALID_CHAR_LITERAL_ESCAPE_SEQUENCE";
    break;
  case lerr_type_t::INVALID_STRING_LITERAL:
    os << "INVALID_STRING_LITERAL";
    break;
  case lerr_type_t::INVALID_NUMBER_LITERAL:
    os << "INVALID_NUMBER_LITERAL";
    break;
  case lerr_type_t::INVALID_PREPROCESSOR_DIRECTIVE:
    os << "INVALID_PREPROCESSOR_DIRECTIVE";
    break;
  case lerr_type_t::UNKNOWN_LEXEME:
    os << "UNKNOWN_LEXEME";
    break;
  default:
    break;
  }
  return os;
}

lerr_t::lerr_t(lerr_type_t type, size_t col, size_t line)
    : col{col}, line{line}, type{type}
{}
