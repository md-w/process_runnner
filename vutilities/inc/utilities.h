// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef utilities_h
#define utilities_h
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace vtpl
{
namespace utilities
{
std::string get_filesystem_directory_seperator();
bool is_directory_exists(const std::string& dir_path);
bool is_regular_file_exists(const std::string& file_path);
bool create_directories(const std::string& dir_path);
std::string create_directories_from_file_path(const std::string& file_path);
std::stringstream end_with_directory_seperator(const std::string& dir_path_0);
std::string merge_directories(const std::string& dir_path_0, const std::string& dir_path_1);
std::string merge_directories(const std::string& dir_path_0, const std::string& dir_path_1,
                              const std::string& dir_path_2);
std::string merge_directories(const std::string& dir_path_0, const std::string& dir_path_1,
                              const std::string& dir_path_2, const std::string& dir_path_3);
std::string merge_directories(const std::string& dir_path_0, const std::string& dir_path_1,
                              const std::string& dir_path_2, const std::string& dir_path_3,
                              const std::string& dir_path_4);
std::string merge_directories(const std::string& dir_path, std::vector<std::string>& dir_n_list);
bool delete_directory(const std::string& dir_path);
bool delete_file(const std::string& file_path);
std::string base64_encode_file(const std::string& file_path);
std::string generate_unique_id();
int64_t get_file_end_timestamp(const std::string& path);
bool get_file_start_timestamp(std::string file_name, int64_t& start_timestamp, int64_t& end_timestamp);
int64_t get_last_modified_time(const std::string& file_abs_path);
void list_files(const std::string& path, std::map<int64_t, std::string>& list, const std::string& extension);
void list_files_sorted(const std::string& path, std::map<int64_t, std::string>& list, const std::string& extension);
size_t get_file_size_bytes(const std::string& abs_file_path);
std::string get_file_name(const std::string& file_name);
std::string shorten_string(const std::string& str);
std::string change_extension(const std::string& str, const std::string& ext);
std::string compose_url(const std::string& url, const std::string& user, const std::string& pass);
std::string get_environment_value(const std::string& value, const std::string& default_value);
} // namespace utilities

} // namespace vtpl

#endif // utilities_h
