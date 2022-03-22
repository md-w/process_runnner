// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "process_runner_service.h"
#include "logging.h"

::grpc::Status ProcessRunnerService::RunProcess(::grpc::ServerContext* context,
                                                const data_models::RunProcessRequest* request,
                                                data_models::RunProcessResponse* response)
{
  data_models::ProcessRunnerArguments process_runner_arguments;
  process_runner_arguments.command = request->command();
  for (auto&& arg : request->args()) {
    process_runner_arguments.args.push_back(arg);
  }
  std::size_t key = data_models::hash_value(process_runner_arguments);
  if (process_runner_map.find(key) == process_runner_map.end()) {
    RAY_LOG_INF << "Adding new process runner " << key;
    try {
      process_runner_map.emplace(std::pair(
          key, std::make_unique<ProcessRunner>(process_runner_arguments.command, process_runner_arguments.args)));

    } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
      return ::grpc::Status::CANCELLED;
    }

  } else {
    RAY_LOG_INF << "Key alrady exists";
  }
  response->set_exit_code(process_runner_map.at(key)->get_last_exit_code());
  response->set_message("Started");
  response->set_key(key);
  response->set_error_code(0);
  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::SignalToStop(::grpc::ServerContext* context,
                                                  const data_models::SignalToStopRequest* request,
                                                  data_models::SignalToStopResponse* response)
{
  bool is_found = false;
  std::size_t key = request->key();
  if (process_runner_map.find(key) != process_runner_map.end()) {
    process_runner_map.at(key)->signal_to_stop();
    process_runner_map.erase(key);
    is_found = true;
    RAY_LOG_INF << "Removing process runner " << key;
  }

  if (is_found) {
    response->set_error_code(0);
  } else {
    response->set_error_code(1);
  }
  response->set_key(key);

  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::IsRunning(::grpc::ServerContext* context,
                                               const data_models::IsRunningRequest* request,
                                               data_models::IsRunningResponse* response)
{
  bool is_found = false;
  std::size_t key = request->key();
  if (process_runner_map.find(key) != process_runner_map.end()) {
    response->set_value(process_runner_map.at(key)->is_running());
    is_found = true;
  }
  if (is_found) {
    response->set_error_code(0);
  } else {
    response->set_error_code(1);
  }
  response->set_key(key);
  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::GetLastExitCode(::grpc::ServerContext* context,
                                                     const data_models::GetLastExitCodeRequest* request,
                                                     data_models::GetLastExitCodeResponse* response)
{
  bool is_found = false;
  std::size_t key = request->key();
  if (process_runner_map.find(key) != process_runner_map.end()) {
    response->set_value(process_runner_map.at(key)->get_last_exit_code());
    is_found = true;
  }
  if (is_found) {
    response->set_error_code(0);
  } else {
    response->set_error_code(1);
  }
  response->set_key(key);

  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::GetId(::grpc::ServerContext* context, const data_models::GetIdRequest* request,
                                           data_models::GetIdResponse* response)
{
  bool is_found = false;
  std::size_t key = request->key();
  if (process_runner_map.find(key) != process_runner_map.end()) {
    response->set_value(process_runner_map.at(key)->get_id());
    is_found = true;
  }
  if (is_found) {
    response->set_error_code(0);
  } else {
    response->set_error_code(1);
  }
  response->set_key(key);

  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::GetCompositeCommand(::grpc::ServerContext* context,
                                                         const data_models::GetCompositeCommandRequest* request,
                                                         data_models::GetCompositeCommandResponse* response)
{
  bool is_found = false;
  std::size_t key = request->key();
  if (process_runner_map.find(key) != process_runner_map.end()) {
    response->set_value(process_runner_map.at(key)->get_composite_command());
    is_found = true;
  }
  if (is_found) {
    response->set_error_code(0);
  } else {
    response->set_error_code(1);
  }
  response->set_key(key);

  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::GetInitialDirectory(::grpc::ServerContext* context,
                                                         const data_models::GetInitialDirectoryRequest* request,
                                                         data_models::GetInitialDirectoryResponse* response)
{
  bool is_found = false;
  std::size_t key = request->key();
  if (process_runner_map.find(key) != process_runner_map.end()) {
    response->set_value(process_runner_map.at(key)->get_initial_directory());
    is_found = true;
  }
  if (is_found) {
    response->set_error_code(0);
  } else {
    response->set_error_code(1);
  }
  response->set_key(key);

  return ::grpc::Status::OK;
}