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
 * Description:
 */

#include <src/base.hpp>

#include <cstdio>

std::optional<std::string> read_file(const char *filename)
{
  FILE *fp = fopen(filename, "rb");
  if (fp)
  {
    std::string contents;
    fseek(fp, 0, SEEK_END);
    contents.resize(ftell(fp));
    rewind(fp);
    fread(&contents[0], 1, contents.size(), fp);
    fclose(fp);

    return contents;
  }
  else
    return std::nullopt;
}
