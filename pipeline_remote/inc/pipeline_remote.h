// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef pipeline_remote_h
#define pipeline_remote_h
#include <atomic>
#include <future>
#include <memory>
#include <mutex>
#include <thread>

class PipelineRemote
{
private:
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

  // std::unique_ptr<Poco::ProcessHandle> _process_handle;

  std::mutex _thread_running_mutex;
  std::condition_variable _thread_running_cv;
  bool _is_thread_running{false};
  void start();
  void stop();
  void run();

public:
  PipelineRemote(std::string command, std::vector<std::string> args);
  ~PipelineRemote();
  void signal_to_stop();
  bool is_running();
  int get_last_exit_code();
};

#endif // pipeline_remote_h
