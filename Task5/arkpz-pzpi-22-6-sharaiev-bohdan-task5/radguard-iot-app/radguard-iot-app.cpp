#include <iostream>
#include <curl/curl.h>
#include <string>
#include <nlohmann/json.hpp>
#include <cmath>
#include <algorithm>
#include <thread>
#include "configManager.h" 

using json = nlohmann::json;
using namespace std;

string response_data;
const nlohmann::json& config = Config::loadConfig("config.json");

struct Sensor {
    int id;
    string sensor_name;
    string status;
    string last_update;
    int location;
    int user;
};


struct Location {
    int id;
    string latitude;
    string longitude;
    string city;
    string description;
};

vector<Sensor> sensors;
vector<Location> locations;

size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total_size = size * nmemb;
    response_data.append((char*)contents, total_size);
    return total_size;
}

string extract_token(const string& json_response) {
    try {
        json j = json::parse(json_response);
        if (j.contains("access")) {
            return j["access"];
        }
        else {
            cout << "Error getting access token!" << endl;
        }
    }
    catch (const json::parse_error& e) {
        cerr << "Error JSON: " << e.what() << endl;
    }
    return "";
}

string getJwtAcces(string email, string password) {
    CURL* curl = curl_easy_init();
    if (curl) {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL,Config::getLoginApi(config));
        string data = "{\"email\":\"" + email + "\", \"password\":\"" + password + "\"}";
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "Error while request: " << curl_easy_strerror(res) << endl;
            return "";
        }
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        cout << "HTTP response code: " << http_code << endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return extract_token(response_data);
    }
    return "";
}

vector<Sensor> getSensors(const string& token) {
    CURL* curl = curl_easy_init();
    vector<Sensor> sensors;
    if (curl) {
        CURLcode res;
        response_data.clear();
        curl_easy_setopt(curl, CURLOPT_URL, Config::getSensorApi(config));
        struct curl_slist* headers = NULL;
        string auth_header = "Authorization: Bearer " + token;
        headers = curl_slist_append(headers, auth_header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "Error while request: " << curl_easy_strerror(res) << endl;
            return sensors;
        }
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        cout << "HTTP code response: " << http_code << endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        try {
            json j = json::parse(response_data);
            for (const auto& sensor : j) {
                Sensor s;
                if (sensor.contains("id") && sensor["id"].is_number_integer()) s.id = sensor["id"];
                if (sensor.contains("sensor_name") && sensor["sensor_name"].is_string()) s.sensor_name = sensor["sensor_name"];
                if (sensor.contains("status") && sensor["status"].is_string()) s.status = sensor["status"];
                if (sensor.contains("last_update") && sensor["last_update"].is_string()) s.last_update = sensor["last_update"];
                if (sensor.contains("location") && sensor["location"].is_number_integer()) s.location = sensor["location"];
                if (sensor.contains("user") && sensor["user"].is_number_integer()) s.user = sensor["user"];
                sensors.push_back(s);
            }
        }
        catch (const json::parse_error& e) {
            cerr << "Error JSON: " << e.what() << endl;
        }
    }
    return sensors;
}

vector<Location> getLocations(const string& token) {
    CURL* curl = curl_easy_init();
    vector<Location> locations;
    if (curl) {
        CURLcode res;
        response_data.clear();
        curl_easy_setopt(curl, CURLOPT_URL, Config::getLocationApi(config));
        struct curl_slist* headers = NULL;
        string auth_header = "Authorization: Bearer " + token;
        headers = curl_slist_append(headers, auth_header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "Error while request: " << curl_easy_strerror(res) << endl;
            return locations;
        }
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        cout << "HTTP code response: " << http_code << endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        try {
            json j = json::parse(response_data);
            for (const auto& location : j) {
                Location loc;
                if (location.contains("id") && location["id"].is_number_integer()) loc.id = location["id"];
                if (location.contains("latitude") && location["latitude"].is_string()) loc.latitude = location["latitude"];
                if (location.contains("longitude") && location["longitude"].is_string()) loc.longitude = location["longitude"];
                if (location.contains("city") && location["city"].is_string()) loc.city = location["city"];
                if (location.contains("description") && location["description"].is_string()) loc.description = location["description"];
                locations.push_back(loc);
            }
        }
        catch (const json::parse_error& e) {
            cerr << "Error JSON: " << e.what() << endl;
        }
    }
    return locations;
}


