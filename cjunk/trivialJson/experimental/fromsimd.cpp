//simdjson
#include <simdjson.h>
#include <simdjson/parsing.h>
#include <simdjson/encoding.h>
#include <simdjson/document.h>
#include <simdjson/writer.h>
#include <simdjson/stringbuffer.h>
#include <simdjson/prettywriter.h>
#include <simdjson/error.h>
#include <variant>

struct JsonNode
    using Map=std::unique_ptr<std::map<std::string, JsonNode>>
    using List = std::unique_ptr<std::vector<JsonNode>>
    std::variant<Map,List, double, long> storage
    };
    
JsonNode convert(simdjson::parsing::node& input)
{
    JsonNode result;
    switch(input.type())
    {
        case simdjson::parsing::node_type::number_integer:
            result.storage=input.get<long>();
            break;
        case simdjson::parsing::node_type::number_unsigned:
            result.storage=input.get<unsigned long>();
            break;
        case simdjson::parsing::node_type::number_float:
            result.storage=input.get<double>();
            break;
        case simdjson::parsing::node_type::string:
            result.storage=input.get<std::string>();
            break;
        case simdjson::parsing::node_type::boolean:
            result.storage=input.get<bool>();
            break;
        case simdjson::parsing::node_type::array:
        {
            auto& array=input.get<simdjson::parsing::array_node>();
            result.storage=std::make_unique<std::vector<JsonNode>>();
            for(auto& element: array)
            {
                result.storage->push_back(convert(element));
            }
            break;
        }
        case simdjson::parsing::node_type::object:
        {
            auto& object=input.get<simdjson::parsing::object_node>();
            result.storage=std::make_unique<std::map<std::string, JsonNode>>();
            for(auto& element: object)
            {
                result.storage->insert({element.first, convert(element.second)});
            }
            break;
        }
        default:
            break;
    }
    return result;
}

int main(int argc, char** argv)
{
    if(argc!=2)
    {
        std::cout<<"Usage: "<<argv[0]<<" <json file>"<<std::endl;
        return 1;
    }
    std::string filename=argv[1];
    std::ifstream file(filename);
    if(!file.is_open())
    {
        std::cout<<"Could not open file "<<filename<<std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer<<file.rdbuf();
    file.close();
    std::string json=buffer.str();
    simdjson::dom::parser parser;
    simdjson::parsing::document doc;
    parser.parse(json, doc);
    JsonNode result=convert(doc.get_root());
    std::cout<<"Result: "<<result.storage<<std::endl;
    return 0;
}
