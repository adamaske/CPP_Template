#include "Config.h"

#include "Core.h"

#include "Graphics.h"

#include "Networking.h"
#include "Server.h"
#include "Client.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


void InitLogger() {
    std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>("Main");
    spdlog::set_default_logger(logger);

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>(); //Console printing
    logger->sinks().push_back(console_sink);
}

void enum_to_char_array_testing() {
    enum test {
        first,
        second,
        third,
        sd,
    };

    std::vector<char> buffer;
    buffer.resize(sizeof(test)); // need enough room for the enum type
    test test_enum = second;
    // We want to get the state of an enum by reading a char array
    // firstly we must populate a char array with an enum
    // Look at the address of the 0th element as an address to a test value
    test* type_ptr = reinterpret_cast<test*>(&buffer[0]);  
    // using the address found above, set the value to the test_enum value, 
    // casting here is not nescary
    *type_ptr = static_cast<test>(test_enum);

    // By only looking at the char array, find the original test_enum value 
    //Look at the address of the 0th element as a address to a test value
    test* returned_enum_type_pointer = reinterpret_cast<test*>(&buffer[0]); 
    //get the value at the address (dereference)
    //the cast is not nessacary
    test actual_returned_enum = static_cast<test>(*returned_enum_type_pointer); 

    std::cout << "Original : ";
    switch (test_enum) {
    case first:
        std::cout << "first\n";
        break;
    case second:
        std::cout << "second\n";
        break;
    case third:
        std::cout << "third\n";
        break;
    }

    std::cout << "Parsed : ";
    switch (actual_returned_enum) {
    case first:
        std::cout << "first\n";
        break;
    case second:
        std::cout << "second\n";
        break;
    case third:
        std::cout << "third\n";
        break;
    }
}

void integer_extraction_test() {
    enum test {
        first,
        second,
        third,
        sd,
    };
    //Insert an integer into a char array
    //Find the inserted int by only reading the char array
    std::vector<char> buffer = {};
    int extraction_offset = 0;

    bool pre_insert_enum = false;
    if (pre_insert_enum) {
        //Before, the test enum is inserted into the char array
        buffer.resize(sizeof(test)); // need enough room for the enum type
        test test_enum = second;
        test* type_ptr = reinterpret_cast<test*>(&buffer[0]);
        *type_ptr = static_cast<test>(test_enum);
        //When reading the value of the integeter to be inserted 
        // skip this enum definiation
        extraction_offset = sizeof(test);
    }

    uint32_t data = 32;
    int size = sizeof(uint32_t);
    //Insert
    buffer.insert(buffer.end(), (char*)&data, (char*)&data + 4);

    //Extract
    uint32_t extracted;
    extracted = *reinterpret_cast<uint32_t*>(&buffer[extraction_offset]);
    
    extraction_offset += sizeof(uint32_t); //4 + 4 = 8 -> 

    std::cout << "Enum inserted : " << (pre_insert_enum ? "true" : "false") << "\n";
    std::cout << "Inserted : " << data << "\n";
    std::cout << "Extracted : " << extracted << "\n";
}

void string_extraction_test() {
    enum test {
        first,
        second,
        third,
        sd,
    };
    std::vector<char> buffer = {};
    int extraction_offset = 0;

    bool pre_insert_enum = false;
    if (pre_insert_enum) {
        //Before, the test enum is inserted into the char array
        buffer.resize(sizeof(test)); // need enough room for the enum type
        test test_enum = second;
        test* type_ptr = reinterpret_cast<test*>(&buffer[0]);
        *type_ptr = static_cast<test>(test_enum);
        //When reading the value of the integeter to be inserted 
        // skip this enum definiation
        extraction_offset = sizeof(test);
    }

    std::string insert_string = "test message";
    uint32_t data = insert_string.size();
    int size = sizeof(uint32_t);
    // Insert size of the string
    buffer.insert(  buffer.end(), 
                    (char*)&data, 
                    (char*)&data + sizeof(uint32_t));

    // Insert the string data
    buffer.insert(  buffer.end(), 
                    insert_string.data(),
                    insert_string.data() + insert_string.size());


    // Read string size
    uint32_t string_size;
    string_size = *reinterpret_cast<uint32_t*>(&buffer[extraction_offset]);
    extraction_offset += sizeof(uint32_t);

    std::string extracted_string;
    extracted_string.resize(string_size);
    extracted_string.assign(&buffer[extraction_offset], string_size);

    std::cout << "Inserted string : " << insert_string << "\n";
    std::cout << "Inserted string size : " << data << "\n";
    std::cout << "Extracted string : " << extracted_string << "\n";
    std::cout << "Extracted string size : " << string_size << "\n";
    
}

int main(int argc, char* argv[]){

    std::cout << "Enum to Char Array Test :\n";
    enum_to_char_array_testing();
    std::cout << "\n\nInteger Extraction Test :\n";
    integer_extraction_test();
    std::cout << "\n\nString Extraction Test :\n";
    string_extraction_test();

    InitLogger();

    Networking::Intialize(),
    spdlog::info("TEMPLATE VERSION " + std::to_string(Template_VERSION_MAJOR) + "." + std::to_string(Template_VERSION_MINOR));

    Graphics graphics;
    graphics.Init("Template"); //TODO : How to handle loggerwindow, its created here if Logger::OutputType = L_GUI

    Server server;
    server.Initialize(IPEndpoint("localhost", 8000));
    
    int result = 1;
    while (result) {

        server.Frame(); //Runs the server
        result = graphics.Render(); //return 0 when window closes
    }

    graphics.Shutdown();
    Networking::Shutdown();
    return 0;
}
