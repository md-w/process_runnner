// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "process_runner_service.h"

::grpc::Status ProcessRunnerService::RunProcess(::grpc::ServerContext* context, const data_models::ProcessRunnerRequest* request,
                                              data_models::ProcessRunnerResponse* response)
{
  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::SignalToStop(::grpc::ServerContext* context,
                                                   const data_models::SignalToStopRequest* request,
                                                   data_models::SignalToStopResponse* response)
{
  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::IsRunning(::grpc::ServerContext* context,
                                                   const data_models::IsRunningRequest* request,
                                                   data_models::IsRunningResponse* response)
{
  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::GetLastExitCode(::grpc::ServerContext* context,
                                                   const data_models::GetLastExitCodeRequest* request,
                                                   data_models::GetLastExitCodeResponse* response)
{
  return ::grpc::Status::OK;
}

::grpc::Status ProcessRunnerService::GetId(::grpc::ServerContext* context,
                                                   const data_models::GetIdRequest* request,
                                                   data_models::GetIdResponse* response)
{
  return ::grpc::Status::OK;
}
