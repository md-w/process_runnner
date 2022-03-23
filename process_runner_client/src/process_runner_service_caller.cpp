// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "process_runner_service_caller.h"

ProcessRunnerServiceCaller::ProcessRunnerServiceCaller(std::shared_ptr<grpc::Channel> channel)
    : _stub(data_models::ProcessRunner::NewStub(channel))
{
}
std::size_t ProcessRunnerServiceCaller::run_process(std::string command, std::vector<std::string> args)
{
  size_t key = 0;
  ::grpc::ClientContext context;
  data_models::RunProcessRequest request;
  request.set_command(command);
  *request.mutable_args() = {args.begin(), args.end()};
  data_models::RunProcessResponse response;
  grpc::Status status = _stub->RunProcess(&context, request, &response);
  if (status.ok()) {
    key = response.key();
    RAY_LOG_INF << "Started with key: " << key;
  } else {
    std::stringstream err;
    err << "Error: ";
    err << status.error_code();
    RAY_LOG_ERR << err.str();
    throw std::runtime_error(err.str());
  }
  return key;
}
void ProcessRunnerServiceCaller::signal_to_stop(std::size_t key)
{
  ::grpc::ClientContext context;
  data_models::SignalToStopRequest request;
  request.set_key(key);
  data_models::SignalToStopResponse response;
  grpc::Status status = _stub->SignalToStop(&context, request, &response);
  if (status.ok()) {
    RAY_LOG_INF << "Stopped with key: " << response.key();
  } else {
    RAY_LOG_ERR << "Error: " << status.error_code();
  }
}
bool ProcessRunnerServiceCaller::is_running(std::size_t key)
{
  bool is_running = false;
  ::grpc::ClientContext context;
  data_models::IsRunningRequest request;
  request.set_key(key);
  data_models::IsRunningResponse response;
  grpc::Status status = _stub->IsRunning(&context, request, &response);
  if (status.ok()) {
    is_running = response.value();
  } else {
    std::stringstream err;
    err << "Error: ";
    err << status.error_code();
    RAY_LOG_ERR << err.str();
    throw std::runtime_error(err.str());
  }
  return is_running;
}

int ProcessRunnerServiceCaller::get_last_exit_code(std::size_t key)
{
  int exit_code = -1;
  ::grpc::ClientContext context;
  data_models::GetLastExitCodeRequest request;
  request.set_key(key);
  data_models::GetLastExitCodeResponse response;
  grpc::Status status = _stub->GetLastExitCode(&context, request, &response);
  if (status.ok()) {
    exit_code = response.value();
  } else {
    std::stringstream err;
    err << "Error: ";
    err << status.error_code();
    RAY_LOG_ERR << err.str();
    throw std::runtime_error(err.str());
  }
  return exit_code;
}
int ProcessRunnerServiceCaller::get_id(std::size_t key)
{
  int id = 0;
  ::grpc::ClientContext context;
  data_models::GetIdRequest request;
  request.set_key(key);
  data_models::GetIdResponse response;
  grpc::Status status = _stub->GetId(&context, request, &response);
  if (status.ok()) {
    id = response.value();
  } else {
    std::stringstream err;
    err << "Error: ";
    err << status.error_code();
    RAY_LOG_ERR << err.str();
    throw std::runtime_error(err.str());
  }
  return id;
}

std::string ProcessRunnerServiceCaller::get_composite_command(std::size_t key)
{
  std::string composite_command;
  ::grpc::ClientContext context;
  data_models::GetCompositeCommandRequest request;
  request.set_key(key);
  data_models::GetCompositeCommandResponse response;
  grpc::Status status = _stub->GetCompositeCommand(&context, request, &response);
  if (status.ok()) {
    composite_command = response.value();
  } else {
    std::stringstream err;
    err << "Error: ";
    err << status.error_code();
    RAY_LOG_ERR << err.str();
    throw std::runtime_error(err.str());
  }
  return composite_command;
}

std::string ProcessRunnerServiceCaller::get_initial_directory(std::size_t key)
{
  std::string initial_directory;
  ::grpc::ClientContext context;
  data_models::GetInitialDirectoryRequest request;
  request.set_key(key);
  data_models::GetInitialDirectoryResponse response;
  grpc::Status status = _stub->GetInitialDirectory(&context, request, &response);
  if (status.ok()) {
    initial_directory = response.value();
  } else {
    std::stringstream err;
    err << "Error: ";
    err << status.error_code();
    RAY_LOG_ERR << err.str();
    throw std::runtime_error(err.str());
  }
  return initial_directory;
}

std::string ProcessRunnerServiceCaller::get_application_installation_directory()
{
  ::grpc::ClientContext context;
  data_models::GetApplicationInstallationDirectoryRequest request;
  data_models::GetApplicationInstallationDirectoryResponse response;
  grpc::Status status = _stub->GetApplicationInstallationDirectory(&context, request, &response);
  if (status.ok()) {
    return response.value();
  }
  std::stringstream err;
  err << "Error: ";
  err << status.error_code();
  RAY_LOG_ERR << err.str();
  throw std::runtime_error(err.str());
}
std::string ProcessRunnerServiceCaller::get_config_directory()
{
  ::grpc::ClientContext context;
  data_models::GetConfigDirectoryRequest request;
  data_models::GetConfigDirectoryResponse response;
  grpc::Status status = _stub->GetConfigDirectory(&context, request, &response);
  if (status.ok()) {
    return response.value();
  }
  std::stringstream err;
  err << "Error: ";
  err << status.error_code();
  RAY_LOG_ERR << err.str();
  throw std::runtime_error(err.str());
}
std::string ProcessRunnerServiceCaller::get_data_directory()
{
  ::grpc::ClientContext context;
  data_models::GetDataDirectoryRequest request;
  data_models::GetDataDirectoryResponse response;
  grpc::Status status = _stub->GetDataDirectory(&context, request, &response);
  if (status.ok()) {
    return response.value();
  }
  std::stringstream err;
  err << "Error: ";
  err << status.error_code();
  RAY_LOG_ERR << err.str();
  throw std::runtime_error(err.str());
}