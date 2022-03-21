// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "process_runner_client.h"

ProcessRunnerClient::ProcessRunnerClient(/* args */)
{
  // Setup request
  data_models::HelloRequest request;
  request.set_name("Monotosh");
  data_models::HelloReply result;
  // Call
  auto channel = grpc::CreateChannel("192.168.1.101:50051", grpc::InsecureChannelCredentials());
  std::unique_ptr<data_models::Greeter::Stub> client = data_models::Greeter::NewStub(channel);
  grpc::ClientContext context;
  grpc::Status status = client->SayHello(&context, request, &result);

  if (status.ok()) {
    // Output result
    std::cout << "I got:" << std::endl;
    std::cout << "Message: " << result.message() << std::endl;
  } else {
    std::cout << "Error received: " << status.error_code() << std::endl;
  }
}

ProcessRunnerClient::~ProcessRunnerClient() {}
