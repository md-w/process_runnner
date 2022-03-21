// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef process_runner_client_h
#define process_runner_client_h
#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>

#include "interfaces/process_runner.grpc.pb.h"
#include "interfaces/process_runner.pb.h"

class ProcessRunnerClient
{
private:
    /* data */
public:
    ProcessRunnerClient();
    ~ProcessRunnerClient();
};

#endif	// process_runner_client_h
