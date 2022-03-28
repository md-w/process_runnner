#include <catch2/catch.hpp>

TEST_CASE("ProcessRunnerArguments", "[protobuf]")
{
  data_models::ProcessRunnerArguments process_runner_arguments;
  process_runner_arguments.command = "/workspaces/vtpl_agent_2022/build/vtpl_agent/ffrecorder";
  process_runner_arguments.args.emplace_back("-o");
  process_runner_arguments.args.emplace_back("/workspaces/vtpl_agent_2022/session/Data/4dc75/98c9a/recording_clip/major");
  process_runner_arguments.args.emplace_back("-i");
  process_runner_arguments.args.emplace_back("rtsp://admin:AdmiN1234@192.168.0.59/media/video1");

  data_models::ProcessRunnerArguments process_runner_arguments1;
  process_runner_arguments1.command = "/workspaces/vtpl_agent_2022/build/vtpl_agent/ffrecorder";
  process_runner_arguments1.args.emplace_back("-i");
  process_runner_arguments1.args.emplace_back("rtsp://admin:AdmiN1234@192.168.0.59/media/video1");
  process_runner_arguments1.args.emplace_back("-o");
  process_runner_arguments1.args.emplace_back("/workspaces/vtpl_agent_2022/session/Data/4dc75/98c9a/recording_clip/major");

  REQUIRE(data_models::hash_value(process_runner_arguments) == data_models::hash_value(process_runner_arguments1));
}