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

#define VCLEAR(V)                       \
  std::for_each((V).begin(), (V).end(), \
                [](token_t *t)          \
                {                       \
                  delete t;             \
                });

pp_err_t preprocess_use_blocks(const vector<token_t *> &tokens,
                               vector<token_t *> &vec_out)
{
  for (size_t i = 0; i < tokens.size(); ++i)
  {
    token_t *t = tokens[i];
    if (t->type == token_type_t::PP_USE)
    {
      if (i + 1 >= tokens.size() ||
          tokens[i + 1]->type != token_type_t::LITERAL_STRING)
      {
        VCLEAR(vec_out);
        vec_out.clear();
        return pp_err_t(pp_err_type_t::EXPECTED_STRING, t);
      }

      token_t *name = tokens[i + 1];
      auto source   = read_file(name->content.c_str());
      if (!source)
      {
        VCLEAR(vec_out);
        vec_out.clear();
        return pp_err_t(pp_err_type_t::FILE_NONEXISTENT, name);
      }

      std::vector<token_t *> ftokens;
      lerr_t lerr = tokenise_buffer(source.value(), ftokens);
      if (lerr.type != lerr_type_t::OK)
      {
        VCLEAR(vec_out);
        vec_out.clear();
        return pp_err_t(pp_err_type_t::FILE_PARSE_ERROR, name, lerr);
      }

      vec_out.insert(vec_out.end(), ftokens.begin(), ftokens.end());

      ++i;
    }
    else
      vec_out.push_back(new token_t{*t});
  }
  return pp_err_t();
}

struct const_t
{
  size_t start, end;
};

pp_err_t preprocess_const_blocks(const vector<token_t *> &tokens,
                                 vector<token_t *> &vec_out)
{
  std::unordered_map<string_view, const_t> blocks;
  for (size_t i = 0; i < tokens.size(); ++i)
  {
    token_t *t = tokens[i];
    if (t->type == token_type_t::PP_CONST)
    {
      string_view capture;
      if (i + 1 >= tokens.size() || tokens[i + 1]->type != token_type_t::SYMBOL)
        return pp_err_type_t::EXPECTED_NAME;

      capture = tokens[++i]->content;

      ++i;
      size_t block_start = i, block_end = 0;
      for (; i < tokens.size() && tokens[i]->type != token_type_t::PP_END; ++i)
        continue;

      if (i == tokens.size())
        return pp_err_t{pp_err_type_t::EXPECTED_END};

      block_end = i;

      blocks[capture] = const_t{block_start, block_end};
    }
  }

  if (blocks.size() == 0)
  {
    // Just construct a new vector and carry on
    for (token_t *token : tokens)
      vec_out.push_back(new token_t{*token});
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
          VCLEAR(vec_out);
          vec_out.clear();
          return pp_err_t(pp_err_type_t::UNKNOWN_NAME, token);
        }

        const_t block = it->second;
        for (size_t i = block.start; i < block.end; ++i)
          vec_out.push_back(new token_t{*tokens[i]});
      }
      else
        vec_out.push_back(new token_t{*token});
    }
  }

  return pp_err_t();
}

pp_err_t preprocesser(const vector<token_t *> &tokens,
                      vector<token_t *> &vec_out)
{
  vector<token_t *> use_block_tokens;
  pp_err_t pperr = preprocess_use_blocks(tokens, use_block_tokens);
  if (pperr.type != pp_err_type_t::OK)
  {
    vec_out = tokens;
    return pperr;
  }

  vector<token_t *> const_block_tokens;
  pperr = preprocess_const_blocks(use_block_tokens, const_block_tokens);
  if (pperr.type != pp_err_type_t::OK)
  {
    VCLEAR(tokens);
    vec_out = use_block_tokens;
    return pperr;
  }

  VCLEAR(use_block_tokens);
  vec_out = const_block_tokens;

  return pp_err_t{pp_err_type_t::OK};
}

// TODO: Implement this
pp_err_t preprocess_macro_blocks(const vector<token_t *> &,
                                 vector<token_t *> &);

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
              << "]:" << err.lerr;
  case UNKNOWN_NAME:
    return os << "UNKNOWN_NAME";
  }
  return os;
}

pp_err_t::pp_err_t() : reference{nullptr}, type{pp_err_type_t::OK}, lerr{}
{}

pp_err_t::pp_err_t(pp_err_type_t e) : reference{nullptr}, type{e}, lerr{}
{}

pp_err_t::pp_err_t(pp_err_type_t err, const token_t *ref)
    : reference{ref}, type{err}
{}

pp_err_t::pp_err_t(pp_err_type_t err, const token_t *ref, lerr_t lerr)
    : reference{ref}, type{err}, lerr{lerr}
{}

// pp_unit_t::pp_unit_t(const token_t *const token) : resolved{false},
// token{token}
// {}

// pp_unit_t::pp_unit_t(std::string_view name, std::vector<pp_unit_t> elements)
//     : resolved{false}, token{nullptr}, container{name, elements}
// {}
