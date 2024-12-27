#include <fstream>
#include <iostream>
#include "configManager.h"


namespace Config {
    nlohmann::json loadConfig(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open config file.");
        }
        nlohmann::json config;
        file >> config;
        return config;
    }

    const char* getBaseUrl(const nlohmann::json& config) {
        static std::string base = config["base_url"].get<std::string>();
        return base.c_str();
    }

    const char* getSensorApi(const nlohmann::json& config) {
        static std::string url = config["base_url"].get<std::string>() + config["sensor_api"].get<std::string>();
        return url.c_str();
    }

    const char* getLocationApi(const nlohmann::json& config) {
        static std::string url = config["base_url"].get<std::string>() + config["location_api"].get<std::string>();
        return url.c_str();
    }

    const char* getRadiationApi(const nlohmann::json& config) {
        static std::string url = config["base_url"].get<std::string>() + config["radiation_api"].get<std::string>();
        return url.c_str();
    }

    const char* getLoginApi(const nlohmann::json& config) {
        static std::string url = config["base_url"].get<std::string>() + config["login_api"].get<std::string>();
        return url.c_str();
    }

    const char* getEmail(const nlohmann::json& config) {
        static std::string email = config["email"].get<std::string>();
        return email.c_str();
    }

    const char* getPassword(const nlohmann::json& config) {
        static std::string password = config["password"].get<std::string>();
        return password.c_str();
    }

    double getLowRadiationLevel(const nlohmann::json& config) {
        return config["radiation_levels"]["low"];
    }

    double getModerateRadiationLevel(const nlohmann::json& config) {
        return config["radiation_levels"]["moderate"];
    }

    double getHighRadiationLevel(const nlohmann::json& config) {
        return config["radiation_levels"]["high"];
    }

    double getCriticalRadiationLevel(const nlohmann::json& config) {
        return config["radiation_levels"]["critical"];
    }

    int getSensorUpdateIntervalMs(const nlohmann::json& config) {
        return config["sensor_update_interval_ms"];
    }
}
