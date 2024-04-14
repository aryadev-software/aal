/* Copyright (C) 2024 Aryadev Chavali

 * You may distribute and modify this code under the terms of the
 * GPLv2 license.  You should have received a copy of the GPLv2
 * license with this file.  If not, please write to:
 * aryadev@aryadevchavali.com.

 * Created: 2024-04-14
 * Author: Aryadev Chavali
 * Description: Preprocessor which occurs after lexing before parsing.
 */

#include "./preprocesser.hpp"
#include "./base.hpp"

#include <algorithm>
#include <unordered_map>

using std::pair, std::vector, std::make_pair, std::string, std::string_view;

#define ERR(E) std::make_pair(tokens, (E))
#define VAL(E) std::make_pair(E, pp_err_t{pp_err_type_t::OK})

pair<vector<token_t *>, pp_err_t>
preprocess_use_blocks(vector<token_t *> tokens)
{
  vector<token_t *> new_tokens;
  for (size_t i = 0; i < tokens.size(); ++i)
  {
    token_t *t = tokens[i];
    if (t->type == token_type_t::PP_USE)
    {
      if (i + 1 >= tokens.size() ||
          tokens[i + 1]->type != token_type_t::LITERAL_STRING)
      {
        new_tokens.clear();
        return ERR(pp_err_t(pp_err_type_t::EXPECTED_STRING, t));
      }

      token_t *name = tokens[i + 1];
      auto source   = read_file(name->content.c_str());
      if (!source)
      {
        new_tokens.clear();
        return ERR(pp_err_t(pp_err_type_t::FILE_NONEXISTENT, name));
      }

      std::vector<token_t *> ftokens;
      lerr_t lerr = tokenise_buffer(source.value(), ftokens);
      if (lerr != lerr_t::OK)
      {
        new_tokens.clear();
        return ERR(pp_err_t(pp_err_type_t::FILE_PARSE_ERROR, name, lerr));
      }

      new_tokens.insert(new_tokens.end(), ftokens.begin(), ftokens.end());

      i += 2;
    }
    else
      new_tokens.push_back(new token_t{*t});
  }
  return VAL(new_tokens);
}

struct const_t
{
  size_t start, end;
};

pair<vector<token_t *>, pp_err_t>
preprocess_const_blocks(vector<token_t *> &tokens)
{
  std::unordered_map<string_view, const_t> blocks;
  for (size_t i = 0; i < tokens.size(); ++i)
  {
    token_t *t = tokens[i];
    if (t->type == token_type_t::PP_CONST)
    {
      string_view capture;
      if (t->content == "" && (i == tokens.size() - 1 ||
                               tokens[i + 1]->type != token_type_t::SYMBOL))
        return ERR(pp_err_t{pp_err_type_t::EXPECTED_NAME});
      else if (t->content != "")
        capture = t->content;
      else
        capture = tokens[++i]->content;

      // Check for brackets
      auto start = capture.find('(');
      auto end   = capture.find(')');
      if (start == string::npos || end == string::npos)
        return ERR(pp_err_t{pp_err_type_t::EXPECTED_NAME});

      ++i;
      size_t block_start = i, block_end = 0;
      for (; i < tokens.size() && tokens[i]->type != token_type_t::PP_END; ++i)
        continue;

      if (i == tokens.size())
        return ERR(pp_err_t{pp_err_type_t::EXPECTED_END});

      block_end = i - 1;

      blocks[capture.substr(start + 1, end - 1)] =
          const_t{block_start, block_end};
    }
  }

  vector<token_t *> new_tokens;
  if (blocks.size() == 0)
  {
    // Just construct a new vector and carry on
    for (token_t *token : tokens)
      new_tokens.push_back(new token_t{*token});
  }
  else
  {
    for (size_t i = 0; i < tokens.size(); ++i)
    {
      token_t *token = tokens[i];
      // Skip the tokens that construct the const
      if (token->type == token_type_t::PP_CONST)
        for (; i < tokens.size() && tokens[i]->type != token_type_t::PP_END;
             ++i)
          continue;
      else if (token->type == token_type_t::PP_REFERENCE)
      {
        auto it = blocks.find(token->content);
        if (it == blocks.end())
        {
          new_tokens.clear();
          return ERR(pp_err_t(pp_err_type_t::UNKNOWN_NAME, token));
        }

        const_t block = it->second;
        for (size_t i = block.start; i < block.end; ++i)
          new_tokens.push_back(new token_t{*tokens[i]});
      }
      else
        new_tokens.push_back(new token_t{*token});
    }
  }

  return VAL(new_tokens);
}

std::ostream &operator<<(std::ostream &os, pp_err_t &err)
{
  os << "PREPROCESSING_";
  switch (err.type)
  {
  case OK:
    return os << "OK";
  case EXPECTED_NAME:
    return os << "EXPECTED_NAME";
  case EXPECTED_STRING:
    return os << "EXPECTED_STRING";
  case EXPECTED_END:
    return os << "EXPECTED_END";
  case FILE_NONEXISTENT:
    return os << "FILE_NONEXISTENT";
  case FILE_PARSE_ERROR:
    return os << "FILE_PARSE_ERROR -> \n\t[" << err.reference->content
              << "]: " << lerr_as_cstr(err.lerr);
  case UNKNOWN_NAME:
    return os << "UNKNOWN_NAME";
  }
  return os;
}

pp_err_t::pp_err_t(pp_err_type_t e)
    : reference{nullptr}, type{e}, lerr{lerr_t::OK}
{}

pp_err_t::pp_err_t(pp_err_type_t err, const token_t *ref)
    : reference{ref}, type{err}
{}

pp_err_t::pp_err_t(pp_err_type_t err, const token_t *ref, lerr_t lerr)
    : reference{ref}, type{err}, lerr{lerr}
{}
