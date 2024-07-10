/* Copyright (C) 2024 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 * Created: 2024-07-05
 * Author: Aryadev Chavali
 * Description:
 */

#include <src/base.hpp>
#include <src/lexer.hpp>
#include <src/preprocesser.hpp>

#include <lib/base.h>

#include <iostream>
#include <sstream>

namespace Preprocesser
{
  using TT  = Lexer::Token::Type;
  using ET  = Err::Type;
  using LET = Lexer::Err::Type;

  Err *preprocess(std::vector<Lexer::Token *> tokens, std::vector<Unit> &units,
                  std::vector<Lexer::Token *> &new_token_bag, Map &const_map,
                  Map &file_map, int depth)
  {
    // Stop preprocessing if we've smashed the preprocessing call stack
    if (depth >= PREPROCESSER_MAX_DEPTH)
      return new Err{ET::EXCEEDED_PREPROCESSER_DEPTH, tokens[0]};

    for (size_t i = 0; i < tokens.size(); ++i)
    {
      const auto token = tokens[i];
      if (token->type == TT::PP_CONST)
      {
        if (i == tokens.size() - 1 || tokens[i + 1]->type != TT::SYMBOL)
          return new Err{ET::EXPECTED_SYMBOL_FOR_NAME, token};
        const auto const_name = tokens[i + 1]->content;

        size_t end = 0;
        for (end = i + 2;
             end < tokens.size() && tokens[end]->type != TT::PP_END; ++end)
        {
          // TODO: Is there a better way to deal with preprocesser calls inside
          // of a constant?
          if (tokens[end]->type == TT::PP_CONST ||
              tokens[end]->type == TT::PP_USE)
            return new Err{ET::DIRECTIVES_IN_CONST_BODY, tokens[end]};
        }

        if (end == tokens.size())
          return new Err{ET::EXPECTED_END, token};
        else if (end - i == 2)
          return new Err{ET::EMPTY_CONST, token};

        // Check if we're redefining a constant.  If the current depth is
        // equivalent or higher than the depth when the constant was defined,
        // then stop.
        if (const_map.find(const_name) != const_map.end() &&
            const_map[const_name].depth <= depth)
        {
          i = end;
#if VERBOSE >= 2
          INFO("PREPROCESSER",
               "<%d> [%lu]:\n\t Preserving definition of `%s` from outer "
               "scope\n",
               depth, i, const_name.c_str());
#endif
          continue;
        }

        std::vector<Lexer::Token *> body{end - i - 2};
        std::copy(std::begin(tokens) + i + 2, std::begin(tokens) + end,
                  std::begin(body));

        const_map[const_name] = {token, body, depth};
        i                     = end;

#if VERBOSE >= 2
        INFO("PREPROCESSER", "<%d> [%lu]:\n\tConstant `%s` {\n", depth, i,
             const_name.c_str());

        for (size_t j = 0; j < body.size(); ++j)
        {
          std::cout << "\t\t[" << j << "]: ";
          if (body[j])
            std::cout << *body[j];
          else
            std::cout << "[NULL]";
          std::cout << "\n";
        }
        std::cout << "\t}\n";
#endif
      }
      else if (token->type == TT::PP_REFERENCE)
      {
        // Reference expansion based on latest constant
        const auto found = const_map.find(token->content);
        if (found == const_map.end())
          return new Err{ET::UNKNOWN_NAME_IN_REFERENCE, token};

        std::vector<Unit> preprocessed;
        Err *err = preprocess(found->second.body, preprocessed, new_token_bag,
                              const_map, file_map, depth + 1);
        if (err)
          return new Err{ET::IN_ERROR, token, err};
        units.push_back(Unit{token, preprocessed});
      }
      else if (token->type == TT::PP_USE)
      {
        // Ensure string in next token
        if (i == tokens.size() - 1 || tokens[i + 1]->type != TT::LITERAL_STRING)
          return new Err{ET::EXPECTED_FILE_NAME_AS_STRING, token};
        // Stops recursive calls on the file currently being preprocessed
        if (file_map.find(token->source_name) == file_map.end())
          file_map[token->source_name] = {};

        const auto name = tokens[i + 1]->content;
#if VERBOSE >= 2
        INFO("PREPROCESSER", "<%d> [%lu]: (", depth, i);
        std::cout << *tokens[i] << "): FILENAME=`" << name << "`\n";
#endif
        // If file has never been encountered, let's tokenise, preprocess then
        // cache the result
        if (file_map.find(name) == file_map.end())
        {
          auto content = read_file(tokens[i + 1]->content.c_str());

          if (!content.has_value())
            return new Err{ET::FILE_NON_EXISTENT, token};

          std::vector<Lexer::Token *> body;
          Lexer::Err lexer_err = Lexer::tokenise_buffer(tokens[i + 1]->content,
                                                        content.value(), body);

          // Add tokens to the bag for deallocation later
          // NOTE: We do this before errors so no memory leaks happen
          new_token_bag.insert(std::end(new_token_bag), std::begin(body),
                               std::end(body));

          if (lexer_err.type != LET::OK)
            return new Err{ET::IN_FILE_LEXING, token, nullptr, lexer_err};

          file_map[name].body = body;
          std::vector<Unit> body_units;
          Err *err = preprocess(body, body_units, new_token_bag, const_map,
                                file_map, depth + 1);
          if (err)
            return new Err{ET::IN_ERROR, token, err};

          // Compile away empty bodies
          if (body_units.size() != 0)
            units.push_back(Unit{token, body_units});
          ++i;
        }
        // Otherwise file must be part of the source tree already, so skip this
        // call
        else
          i += 1;
      }
      else if (token->type == TT::PP_END)
        return new Err{ET::NO_CONST_AROUND, token};
      else
        units.push_back(Unit{token, {}});
    }
    return nullptr;
  }

