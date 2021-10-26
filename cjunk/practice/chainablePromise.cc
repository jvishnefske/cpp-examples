// modernize
#include <iostream>
typename<class T>
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
    });
    p.operator()(0);
}
int main(){
    test_chainable_promise();
}

