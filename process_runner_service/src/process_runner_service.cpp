// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#include <Poco/Util/JSONConfiguration.h>
#include <fstream>

#include "logging.h"
#include "process_runner_service.h"
#include "utilities.h"

ProcessRunnerService::ProcessRunnerService(std::string application_installation_directory, std::string config_directory,
                                           std::string data_directory, int number_start, int number_end)
    : _application_installation_directory(std::move(application_installation_directory)),
      _config_directory(std::move(config_directory)), _data_directory(std::move(data_directory)),
      _number_start(number_start), _number_end(number_end)
{
  ProcessRunner::set_application_installation_directory(_application_installation_directory);
  ProcessRunner::set_config_directory(_config_directory);
  ProcessRunner::set_data_directory(_data_directory);
  ProcessRunner::set_initial_directory(_application_installation_directory);
  _thread = std::make_unique<std::thread>(&ProcessRunnerService::monitor, this);
}

ProcessRunnerService::~ProcessRunnerService()
{
  _do_shutdown = true;
  if (_thread->joinable()) {
    _thread->join();
  }
}
void ProcessRunnerService::_set_keep_alive(std::size_t key)
{

  std::lock_guard<std::mutex> lock(_process_runner_last_keep_alive_map_mtx);
  if (_process_runner_last_keep_alive_map.find(key) == _process_runner_last_keep_alive_map.end()) {
    _process_runner_last_keep_alive_map.emplace(std::pair(key, std::chrono::high_resolution_clock::now()));
  } else {
    _process_runner_last_keep_alive_map.at(key) = std::chrono::high_resolution_clock::now();
  }
}
std::optional<bool> ProcessRunnerService::_is_running(std::size_t key)
{
  _set_keep_alive(key);
  std::optional<bool> ret_val;
  std::lock_guard<std::mutex> lock(_process_runner_map_mtx);
  if (process_runner_map.find(key) != process_runner_map.end()) {
    ret_val.emplace(process_runner_map.at(key)->is_running());
  }
  return ret_val;
}
std::optional<int> ProcessRunnerService::_get_last_exit_code(std::size_t key)
{
  _set_keep_alive(key);
  std::optional<int> ret_val;
  std::lock_guard<std::mutex> lock(_process_runner_map_mtx);
  if (process_runner_map.find(key) != process_runner_map.end()) {
    ret_val.emplace(process_runner_map.at(key)->get_last_exit_code());
  }
  return ret_val;
}
std::optional<int> ProcessRunnerService::_get_id(std::size_t key)
{
  _set_keep_alive(key);
  std::optional<int> ret_val;
  std::lock_guard<std::mutex> lock(_process_runner_map_mtx);
  if (process_runner_map.find(key) != process_runner_map.end()) {
    ret_val.emplace(process_runner_map.at(key)->get_id());
  }
  return ret_val;
}
std::optional<std::string> ProcessRunnerService::_get_composite_command(std::size_t key)
{
  _set_keep_alive(key);
  std::optional<std::string> ret_val;
  std::lock_guard<std::mutex> lock(_process_runner_map_mtx);
  if (process_runner_map.find(key) != process_runner_map.end()) {
    ret_val.emplace(process_runner_map.at(key)->get_composite_command());
  }
  return ret_val;
}
std::optional<std::string> ProcessRunnerService::_get_initial_directory(std::size_t key)
{
  _set_keep_alive(key);
  std::optional<std::string> ret_val;
  std::lock_guard<std::mutex> lock(_process_runner_map_mtx);
  if (process_runner_map.find(key) != process_runner_map.end()) {
    ret_val.emplace(process_runner_map.at(key)->get_initial_directory());
  }
  return ret_val;
}

int ProcessRunnerService::get_usable_number(const std::string& key)
{
  int config_file_save_counter = 0;
  std::string file_name = vtpl::utilities::merge_directories(_config_directory, "key_file_map.json");
  Poco::AutoPtr<Poco::Util::JSONConfiguration> config;
  if (vtpl::utilities::is_regular_file_exists(file_name)) {
    config->load(file_name);
  } else {
    config_file_save_counter++;
  }
  std::vector<int> usable_port =
  // std::vector<std::string> keys;
  // config->keys(keys);
  // if (!config->has(key)) {

  // }
  if (config_file_save_counter > 0) {
    std::ofstream out_file = std::ofstream(file_name);
    config->save(out_file);
  }
  return 0;
}

