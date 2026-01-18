#include "api/api_server.h"
#include <iostream>

int main() {
    try {
        ApiServer server;
        server.run("0.0.0.0", 8000);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}