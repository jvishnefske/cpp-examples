
// check if posix terminal is available
#ifdef __unix__
    #define HAS_POSIX_TERMINAL
    #include <termios.h>
#endif
#include <iostream>
#include <future>
#include <chrono>
#include <atomic>
#include <unistd.h>
#include <fcntl.h>
std::atomic_bool running{true};


// get a single character from stdin
char getch(){
    char buf = 0;
    std::cin.get(buf);
    return buf;
}

// get a single character from stdin without echo
char getche(){
    char buf = 0;
    std::cin.get(buf);
    std::cout.put(buf);
    return buf;
}

// get a single line from stdin
std::string getline(){
    std::string line;
    std::getline(std::cin, line);
    return line;
}
// check if a char is avaliable
bool kbhit(){
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if(ch != EOF) {
        ungetc(ch, stdin);
        return true;
    }
    return false;
}



void io(){
  raw();
  while(running){
     char ch{};
     //std::cin.get(ch);
      //read(0,&ch,1);
      //ch = getchar();
     //try to read char unbuffered since the other two options wait for return on linux
      getch();

      (void) ch;
     std::cout << "got input" << std::endl;
  }
  normal();
}

int main(){
    auto fut = std::async(io);
    fut.wait_for(std::chrono::seconds(5));
    running = false;
    std::cout << "waiting for future to stop" <<std::endl;
    fut.wait();
    std::cout << "complete" <<std::endl;
}
