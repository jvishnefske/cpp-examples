#include <variant>
#include <array>
#include <future>
#include <functional>
#include <algorithm>
#include <asio.hpp>
// quic server
#include <quic/quic_server.h>
#include <quic/quic_connection.h>
#include <quic/quic_connection_helper.h>
#include <quic/quic_config.h>
#include <quic/quic_packet_writer.h>
#include <quic/quic_time.h>
#include <quic/quic_utils.h>
#include <quic/quic_flags.h>
#include <quic/quic_simple_server_session.h>
#include <quic/quic_simple_server_stream.h>
#include <quic/quic_simple_server_connection.h>
#include <quic/quic_simple_server_session.h>
struct QuicRequest{
    std::string method;
    std::string path;
    std::string version;
    std::string host;
    std::string content_type;
    std::string body;
    std::string header;
    std::string response;
    std::string response_header;
    std::string response_body;
    std::string response_status;
}
struct QuicResponse{
    std::string status;
    std::string header;
    std::string body;
}
class QuicServer: public quic::QuicServer {
public:
    QuicServer(const std::string& server_address, const std::string& config_file, const std::string& cert_file, const std::string& key_file)
        : QuicServer(server_address, quic::QuicConfig(), config_file, cert_file, key_file) {}

    QuicServer(const std::string& server_address,
               const quic::QuicConfig& config,
               const std::string& config_file,
               const std::string& cert_file,
               const std::string& key_file)
        : quic::QuicServer(quic::QuicConfig(), config_file, cert_file, key_file) {
        StartServer(server_address, config);
    }
    void OnConnection(quic::QuicConnection* connection) override {
        std::cout << "OnConnection" << std::endl;
        connection->set_debug_visitor(this);
        connection->set_visitor(this);
    }
    void OnSession(quic::QuicSession* session) override {
        std::cout << "OnSession" << std::endl;
        session->set_debug_visitor(this);
        session->set_visitor(this);
    }
    void OnStream(quic::QuicStream* stream) override {
        std::cout << "OnStream" << std::endl;
        stream->set_debug_visitor(this);
        stream->set_visitor(this);
    }
    void OnSessionClosed(quic::QuicSession* session) override {
        std::cout << "OnSessionClosed" << std::endl;
    }
    void OnStreamClosed(quic::QuicStream* stream) override {
        std::cout << "OnStreamClosed" << std::endl;
    }
    void OnSessionReady(quic::QuicSession* session) override {
        std::cout << "OnSessionReady" << std::endl;
    }
    void OnStreamReady(quic::QuicStream* stream) override {
        std::cout << "OnStreamReady" << std::endl;
    }
    void OnSessionConnectTimeout(quic::QuicSession* session) override {
        std::cout << "OnSessionConnectTimeout" << std::endl;
    }
    void OnStreamConnectTimeout(quic::QuicStream* stream) override {
        std::cout << "OnStreamConnectTimeout" << std::endl;
    }
    void OnSessionTicket(quic::QuicSession* session, const quic::QuicEncryptedPacket& packet) override {
        std::cout << "OnSessionTicket" << std::endl;
    }
    void OnStreamFrame(quic::QuicStream* stream, const quic::QuicStreamFrame& frame) override {
        std::cout << "OnStreamFrame" << std::endl;
    }
    void OnRstStream(quic::QuicStreamId stream_id, quic::QuicRstStreamErrorCode error_code) override {
        std::cout << "OnRstStream" << std::endl;
    }
    void OnConnectionClose(quic::QuicErrorCode error_code, const std::string& error_details, quic::ConnectionCloseSource source) override {
        std::cout << "OnConnectionClose" << std::endl;
    }
    void OnApplicationClose(quic::QuicErrorCode error_code, const std::string& error_details) override {
        std::cout << "OnApplicationClose" << std::endl;
    }
    void OnHandshakeDoneReceived(quic::QuicConnection* connection) override {
        std::cout << "OnHandshakeDoneReceived" << std::endl;
    }
    void OnHandshakeDoneSent(quic::QuicConnection* connection) override {
        std::cout << "OnHandshakeDoneSent" << std::endl;
    }
    void OnConnectionClosed(quic::QuicConnection* connection, quic::QuicErrorCode error_code, const std::string& error_details, quic::ConnectionCloseSource source) override {
        std::cout << "OnConnectionClosed" << std::endl;
    }
    void OnWriteBlocked(quic::QuicBlockedWriterInterface* blocked_writer) override {
        std::cout << "OnWriteBlocked" << std::endl;
    }
    void OnMessageReceived(quic::QuicConnection* connection, const quic::QuicMessageFrame& message) override {
        std::cout << "OnMessageReceived" << std::endl;
    }
    void OnMessageSent(quic::QuicConnection* connection, const quic::QuicMessageFrame& message) override {
        std::cout << "OnMessageSent" << std::endl;
    }
    void OnMessageAcked(quic::QuicConnection* connection, const quic::QuicMessageFrame& message, quic
