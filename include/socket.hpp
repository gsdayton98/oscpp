// Copyright 2023. Glen S. Dayton. Rights reserved according to included license.
// Created by Glen Dayton on 8/13/23.
//

#ifndef OSCPP_SOCKET_HPP
#define OSCPP_SOCKET_HPP
#include <sys/types.h>
#include <sys/socket.h>
#include <utility>

namespace oscpp {
    class [[maybe_unused]] __attribute__((visibility("default"))) Socket {
        int handle;

    private:
        explicit Socket(const int sysFileDescriptor) noexcept : handle{sysFileDescriptor} {}

    public:
        Socket(const Socket &) = delete;

        [[maybe_unused]] Socket(Socket &&) noexcept;

        ~Socket() noexcept;

        Socket &operator=(const Socket &) = delete;

        static auto create(int domain = PF_INET, int socketType = SOCK_STREAM, int protocol = 0) noexcept -> std::pair<Socket, int>;

        [[maybe_unused]] [[nodiscard]] auto clone() const noexcept -> std::pair<Socket, int>;

        [[maybe_unused]] [[nodiscard]] int descriptor() const noexcept { return handle; }
    };
}
#endif //OSCPP_SOCKET_HPP
