#include <future>
#include <functional>

template<class T>
class ChainablePromise {

public:
    using Runnable = std::function<T(T)>;
    using FutureReturnType = std::future<T>;

    void operator()(T);

    ChainablePromise then(Runnable onSuccess)
    {
        ChainablePromise<T> promise(onSuccess);
        m_children.push_back(promise);
        return promise;
    }

    ChainablePromise(Runnable functor);

    ChainablePromise();

    FutureReturnType runAsync(T arg)
    {
        auto future = std::async(std::launch::async, [this, arg]() {
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
    auto result = m_functor(arg);
    // exec all children async
    std::vector <FutureReturnType> futures;
    for (auto &child: m_children) {
        futures.push_back(child.runAsync(result));
    }
    for (auto &future: futures) {
        future.wait();
    }
}

void test_then()
{
    ChainablePromise<int> p;
    p.then([](int i) {
        return i + 1;
    });
    p(1);
}

int main()
{
    test_then();
}
