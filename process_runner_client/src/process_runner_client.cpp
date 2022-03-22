// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Exception.h>

#include "logging.h"
#include "process_runner_client.h"

ProcessRunnerClient::ProcessRunnerClient(std::string command, std::vector<std::string> args,
                                         std::shared_ptr<grpc::Channel> channel)
{
  _stub = data_models::ProcessRunner::NewStub(channel);
  ::grpc::ClientContext context;
  data_models::RunProcessRequest request;
  request.set_command(command);
  *request.mutable_args() = {args.begin(), args.end()};
  data_models::RunProcessResponse response;
  grpc::Status status = _stub->RunProcess(&context, request, &response);
  if (status.ok()) {
    _key = response.key();
    RAY_LOG_INF << "Started with key: " << _key;
  } else {
    RAY_LOG_ERR << "Error: " << status.error_code();
  }
}

ProcessRunnerClient::~ProcessRunnerClient() { signal_to_stop(); }

void ProcessRunnerClient::signal_to_stop()
{
  if (_stub) {
    ::grpc::ClientContext context;
    data_models::SignalToStopRequest request;
    request.set_key(_key);
    data_models::SignalToStopResponse response;
    grpc::Status status = _stub->SignalToStop(&context, request, &response);
    if (status.ok()) {
      _key = response.key();
      RAY_LOG_INF << "Stopped with key: " << _key;
    } else {
      RAY_LOG_ERR << "Error: " << status.error_code();
    }
  }
}

bool ProcessRunnerClient::is_running() { return false; }

int ProcessRunnerClient::get_last_exit_code() { return -1; }