double mymax(double a, double b) {
    return a > b ? a : b;
}


double calculateRadiationLevel(double latitude, double longitude, time_t lastUpdate) {
    double baseRadiation = Config::getLowRadiationLevel(config); 
    double variation = 0.1;          
    double timeFactor = 0.00005;    
    double geoFactor = sin(latitude) * cos(longitude);

    time_t currentTime = time(nullptr);
    double timeDiff = difftime(currentTime, lastUpdate); 

    double radiationLevel = baseRadiation +
        variation * geoFactor +
        timeFactor * timeDiff;

    double randomNoise = (rand() % 100 - 50) / 1000.0;
    radiationLevel += randomNoise;

   
    int randomChance = rand() % 100; 
    if (randomChance < 5) { 
        radiationLevel = Config::getCriticalRadiationLevel(config) + (rand() % 10) / 100.0;
    }
    else if (randomChance < 15) {
        radiationLevel = Config::getHighRadiationLevel(config) + (rand() % 5) / 100.0;
    }

    return mymax(0.0, radiationLevel);
}



double roundToTwoDecimalPlaces(double value) {
    return std::round(value * 100.0) / 100.0;
}

void updateSensorRadiation(std::vector<Sensor>& sensors, const std::vector<Location>& locations, const std::string& token) {
    CURL* curl = curl_easy_init();  


    if (!curl) {
        std::cerr << "Error init cURL" << std::endl;
        return;
    }

    struct curl_slist* headers = NULL;
    std::string auth_header = "Authorization: Bearer " + token;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth_header.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    for (auto& sensor : sensors) {
        auto location = std::find_if(locations.begin(), locations.end(),
            [&sensor](const Location& loc) { return loc.id == sensor.location; });

        if (location != locations.end()) {
            double radiationLevel = calculateRadiationLevel(
                std::stod(location->latitude), std::stod(location->longitude), time(nullptr));
            
            radiationLevel = roundToTwoDecimalPlaces(radiationLevel);

            std::cout << "Radiation: " << std::fixed << std::setprecision(2) << radiationLevel << " mSv" << std::endl;

            json data;
            data["sensor"] = sensor.id;
            data["radiation_level"] = radiationLevel;
            data["alert_triggered"] = Config::getHighRadiationLevel(config) <= radiationLevel ? true : false;

            std::string data_str = data.dump();

            curl_easy_setopt(curl, CURLOPT_URL, Config::getRadiationApi(config));
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data_str.c_str());

            std::string response_data;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
            
            

            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "Error while request: " << curl_easy_strerror(res) << std::endl;
            }
            else {
                long http_code = 0;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
                std::cout << "HTTP code response: " << http_code << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
        }
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}


int main() {
    string token = getJwtAcces(Config::getEmail(config), Config::getPassword(config));

    while (true) {
        if (!token.empty()) {
            vector<Sensor> sensors = getSensors(token);
            cout << "Sensor List:" << endl;
            for (const auto& sensor : sensors) {
                cout << "ID: " << sensor.id << ", "
                    << "Name: " << sensor.sensor_name << ", "
                    << "Status: " << sensor.status << ", "
                    << "Last Update: " << sensor.last_update <<
                    " Location: " << sensor.location << endl;
            }


            vector<Location> locations = getLocations(token);
            cout << "\nLocation list:" << endl;
            for (const auto& location : locations) {
                cout << "ID: " << location.id << ", "
                    << "Latitude: " << location.latitude << ", "
                    << "Longitude: " << location.longitude << ", "
                    << "City: " << location.city << ", "
                    << "Description: " << location.description << endl;
            }

            std::cout << "\n\n";

            updateSensorRadiation(sensors, locations, token);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(Config::getSensorUpdateIntervalMs(config))); 
    }

    return 0;
}
