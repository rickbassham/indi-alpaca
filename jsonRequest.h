#pragma once

#ifndef JSONREQUEST_H
#define JSONREQUEST_H

#include <libindi/json.h>

nlohmann::json get_json(const char *url);
nlohmann::json put_json(const char* url, const std::map<std::string, std::string> &body);

#endif // JSONREQUEST_H
