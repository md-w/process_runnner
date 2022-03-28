// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "process_runner_service_run.h"
#include "logging.h"
#include "process_runner_service.h"

ProcessRunnerServiceRun::ProcessRunnerServiceRun(std::string application_installation_directory,
                                                 std::string config_directory, std::string data_directory,
                                                 int listening_port)
    : _application_installation_directory(std::move(application_installation_directory)),
      _config_directory(std::move(config_directory)), _data_directory(std::move(data_directory)),
      _listening_port(listening_port)
{
  _thread = std::make_unique<std::thread>(&ProcessRunnerServiceRun::run, this);
}

ProcessRunnerServiceRun::~ProcessRunnerServiceRun()
{
  signal_to_stop();
  if (_thread->joinable()) {
    _thread->join();
  }
}

void ProcessRunnerServiceRun::signal_to_stop()
{
  if (server) {
    server->Shutdown();
  }
}

void ProcessRunnerServiceRun::run()
{
  RAY_LOG_INF << "Started";
  ::grpc::ServerBuilder builder;
  std::string listening_url = fmt::format("[::]:{}", _listening_port);
  RAY_LOG_INF << "Trying to listen at: " << listening_url;
  builder.AddListeningPort(listening_url, ::grpc::InsecureServerCredentials());
  ProcessRunnerService my_service(_application_installation_directory, _config_directory, _data_directory);
  builder.RegisterService(&my_service);

  server = builder.BuildAndStart();
  server->Wait();
  RAY_LOG_INF << "Stopped";
}