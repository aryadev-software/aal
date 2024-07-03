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

using std::cout, std::cerr, std::endl;
using std::string, std::string_view, std::vector;

using Lexer::Token;
using Lex_Err = Lexer::Err;

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
  printf("[%sASSEMBLER%s]: Assembling `%s` to `%s`\n", TERM_YELLOW, TERM_RESET,
         source_name, out_name);
#endif

  auto file_source = read_file(source_name);

#if VERBOSE >= 1
  printf("[%sASSEMBLER%s]: `%s` -> %lu bytes\n", TERM_YELLOW, TERM_RESET,
         source_name, file_source.has_value() ? file_source.value().size() : 0);
#endif

  string source_str;
  string_view original;
  string_view src;
  vector<Token *> tokens, preprocessed_tokens;
  Lex_Err lerr;

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
  lerr     = tokenise_buffer(src, tokens);

  if (lerr.type != Lexer ::Err::Type::OK)
  {
    cerr << source_name << ":" << lerr << endl;
    ret = 255 - static_cast<int>(lerr.type);
    goto end;
  }
  else
  {

#if VERBOSE >= 1
    printf("[%sLEXER%s]: %lu bytes -> %lu tokens\n", TERM_GREEN, TERM_RESET,
           source_str.size(), tokens.size());
#endif

#if VERBOSE == 2
    printf("[%sLEXER%s]: Tokens "
           "parsed:\n----------------------------------------------------------"
           "----------------------\n",
           TERM_GREEN, TERM_RESET);
    for (auto token : tokens)
      cout << "\t" << *token << endl;
    printf("-------------------------------------------------------------"
           "-------------------\n");
#endif
  }

end:
  for (auto token : tokens)
    delete token;

  return ret;
}
