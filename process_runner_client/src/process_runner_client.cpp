// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Exception.h>

#include "logging.h"
#include "process_runner_client.h"

ProcessRunnerClient::ProcessRunnerClient(std::string command, std::vector<std::string> args,
                                         std::shared_ptr<grpc::Channel> channel)
    : _command(std::move(command)), _args(std::move(args))
{
  std::stringstream ss;
  ss << "[";
  ss << _command;
  for (const auto& piece : _args) {
    ss << " ";
    ss << piece;
  }
  ss << "]";

  _composite_command = ss.str();

  _stub = data_models::ProcessRunner::NewStub(channel);
  _thread = std::make_unique<std::thread>(&ProcessRunnerClient::run, this);
}

ProcessRunnerClient::~ProcessRunnerClient() { stop(); }

void ProcessRunnerClient::stop()
{
  signal_to_stop();
  if (_thread) {
    if (_thread->joinable()) {
      _thread->join();
    }
  }
}

void ProcessRunnerClient::signal_to_stop()
{
  if (_is_already_shutting_down)
    return;
  _is_already_shutting_down = true;
  _do_shutdown = true;
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

void ProcessRunnerClient::run()
{
  RAY_LOG_INF << "Thread Started for " << _composite_command;
  while (!_do_shutdown) {
    try {
      run_process();
      RAY_LOG_INF << "Process Started for " << get_composite_command() << " From: " << get_initial_directory();
      while (!_do_shutdown) {
        int exit_code = get_last_exit_code();
        if (exit_code != 0) {
          RAY_LOG_INF << "Process returned with:: " << exit_code;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    } catch (const std::exception& e) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
}

void ProcessRunnerClient::run_process()
{
  if (_stub) {
    ::grpc::ClientContext context;
    data_models::RunProcessRequest request;
    request.set_command(_command);
    *request.mutable_args() = {_args.begin(), _args.end()};
    data_models::RunProcessResponse response;
    grpc::Status status = _stub->RunProcess(&context, request, &response);
    if (status.ok()) {
      _key = response.key();
      RAY_LOG_INF << "Started with key: " << _key;
    } else {
      std::stringstream err;
      err << "Error: ";
      err << status.error_code();
      RAY_LOG_ERR << err.str();
      throw std::runtime_error(err.str());
    }
  }
}

bool ProcessRunnerClient::is_running()
{
  bool is_running = false;
  if (_stub) {
    ::grpc::ClientContext context;
    data_models::IsRunningRequest request;
    request.set_key(_key);
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
  }
  return is_running;
}

int ProcessRunnerClient::get_last_exit_code()
{
  int exit_code = -1;
  if (_stub) {
    ::grpc::ClientContext context;
    data_models::GetLastExitCodeRequest request;
    request.set_key(_key);
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
  }
  return exit_code;
}
int ProcessRunnerClient::get_id()
{
  int id = 0;
  if (_stub) {
    ::grpc::ClientContext context;
    data_models::GetIdRequest request;
    request.set_key(_key);
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
  }
  return id;
}
std::string ProcessRunnerClient::get_composite_command()
{
  std::string composite_command;
  if (_stub) {
    ::grpc::ClientContext context;
    data_models::RunProcessRequest request;
    request.set_command(_command);
    *request.mutable_args() = {_args.begin(), _args.end()};
    data_models::RunProcessResponse response;
    grpc::Status status = _stub->RunProcess(&context, request, &response);
    if (status.ok()) {
      _key = response.key();
    } else {
      std::stringstream err;
      err << "Error: ";
      err << status.error_code();
      RAY_LOG_ERR << err.str();
      throw std::runtime_error(err.str());
    }
  }

  return composite_command;
}
std::string ProcessRunnerClient::get_initial_directory()
{
  std::string initial_directory;
  if (_stub) {
    ::grpc::ClientContext context;
    data_models::GetInitialDirectoryRequest request;
    request.set_key(_key);
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
  }
  return initial_directory;
}