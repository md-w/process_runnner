#include <Poco/NumberParser.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/XMLConfiguration.h>
#include <iostream>
#include <sstream>
#include <string_view>

#include "Application.h"
#include "Version.h"
#include "globalerrorhandler.h"
#include "logging.h"
#include "process_runner_service_run.h"
#include "utilities.h"
#include "version_info.h"

class entry_point : public Poco::Util::ServerApplication
{
private:
  std::string_view INITIAL_CONFIGURATION_FILE_NAME{"conf.xml"};
  std::string_view BASE_LOG_DIR_CONFIGURATION_NAME{"logDir"};
  std::string_view BASE_DATA_DIR_CONFIGURATION_NAME{"dataDir"};
  std::string_view BASE_CONFIG_DIR_CONFIGURATION_NAME{"configDir"};
  std::string_view BASE_LOG_LEVEL_CONFIGURATION_NAME{"logLevel"};

  std::string_view LOG_DIR_NAME{"Logs"};
  std::string_view DATA_DIR_NAME{"Data"};
  std::string_view CONFIG_DIR_NAME{"Config"};

  std::string_view DATA_DIR_ENV_VARIABLE_NAME{"DATA_ROOT"};

  std::string _name_of_app{};
  vtpl::globalerrorhandler _globalerrorhandler;

protected:
  std::string get_application_running_folder()
  {

    Poco::Path base_path;
    if (config().getBool("application.runAsService", false) || config().getBool("application.runAsDaemon", false)) {
      base_path.assign(config().getString("application.dataDir", Poco::Path::current()));
      base_path.pushDirectory(config().getString("application.baseName", "ojana"));
    } else {
      base_path.assign(Poco::Path::current());
    }
    return base_path.toString();
  }

  std::string get_session_folder()
  {
    Poco::Path base_path(get_application_running_folder());
    base_path.pushDirectory("session");
    return base_path.toString();
  }

  std::string get_application_installation_folder()
  {

    Poco::Path base_path;
    if (config().getBool("application.runAsService", false) || config().getBool("application.runAsDaemon", false)) {
      base_path.assign(
          config().getString("application.dataDir", config().getString("application.dir", Poco::Path::current())));
      base_path.pushDirectory(config().getString("application.baseName", "ojana"));
    } else {
      base_path.assign(config().getString("application.dir", Poco::Path::current()));
    }
    return base_path.toString();
  }

  void initialize(Application& self) override
  {
    loadConfiguration();
    ServerApplication::initialize(self);
    Poco::ErrorHandler::set(&_globalerrorhandler);
    load_first_configuration();
    _name_of_app = config().getString("application.baseName");
    std::string s = vtpl::utilities::end_with_directory_seperator(_base_log_directory_path).str();
    ::ray::RayLog::GetLoggerName();
    ::ray::RayLog::StartRayLog(_name_of_app, ::ray::RayLogLevel::INFO, s, false);
  }

  void defineOptions(Poco::Util::OptionSet& options) override { ServerApplication::defineOptions(options); }

  void handle_option(const std::string& name, const std::string& value) {}

  static void display_info()
  {
    std::cout << vtpl::version_info::get(APPLICATION_NAME, std::to_string(VERSION_MAJOR), std::to_string(VERSION_MINOR),
                                         BUILD_NUMBER)
              << std::endl;
  }

private:
  std::string _application_base_path;
  std::string _base_log_directory_path;
  std::string _base_data_directory_path;
  std::string _base_config_directory_path;
  Poco::AutoPtr<Poco::Util::XMLConfiguration> _p_first_conf;

  bool _start_playing{true};

public:
  entry_point() : _p_first_conf(new Poco::Util::XMLConfiguration()) { setUnixOptions(true); }

  // void get_application_base_dirs()
  // {
  // _base_config_directory_path = config().getString("application.configDir");
  // _base_log_directory_path = config().getString("application.configDir");
  // _base_data_directory_path = config().getString("application.dataDir");
  // }

  void load_first_configuration()
  {
    int config_file_save_counter = 0;

    _application_base_path = get_session_folder();

    std::string first_conf_file_path = vtpl::utilities::merge_directories(get_application_installation_folder(),
                                                                          std::string(INITIAL_CONFIGURATION_FILE_NAME));

    if (vtpl::utilities::is_regular_file_exists(first_conf_file_path)) {
      _p_first_conf->load(first_conf_file_path);
    } else {
      config_file_save_counter++;
    }

    _base_log_directory_path = vtpl::utilities::merge_directories(_application_base_path, std::string(LOG_DIR_NAME));
    // base log dir configuration
    if (!_p_first_conf->has(std::string{BASE_LOG_DIR_CONFIGURATION_NAME})) {
      _p_first_conf->setString(std::string(BASE_LOG_DIR_CONFIGURATION_NAME), _base_log_directory_path);
      config_file_save_counter++;
    }
    _base_log_directory_path =
        _p_first_conf->getString(std::string(BASE_LOG_DIR_CONFIGURATION_NAME), _base_log_directory_path);

    vtpl::utilities::create_directories(_base_log_directory_path);

    // base data dir configuration
    _base_data_directory_path = vtpl::utilities::get_environment_value(std::string(DATA_DIR_ENV_VARIABLE_NAME));
    if (_base_data_directory_path.empty()) {
      _base_data_directory_path =
          vtpl::utilities::merge_directories(_application_base_path, std::string(DATA_DIR_NAME));
    }
    if (!_p_first_conf->has(std::string(BASE_DATA_DIR_CONFIGURATION_NAME))) {
      _p_first_conf->setString(std::string(BASE_DATA_DIR_CONFIGURATION_NAME), _base_data_directory_path);
      config_file_save_counter++;
    }

    _base_data_directory_path =
        _p_first_conf->getString(std::string(BASE_DATA_DIR_CONFIGURATION_NAME), _base_data_directory_path);

    vtpl::utilities::create_directories(_base_data_directory_path);

    // base config dir configuration
    _base_config_directory_path =
        vtpl::utilities::merge_directories(_application_base_path, std::string(CONFIG_DIR_NAME));
    if (!_p_first_conf->has(std::string(BASE_CONFIG_DIR_CONFIGURATION_NAME))) {
      _p_first_conf->setString(std::string(BASE_CONFIG_DIR_CONFIGURATION_NAME), _base_config_directory_path);
      config_file_save_counter++;
    }
    _base_config_directory_path =
        _p_first_conf->getString(std::string(BASE_CONFIG_DIR_CONFIGURATION_NAME), _base_config_directory_path);

    vtpl::utilities::create_directories(_base_config_directory_path);

    if (config_file_save_counter > 0) {
      _p_first_conf->save(first_conf_file_path);
    }
  }

  int main(const ArgVec& /*args*/) final
  {
    RAY_LOG_INF << "Started";
    load_first_configuration();
    {
      std::string key = "COMMAND_PORT";
      // RAY_LOG_INF << "Environment value: for " << key << " [" << vtpl::utilities::get_environment_value(key) << "]";
      int listening_port = Poco::NumberParser::parse(vtpl::utilities::get_environment_value(key));
      // int listening_port = config().getInt("system.env.COMMAND_PORT");

      std::unique_ptr<ProcessRunnerServiceRun> process_runner =
          std::make_unique<ProcessRunnerServiceRun>(get_application_installation_folder(), _base_config_directory_path,
                                                    _base_data_directory_path, listening_port);
      waitForTerminationRequest();
      process_runner->signal_to_stop();
    }
    RAY_LOG_INF << "Stopped";
    return Application::EXIT_OK;
  }
};
POCO_SERVER_MAIN(entry_point);