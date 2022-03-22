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

#include "data_models.h"
#include "protobuf_helper.h"

class ProcessRunnerClient : public data_models::IProcessRunner
{
private:
  std::string _command;
  std::vector<std::string> _args;
  std::string _composite_command;

  std::size_t _key{0};
  std::atomic_bool _do_shutdown{false};
  std::atomic_bool _is_internal_shutdown{false};
  std::unique_ptr<data_models::ProcessRunner::Stub> _stub;
  std::unique_ptr<std::thread> _thread;
  void run_process();
  void run();
public:
  ProcessRunnerClient(std::string command, std::vector<std::string> args, std::shared_ptr<grpc::Channel> channel);
  ~ProcessRunnerClient();
  void signal_to_stop();
  bool is_running();
  int get_last_exit_code();
  int get_id();
  std::string get_composite_command();
  std::string get_initial_directory();
};

#endif // pipeline_remote_h
