// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "process_runner_service_run.h"
#include "logging.h"
#include "process_runner_service.h"


ProcessRunnerServiceRun::ProcessRunnerServiceRun()
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
  builder.AddListeningPort("0.0.0.0:50051", ::grpc::InsecureServerCredentials());
  ProcessRunnerService my_service;
  builder.RegisterService(&my_service);

  server = builder.BuildAndStart();
  server->Wait();
  RAY_LOG_INF << "Stopped";
}