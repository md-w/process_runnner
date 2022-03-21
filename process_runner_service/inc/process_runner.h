// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef process_runner_h
#define process_runner_h
#include <atomic>
#include <memory>
#include <string>
#include <thread>

#include "process_runner_service.h"

class ProcessRunner
{
private:
  std::unique_ptr<std::thread> _thread;
  void run();
  std::unique_ptr<grpc::Server> server;

public:
  ProcessRunner();
  ~ProcessRunner();
  void signal_to_stop();
};

#endif // process_runner_h
