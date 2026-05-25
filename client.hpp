#pragma once

#include <string>

void run_sync_client(const std::string& host, unsigned short port, const std::string& message);
void run_async_client(const std::string& host, unsigned short port, const std::string& message);
