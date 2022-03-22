// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef process_runner_service_h
#define process_runner_service_h

#include <map>
#include <memory>
#include <grpc/grpc.h>

#include "protobuf_helper.h"

class ProcessRunnerService final : public data_models::ProcessRunner::Service
{
private:
  std::map<std::size_t, std::unique_ptr<ProcessRunner>> process_runner_map;
public:
  ::grpc::Status RunProcess(::grpc::ServerContext* context, const data_models::ProcessRunnerRequest* request,
                          data_models::ProcessRunnerResponse* response) override;

  ::grpc::Status SignalToStop(::grpc::ServerContext* context, const data_models::SignalToStopRequest* request,
                               data_models::SignalToStopResponse* response) override;

  ::grpc::Status IsRunning(::grpc::ServerContext* context, const data_models::IsRunningRequest* request,
                               data_models::IsRunningResponse* response) override;

  ::grpc::Status GetLastExitCode(::grpc::ServerContext* context, const data_models::GetLastExitCodeRequest* request,
                               data_models::GetLastExitCodeResponse* response) override;

  ::grpc::Status GetId(::grpc::ServerContext* context, const data_models::GetIdRequest* request,
                               data_models::GetIdResponse* response) override;

};

#endif // process_runner_service_h
