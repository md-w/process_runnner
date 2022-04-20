// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef process_runner_client_h
#define process_runner_client_h
#include <atomic>
#include <future>
#include <grpcpp/grpcpp.h>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "data_models_process_runner.h"
#include "process_runner_service_caller.h"
#include "protobuf_helper.h"

class ProcessRunnerClient : public data_models::IProcessRunner
{
private:
  std::string _application_installation_folder;
  std::string _streaming_command_service_url;
  std::string _rtmp_server_host;
  std::string _data_dir;
  std::string _customer_id;
  std::string _camera_uuid;
  std::string _camera_url;
  std::string _camera_user_name;
  std::string _camera_password;
  std::string _stream_type;
  int _rtmp_port{-1};

  std::size_t _key{0};
  std::atomic_bool _do_shutdown{false};
  bool _is_already_shutting_down{false};
  std::shared_ptr<ProcessRunnerServiceCaller> _service_caller;
  std::unique_ptr<std::thread> _thread;
  void run();
  void stop();

  std::optional<std::string> _live_stream_relative_url;

public:
  ProcessRunnerClient(std::string application_installation_folder, std::string streaming_command_service_url,
                      std::string rtmp_server_host, std::string data_dir, std::string customer_id,
                      std::string camera_uuid, std::string camera_url, std::string camera_user_name,
                      std::string camera_password, std::string stream_type);
  virtual ~ProcessRunnerClient();

  void signal_to_stop();
  void remote_signal_to_stop();
  void run_process(const std::string& remote_command, const std::vector<std::string>& remote_args,
                   const std::string& unique_id);
  bool is_running();
  int get_last_exit_code();
  int get_id();
  int get_number();

  std::string get_composite_command();
  std::string get_initial_directory();
  std::string get_application_installation_directory();
  std::string get_data_directory();

  std::optional<std::string> get_live_stream_relative_url();
  std::optional<int> get_rtmp_port();
};

#endif // process_runner_client_h
