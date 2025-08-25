// Simplified parser using trivialJson instead of external JsonCpp
#include <iostream>
#include "json.hpp"

// The parse_json function is now provided by the Json::Reader compatibility layer
// No additional wrapper needed since Json::Value is aliased to Node

#ifdef STANDALONE_TEST
#include <iostream>

int main() {
    std::cout << "Testing trivialJson parser..." << std::endl;
    
    try {
        std::string test_json = "{\"key\": \"value\", \"number\": 42}";
        Json::Value result;
        Json::Reader reader;
        bool success = reader.parse(test_json, result);
        if (success) {
            std::cout << "JSON parsing test passed" << std::endl;
            std::cout << "Result: " << result.serialize() << std::endl;
        } else {
            std::cout << "JSON parsing failed: " << reader.getFormattedErrorMessages() << std::endl;
            return 1;
        }
        
        // Test JsonBuilder
        JsonBuilder builder;
        builder.start_object()
               .add_string_field("test", "data")
               .add_int_field("id", 100)
               .end_object();
        Node built = builder.build();
        std::cout << "JsonBuilder test: " << built.serialize() << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "JSON parsing test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
#endif