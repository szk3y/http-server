#pragma once

int sock_listen(const char* port);
int accept_loop(int listen_fd);
