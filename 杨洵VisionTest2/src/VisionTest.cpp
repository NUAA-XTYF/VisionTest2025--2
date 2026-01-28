#include "VisionTest.h"
#include <yaml-cpp/yaml.h>
#include <iostream>

std::string VisionTest::run() {
	try {
		// Construct a small node and read it back via yaml-cpp
		YAML::Node node;
		node["status"] = "ok";

		std::string status = node["status"].as<std::string>();
		std::cout << "yaml-cpp produced status: " << status << std::endl;

		return (status == "ok") ? "Passed" : throw std::runtime_error("Status not ok");
	} catch (const std::exception &e) {
		std::cerr << "yaml-cpp exception: " << e.what() << std::endl;
        throw;
    }
}
