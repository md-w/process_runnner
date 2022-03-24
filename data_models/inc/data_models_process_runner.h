// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef data_models_process_runner_h
#define data_models_process_runner_h
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

class IProcessRunner
{
public:
  virtual void signal_to_stop() = 0;
  virtual bool is_running() = 0;
  virtual int get_last_exit_code() = 0;
  virtual int get_id() = 0;
  virtual std::string get_composite_command() = 0;
  virtual std::string get_initial_directory() = 0;
  virtual ~IProcessRunner() = default;
};
} // namespace data_models

// custom specialization of std::hash can be injected in namespace std
template <> struct std::hash<data_models::ProcessRunnerArguments> {
  std::size_t operator()(data_models::ProcessRunnerArguments const& s) const noexcept
  {
    return data_models::hash_value(s);
  }
};
#endif // data_models_process_runner_h
