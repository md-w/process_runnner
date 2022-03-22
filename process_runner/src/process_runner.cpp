// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Exception.h>
#include <Poco/Path.h>
#include <sstream>

#include "logging.h"
#include "process_runner.h"

std::string ProcessRunner::initial_directory{Poco::Path::current()};
void ProcessRunner::set_initial_directory(const std::string& initial_directory_)
{
  initial_directory = initial_directory_;
}
std::string ProcessRunner::get_initial_directory() { return _initial_directory; }

ProcessRunner::ProcessRunner(std::string command, std::vector<std::string> args, std::string initial_directory)
    : _command(std::move(command)), _args(std::move(args)), _initial_directory(std::move(initial_directory))
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
  start();
}
ProcessRunner::~ProcessRunner() { stop(); }
void ProcessRunner::start()
{
  //_thread = std::make_unique<std::thread>(&ProcessRunner::run, this);
  _thread = std::make_unique<std::future<void>>(std::async(std::launch::async, &ProcessRunner::run, this));
}
void ProcessRunner::signal_to_stop()
{
  int id = get_id();
  _do_shutdown = true;
  RAY_LOG_INF << "signal_to_stop called";
  if (id > 0) {
    Poco::Process::requestTermination(id);
  }
}
void ProcessRunner::stop()
{
  if (_is_already_shutting_down) {
    return;
  }
  _is_already_shutting_down = true;
  signal_to_stop();

  // if (_thread) {
  //   if (_thread->joinable()) {
  //     _thread->join();
  //   }
  // }
  if (_thread) {
    if (_thread->wait_for(std::chrono::seconds(2)) == std::future_status::timeout) {
      if (_process_handle) {
        Poco::Process::kill(*_process_handle);
      }
      _thread->wait();
    }
    _thread = nullptr;
  }
}
void ProcessRunner::run()
{

  RAY_LOG_INF << "Thread Started for " << _composite_command << " from: " << _initial_directory;
  while (!_do_shutdown_composite()) {
    RAY_LOG_INF << "Starting process " << _composite_command << " from: " << _initial_directory;
    {
      std::lock_guard<std::mutex> lock_thread_running(_thread_running_mutex);
      try {
        _process_handle =
            std::make_unique<Poco::ProcessHandle>(Poco::Process::launch(_command, _args, _initial_directory));
      } catch (Poco::Exception& e) {
        RAY_LOG_ERR << "MONOTOSH:: Poco::Exception " << e.what();
      } catch (const std::exception& e) {
        RAY_LOG_ERR << "MONOTOSH:: " << e.what();
      }
      _is_thread_running = true;
      _thread_running_cv.notify_all();
    }

    if (_process_handle) {
      if (_process_handle->id() > 0) {
        _last_exit_code = 0;
        _last_exit_code = _process_handle->wait();
        RAY_LOG_INF << "Process returned with:: " << _last_exit_code;
        _process_handle = nullptr;
      }
    } else {
      break;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  // try {
  //   if (_process_handle) {
  //     if (_process_handle->id() > 0) {
  //       Poco::Process::kill(_process_handle->id());
  //     }
  //   }
  // } catch (Poco::Exception& exc) {
  //   RAY_LOG_ERR << "Killing process  failed" << _composite_command << " " << exc.displayText();
  // } catch (std::exception& exc) {
  //   RAY_LOG_ERR << "Killing process  failed" << _composite_command << " " << exc.what();
  // } catch (...) {
  //   RAY_LOG_ERR << "Killing process  failed" << _composite_command;
  // }
  // RAY_LOG_INF << "Thread Stopped for " << _composite_command;
}

bool ProcessRunner::is_running() { return get_id() > 0; }

int ProcessRunner::get_last_exit_code() { return _last_exit_code; }

int ProcessRunner::get_id()
{
  std::unique_lock<std::mutex> lock_thread_running(_thread_running_mutex);
  if (!_is_thread_running) {
    _thread_running_cv.wait(lock_thread_running);
  }
  if (_process_handle) {
    return _process_handle->id();
  }
  return -1;
}
std::string ProcessRunner::get_composite_command() { return _composite_command; }