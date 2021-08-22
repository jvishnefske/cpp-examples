protobuf, flatpack, and qt moc allow runtime introspection of objects based on generated code. 
This considers wether it is possible to take one of those, and create a compile time (constexpr) introspectable interface such that arbitrary serializers/ deserialzer interfaces could be generated for new, or future interfaces.

- we prefer a pluggable template, or c++20 Concept based archatecture

### concept
 
    // my.pb -> protoc -> my.proto.hpp
    #import<my.proto.cpp>
    using MyDdsType = introspect<MyClass>::as<ddsType>;
    Publisher<MyDdsType> pub{"topicName"};
    p.publish("");
   

    // on client
    Subscriber<MyDdsType> sub{"topicName", [](MyDdsType d){
        // do we have an operator<< for dds message types?
        std::cout << d << std::endl;
    }}
 
