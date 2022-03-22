// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef pipeline_remote_h
#define pipeline_remote_h
#include <atomic>
#include <future>
#include <grpcpp/grpcpp.h>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "protobuf_helper.h"

class ProcessRunnerClient
{
private:
  std::string host{"192.168.1.101:50051"};
  std::size_t _key{0};
  std::unique_ptr<data_models::ProcessRunner::Stub> _stub;

  // ::grpc::CreateChannel(target_str, ::grpc::InsecureChannelCredentials()));
public:
  ProcessRunnerClient(std::string command, std::vector<std::string> args, std::shared_ptr<grpc::Channel> channel);
  ~ProcessRunnerClient();
  void signal_to_stop();
  bool is_running();
  int get_last_exit_code();
};

#endif // pipeline_remote_h
