# asio_http
A simple multithread async http server based on asio

[![Build Status](https://travis-ci.org/guoxiao/asio_http.svg?branch=master)](https://travis-ci.org/guoxiao/asio_http)
[![Coverage Status](https://coveralls.io/repos/guoxiao/asio_http/badge.svg?branch=master&service=github)](https://coveralls.io/github/guoxiao/asio_http?branch=master)


## Usage

```cpp
using namespace http::server;
server s("127.0.0.1", "8080", 1);

s.add_handler("/hello", [](const request &req) {
  reply rep;
  rep.content = "hello world!";
  rep.status = reply::ok;
  rep.headers["Content-Length"] = std::to_string(rep.content.size());
  rep.headers["Content-Type"] = "text/plain";
  return rep;
});

// Run the server until stopped.
s.run();
```

See `example/main.cpp` for a complete example

## Build

```bash
mkdir build && cd build
cmake .. -DBUILD_EXAMPLE -DCMAKE_BUILD_TYPE=Release
```

## License

MIT License

Based on the examples from asio licensed by Boost License.
