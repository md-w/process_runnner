// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#include <Poco/Exception.h>
#include <Poco/Path.h>
#include <sstream>

#include "logging.h"
#include "process_runner.h"
std::string ProcessRunner::_get_current_directory()
{
  try {
    return Poco::Path::current();
  } catch (const std::exception& e) {
  }
  return "./";
}
// NOLINTNEXTLINE(cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
std::string ProcessRunner::initial_directory{_get_current_directory()};
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
std::string ProcessRunner::application_installation_directory{ProcessRunner::initial_directory};
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
std::string ProcessRunner::config_directory{ProcessRunner::initial_directory};
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables)
std::string ProcessRunner::data_directory{ProcessRunner::initial_directory};

void ProcessRunner::set_initial_directory(const std::string& initial_directory_)
{
  initial_directory = initial_directory_;
}

void ProcessRunner::set_application_installation_directory(const std::string& application_installation_directory_)
{
  application_installation_directory = application_installation_directory_;
}

void ProcessRunner::set_config_directory(const std::string& config_directory_) { config_directory = config_directory_; }

void ProcessRunner::set_data_directory(const std::string& data_directory_) { data_directory = data_directory_; }

ProcessRunner::ProcessRunner(std::string command, std::vector<std::string> args, int number,
                             std::string initial_directory)
    : _command(std::move(command)), _args(std::move(args)), _initial_directory(std::move(initial_directory)),
      _number(number)
{
  std::stringstream ss;
  ss << "[";
  ss << _command;
  for (const auto& piece : _args) {
    ss << " ";
    ss << piece;
  }
  ss << "] number: " << _number;

  _composite_command = ss.str();
  // _thread = std::make_unique<std::thread>(&ProcessRunner::run, this);
  _thread = std::make_unique<std::future<void>>(std::async(std::launch::async, &ProcessRunner::run, this));
}

ProcessRunner::~ProcessRunner()
{
  _signal_to_stop();
  // if (_thread->joinable()) {
  //   _thread->join();
  // }
  if (_thread->wait_for(std::chrono::seconds(2)) == std::future_status::timeout) {
    RAY_LOG_INF << "Thread not yet terminated, forcing, for " << _composite_command << " from: " << _initial_directory;
    if (_process_handle) {
      Poco::Process::kill(*_process_handle);
    }
  }
  _thread->wait();
}

void ProcessRunner::signal_to_stop() { _signal_to_stop(); }

void ProcessRunner::_signal_to_stop()
{
  if (_is_already_shutting_down) {
    return;
  }
  _is_already_shutting_down = true;
  _do_shutdown = true;
  int id = _get_id();
  if (id > 0) {
    Poco::Process::requestTermination(id);
  }
}

bool ProcessRunner::is_running() { return get_id() > 0; }

int ProcessRunner::get_last_exit_code() { return _last_exit_code; }

int ProcessRunner::get_id() { return _get_id(); }

int ProcessRunner::_get_id()
{
  std::unique_lock<std::mutex> lock_thread_running(_thread_running_mutex);
  _thread_running_cv.wait(lock_thread_running, [this] { return _is_thread_running; });

  if (_process_handle) {
    return _process_handle->id();
  }
  return -1;
}

std::string ProcessRunner::get_composite_command() { return _composite_command; }

std::string ProcessRunner::get_initial_directory() { return _initial_directory; }

void ProcessRunner::run()
{
  RAY_LOG_INF << "Thread Started for " << _composite_command << " from: " << _initial_directory;
  while (!_do_shutdown) {
    RAY_LOG_INF << "Starting process " << _composite_command << " from: " << _initial_directory;
    try {
      _process_handle =
          std::make_unique<Poco::ProcessHandle>(Poco::Process::launch(_command, _args, _initial_directory));
    } catch (Poco::Exception& e) {
      RAY_LOG_ERR << "MONOTOSH:: Poco::Exception " << e.what();
    } catch (const std::exception& e) {
      RAY_LOG_ERR << "MONOTOSH:: " << e.what();
    }
    {
      std::lock_guard<std::mutex> lock_thread_running(_thread_running_mutex);

      _is_thread_running = true;
    }
    _thread_running_cv.notify_all();
    if (_process_handle) {
      if (_process_handle->id() > 0) {
        _last_exit_code = 0;
        _last_exit_code = _process_handle->wait();
        RAY_LOG_INF << "Process returned with:: " << _last_exit_code;
      }
      _process_handle = nullptr;
    } else {
      // break; keep on trying
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  RAY_LOG_INF << "Thread Stopped for " << _composite_command << " from: " << _initial_directory;
}

int ProcessRunner::get_number() { return _number; }