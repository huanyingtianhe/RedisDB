/* server.cpp
 *
 * Copyright (c) 2015-2016 Guo Xiao <guoxiao08@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
// This code is based on the original code which has the following license:
//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.hpp"
#include <thread>

using namespace boost;

namespace http {
namespace server {

server::server(const std::string& address, const std::string& port, std::size_t thread_num)
  : signals_(io_service_),
    acceptor_(io_service_),
    new_connection_(),
    request_handler_(),
    thread_num_(thread_num)
{
  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
#if defined(SIGQUIT)
  signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)

  do_await_stop();

  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  asio::ip::tcp::resolver resolver(io_service_);
  asio::ip::tcp::endpoint endpoint = *resolver.resolve({address, port});
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  do_accept();
}

void server::run() {
  // The io_service::run() call will block until all asynchronous operations
  // have finished. While the server is running, there is always at least one
  // asynchronous operation outstanding: the asynchronous accept call waiting
  // for new incoming connections.
  if (thread_num_ > 1) {
    std::vector<std::shared_ptr<std::thread>> threads;
    for (std::size_t i = 0; i < thread_num_; i ++) {
      std::shared_ptr<std::thread> thread = std::make_shared<std::thread>([&]() {
        io_service_.run();
      });
      threads.push_back(thread);
    }
    for (auto&& thread : threads) {
      thread->join();
    }
  } else {
    io_service_.run();
  }
}

void server::do_accept()
{
  new_connection_.reset(new connection(io_service_, request_handler_));
  acceptor_.async_accept(new_connection_->socket(),
      [this](std::error_code ec)
      {
        // Check whether the server was stopped by a signal before this
        // completion handler had a chance to run.
        if (!acceptor_.is_open())
        {
          return;
        }

        if (!ec)
        {
          new_connection_->start();
        }

        do_accept();
      });
}

void server::do_await_stop()
{
  signals_.async_wait(
      [this](std::error_code /*ec*/, int /*signo*/)
      {
        // The server is stopped by cancelling all outstanding asynchronous
        // operations. Once all operations have finished the io_service::run()
        // call will exit.
        io_service_.stop();
        acceptor_.close();
      });
}

void server::add_handler(const std::string &url, request_handler::handle handler) {
  request_handler_.reg(url, std::move(handler));
}
} // namespace server
} // namespace http
