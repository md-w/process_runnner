// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/File.h>
#include <Poco/UUID.h>
#include <Poco/UUIDGenerator.h>
#include <absl/strings/match.h>
#include <absl/strings/string_view.h>
#include <iostream>
#include <sstream>

#ifdef WIN32
#include <filesystem>
namespace filesystem = std::filesystem;
#else
/* https://github.com/scylladb/seastar/issues/648 */
#if __cplusplus >= 201703L && __has_include(<filesystem>)
#include <filesystem>
namespace filesystem = std::filesystem;
#else
#include <experimental/filesystem>
namespace filesystem = std::experimental::filesystem;
#endif
#endif

#include <absl/strings/escaping.h>
#include <algorithm>
#include <fstream>

#include "utilities.h"

std::string vtpl::utilities::get_filesystem_directory_seperator()
{
#ifdef _WIN32
  return "\\";
#else
  return "/";
#endif
  // return "/";
}

bool vtpl::utilities::is_directory_exists(const std::string& dir_path)
{
  if (dir_path.empty()) {
    throw std::runtime_error("Input argument is empty");
  }
  return (filesystem::exists(dir_path) && filesystem::is_directory(dir_path));
}

bool vtpl::utilities::is_regular_file_exists(const std::string& file_path)
{
  if (file_path.empty()) {
    throw std::runtime_error("Input argument is empty");
  }
  return (filesystem::exists(file_path) && filesystem::is_regular_file(file_path));
}

bool vtpl::utilities::create_directories(const std::string& dir_path)
{
  if (dir_path.empty()) {
    throw std::runtime_error("Input argument is empty");
  }
  if (is_directory_exists(dir_path)) {
    return true;
  }
  std::error_code error_code;
  if (!filesystem::create_directories(dir_path, error_code)) {
    std::cout << "Unable to create directories [" << dir_path << "], "
              << "Error Code[" << error_code.value() << "], "
              << "Error Message[" << error_code.message() << "], "
              << "Error category[" << error_code.category().name() << "] " << std::endl;
    return false;
  }
  return vtpl::utilities::is_directory_exists(dir_path);
}

bool vtpl::utilities::create_directories_from_file_path(const std::string& file_path)
{
  if (file_path.empty()) {
    throw std::runtime_error("Input argument is empty");
  }
  std::string dir_path;
  size_t pos = file_path.find_last_of(get_filesystem_directory_seperator());
  if (pos != std::string::npos) {
    dir_path = file_path.substr(0, pos);
  }
  if (dir_path.empty()) {
    throw std::runtime_error("Directory path is empty" + dir_path);
  }
  return vtpl::utilities::create_directories(dir_path);
}

std::stringstream vtpl::utilities::end_with_directory_seperator(const std::string& dir_path_0)
{
  std::stringstream ss_path;
  ss_path << dir_path_0;
  if (!((dir_path_0.back() == '/') || (dir_path_0.back() == '\\'))) {
    ss_path << get_filesystem_directory_seperator();
  }
  return ss_path;
}

std::string vtpl::utilities::merge_directories(const std::string& dir_path_0, const std::string& dir_path_1)
{
  std::stringstream ss_path;
  if ((dir_path_0.empty()) || (dir_path_1.empty())) {
    throw std::runtime_error("Input argument is empty");
  }
  ss_path << dir_path_0;
  std::stringstream ss_path1 = end_with_directory_seperator(ss_path.str());
  ss_path.swap(ss_path1);
  ss_path << dir_path_1;
  // if (!((ss_path.str().back() == '/') || (ss_path.str().back() == '\\')))
  //   ss_path << get_filesystem_directory_seperator();
  return ss_path.str();
}

std::string vtpl::utilities::merge_directories(const std::string& dir_path_0, const std::string& dir_path_1,
                                               const std::string& dir_path_2)
{
  // NOLINTNEXTLINE(readability-suspicious-call-argument)
  return merge_directories(dir_path_0, merge_directories(dir_path_1, dir_path_2));
}

std::string vtpl::utilities::merge_directories(const std::string& dir_path_0, const std::string& dir_path_1,
                                               const std::string& dir_path_2, const std::string& dir_path_3)
{
  return merge_directories(dir_path_0, merge_directories(dir_path_1, merge_directories(dir_path_2, dir_path_3)));
}

std::string vtpl::utilities::merge_directories(const std::string& dir_path, std::vector<std::string>& dir_n_list)
{
  std::string path(dir_path);
  for (auto& it : dir_n_list) {
    path = merge_directories(path, it);
  }
  return path;
}

bool vtpl::utilities::delete_file(const std::string& file_path)
{
  if (file_path.empty()) {
    throw std::runtime_error("Input argument is empty");
  }
  if (is_regular_file_exists(file_path)) {
    return filesystem::remove(file_path);
  }
  return true;
}

bool vtpl::utilities::delete_directory(const std::string& dir_path)
{
  if (dir_path.empty()) {
    throw std::runtime_error("Input argument is empty");
  }
  if (is_directory_exists(dir_path)) {
    return filesystem::remove_all(dir_path) != 0U;
  }
  return true;
}
std::string vtpl::utilities::base64_encode_file(const std::string& file_path)
{
  if (file_path.empty()) {
    throw std::runtime_error("Input argument is empty");
  }
  if (!is_regular_file_exists(file_path)) {
    throw std::runtime_error("File does not exists");
  }

  std::stringstream ss;
  std::ifstream istream(file_path);
  std::copy(std::istreambuf_iterator<char>(istream), std::istreambuf_iterator<char>(),
            std::ostreambuf_iterator<char>(ss));
  std::string ret;
  absl::Base64Escape(ss.str(), &ret);
  return std::move(ret);
}
std::string vtpl::utilities::generate_unique_id()
{
  std::string id{};
  try {
    Poco::UUIDGenerator& generator = Poco::UUIDGenerator::defaultGenerator();
    Poco::UUID uuid(generator.createOne());
    id = uuid.toString(); // default value
  } catch (std::exception& ex) {
    std::cerr << "vtpl::utilities::generate_unique_id Exception " << ex.what() << std::endl;
  }
  return id;
}

std::string vtpl::utilities::compose_url(const std::string& url, const std::string& user, const std::string& pass)
{
  if (absl::StrContains(url, ':') && absl::StrContains(url, '@')) {
    return url;
  }
  const std::string search_string("://");
  std::string url_1;
  std::string url_2;

  size_t pos = url.find(search_string);
  if (pos != std::string::npos) {
    size_t t = static_cast<std::string::difference_type>(pos) +
               static_cast<std::string::difference_type>(search_string.length());
    url_1 = url.substr(0, t);
    url_2 = url.substr(t, static_cast<std::string::difference_type>(url.length()) -
                              static_cast<std::string::difference_type>(url_1.length()));

    return url_1.append(user).append(":").append(pass).append("@").append(url_2);
  }

  return url;
}