#include <sys/sendfile.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

class Socket {
 public:
  Socket() {
    if ((sock_ = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
      std::cerr << "socket() failed: " << strerror(errno) << std::endl;
      exit(1);
    }
  }

  ~Socket() {
    if (close(sock_) == -1) {
      std::cerr << "close() failed: " << strerror(errno) << std::endl;
      exit(1);
    }
  }

  int sock() const { return sock_; }

 private:
  int sock_;
};

class Server {
 public:
  Server(const std::string& path) : path_(path) {
    if (bind(sock_.sock(), reinterpret_cast<const struct sockaddr*>(&addr_),
             sizeof(addr_)) == -1) {
      std::cerr << "bind() failed: " << strerror(errno) << std::endl;
      exit(1);
    }
  }

  ~Server() {
    if (close(sock_.sock()) == -1) {
      std::cerr << "close() failed: " << strerror(errno) << std::endl;
      exit(1);
    }
  }

  void Listen() {
    if (listen(sock_.sock(), SOMAXCONN) == -1) {
      std::cerr << "listen() failed: " << strerror(errno) << std::endl;
      exit(1);
    }
  }

  int Accept() {
    int client = accept(sock_.sock(), nullptr, nullptr);
    if (client == -1) {
      std::cerr << "accept() failed: " << strerror(errno) << std::endl;
      exit(1);
    }
    return client;
  }

 private:
  Socket sock_;
  struct sockaddr_un addr_;
  std::string path_;
};

class Client {
 public:
  Client(const std::string& path) : path_(path) {
    if (connect(sock_.sock(), reinterpret_cast<const struct sockaddr*>(&addr_),
                sizeof(addr_)) == -1) {
      std::cerr << "connect() failed: " << strerror(errno) << std::endl;
      exit(1);
    }
  }

  ~Client() {
    if (close(sock_.sock()) == -1) {
      std::cerr << "close() failed: " << strerror(errno) << std::endl;
      exit(1);
    }
  }

  int sock() const { return sock_.sock(); }

 private:
  Socket sock_;
  struct sockaddr_un addr_;
  std::string path_;
};

class File {
 public:
  File(const std::string& path) : path_(path) {
    fd_ = open(path_.c_str(), O_RDONLY);
    if (fd_ == -1) {
      std::cerr << "open() failed: " << strerror(errno) << std::endl;
      exit(1);
    }
  }

  ~File() {
    if (close(fd_) == -1) {
      std::cerr << "close() failed: " << strerror(errno) << std::endl;
      exit(1);
    }
  }

  int fd() const { return fd_; }

 private:
  int fd_;
  std::string path_;
};

class FileDescriptor {
 public:
  FileDescriptor(int fd) : fd_(fd) {}

  ~FileDescriptor() {
    if (close(fd_) == -1) {
      std::cerr << "close() failed: " << strerror(errno) << std::endl;
      exit(1);
    }
  }

  int fd() const { return fd_; }

 private:
  int fd_;
};

class FileDescriptorVector {
 public:
  FileDescriptorVector() {}

  ~FileDescriptorVector() {
    for (auto& fd : fds_) {
      if (close(fd.fd()) == -1) {
        std::cerr << "close() failed: " << strerror(errno) << std::endl;
        exit(1);
      }
    }
  }

  void Add(int fd) { fds_.push_back(FileDescriptor(fd)); }

  int fd(size_t i) const { return fds_[i].fd(); }

 private:
  std::vector<FileDescriptor> fds_;
};

class FileVector {
 public:
  FileVector() {}

  ~FileVector() {
    for (auto& f : files_) {
      if (close(f.fd()) == -1) {
        std::cerr << "close() failed: " << strerror(errno) << std::endl;
        exit(1);
      }
    }
  }

  void Add(const std::string& path) { files_.push_back(File(path)); }

  int fd(size_t i) const { return files_[i].fd(); }

 private:
  std::vector<File> files_;
};
int main() {
  Server server("/tmp/socket");
  server.Listen();
  int client = server.Accept();
  FileVector files;
  FileDescriptorVector fds;
  for (int i = 0; i < 10; i++) {
    files.Add("/tmp/file");
    fds.Add(files.fd(i));
  }
  std::vector<struct iovec> iov;
  for (int i = 0; i < 10; i++) {
    struct iovec iov_;
    iov_.iov_base = reinterpret_cast<void*>(static_cast<uintptr_t>(i));
    iov_.iov_len = sizeof(i);
    iov.push_back(iov_);
  }
  struct msghdr msg;
  msg.msg_name = nullptr;
  msg.msg_namelen = 0;
  msg.msg_iov = &iov[0];
  msg.msg_iovlen = iov.size();
  msg.msg_control = nullptr;
  msg.msg_controllen = 0;
  msg.msg_flags = 0;
  if (sendmsg(client, &msg, 0) == -1) {
    std::cerr << "sendmsg() failed: " << strerror(errno) << std::endl;
    exit(1);
  }
  std::vector<struct iovec> iov2;
  for (int i = 0; i < 10; i++) {
    struct iovec iov_;
    iov_.iov_base = reinterpret_cast<void*>(static_cast<uintptr_t>(i));
    iov_.iov_len = sizeof(i);
    iov2.push_back(iov_);
  }
  struct msghdr msg2;
  msg2.msg_name = nullptr;
  msg2.msg_namelen = 0;
  msg2.msg_iov = &iov2[0];
  msg2.msg_iovlen = iov2.size();
  msg2.msg_control = nullptr;
  msg2.msg_controllen = 0;
  msg2.msg_flags = 0;
  if (recvmsg(client, &msg2, 0) == -1) {
    std::cerr << "recvmsg() failed: " << strerror(errno) << std::endl;
    exit(1);
  }
  for (int i = 0; i < 10; i++) {
    if (msg2.msg_iov[i].iov_len != sizeof(i)) {
      std::cerr << "msg_iovlen != sizeof(i)" << std::endl;
      exit(1);
    }
    if (msg2.msg_iov[i].iov_base != reinterpret_cast<void*>(static_cast<uintptr_t>(i))) {
      std::cerr << "msg_iovbase != reinterpret_cast<void*>(static_cast<uintptr_t>(i))"
                << std::endl;
      exit(1);
    }
  }
  if (msg2.msg_controllen != 0) {
    std::cerr << "msg_controllen != 0" << std::endl;
    exit(1);
  }
  if (msg2.msg_flags != 0) {
    std::cerr << "msg_flags != 0" << std::endl;
    exit(1);
  }
  if (msg2.msg_namelen != 0) {
    std::cerr << "msg_namelen != 0" << std::endl;
    exit(1);
  }
  if (msg2.msg_name != nullptr) {
    std::cerr << "msg_name != nullptr" << std::endl;
    exit(1);
  }
  if (msg2.msg_iovlen != 10) {
    std::cerr << "msg_iovlen != 10" << std::endl;
    exit(1);
  } 
  //print status and return
  std::cout << "status: " << msg2.msg_iovlen << std::endl;
  return 0;
}
