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
  std::string _application_installation_directory;
  std::string _config_directory;
  std::string _data_directory;
  std::unique_ptr<std::thread> _thread;
  void run();
  std::unique_ptr<grpc::Server> server;

public:
  ProcessRunnerServiceRun(std::string application_installation_directory, std::string config_directory,
                          std::string data_directory);
  ~ProcessRunnerServiceRun();
  void signal_to_stop();
};

#endif // process_runner_service_run_h
