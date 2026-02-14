#pragma once
#include <string>
#include <vector>
#include <map>

struct City {
    std::string name;
    double x, y;
};

struct CityDistance {
    int targetIndex;
    double distance;
};

std::pair<std::vector<City>, std::map<int, std::vector<CityDistance>>> ReadCitiesFromFile(const std::string& filename);
