#include <catch2/catch.hpp>

TEST_CASE("ProcessRunnerArguments", "[protobuf]")
{
  data_models::ProcessRunnerArguments process_runner_arguments;
  process_runner_arguments.command = "ffrecorder";
  process_runner_arguments.args.push_back("-i");
  process_runner_arguments.args.push_back("-o");
  data_models::ProcessRunnerArguments process_runner_arguments1;
  process_runner_arguments1.command = "ffrecorder";
  process_runner_arguments1.args.push_back("-i");
  process_runner_arguments1.args.push_back("-o");
  process_runner_arguments1.args.push_back("aa");

  REQUIRE(data_models::hash_value(process_runner_arguments) == data_models::hash_value(process_runner_arguments1));
}