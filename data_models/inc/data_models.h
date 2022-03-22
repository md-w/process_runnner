// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef data_models_h
#define data_models_h
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace data_models
{
struct ProcessRunnerArguments {
  std::string command;
  std::vector<std::string> args;
};
std::size_t hash_value(ProcessRunnerArguments const& s);
} // namespace data_models

// custom specialization of std::hash can be injected in namespace std
template <> struct std::hash<data_models::ProcessRunnerArguments> {
  std::size_t operator()(data_models::ProcessRunnerArguments const& s) const noexcept
  {
    return data_models::hash_value(s);
  }
};
#endif // data_models_h
