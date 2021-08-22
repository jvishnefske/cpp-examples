/**
 *stream tests
 */
#include <gmock/gmock.h>
#include <iosfwd>
#include <gtest/gtest.h>
#include <iostream>
#include <iosfwd>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/copy.hpp>

class CombinerDevice{
public:
    typedef char    char_type;
    typedef boost::iostreams::source_tag category;
    std::streamsize read(char_type *s, std::streamsize n){
        std::streamsize totalcount;
        //todo copy here.
        return -1;
    }
    std::vector<std::istream> m_streams;
};

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
TEST(tests, iostreamTransferTest){
    std::ostringstream oss{};
    std::istringstream iss{"inputString"};
    const int bufLen = 4;
    char buffer[bufLen] = {0,0,0,0};
    // stream longer than buffer
//    ASSERT_THAT(buffer,testing::ElementsAreArray({0, 0, 0, 0}));
}
TEST(tests, iostreamContainer){
    std::string input = "test string 1";
    std::string output;
    boost::iostreams::stream<container_source<std::string> > in(input);
    getline(in,output);
    ASSERT_EQ(input, output);
}
TEST(tests, multipleFiles){
    // open two temp files, and write to them.
    namespace io=boost::iostreams;
    {
        io::file_descriptor_sink f1("/tmp/file1");
        io::file_descriptor_sink f2("/tmp/file2");
        io::filtering_ostream o1;
        io::filtering_ostream o2;
        o1.push(f1);
        o2.push(f2);
        o1<<"file1";
        o2<<"file2";
    }
    {
        //read created files.
        io::filtering_istream input1;
        io::filtering_istream input2;

        input1.push(io::file_descriptor_source("/tmp/file1"));
        input2.push(io::file_descriptor_source("/tmp/file2"));
        std::string result{};
        io::filtering_ostream oss(io::back_inserter(result));
        io::copy(input2, oss);
        ASSERT_TRUE(oss.is_complete());
        //io::copy(input1, oss);
        ASSERT_EQ("file1file2", result);
    }
}