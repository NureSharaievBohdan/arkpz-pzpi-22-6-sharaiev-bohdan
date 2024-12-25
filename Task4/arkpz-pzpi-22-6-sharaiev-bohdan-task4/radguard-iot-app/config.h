#pragma once
#include <string>

namespace Config {

	// Server config
	const std::string BASE_URL = "http://localhost:8000/api/";
	const std::string SENSOR_API = BASE_URL + "sensors/";
	const std::string LOCATION_API = BASE_URL + "locations/";
	const std::string RADIATION_API = BASE_URL + "radiation-data/";
	const std::string LOGIN_API = BASE_URL + "auth/login/";

	// Admin Credetionals
	const std::string EMAIL = "bohdan.sharaiev@nure.ua";
	const std::string PASSWORD = "12345678";

	// Radiation Levels (in mSv/h)
	const double LEVEL_LOW = 0.1;
	const double LEVEL_MODERATE = 0.3;
	const double LEVEL_HIGH = 0.5;
	const double LEVEL_CRITICAL = 1.0;


	// Sleep Interval in ms between sensor 
	const int SENSOR_UPDATE_INTERVAL_MS = 3000;

}  
