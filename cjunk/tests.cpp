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
    typedef boost::iostreams::source_tag    category;
    container_source(Container& container)
            : container_(container), pos_(0)
    { }
    std::streamsize read(char_type* s, std::streamsize n)
    {
        using namespace std;
        streamsize amt = static_cast<streamsize>(container_.size() - pos_);
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
    Container& container() { return container_; }
private:
    typedef typename Container::size_type   size_type;
    Container&  container_;
    size_type   pos_;
};


TEST(tests, iostreamContainer){
    std::string input = "test string 1";
    std::string output;
    boost::iostreams::stream<container_source<std::string> > in(input);
    getline(in,output);
    ASSERT_EQ(input, output);
}

TEST(tests, test1){
    std::clog << "startying test1" <<std::endl;
    ASSERT_TRUE(false);
}