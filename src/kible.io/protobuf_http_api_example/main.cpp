#include "../json/json.hpp"
#include <iostream>

#define LOGGER_INFO(...) logger_func(__VA_ARGS__, "INFO")

void logger_func(nlohmann::json obj, std::string hello) {
	obj["hello"] = hello;

	std::cout << obj.dump() << std::endl;
}

int main() {

LOGGER_INFO({
  {"pi", 3.141},
  {"happy", true},
  {"name", "Niels"},
  {"nothing", nullptr},
  {"answer", {
    {"everything", 42}
  }},
  {"list", {1, 0, 2}},
  {"object", {
    {"currency", "USD"},
    {"value", 42.99}
  }}
});

	return 0;
}