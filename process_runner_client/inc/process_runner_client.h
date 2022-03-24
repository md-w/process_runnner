// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef pipeline_remote_h
#define pipeline_remote_h
#include <atomic>
#include <future>
#include <grpcpp/grpcpp.h>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "data_models_process_runner.h"
#include "process_runner_service_caller.h"
#include "protobuf_helper.h"

class ProcessRunnerClient : public data_models::IProcessRunner
{
private:
  std::string _command;
  std::vector<std::string> _args;
  std::string _composite_command;

  std::size_t _key{0};
  std::atomic_bool _do_shutdown{false};
  bool _is_already_shutting_down{false};
  std::shared_ptr<ProcessRunnerServiceCaller> _service_caller;
  std::unique_ptr<std::thread> _thread;
  void run_process();
  void run();
  void stop();

public:
  ProcessRunnerClient(std::string command, std::vector<std::string> args,
                      std::shared_ptr<ProcessRunnerServiceCaller> service_caller);
  virtual ~ProcessRunnerClient();

  void signal_to_stop();
  bool is_running();
  int get_last_exit_code();
  int get_id();
  std::string get_composite_command();
  std::string get_initial_directory();
};

#endif // pipeline_remote_h
