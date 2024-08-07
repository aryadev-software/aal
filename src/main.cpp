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
 * Description: Entrypoint for assembly program
 */

#include <cstdio>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

extern "C"
{
#include <lib/inst.h>
}

#include <src/base.hpp>
#include <src/lexer.hpp>
#include <src/preprocesser.hpp>

using std::cout, std::cerr, std::endl;
using std::string, std::string_view, std::vector;

using Lexer::Token;
using Preprocesser::Unit;
using Lex_Err = Lexer::Err;
using PP_Err  = Preprocesser::Err;

void usage(const char *program_name, FILE *fp)
{
  fprintf(fp,
          "Usage: %s FILE OUT-FILE\n"
          "\tFILE: Source code to compile\n"
          "\tOUT-FILE: Name of file to store bytecode\n",
          program_name);
}

int main(int argc, const char *argv[])
{
  if (argc == 1 || argc > 3)
  {
    usage(argv[0], stderr);
    return -1;
  }
  int ret                 = 0;
  const char *source_name = argv[1];
  const char *out_name    = argv[2];
  (void)out_name;

#if VERBOSE >= 1
  INFO("ASSEMBLER", "Assembling `%s` to `%s`\n", source_name, out_name);
#endif

  auto file_source = read_file(source_name);

#if VERBOSE >= 1
  SUCCESS("ASSEMBLER", "`%s` -> %lu bytes\n", source_name,
          file_source.has_value() ? file_source.value().size() : 0);
#endif

  string source_str;
  string_view original;
  string_view src;
  vector<Token *> tokens;
  Lex_Err lerr;

  Preprocesser::Map const_map, file_map;
  vector<Token *> token_bag;
  vector<Unit> units;
  PP_Err *perr = nullptr;

  // Highest scoped variable cut off point

  if (file_source.has_value())
    source_str = file_source.value();
  else
  {
    cerr << "ERROR: file `" << source_name << "` does not exist!" << endl;
    ret = -1;
    goto end;
  }
  original = string_view{source_str};
  src      = string_view{source_str};
  lerr     = tokenise_buffer(source_name, src, tokens);

  if (lerr.type != Lex_Err::Type::OK)
  {
    cerr << lerr << endl;
    ret = 255 - static_cast<int>(lerr.type);
    goto end;
  }
  else
  {
#if VERBOSE >= 1
    SUCCESS("LEXER", "%lu bytes -> %lu tokens\n", source_str.size(),
            tokens.size());
#endif

#if VERBOSE == 2
    SUCCESS("LEXER", "Tokens parsed:%s\n", "");
    printf("-------------------------------------------------------------------"
           "-------------\n");
    for (auto token : tokens)
      cout << "\t" << *token << endl;
    printf("-------------------------------------------------------------------"
           "-------------\n");
#endif
  }

  perr =
      Preprocesser::preprocess(tokens, units, token_bag, const_map, file_map);
  if (perr)
  {
    cerr << *perr << endl;
    ret = 255 - static_cast<int>(perr->type);
    goto end;
  }
  else
  {
#if VERBOSE >= 1
    SUCCESS("PREPROCESSER", "%lu tokens -> %lu units\n", tokens.size(),
            units.size());
#endif

#if VERBOSE == 2
    SUCCESS("PREPROCESSER", "Units constructed:%s\n", "");
    printf("-------------------------------------------------------------------"
           "-------------\n");
    for (auto unit : units)
      cout << unit << endl;
    printf("-------------------------------------------------------------------"
           "-------------\n");
#endif
  }

end:
  for (auto token : tokens)
    delete token;

  for (auto token : token_bag)
    delete token;
  if (perr)
    delete perr;

  return ret;
}
