#include <future>
#include <array>
#include <variant>
#include <algorithm>
#include <iostream>
template<class T>
class ChainablePromise{
    public:
    ChainablePromise then(std::function<T(T)>);
    // takes same arguments as std::function Runable
    void operator()(T);
    private:
    Runnable functor;
    // run functor, and call all Children with result
    void run();
    // add child functor 
    ChainablePromise then(Runnable onSuccess){
        ChainablePromise child;
        child.functor = onSuccess;
        children.push_back(child);
        return child;
    }
    // run all children
    void operator()(ChainablePromise::FutureReturnType result){
        auto result = functor();
        for(auto& child : children){
            child(result);
        }
    }
    std::vector<ChainablePromise> children;
    using Runable=std::function<T(T)>;
    using FutureReturnType=std::future<T>;
    using FutureType=std::future<ChainablePromise::FutureReturnType>;
    using FutureVectorType=std::vector<FutureType>;
    using FutureVectorReturnType=std::vector<ChainablePromise::FutureReturnType>;
    // create a future that will return a vector of futures
    FutureVectorType createFutureVector(T);
    // create a future that will return a vector of futures
    FutureVectorReturnType createFutureVectorReturn(T);
    // create a future that will return a vector of futures
    FutureReturnType createFutureReturn(T);
    public:
    // create a future that will return a vector of futures
    FutureVectorType createFutureVector(T){
        FutureVectorType futures;
        for(auto& child : children){
            futures.push_back(child.createFutureReturn(result));
        }
        return futures;
    }
    // create a future that will return a vector of futures
    FutureVectorReturnType createFutureVectorReturn(T){
        FutureVectorReturnType futures;
        for(auto& child : children){
            futures.push_back(child.createFutureReturn(result));
        }
        return futures;
    }
    // create a future that will return a vector of futures
    FutureReturnType createFutureReturn(T){
        return std::async(std::launch::async, [this, result](){
            auto result = functor();
            for(auto& child : children){
                child(result);
            }
            return result;
        });
    }
};
void test_chainable_promise(){
    ChainablePromise p;
    p.then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        std::cout << i;
}
test_future_vector(){
    ChainablePromise p;
    p.then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        std::cout << i;
    });
    auto result = p.createFutureVector(0);
    for(auto& future : result){
        std::cout << future.get() << std::endl;
    }
}
test_future_vector_return(){
    ChainablePromise p;
    p.then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        std::cout << i;
    });
    auto result = p.createFutureVectorReturn(0);
    for(auto& future : result){
        std::cout << future << std::endl;
    }
}
test_future_return(){
    ChainablePromise p;
    p.then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        return i + 1;
    }).then([](int i){
        std::cout << i;
    });
    auto result = p.createFutureReturn(0);
    std::cout << result.get() << std::endl;
}
int main(){
    test_future_vector();
    test_future_vector_return();
    test_future_return();
}

