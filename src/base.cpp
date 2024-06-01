/* Copyright (C) 2024 Aryadev Chavali

 * You may distribute and modify this code under the terms of the
 * GPLv2 license.  You should have received a copy of the GPLv2
 * license with this file.  If not, please write to:
 * aryadev@aryadevchavali.com.

 * Created: 2024-04-14
 * Author: Aryadev Chavali
 * Description:
 */

#include "./base.hpp"

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
