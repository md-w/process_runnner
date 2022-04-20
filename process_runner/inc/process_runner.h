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

  std::string _command;
  std::vector<std::string> _args;
  std::string _initial_directory;
  std::atomic_int _last_exit_code{-1};
  std::string _composite_command;

  // std::unique_ptr<std::thread> _thread;
  std::unique_ptr<std::future<void>> _thread;
  std::atomic_bool _do_shutdown{false};

  std::unique_ptr<Poco::ProcessHandle> _process_handle;
  std::mutex _thread_running_mutex;
  std::condition_variable _thread_running_cv;
  bool _is_thread_running{false};
  bool _is_already_shutting_down{false};

  std::string _unique_id;

  static std::string _get_current_directory();
  int _get_id();
  void run();
  void _signal_to_stop();

public:
  static void set_initial_directory(const std::string& initial_directory_);
  static void set_application_installation_directory(const std::string& application_installation_directory_);
  static void set_config_directory(const std::string& config_directory_);
  static void set_data_directory(const std::string& data_directory_);

  static std::string get_application_installation_directory();
  static std::string get_config_directory();
  static std::string get_data_directory();

  ProcessRunner(std::string command, std::vector<std::string> args, std::string unique_id = "",
                std::string initial_directory = ProcessRunner::initial_directory);
  virtual ~ProcessRunner();
  void signal_to_stop();
  bool is_running();
  int get_last_exit_code();
  int get_id();
  int get_number();
  std::string get_composite_command();
  std::string get_initial_directory();
};
#endif // process_runner_h
