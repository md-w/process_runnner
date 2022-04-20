// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Exception.h>

#include "logging.h"
#include "process_runner.h"
#include "process_runner_client.h"
#include "utilities.h"

ProcessRunnerClient::ProcessRunnerClient(std::string application_installation_folder,
                                         std::string streaming_command_service_url, std::string rtmp_server_host,
                                         std::string data_dir, std::string customer_id, std::string camera_uuid,
                                         std::string camera_url, std::string camera_user_name,
                                         std::string camera_password, std::string stream_type)
    : _application_installation_folder(std::move(application_installation_folder)),
      _streaming_command_service_url(std::move(streaming_command_service_url)),
      _rtmp_server_host(std::move(rtmp_server_host)), _data_dir(std::move(data_dir)),
      _customer_id(std::move(customer_id)), _camera_uuid(std::move(camera_uuid)), _camera_url(std::move(camera_url)),
      _camera_user_name(std::move(camera_user_name)), _camera_password(std::move(camera_password)),
      _stream_type(std::move(stream_type))
{
  _thread = std::make_unique<std::thread>(&ProcessRunnerClient::run, this);
}

ProcessRunnerClient::~ProcessRunnerClient() { stop(); }

void ProcessRunnerClient::stop()
{
  // NOLINTNEXTLINE(clang-analyzer-optin.cplusplus.VirtualCall)
  signal_to_stop();
  if (_thread) {
    if (_thread->joinable()) {
      _thread->join();
    }
  }
}

void ProcessRunnerClient::signal_to_stop()
{
  if (_is_already_shutting_down) {
    return;
  }
  _is_already_shutting_down = true;
  _do_shutdown = true;
  // remote_signal_to_stop();
}

void ProcessRunnerClient::run()
{
  RAY_LOG_INF << "Thread Started for ProcessRunnerClient";
  std::string url = fmt::format("dns:///{}", _streaming_command_service_url);
  RAY_LOG_INF << "Trying to connect: " << url;
  std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(url, grpc::InsecureChannelCredentials());

  while (!_do_shutdown) {
    try {
      _service_caller = std::make_shared<ProcessRunnerServiceCaller>(channel);
      std::string remote_command =
          vtpl::utilities::merge_directories(get_application_installation_directory(), "media_converter");
      _live_stream_relative_url.emplace(
          fmt::format("{0}_HLS_SERVER_{1}/index.m3u8", vtpl::utilities::shorten_string(_camera_uuid),
                      vtpl::utilities::get_first_two_letters_in_upper_case(_stream_type)));
      std::string live_dir =
          vtpl::utilities::merge_directories(get_data_directory(), _live_stream_relative_url.value());
      int rtmp_port = get_number();

      std::vector<std::string> remote_args;

      remote_args.emplace_back("-i");
      remote_args.emplace_back(fmt::format("rtmp://0.0.0.0:{}", rtmp_port));
      remote_args.emplace_back("-o");
      remote_args.emplace_back(live_dir);
      remote_args.emplace_back("-u");
      remote_args.emplace_back(_camera_uuid);

      run_process(remote_command, remote_args, _camera_uuid);

      RAY_LOG_INF << "Remote Process Started for " << get_composite_command() << " From: " << get_initial_directory();

      // run local process runner <- livesteram_controller
      std::vector<std::string> client_process_runner_args;
      client_process_runner_args.emplace_back("-i");
      std::string composed_url = vtpl::utilities::compose_url(_camera_url, _camera_user_name, _camera_password);
      client_process_runner_args.emplace_back(fmt::format("{}", composed_url));
      client_process_runner_args.emplace_back("-o");
      client_process_runner_args.emplace_back(fmt::format("rtmp://{}:{}", _rtmp_server_host, rtmp_port));
      client_process_runner_args.emplace_back("-u");
      client_process_runner_args.emplace_back(_camera_uuid);

      std::unique_ptr<ProcessRunner> client_process_runner = std::make_unique<ProcessRunner>(
          vtpl::utilities::merge_directories(_application_installation_folder, "media_converter"),
          client_process_runner_args, _camera_uuid, _application_installation_folder);

      while (!_do_shutdown) {
        int exit_code = get_last_exit_code();
        if (exit_code != 0) {
          RAY_LOG_INF << "Remote Process returned with:: " << exit_code;
          break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
      client_process_runner = nullptr;
    } catch (const std::exception& e) {
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  remote_signal_to_stop();
}

void ProcessRunnerClient::run_process(const std::string& remote_command, const std::vector<std::string>& remote_args,
                                      const std::string& unique_id)
{
  _key = _service_caller->run_process(remote_command, remote_args, unique_id);
}

void ProcessRunnerClient::remote_signal_to_stop() { _service_caller->signal_to_stop(_key); }
bool ProcessRunnerClient::is_running() { return _service_caller->is_running(_key); }
int ProcessRunnerClient::get_last_exit_code() { return _service_caller->get_last_exit_code(_key); }
int ProcessRunnerClient::get_id() { return _service_caller->get_id(_key); }
std::string ProcessRunnerClient::get_composite_command() { return _service_caller->get_composite_command(_key); }
std::string ProcessRunnerClient::get_initial_directory() { return _service_caller->get_initial_directory(_key); }
int ProcessRunnerClient::get_number() { return _service_caller->get_next_number(_camera_uuid); }
std::string ProcessRunnerClient::get_application_installation_directory()
{
  return _service_caller->get_application_installation_directory();
}
std::string ProcessRunnerClient::get_data_directory() { return _service_caller->get_data_directory(); }