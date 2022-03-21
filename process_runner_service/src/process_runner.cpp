// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "process_runner.h"
#include "logging.h"

ProcessRunner::ProcessRunner() { _thread = std::make_unique<std::thread>(&ProcessRunner::run, this); }

ProcessRunner::~ProcessRunner()
{
  signal_to_stop();
  if (_thread->joinable()) {
    _thread->join();
  }
}

void ProcessRunner::signal_to_stop()
{
  if (server) {
    server->Shutdown();
  }
}

void ProcessRunner::run()
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