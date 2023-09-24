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
        explicit Socket(int domain = PF_INET, int socketType = SOCK_STREAM, int protocol = 2);

        Socket(Socket &) = delete;

        [[maybe_unused]] [[maybe_unused]] Socket(Socket &&) noexcept;

        ~Socket();

        Socket &operator=(Socket &) = delete;

        Socket &operator=(Socket &&) = delete;

        static std::pair<int, int> create(int domain, int socketType, int protocol);

        [[maybe_unused]] [[nodiscard]] Socket clone() const;

    protected:
        [[maybe_unused]] [[nodiscard]] int descriptor() const { return handle; }
    };
}
#endif //OSCPP_SOCKET_HPP
