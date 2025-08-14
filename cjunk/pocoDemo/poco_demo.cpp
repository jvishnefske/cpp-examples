//#define CATCH_CONFIG_MAIN 1
//#include <catch/catch.hpp>
#include <Poco/Task.h>
#include <Poco/TaskManager.h>
#include <Poco/Thread.h>
#include <Poco/LogStream.h>
#include <Poco/ThreadPool.h>
#include <iostream>

class myTask: public Poco::Task{
// disable copy
public:
    // cppcheck-suppress unusedFunction
    explicit myTask(const std::string & name) :Task(name) {
        (void) name;
    }
    int jumps=0;
    void runTask() override{
        std::cout << "starting task " << name()<< std::endl;
        while(!isCancelled()){
            jumps++;
        }
        std::cout << name() << " jumped " << jumps << "times."<<std::endl;

    }
};
using Poco::LogStream;
void task_from_pocoThreadPool(){

    //    Poco::ThreadPool::defaultPool().start(t1);
//    Poco::ThreadPool::defaultPool().
    std::cout << "started t1." << std::endl;
    //Poco::ThreadPool::defaultPool().start(t2);
    //t1.cancel();
    std::cout << "requested cancel t1." << std::endl;

//    t2.cancel();
    Poco::ThreadPool::defaultPool().joinAll();
}

void TEST_CASE(){
    //poco_bugcheck_msg("starting testcase");
    Poco::Logger& log = Poco::Logger::get("testcase");
    Poco::ThreadPool threadPool;
    log.fatal("starting");
    Poco::TaskManager tm;
    myTask t1{"first"};
    myTask t2{"second"};
    tm.start(&t1);
    tm.start(&t2);
    tm.cancelAll();
}
auto main() -> int{
    Poco::Logger& log = Poco::Logger::get("l1");
    log.fatal("main");
    log.debug("debug");
    log.information("info");
    log.warning("warn");
    std::cout << "log1" << std::endl;
    task_from_pocoThreadPool();
//    TEST_CASE();

    std::clog << "log2" << std::endl;
    return 0;
}