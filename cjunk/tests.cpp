/**
 *stream tests
 */

#include <gtest/gtest.h>
#include <iostream>
#include <iosfwd>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>



template<typename Container>
class container_source {
public:
    typedef typename Container::value_type  char_type;
    using category = boost::iostreams::source_tag;
    explicit container_source(Container& container)
            : container_(container), pos_(0)
    { }
    auto read(char_type* s, std::streamsize n) -> std::streamsize
    {
        using namespace std;
        auto amt = static_cast<streamsize>(container_.size() - pos_);
        streamsize result = (min)(n, amt);
        if (result != 0) {
            std::copy( container_.begin() + pos_,
                       container_.begin() + pos_ + result,
                       s );
            pos_ += result;
            return result;
        } else {
            return -1; // EOF
        }
    }
    auto container() -> Container&{ return container_; }
private:
    using size_type = typename Container::size_type;
    Container&  container_;
    size_type   pos_;
};


TEST(tests, iostreamContainer){
    std::string input = "test string 1";
    std::string output;
    boost::iostreams::stream<container_source<std::string> > in(input);
    std::string s1("");
    container_source<std::string> s(s1);
    std::vector<char> output2;
    s.read(output2.data(), output2.size());
    getline(in,output);
    ASSERT_EQ(input, output);
}

TEST(tests, test1){
    std::clog << "startying test1" <<std::endl;
    ASSERT_TRUE(false);
}