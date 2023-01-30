// import opencv
//#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <vector>
#if 0
// include ffmpeg network stream reader
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#endif
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/host_name.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
class NetworkStreamBase{
    static constexpr int kMaxBuffer=1024;
    protected:
    NetworkStreamBase():
            io_service_()
            , socket_(io_service_)
            , read_buffer_(kMaxBuffer)
            , write_buffer_(kMaxBuffer)
            , is_running_(false)
            , is_reading_(false)
            , is_writing_(false)
            , is_connected_(false)
            , is_disconnected_(false)
    {}
    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf read_buffer_;
    boost::asio::streambuf write_buffer_;
    bool is_running_;
    bool is_reading_;
    bool is_writing_;
    bool is_connected_;
    bool is_disconnected_;
    std::thread read_thread_;
    std::thread write_thread_;
    public:
    void stop(){
        is_running_ = false;
    }
};
class NetworkVideoStreamObservable: public NetworkStreamBase{
    NetworkVideoStreamObservable(std::string url)
            : url_(url)
    {
        boost::system::error_code ec;
        boost::asio::ip::tcp::resolver resolver(io_service_);
        boost::asio::ip::tcp::resolver::query query(url_, "80");
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query, ec);
        if (ec) {
            std::cout << "Error: " << ec.message() << std::endl;
            return;
        }
        boost::asio::connect(socket_, iterator, ec);
        if (ec) {
            std::cout << "Error: " << ec.message() << std::endl;
            return;
        }
        is_connected_ = true;
        is_disconnected_ = false;
        is_running_ = true;
        is_reading_ = true;
        is_writing_ = true;
        read_thread_ = std::thread([this](){read_loop();});
        write_thread_ = std::thread([this](){write_loop();});
    }
    ~NetworkVideoStreamObservable() {
        is_running_ = false;
        is_reading_ = false;
        is_writing_ = false;
        read_thread_.join();
        write_thread_.join();
    }
    void read_loop() {
        while (is_running_) {
            if (is_reading_) {
                boost::system::error_code ec;
                boost::asio::read(socket_, read_buffer_, boost::asio::transfer_at_least(1), ec);
                if (ec) {
                    std::cout << "Error: " << ec.message() << std::endl;
                    is_connected_ = false;
                    is_disconnected_ = true;
                    is_reading_ = false;
                    break;
                }
            }
        }
    }
    void write_loop() {
        while (is_running_) {
            if (is_writing_) {
                boost::system::error_code ec;
                boost::asio::write(socket_, write_buffer_, boost::asio::transfer_at_least(1), ec);
                if (ec) {
                    std::cout << "Error: " << ec.message() << std::endl;
                    is_connected_ = false;
                    is_disconnected_ = true;
                    is_writing_ = false;
                    break;
                }
            }
        }
    }
    void write(const char* data, int size) {
        if (is_connected_) {
            std::string tmp(data,size);
            std::ostream os(&write_buffer_);
            os << tmp;
        }
    }
    void write(const std::string& data) {
        if (is_connected_) {
            std::ostream os(&write_buffer_);
            os << data;
        }
    }
    private:
    std::string url_;
};
#if USE_X264
// x264 frame decoder
class X264FrameDecoder {
    X264FrameDecoder()
            : av_codec_context_(NULL)
            , av_frame_(NULL)
            , av_packet_(NULL)
            , av_codec_(NULL)
            , av_format_context_(NULL)
            , av_stream_(NULL)
            , av_codec_context_stream_(NULL)
            , av_codec_context_stream_index_(0)
            , av_frame_stream_(NULL)
            , av_frame_stream_index_(0)
    {
        av_init_packet(av_packet_);
    }
    ~X264FrameDecoder() {
        avcodec_close(av_codec_context_);
        av_free(av_packet_);
        av_free(av_frame_);
        av_free(av_codec_context_);
        av_free(av_frame_stream_);
        av_free(av_codec_context_stream_);
        avformat_close_input(&av_format_context_);
    }
    bool open(const char* url) {
        av_register_all();
        av_format_context_ = avformat_alloc_context();
        if (avformat_open_input(&av_format_context_, url, NULL, NULL) != 0) {
            std::cout << "Error: avformat_open_input" << std::endl;
            return false;
        }
        if (avformat_find_stream_info(av_format_context_, NULL) < 0) {
            std::cout << "Error: avformat_find_stream_info" << std::endl;
            return false;
        }
        av_codec_context_stream_index_ = av_find_best_stream(av_format_context_, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
        if (av_codec_context_stream_index_ < 0) {
            std::cout << "Error: av_find_best_stream" << std::endl;
            return false;
        }
        av_codec_context_stream_ = av_format_context_->streams[av_codec_context_stream_index_];
        av_codec_context_ = avcodec_alloc_context3(NULL);
        avcodec_parameters_to_context(av_codec_context_, av_codec_context_stream_->codecpar);
        av_codec_ = avcodec_find_decoder(av_codec_context_->codec_id);
        if (av_codec_ == NULL) {
            std::cout << "Error: avcodec_find_decoder" << std::endl;
            return false;
        }
        if (avcodec_open2(av_codec_context_, av_codec_, NULL) < 0) {
            std::cout << "Error: avcodec_open2" << std::endl;
            return false;
        }
        av_frame_stream_ = av_frame_alloc();
        av_frame_stream_index_ = av_find_best_stream(av_format_context_, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
        if (av_frame_stream_index_ < 0) {
            std::cout << "Error: av_find_best_stream" << std::endl;
            return false;
        }
        av_frame_stream_ = av_format_context_->streams[av_frame_stream_index_]->codec;
        return true;
    }
    bool read(cv::Mat& frame) {
        if (av_frame_stream_ == NULL) {
            std::cout << "Error: av_frame_stream_ is NULL" << std::endl;
            return false;
        }
        if (av_frame_stream_->width == 0 || av_frame_stream_->height == 0) {
            std::cout << "Error: av_frame_stream_->width or av_frame_stream_->height is 0" << std::endl;
            return false;
        }
        if (av_frame_stream_->format != AV_PIX_FMT_YUV420P) {
            std::cout << "Error: av_frame_stream_->format is not AV_PIX_FMT_YUV420P" << std::endl;
            return false;
        }
        if (av_frame_stream_->width != frame.cols || av_frame_stream_->height != frame.rows) {
            frame.create(av_frame_stream_->height, av_frame_stream_->width, CV_8UC3);
        }
        if (av_read_frame(av_format_context_, av_packet_) < 0) {
            std::cout << "Error: av_read_frame" << std::endl;
            return false;
        }
        if (av_packet_->stream_index != av_frame_stream_index_) {
            std::cout << "Error: av_packet_->stream_index != av_frame_stream_index_" << std::endl;
            return false;
        }
        if (avcodec_send_packet(av_codec_context_, av_packet_) < 0) {
            std::cout << "Error: avcodec_send_packet" << std::endl;
            return false;
        }
        if (avcodec_receive_frame(av_codec_context_, av_frame_stream_) < 0) {
            std::cout << "Error: avcodec_receive_frame" << std::endl;
            return false;
        }
        if (av_frame_stream_->width != frame.cols || av_frame_stream_->height != frame.rows) {
            frame.create(av_frame_stream_->height, av_frame_stream_->width, CV_8UC3);
        }
        if (av_frame_stream_->format != AV_PIX_FMT_YUV420P) {
            std::cout << "Error: av_frame_stream_->format is not AV_PIX_FMT_YUV420P" << std::endl;
            return false;
        }
        if (av_frame_stream_->width != frame.cols || av_frame_stream_->height != frame.rows) {
            frame.create(av_frame_stream_->height, av_frame_stream_->width, CV_8UC3);
        }
        if (av_frame_stream_->format != AV_PIX_FMT_YUV420P) {
            std::cout << "Error: av_frame_stream_->format is not AV_PIX_FMT_YUV420P" << std::endl;
            return false;
        }
        if (av_frame_stream_->width != frame.cols || av_frame_stream_->height != frame.rows) {
            frame.create(av_frame_stream_->height, av_frame_stream_->width, CV_8UC3);
        }
        if (av_frame_stream_->format != AV_PIX_FMT_YUV420P) {
            std::cout << "Error: av_frame_stream_->format is not AV_PIX_FMT_YUV420P" << std::endl;
            return false;
        }
        if (av_frame_stream_->format != AV_PIX_FMT_YUV420P) {
            std::cout << "Error: av_frame_stream_->format is not AV_PIX_FMT_YUV420P" << std::endl;
            return false;
        }
        if (av_frame_stream_->format != AV_PIX_FMT_YUV420P) {
            std::cout << "Error: av_frame_stream_->format is not AV_PIX_FMT_YUV420P" << std::endl;
            return false;
        }
        //save result
        for (int i = 0; i < av_frame_stream_->height; i++) {
            for (int j = 0; j < av_frame_stream_->width; j++) {
                frame.at<cv::Vec3b>(i, j)[0] = av_frame_stream_->data[0][i * av_frame_stream_->linesize[0] + j];
                frame.at<cv::Vec3b>(i, j)[1] = av_frame_stream_->data[1][i * av_frame_stream_->linesize[1] + j];
                frame.at<cv::Vec3b>(i, j)[2] = av_frame_stream_->data[2][i * av_frame_stream_->linesize[2] + j];
            }
        }
        return true;
    }
    void set_frame_size(int width, int height) {
        av_frame_stream_->width = width;
        av_frame_stream_->height = height;
    }
    void set_frame_rate(int fps) {
        av_frame_stream_->time_base.num = 1;
        av_frame_stream_->time_base.den = fps;
    }
    void set_frame_rate(int num, int den) {
        av_frame_stream_->time_base.num = num;
        av_frame_stream_->time_base.den = den;
    }
    void set_frame_rate(double fps) {
        av_frame_stream_->time_base.num = 1;
        av_frame_stream_->time_base.den = (int)(fps * 100);
    }
    void set_frame_rate(double num, double den) {
        av_frame_stream_->time_base.num = (int)(num * 100);
        av_frame_stream_->time_base.den = (int)(den * 100);
    }
};
void test_x264_frame_decoder() {
    X264FrameDecoder x264_frame_decoder;
    x264_frame_decoder.set_frame_size(640, 480);
    x264_frame_decoder.set_frame_rate(30);
    x264_frame_decoder.open("rtsp://192.168.1.1/live.sdp");
    cv::Mat frame;
    while (true) {
        x264_frame_decoder.read(frame);
        cv::imshow("frame", frame);
        cv::waitKey(1);
    }
}

#endif // USE_X264
// ffmpeg network stream reader

// ffmpeg network stream reader
class FFmpegNetworkStreamReader: public NetworkStreamBase {
    public:
    FFmpegNetworkStreamReader(std::string url)
            : url_(url)
    {
        boost::system::error_code ec;
        boost::asio::ip::tcp::resolver resolver(io_service_);
        boost::asio::ip::tcp::resolver::query query(url_, "80");
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query, ec);
        if (ec) {
            std::cout << "Error: " << ec.message() << std::endl;
            return;
        }
        boost::asio::connect(socket_, iterator, ec);
        if (ec) {
            std::cout << "Error: " << ec.message() << std::endl;
            return;
        }
        is_connected_ = true;
    }

    ~FFmpegNetworkStreamReader() {
        if (is_connected_) {
            socket_.close();
        }
    }

    void start() {
        is_running_ = true;
        is_reading_ = true;
        is_writing_ = true;
        read_thread_ = std::thread(&FFmpegNetworkStreamReader::read_loop, this);
        write_thread_ = std::thread(&FFmpegNetworkStreamReader::write_loop, this);
    }

    void stop() {
        is_running_ = false;
        is_reading_ = false;
        is_writing_ = false;
        read_thread_.join();
        write_thread_.join();
    }
    void read_loop() {
        while (is_running_) {
            if (!is_connected_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            if (!is_reading_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
#if 0
            // todo fix buffer type conversion.
            boost::system::error_code ec;
            size_t bytes_read = socket_.read_some(read_buffer_, ec);
            if (ec) {
                std::cout << "Error: " << ec.message() << std::endl;
                is_connected_ = false;
                is_disconnected_ = true;
                break;
            }
            if (bytes_read == 0) {
                std::cout << "Error: read 0 bytes" << std::endl;
                is_connected_ = false;
                is_disconnected_ = true;
                break;
            }
            if (bytes_read > 0) {
                std::cout << "Read " << bytes_read << " bytes" << std::endl;
            }
#endif
        }
    }

    void write_loop() {
        while (is_running_) {
            if (!is_connected_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            if (!is_writing_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
#if 0
            // todo fix buffer type conversion.
            boost::system::error_code ec;
            size_t bytes_written = socket_.write_some(write_buffer_, ec);
            if (ec) {
                std::cout << "Error: " << ec.message() << std::endl;
                is_connected_ = false;
                is_disconnected_ = true;
                break;
            }
            if (bytes_written == 0) {
                std::cout << "Error: write 0 bytes" << std::endl;
                is_connected_ = false;
                is_disconnected_ = true;
                break;
            }
            if (bytes_written > 0) {
                std::cout << "Write " << bytes_written << " bytes" << std::endl;
            }
#endif
        }
    }
private:
    std::string url_;
};

// ffmpeg network stream writer
class FFmpegNetworkStreamWriter :public NetworkStreamBase{
    public:
    FFmpegNetworkStreamWriter(std::string url)
            : url_(url)
    {
        boost::system::error_code ec;
        boost::asio::ip::tcp::resolver resolver(io_service_);
        boost::asio::ip::tcp::resolver::query query(url_, "80");
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query, ec);
        if (ec) {
            std::cout << "Error: " << ec.message() << std::endl;
            return;
        }
        boost::asio::connect(socket_, iterator, ec);
        if (ec) {
            std::cout << "Error: " << ec.message() << std::endl;
            return;
        }
        is_connected_ = true;
    }

    ~FFmpegNetworkStreamWriter() {
        if (is_connected_) {
            socket_.close();
        }
    }
    void start() {
#if 0 // write thread currently unimplemented, may not be needed for design.
        is_running_ = true;
        is_reading_ = true;
        is_writing_ = true;
        read_thread_ = std::thread([this](){read_loop();});
        write_thread_ = std::thread([this](){write_loop();});
#endif
    }
    private:
    std::string url_;
};
    // ffmpeg network stream
class FFmpegNetworkStream {
    public:
    FFmpegNetworkStream(std::string url)
            : url_(url)
            , reader_(url)
            , writer_(url)
    {
        reader_.start();
        writer_.start();
    }

    ~FFmpegNetworkStream() {
        reader_.stop();
        writer_.stop();
    }
    private:
    std::string url_;
    FFmpegNetworkStreamReader reader_;
    FFmpegNetworkStreamWriter writer_;
};
void test_ffmpeg_network_stream() {
    FFmpegNetworkStream stream("http://127.0.0.1:8080/live.flv");
}

int main() {
    test_ffmpeg_network_stream();
    return 0;
}
