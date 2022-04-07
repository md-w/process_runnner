// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef process_runner_service_caller_h
#define process_runner_service_caller_h
#include <memory>

#include "protobuf_helper.h"

class ProcessRunnerServiceCaller
{
private:
  std::unique_ptr<data_models::ProcessRunner::Stub> _stub;

public:
  ProcessRunnerServiceCaller(std::shared_ptr<grpc::Channel> channel);
  ~ProcessRunnerServiceCaller() = default;
  std::size_t run_process(std::string command, std::vector<std::string> args, int number);
  void signal_to_stop(std::size_t key);
  bool is_running(std::size_t key);
  int get_last_exit_code(std::size_t key);
  int get_id(std::size_t key);
  std::string get_composite_command(std::size_t key);
  std::string get_initial_directory(std::size_t key);
  std::string get_application_installation_directory();
  std::string get_config_directory();
  std::string get_data_directory();
  int get_next_number();
};

#endif // process_runner_service_caller_h
