#include "audio.hpp"

#include <boost/asio.hpp>

#include <iostream>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <chrono>

namespace net = boost::asio;
namespace sys = boost::system;
using namespace std::chrono;
using namespace std::literals;

using net::ip::udp;
using net::ip::tcp;

constexpr static int max_frames = 65000;

void StartServer(std::uint16_t port, int frame_size) {
    Player player(ma_format_u8, 1);
    net::io_context io_context;
    udp::socket socket(io_context, udp::endpoint(udp::v4(), port));

    sys::error_code ec;
    std::vector<char> recv_buf(max_frames * frame_size);
    while (true) {
        udp::endpoint remote_endpoint;
        auto size = socket.receive_from(net::buffer(recv_buf), remote_endpoint, 0, ec);
        if (ec) {
            throw std::runtime_error(ec.message());
        }
        player.PlayBuffer(recv_buf.data(), size / frame_size, 1.5s);
        std::cout << "Playing done" << std::endl;
    }
}

void StartClient(Recorder& recorder, std::uint16_t port, int frame_size) {
    net::io_context io_context;
    udp::socket socket(io_context, udp::v4());

    sys::error_code ec;
    while (true) {
        std::string address;
        std::cout << "Enter server address to record message..." << std::endl;
        std::getline(std::cin, address);

        auto endpoint = udp::endpoint(net::ip::make_address(address, ec), port);
        if (ec) {
            throw std::runtime_error(ec.message());
        }

        auto rec_result = recorder.Record(max_frames, 1.5s);
        std::cout << "Recording done" << std::endl;

        socket.send_to(net::buffer(rec_result.data.data(), rec_result.frames * frame_size), endpoint);
        std::cout << "Sending done" << std::endl;
    }
}

int main(int argc, char** argv) {
    Recorder recorder(ma_format_u8, 1);
    int frame_size = recorder.GetFrameSize();

    if (argc != 3) {
        std::cout << "Usage: "sv << argv[0] << " <mode> <port>"sv << std::endl;
        return 1;
    }
    std::string_view mode = argv[1];
    std::uint16_t port = std::stoi(argv[2]);

    if (mode == "client") {
        StartClient(recorder, port, frame_size);
    } else if (mode == "server") {
        StartServer(port, frame_size);
    } else {
        std::cout << "Usage: "sv << argv[0] << " <mode> <port>"sv << std::endl;
        return 1;
    }

    return 0;
}