int ProcessRunnerService::get_usable_number()
{
  int ret_val = 0;
  bool is_usable = false;
  for (int number = _number_start; number <= _number_end; number++) {
    {
      std::lock_guard<std::mutex> lock(_blocked_number_keep_block_map_mtx);
      if (_blocked_number_keep_block_map.find(number) == _blocked_number_keep_block_map.end()) {
        is_usable = true;
      }
    }
    if (is_usable) {
      std::lock_guard<std::mutex> lock(_process_runner_map_mtx);

      for (auto&& v : process_runner_map) {
        if (v.second->get_number() == number) {
          is_usable = false;
          break;
        }
      }
    }
    if (is_usable) {
      std::lock_guard<std::mutex> lock(_blocked_number_keep_block_map_mtx);
      _blocked_number_keep_block_map.emplace(std::pair(number, std::chrono::high_resolution_clock::now()));
      ret_val = number;
      break;
    }
  }
  return ret_val;
}
::grpc::Status ProcessRunnerService::RunProcess(::grpc::ServerContext* /*context*/,
                                                const data_models::RunProcessRequest* request,
                                                data_models::RunProcessResponse* response)
{
  data_models::ProcessRunnerArguments process_runner_arguments;
  process_runner_arguments.command = request->command();
  for (auto&& arg : request->args()) {
    process_runner_arguments.args.push_back(arg);
  }
  std::size_t key = data_models::hash_value(process_runner_arguments);
  _set_keep_alive(key);
  int number = request->number();
  bool is_found = false;
  {
    std::lock_guard<std::mutex> lock(_process_runner_map_mtx);
    if (process_runner_map.find(key) == process_runner_map.end()) {
      try {
        process_runner_map.emplace(
            std::pair(key, std::make_unique<ProcessRunner>(process_runner_arguments.command,
                                                           process_runner_arguments.args, number)));

      } catch (const std::exception& e) {
        RAY_LOG_ERR << "Error adding new process: " << key << " : " << e.what();
        return ::grpc::Status::CANCELLED;
      }
    } else {
      is_found = true;
    }
    response->set_exit_code(process_runner_map.at(key)->get_last_exit_code());
  }
  {
    std::lock_guard<std::mutex> lock(_blocked_number_keep_block_map_mtx);
    if (_blocked_number_keep_block_map.find(number) != _blocked_number_keep_block_map.end()) {
      _blocked_number_keep_block_map.erase(number);
    }
  }
  response->set_message("Started");
  response->set_key(key);
  response->set_error_code(0);
  if (is_found) {
    RAY_LOG_INF << "Adding new process runner " << key;
  } else {
    RAY_LOG_INF << "Key alrady exists " << key;
  }
  return ::grpc::Status::OK;
}

