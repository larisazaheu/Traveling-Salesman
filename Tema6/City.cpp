#include <fstream>
#include <sstream>
#include <iostream>

#include "City.h"

std::pair<std::vector<City>, std::map<int, std::vector<CityDistance>>> ReadCitiesFromFile(const std::string& filename) {
    std::vector<City> cities;
    std::map<int, std::vector<CityDistance>> distances;

    std::ifstream file(filename);
    if (!file.is_open()) return { cities, distances };

    std::string line;
    int cityIndex = 0;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        std::string name;
        double x, y;
        ss >> name >> x >> y;
        if (ss.fail()) continue;
        cities.push_back({ name, x, y });

        std::vector<CityDistance> distList;
        std::string token;
        while (ss >> token) {
            std::string targetName = token;
            double d;
            ss >> d;
            int targetIndex = -1;
            for (int i = 0; i < cities.size(); ++i)
                if (cities[i].name == targetName) { 
                    targetIndex = i; 
                    break; 
                }
            if (targetIndex != -1) {
                distList.push_back({ targetIndex, d });
            }
        }
        if (!distList.empty()) distances[cityIndex] = distList;
        cityIndex++;
    }
    return { cities, distances };
}
