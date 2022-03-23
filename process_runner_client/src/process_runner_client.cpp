// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Exception.h>

#include "logging.h"
#include "process_runner_client.h"

ProcessRunnerClient::ProcessRunnerClient(std::string command, std::vector<std::string> args,
                                         std::shared_ptr<ProcessRunnerServiceCaller> service_caller)
    : _command(std::move(command)), _args(std::move(args)), _service_caller(service_caller)
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
  _service_caller->signal_to_stop(_key);
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

void ProcessRunnerClient::run_process() { _key = _service_caller->run_process(_command, _args); }

bool ProcessRunnerClient::is_running()
{
  return _service_caller->is_running(_key);
}

int ProcessRunnerClient::get_last_exit_code()
{
  return _service_caller->get_last_exit_code(_key);
}
int ProcessRunnerClient::get_id()
{
  return _service_caller->get_id(_key);
}
std::string ProcessRunnerClient::get_composite_command()
{
  return _service_caller->get_composite_command(_key);
}
std::string ProcessRunnerClient::get_initial_directory()
{
  return _service_caller->get_initial_directory(_key);
}