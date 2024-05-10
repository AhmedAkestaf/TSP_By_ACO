#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include <ctime>

// Define the parameters
const int screenWidth = 800;
const int screenHeight = 600;
const float coolingRate = 0.9999;
const int maxIterations = 50000;

// Define the city structure
struct City {
    float x, y;
};

// Function to calculate distance between two cities
float distance(const City& city1, const City& city2) {
    float dx = city1.x - city2.x;
    float dy = city1.y - city2.y;
    return std::sqrt(dx*dx + dy*dy);
}

// Function to calculate total distance of a tour
float tourDistance(const std::vector<City>& tour) {
    float totalDistance = 0;
    for (int i = 0; i < tour.size() - 1; ++i) {
        totalDistance += distance(tour[i], tour[i+1]);
    }
    totalDistance += distance(tour.back(), tour.front()); // Connect last city to first city
    return totalDistance;
}

// Function to simulate annealing process
std::vector<City> simulateAnnealing(const std::vector<City>& cities) {
    std::vector<City> currentTour = cities;
    std::vector<City> bestTour = cities;
    float currentDistance = tourDistance(currentTour);
    float bestDistance = currentDistance;
    std::default_random_engine generator(time(nullptr));
    std::uniform_int_distribution<int> distribution(0, cities.size() - 1);

    for (int i = 0; i < maxIterations; ++i) {
        // Generate random indices for swapping cities
        int cityIndex1 = distribution(generator);
        int cityIndex2 = distribution(generator);
        std::swap(currentTour[cityIndex1], currentTour[cityIndex2]);

        // Calculate new distance
        float newDistance = tourDistance(currentTour);

        // Acceptance probability
        float acceptanceProbability = std::exp((currentDistance - newDistance) / i);

        // Decide whether to accept the new tour
        if (acceptanceProbability > static_cast<float>(rand()) / RAND_MAX) {
            currentDistance = newDistance;
            if (currentDistance < bestDistance) {
                bestTour = currentTour;
                bestDistance = currentDistance;
            }
        } else {
            // Revert back to the previous tour
            std::swap(currentTour[cityIndex1], currentTour[cityIndex2]);
        }

        // Cool down
        currentDistance *= coolingRate;
    }

    return bestTour;
}

int main() {
    // Create SFML window
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Traveling Salesman Problem");

    // Vector to store selected cities
    std::vector<City> cities;

    // Vector to store the optimal tour
    std::vector<City> optimalTour;

    // Event loop
    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                // Add a city at mouse click position
                if (event.mouseButton.button == sf::Mouse::Left) {
                    City newCity;
                    newCity.x = event.mouseButton.x;
                    newCity.y = event.mouseButton.y;
                    cities.push_back(newCity);
                    optimalTour = simulateAnnealing(cities);
                }
            }
        }

        // Clear the window
        window.clear();

        // Draw the cities
        for (const auto& city : cities) {
            sf::CircleShape cityShape(5);
            cityShape.setFillColor(sf::Color::Blue);
            cityShape.setPosition(city.x, city.y);
            window.draw(cityShape);
        }

        // Draw the optimal tour
        sf::VertexArray lines(sf::LinesStrip, optimalTour.size() + 1);
        for (int i = 0; i < optimalTour.size(); ++i) {
            lines[i].position = sf::Vector2f(optimalTour[i].x, optimalTour[i].y);
            lines[i].color = sf::Color::Green;
        }
        if (!optimalTour.empty())
            lines[optimalTour.size()] = lines[0]; // Connect last city to first city
        window.draw(lines);

        // Display the window
        window.display();
    }

    return 0;
}
