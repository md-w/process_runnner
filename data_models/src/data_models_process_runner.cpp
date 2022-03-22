// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "data_models_process_runner.h"
namespace data_models
{
std::size_t hash_value(ProcessRunnerArguments const& s)
{
  std::size_t h1 = std::hash<std::string>{}(s.command);

  std::ostringstream os;
  std::copy(s.args.begin(), s.args.end(), std::ostream_iterator<std::string>(os));
  std::size_t h2 = std::hash<std::string>{}(os.str());
  return h1 ^ (h2 << 1); // or use boost::hash_combine
}

} // namespace data_models
