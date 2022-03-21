// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef process_runner_service_run_h
#define process_runner_service_run_h
#include <atomic>
#include <grpcpp/server_builder.h>
#include <memory>
#include <string>
#include <thread>

class ProcessRunnerServiceRun
{
private:
  std::unique_ptr<std::thread> _thread;
  void run();
  std::unique_ptr<grpc::Server> server;

public:
  ProcessRunnerServiceRun();
  ~ProcessRunnerServiceRun();
  void signal_to_stop();
};

#endif // process_runner_service_run_h
