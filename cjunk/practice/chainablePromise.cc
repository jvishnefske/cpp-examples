#include <future>
#include <functional>
#include <iostream>
#include <vector>
template<class T>
class ChainablePromise {

public:
    using Runnable = std::function<T(T)>;
    using FutureReturnType = std::future<T>;

    void operator()(T);
    ChainablePromise then(ChainablePromise<T> promise){
        m_children.push_back(promise);
        return promise;
    }

    ChainablePromise then(Runnable onSuccess)
    {
        ChainablePromise<T> promise(onSuccess);
        m_children.push_back(promise);
        return promise;
    }
    // explicit destructor to disable trivial copy constructor.
    ~ChainablePromise(){}
    ChainablePromise(Runnable functor);

    ChainablePromise();

    FutureReturnType runAsync(T arg, std::launch policy=std::launch::async)
    {
        std::cout << __FUNCTION__  << ":" << __LINE__ << std::endl;

        auto future = std::async(policy, [this, arg]() {
            std::cout << __FUNCTION__  << ":" << __LINE__ << std::endl;

            return m_functor(arg);
        });
        return future;
    }

private:
    Runnable m_functor;
    std::vector <ChainablePromise<T>> m_children;
};

template<class T>
ChainablePromise<T>::ChainablePromise()
        : m_functor([](T arg) { return arg; })
{}

template<class T>
ChainablePromise<T>::ChainablePromise(Runnable functor)
        : m_functor(functor)
{
}

template<class T>
void ChainablePromise<T>::operator()(T arg)
{
    std::cout << __FUNCTION__  << ":" << __LINE__ << "functor" << (bool)(m_functor) << " type " << m_functor.target_type().name() << std::endl;

    auto result = m_functor(arg);
    // exec all children async
    std::vector <FutureReturnType> futures;
    for (auto &child: m_children) {
        std::cout << __FUNCTION__  << ":" << __LINE__ << std::endl;
        child(result);
        //futures.push_back(child.runAsync(result, std::launch{}));
    }
    for (auto &future: futures) {
        std::cout << __FUNCTION__  << ":" << __LINE__ << std::endl;

        future.wait();
    }
}

void test_then()
{
    std::cout << __FUNCTION__  << ":" << __LINE__ << std::endl;
    ChainablePromise<int> p;
    p.then([](int i) {
        return i + 1;
    }).then([](int t){
        std::cout << "##################" << t << std::endl;
        std::cout << __FUNCTION__  << ":" << __LINE__ << std::endl;

        return 0;
    });
    p(1);
    //p(2);

}

int main()
{
    std::cout << __FUNCTION__  << ":" << __LINE__ << std::endl;
    test_then();
}
