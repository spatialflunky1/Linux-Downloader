#include "httplib.h"
#include <nlohmann/json.hpp>
#include <sstream>

int main() {
    httplib::Client cli("72.231.177.233",80);
    auto res = cli.Post("/", "Ubuntu getvers", "text/plain");
    // auto res = cli.Get("/");
    if (res) {
        std::string body_text = res->body;
        std::vector<std::string> versions;
        std::stringstream s_stream(body_text);
        while (s_stream.good()) {
            std::string substr;
            getline(s_stream,substr,',');
            versions.push_back(substr);
        }
        for (int i=0; i<versions.size(); i++) {
            std::cout << versions.at(i) << "\n";
        }

    }
    else {
        std::cout << "Error: " << httplib::to_string(res.error()) << "\n";;
    }
}
