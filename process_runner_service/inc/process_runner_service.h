// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef process_runner_service_h
#define process_runner_service_h

#include <chrono>
#include <grpc/grpc.h>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>

#include "data_models_process_runner.h"
#include "protobuf_helper.h"

class ProcessRunnerService final : public data_models::ProcessRunner::Service
{
private:
  std::mutex _process_runner_map_mtx;
  std::mutex _process_runner_last_keep_alive_map_mtx;
  std::mutex _blocked_number_keep_block_map_mtx;
  std::map<std::size_t, std::unique_ptr<data_models::IProcessRunner>> process_runner_map;
  std::map<std::size_t, std::chrono::high_resolution_clock::time_point> _process_runner_last_keep_alive_map;
  std::map<int, std::chrono::high_resolution_clock::time_point> _blocked_number_keep_block_map;
  std::string _application_installation_directory;
  std::string _config_directory;
  std::string _data_directory;
  std::atomic_bool _do_shutdown{false};
  std::unique_ptr<std::thread> _thread;
  int _number_start;
  int _number_end;
  /**
   * This function runs in a different thread and monitors if the IsRunning called at regular interval
   * If the keep alive is not called then the process_runner is removed from the list
   */
  void monitor();
  bool _signal_to_stop(std::size_t key);
  void _set_keep_alive(std::size_t key);
  std::optional<bool> _is_running(std::size_t key);
  std::optional<int> _get_last_exit_code(std::size_t key);
  std::optional<int> _get_id(std::size_t key);
  std::optional<std::string> _get_composite_command(std::size_t key);
  std::optional<std::string> _get_initial_directory(std::size_t key);
  int get_usable_number();

public:
  int get_usable_number(const std::string& key, int start, int end, const std::string& file_name);

  ProcessRunnerService(std::string application_installation_directory, std::string config_directory,
                       std::string data_directory, int number_start, int number_end);
  ~ProcessRunnerService();

  ::grpc::Status
  GetApplicationInstallationDirectory(::grpc::ServerContext* context,
                                      const data_models::GetApplicationInstallationDirectoryRequest* request,
                                      data_models::GetApplicationInstallationDirectoryResponse* response) override;

  ::grpc::Status GetConfigDirectory(::grpc::ServerContext* context,
                                    const data_models::GetConfigDirectoryRequest* request,
                                    data_models::GetConfigDirectoryResponse* response) override;

  ::grpc::Status GetDataDirectory(::grpc::ServerContext* context, const data_models::GetDataDirectoryRequest* request,
                                  data_models::GetDataDirectoryResponse* response) override;

  ::grpc::Status RunProcess(::grpc::ServerContext* context, const data_models::RunProcessRequest* request,
                            data_models::RunProcessResponse* response) override;

  ::grpc::Status SignalToStop(::grpc::ServerContext* context, const data_models::SignalToStopRequest* request,
                              data_models::SignalToStopResponse* response) override;

  ::grpc::Status IsRunning(::grpc::ServerContext* context, const data_models::IsRunningRequest* request,
                           data_models::IsRunningResponse* response) override;

  ::grpc::Status GetLastExitCode(::grpc::ServerContext* context, const data_models::GetLastExitCodeRequest* request,
                                 data_models::GetLastExitCodeResponse* response) override;

  ::grpc::Status GetId(::grpc::ServerContext* context, const data_models::GetIdRequest* request,
                       data_models::GetIdResponse* response) override;

  ::grpc::Status GetCompositeCommand(::grpc::ServerContext* context,
                                     const data_models::GetCompositeCommandRequest* request,
                                     data_models::GetCompositeCommandResponse* response) override;

  ::grpc::Status GetInitialDirectory(::grpc::ServerContext* context,
                                     const data_models::GetInitialDirectoryRequest* request,
                                     data_models::GetInitialDirectoryResponse* response) override;

  ::grpc::Status GetNextNumber(::grpc::ServerContext* context, const data_models::GetNextNumberRequest* request,
                               data_models::GetNextNumberResponse* response) override;
};

#endif // process_runner_service_h
