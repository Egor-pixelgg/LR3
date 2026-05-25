#pragma once
#include <cstddef>

void run_sync_server(unsigned short port);
void run_async_server(unsigned short port, std::size_t thread_count);
