#include <iostream>
#include <http/server.hpp>
#include <http/request.hpp>
#include <http/reply.hpp>

int main(int argc, char *argv[]) {
  // Initialise the server.
  using namespace http::server;
  server s("127.0.0.1", "8080", 1);

  s.add_handler("/hello", [](const request &req) {
    reply rep("hello world");
    rep.headers["Content-Type"] = "text/plain";
    return rep;
  });

  // Run the server until stopped.
  s.run();
  return 0;
}
