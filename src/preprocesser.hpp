/* Copyright (C) 2024 Aryadev Chavali

 * You may distribute and modify this code under the terms of the GPLv2
 * license.  You should have received a copy of the GPLv2 license with
 * this file.  If not, please write to: aryadev@aryadevchavali.com.

 * Created: 2024-04-14
 * Author: Aryadev Chavali
 * Description: Preprocessor which occurs after lexing before parsing.
 */

#ifndef PREPROCESSER_HPP
#define PREPROCESSER_HPP

#include <ostream>
#include <tuple>

#include "./lexer.hpp"

enum pp_err_type_t
{
  OK = 0,
  EXPECTED_NAME,
  EXPECTED_STRING,
  EXPECTED_END,
  FILE_NONEXISTENT,
  FILE_PARSE_ERROR,
  UNKNOWN_NAME,
};

struct pp_err_t
{
  const token_t *reference;
  pp_err_type_t type;
  lerr_t lerr;

  pp_err_t();
  pp_err_t(pp_err_type_t);
  pp_err_t(pp_err_type_t, const token_t *);
  pp_err_t(pp_err_type_t, const token_t *, lerr_t);
};

std::ostream &operator<<(std::ostream &, pp_err_t &);

struct pp_unit_t
{
  const token_t *const token;
  struct
  {
    std::string_view name;
    std::vector<pp_unit_t> elements;
  } container;

  pp_unit_t(const token_t *const);
  pp_unit_t(std::string_view, std::vector<pp_unit_t>);
};

std::vector<pp_unit_t> tokens_to_units(const std::vector<token_t *> &);
pp_err_t preprocess_use(std::vector<pp_unit_t> &);
pp_err_t preprocesser(const std::vector<token_t *> &, std::vector<token_t *> &);

#endif