void ProcessRunnerService::monitor()
{
  while (!_do_shutdown) {
    std::vector<std::size_t> keys_to_delete;
    auto now = std::chrono::high_resolution_clock::now();
    {
      std::lock_guard<std::mutex> lock(_process_runner_last_keep_alive_map_mtx);
      for (auto&& v : _process_runner_last_keep_alive_map) {
        auto stale_duration = std::chrono::duration_cast<std::chrono::seconds>(now - v.second).count();
        if (stale_duration > 10) {
          keys_to_delete.emplace_back(v.first);
        }
      }
    }
    for (auto&& key : keys_to_delete) {
      _signal_to_stop(key);
    }
    {
      std::lock_guard<std::mutex> lock(_blocked_number_keep_block_map_mtx);
      for (auto it = _blocked_number_keep_block_map.begin(); it != _blocked_number_keep_block_map.end();) {
        auto stale_duration = std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count();
        if (stale_duration > 10) {
          it = _blocked_number_keep_block_map.erase(it);
        } else {
          it++;
        }
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

bool ProcessRunnerService::_signal_to_stop(std::size_t key)
{
  RAY_LOG_INF << "Remove request received for " << key;
  {
    std::lock_guard<std::mutex> lock(_process_runner_last_keep_alive_map_mtx);
    if (_process_runner_last_keep_alive_map.find(key) != _process_runner_last_keep_alive_map.end()) {
      _process_runner_last_keep_alive_map.erase(key);
    }
  }
  bool is_found = false;
  {
    std::lock_guard<std::mutex> lock(_process_runner_map_mtx);
    if (process_runner_map.find(key) != process_runner_map.end()) {
      process_runner_map.at(key)->signal_to_stop();
      process_runner_map.erase(key);
      is_found = true;
    }
  }
  if (is_found) {
    RAY_LOG_INF << "Removed process runner " << key;
  }
  return is_found;
}

::grpc::Status ProcessRunnerService::SignalToStop(::grpc::ServerContext* /*context*/,
                                                  const data_models::SignalToStopRequest* request,
                                                  data_models::SignalToStopResponse* response)
{
  std::size_t key = request->key();
  bool is_found = _signal_to_stop(key);
  if (is_found) {
    response->set_error_code(0);
  } else {
    response->set_error_code(1);
  }
  response->set_key(key);

  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::IsRunning(::grpc::ServerContext* /*context*/,
                                               const data_models::IsRunningRequest* request,
                                               data_models::IsRunningResponse* response)
{
  bool is_found = false;
  std::size_t key = request->key();
  bool is_running = false;
  std::optional<bool> ret_val = _is_running(key);
  if (ret_val) {
    is_found = true;
    is_running = *ret_val;
  }
  if (is_found) {
    response->set_error_code(0);
    response->set_value(is_running);
  } else {
    response->set_error_code(1);
  }
  response->set_key(key);
  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::GetLastExitCode(::grpc::ServerContext* /*context*/,
                                                     const data_models::GetLastExitCodeRequest* request,
                                                     data_models::GetLastExitCodeResponse* response)
{
  bool is_found = false;
  std::size_t key = request->key();
  int get_last_exit_code = 0;
  std::optional<int> ret_val = _get_last_exit_code(key);
  if (ret_val) {
    is_found = true;
    get_last_exit_code = *ret_val;
  }
  if (is_found) {
    response->set_error_code(0);
    response->set_value(get_last_exit_code);
  } else {
    response->set_error_code(1);
  }
  response->set_key(key);

  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::GetId(::grpc::ServerContext* /*context*/, const data_models::GetIdRequest* request,
                                           data_models::GetIdResponse* response)
{
  bool is_found = false;
  std::size_t key = request->key();
  int get_id = 0;
  std::optional<int> ret_val = _get_id(key);
  if (ret_val) {
    is_found = true;
    get_id = *ret_val;
  }

  if (is_found) {
    response->set_error_code(0);
    response->set_value(get_id);
  } else {
    response->set_error_code(1);
    response->set_value(get_id);
  }
  response->set_key(key);

  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::GetCompositeCommand(::grpc::ServerContext* /*context*/,
                                                         const data_models::GetCompositeCommandRequest* request,
                                                         data_models::GetCompositeCommandResponse* response)
{
  bool is_found = false;
  std::size_t key = request->key();
  std::string get_composite_command;
  std::optional<std::string> ret_val = _get_composite_command(key);
  if (ret_val) {
    is_found = true;
    get_composite_command = *ret_val;
  }

  if (is_found) {
    response->set_error_code(0);
    response->set_value(get_composite_command);
  } else {
    response->set_error_code(1);
  }
  response->set_key(key);

  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::GetInitialDirectory(::grpc::ServerContext* /*context*/,
                                                         const data_models::GetInitialDirectoryRequest* request,
                                                         data_models::GetInitialDirectoryResponse* response)
{
  bool is_found = false;
  std::size_t key = request->key();
  std::string get_initial_directory;
  std::optional<std::string> ret_val = _get_initial_directory(key);
  if (ret_val) {
    is_found = true;
    get_initial_directory = *ret_val;
  }
  if (is_found) {
    response->set_error_code(0);
    response->set_value(get_initial_directory);
  } else {
    response->set_error_code(1);
  }
  response->set_key(key);

  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::GetApplicationInstallationDirectory(
    ::grpc::ServerContext* /*context*/, const data_models::GetApplicationInstallationDirectoryRequest* /*request*/,
    data_models::GetApplicationInstallationDirectoryResponse* response)
{
  response->set_error_code(0);
  response->set_value(_application_installation_directory);
  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::GetConfigDirectory(::grpc::ServerContext* /*context*/,
                                                        const data_models::GetConfigDirectoryRequest* /*request*/,
                                                        data_models::GetConfigDirectoryResponse* response)
{
  response->set_error_code(0);
  response->set_value(_config_directory);
  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::GetDataDirectory(::grpc::ServerContext* /*context*/,
                                                      const data_models::GetDataDirectoryRequest* /*request*/,
                                                      data_models::GetDataDirectoryResponse* response)
{
  response->set_error_code(0);
  response->set_value(_data_directory);
  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::GetNextNumber(::grpc::ServerContext* /*context*/,
                                                   const data_models::GetNextNumberRequest* /*request*/,
                                                   data_models::GetNextNumberResponse* response)
{
  response->set_error_code(0);
  int number = get_usable_number();
  RAY_LOG_INF << "Returning usable number as: " << number;
  response->set_value(number);
  return ::grpc::Status::OK;
}