// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef process_runner_h
#define process_runner_h
#include <Poco/Process.h>
#include <atomic>
#include <future>
#include <memory>
#include <mutex>
#include <thread>

#include "data_models_process_runner.h"
class ProcessRunner : public data_models::IProcessRunner
{
private:
  static std::string initial_directory;
  static std::string application_installation_directory;
  static std::string config_directory;
  static std::string data_directory;
  std::atomic_bool _do_shutdown{false};
  std::atomic_bool _is_internal_shutdown{false};
  std::atomic_int _last_exit_code{-1};
  bool _is_already_shutting_down{false};
  inline bool _do_shutdown_composite() { return (_do_shutdown || _is_internal_shutdown); }

  // std::unique_ptr<std::thread> _thread;
  std::unique_ptr<std::future<void>> _thread;

  std::string _command;
  std::vector<std::string> _args;
  std::string _initial_directory;

  std::string _composite_command{};
  std::unique_ptr<Poco::ProcessHandle> _process_handle;
  std::mutex _thread_running_mutex;
  std::condition_variable _thread_running_cv;
  bool _is_thread_running{false};
  void start();
  void stop();
  void run();
  static std::string get_current_directory();

public:
  static void set_initial_directory(const std::string& initial_directory_);
  static void set_application_installation_directory(const std::string& application_installation_directory_);
  static void set_config_directory(const std::string& config_directory_);
  static void set_data_directory(const std::string& data_directory_);

  ProcessRunner(std::string command, std::vector<std::string> args,
                std::string initial_directory = ProcessRunner::initial_directory);
  virtual ~ProcessRunner();
  void signal_to_stop();
  bool is_running();
  int get_last_exit_code();
  int get_id();
  std::string get_composite_command();
  std::string get_initial_directory();
  std::string get_application_installation_directory();
  std::string get_config_directory();
  std::string get_data_directory();
};
#endif // process_runner_h
