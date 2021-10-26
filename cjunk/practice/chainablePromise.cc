// fix compile errors 
#include <future>
#include <array>
#include <variant>
#include <algorithm>
#include <iostream>

template<class T>
class ChainablePromise {
public:
    using Runnable = std::function<T(T)>;
    using FutureReturnType = std::future<T>;
    using FutureType = std::future<ChainablePromise::FutureReturnType>;
    using FutureVectorType = std::vector<FutureType>;
    using FutureVectorReturnType = std::vector<ChainablePromise::FutureReturnType>;


    // takes same arguments as std::function Runable
    void operator()(T);

    // run functor, and call all Children with result
    // add child functor
    ChainablePromise<T> then(Runnable onSuccess)
    {
        ChainablePromise<T> child;
        child.functor = onSuccess;
        children.push_back(child);
        return child;
    }

public:
    // run all children
    void operator()(T argument)
    {
        auto result = functor(argument);
        for (auto &child: children) {
            child(result);
        }
    }
    // this seems like an interesting direction if it could be implemented

#if 0

public:
    // create a future that will return a vector of futures
    FutureVectorType createFutureVector(T);

    // create a future that will return a vector of futures
    FutureVectorReturnType createFutureVectorReturn(T);

    // create a future that will return a vector of futures
    FutureReturnType createFutureReturn(T);

    // create a future that will return a vector of futures
    FutureVectorType createFutureVector(T)
    {
        FutureVectorType futures;
        for (auto &child: children) {
            futures.push_back(child.createFutureReturn(result));
        }
        return futures;
    }

    // create a future that will return a vector of futures
    FutureVectorReturnType createFutureVectorReturn(T)
    {
        FutureVectorReturnType futures;
        for (auto &child: children) {
            futures.push_back(child.createFutureReturn(result));
        }
        return futures;
    }

    // create a future that will return a vector of futures
    FutureReturnType createFutureReturn(T)
    {
        FutureReturnType result;
        return std::async(std::launch::async, [this, result]() {
            auto result = functor();
            for (auto &child: children) {
                child(result);
            }
            return result;
        });

    }

#endif

public:
    Runnable functor;
private:
    std::vector <ChainablePromise> children;
};

void test_chainable_promise()
{
    ChainablePromise<int> p;
    p.then([](int i) {
        return i + 1;
    }).then([](int i) {
        return i + 1;
    });
    // check result
    std::cout << p.createFutureReturn(0).get() << std::endl;
}

int main()
{
    test_chainable_promise();
}
