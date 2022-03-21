// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef pipeline_h
#define pipeline_h
#include <Poco/Process.h>
#include <atomic>
#include <future>
#include <memory>
#include <mutex>
#include <thread>

class Pipeline
{
private:
  static std::string initial_directory;
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

public:
  static void set_initial_directory(const std::string& initial_directory_);
  static std::string get_initial_directory();
  Pipeline(std::string command, std::vector<std::string> args, std::string initial_directory = Pipeline::get_initial_directory());
  ~Pipeline();
  void signal_to_stop();
  bool is_running();
  int get_last_exit_code();
  int get_id();
};
#endif // pipeline_h