  std::string to_string(const Unit &unit, int depth)
  {
    std::stringstream ss;
    for (int i = 0; i < depth; ++i)
      ss << "\t";
    ss << Lexer::to_string(*unit.root) << " => {";
    if (unit.expansion.size() != 0)
    {
      ss << "\n";
      for (auto child : unit.expansion)
        ss << to_string(child, depth + 1) << "\n";
      for (int i = 0; i < depth; ++i)
        ss << "\t";
    }
    ss << "}";
    return ss.str();
  }

  std::string to_string(const Err::Type &type)
  {
    switch (type)
    {
    case ET::EXPECTED_END:
      return "EXPECTED_END";
    case ET::EMPTY_CONST:
      return "EMPTY_CONST";
    case ET::NO_CONST_AROUND:
      return "NO_CONST_AROUND";
    case ET::EXPECTED_SYMBOL_FOR_NAME:
      return "EXPECTED_SYMBOL_FOR_NAME";
    case ET::DIRECTIVES_IN_CONST_BODY:
      return "DIRECTIVES_IN_CONST_BODY";
    case ET::UNKNOWN_NAME_IN_REFERENCE:
      return "UNKNOWN_NAME_IN_REFERENCE";
    case ET::EXPECTED_FILE_NAME_AS_STRING:
      return "EXPECTED_FILE_NAME_AS_STRING";
    case ET::FILE_NON_EXISTENT:
      return "FILE_NON_EXISTENT";
    case ET::IN_FILE_LEXING:
      return "IN_FILE_LEXING";
    case ET::SELF_RECURSIVE_USE_CALL:
      return "SELF_RECURSIVE_USE_CALL";
    case ET::IN_ERROR:
      return "IN_ERROR";
    case ET::EXCEEDED_PREPROCESSER_DEPTH:
      return "EXCEEDED_PREPROCESSER_DEPTH";
    default:
      return "";
    }
  }

  std::string to_string(const Err &err)
  {
    std::stringstream ss;
    // Reverse traversal of the linked list of errors
    std::vector<Err *> errors;
    errors.push_back((Err *)&err);
    for (Err *e = err.child_error; e; e = e->child_error)
      errors.insert(errors.begin(), e);
    for (size_t depth = 0; depth < errors.size(); ++depth)
    {
      const Err &e = *errors[depth];
      ss << e.token->source_name << ":" << e.token->line << ":"
         << e.token->column << ": " << to_string(e.type);
      if (e.type == ET::IN_FILE_LEXING)
        ss << ":\n" << e.lexer_error;
      if (depth != errors.size() - 1)
        ss << "\n";
    }
    return ss.str();
  }

  std::ostream &operator<<(std::ostream &stream, const Unit &unit)
  {
    return stream << to_string(unit, 1);
  }

  std::ostream &operator<<(std::ostream &stream, const Err &err)
  {
    return stream << to_string(err);
  }

  Err::Err()
  {
  }

  Err::Err(Err::Type type, Lexer::Token *root, Err *child, Lexer::Err err)
      : token{root}, child_error{child}, lexer_error{err}, type{type}
  {
  }

  Err::~Err(void)
  {
    if (this->child_error)
      delete this->child_error;
  }

} // namespace Preprocesser
