//
// Created by Glen Dayton on 8/13/23.
//

#ifndef OSCPP_SOCKET_HPP
#define OSCPP_SOCKET_HPP
#include <sys/types.h>
#include <sys/socket.h>
#include <utility>

namespace oscpp {
    class [[maybe_unused]] Socket {
    private:
        int handle;

    public:
        explicit Socket(int sysFileDescriptor) noexcept : handle{sysFileDescriptor} {}

        Socket(Socket &) = delete;

        [[maybe_unused]] Socket(Socket &&) noexcept;

        ~Socket() noexcept;

        Socket &operator=(Socket &) = delete;

        Socket &operator=(Socket &&) = delete;

        static auto create(int domain = PF_INET, int socketType = SOCK_STREAM, int protocol = 0) noexcept -> std::pair<Socket, int>;

        [[maybe_unused]] [[nodiscard]] auto clone() const noexcept -> std::pair<Socket, int>;

        [[maybe_unused]] [[nodiscard]] int descriptor() const noexcept { return handle; }
    };
}
#endif //OSCPP_SOCKET_HPP
