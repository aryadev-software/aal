/* Copyright (C) 2024 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 * Created: 2024-07-03
 * Author: Aryadev Chavali
 * Description:
 */

#ifndef PREPROCESSER_HPP
#define PREPROCESSER_HPP

#include <ostream>
#include <unordered_map>

#include <src/lexer.hpp>

namespace Preprocesser
{
#define PREPROCESSER_MAX_DEPTH 16
  struct Block
  {
    Lexer::Token *root;
    std::vector<Lexer::Token *> body;
    int depth;
  };

  typedef std::unordered_map<std::string, Block> Map;

  struct Unit
  {
    Lexer::Token *const root;
    std::vector<Unit> expansion;
  };

  struct Err
  {
    Lexer::Token *token;
    Err *child_error;
    Lexer::Err lexer_error;
    enum class Type
    {
      EXPECTED_END,
      NO_CONST_AROUND,
      EMPTY_CONST,
      EXPECTED_SYMBOL_FOR_NAME,
      DIRECTIVES_IN_CONST_BODY,
      UNKNOWN_NAME_IN_REFERENCE,

      EXPECTED_FILE_NAME_AS_STRING,
      FILE_NON_EXISTENT,
      IN_FILE_LEXING,
      SELF_RECURSIVE_USE_CALL,

      IN_ERROR,
      EXCEEDED_PREPROCESSER_DEPTH,
    } type;

    Err();
    Err(Err::Type, Lexer::Token *, Err *child = nullptr, Lexer::Err err = {});
    ~Err(void);
  };

  std::string to_string(const Unit &, int depth = 0);
  std::string to_string(const Err::Type &);
  std::string to_string(const Err &);
  std::ostream &operator<<(std::ostream &, const Unit &);
  std::ostream &operator<<(std::ostream &, const Err &);

  Err *preprocess(std::vector<Lexer::Token *> tokens, std::vector<Unit> &units,
                  std::vector<Lexer::Token *> &new_token_bag, Map &const_map,
                  Map &file_map, int depth = 0);
}; // namespace Preprocesser
#endif
