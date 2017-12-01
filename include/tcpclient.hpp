//
// Created by Jake Rachleff on 11/25/17.
//

#ifndef TINYTORRENT_TCPCLIENT_H
#define TINYTORRENT_TCPCLIENT_H

#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

class tcpclient {
public:

    tcpclient(boost::asio::io_service& io_service, std::string ip_addr, unsigned short port)
        : socket_(io_service), endpoint_(boost::asio::ip::address::from_string("127.0.0.1"), 12345)
    {
        m_pWork.reset(new boost::asio::io_service::work(io_service));
    }

    void request_song(std::string id)
    {
        song_id_ = id;
        boost::system::error_code connect_error = boost::asio::error::host_not_found;

        std::string output_filename = "../clientfiles/" + id;
        std::ofstream output_filestream;
        output_filestream.open(output_filename);


        std::cout << "About to connect to socket" << std::endl;
        socket_.connect(endpoint_, connect_error);


        if (connect_error)
            throw boost::system::system_error(connect_error);

        std::cout << "Client: Successfully connected to socket" << std::endl;


        boost::array<char, 1024> buf;
        boost::system::error_code err;

        size_t len = socket_.write_some(boost::asio::buffer(id), err);
        if (err) throw boost::system::system_error(err);

        for (;;)
        {
            len = socket_.read_some(boost::asio::buffer(buf), err);
            std::cout << "Client: Read " << buf.data() << " from server." << std::endl;
            output_filestream.write(buf.data(), len);
//            std::cout << len << std::endl;
            std::this_thread::sleep_for (std::chrono::milliseconds(200));
            if (err == boost::asio::error::eof)
            {
                std::cout << "Client: Sawa EOF, shutting down..." << std::endl;
                break; // Connection closed cleanly by peer.
            }
            else if (err)
                throw boost::system::system_error(err); // Some other error.

        }
        output_filestream.close();
    }

private:
    boost::shared_ptr<boost::asio::io_service::work> m_pWork;
    std::string song_id_;
    tcp::socket socket_;
    tcp::endpoint endpoint_;
};


#endif //TINYTORRENT_TCPCLIENT_H
