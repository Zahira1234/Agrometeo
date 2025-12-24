#ifndef WEATHER_H
#define WEATHER_H

#include <string>
#include <ctime>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <thread>
#include <chrono>
#include "json.hpp"

// Structure pour les donnees meteo
struct WeatherData {
    std::string nomVille;
    std::string pays;
    float temp;
    float ressentie;
    float tempMin;
    float tempMax;
    int humidite;
    float vent;
    int nuages;
    std::string ciel;
    std::string leverSoleil;
    std::string coucherSoleil;
    std::string rechercheTime;
    bool valide = false;
};

// Fonctions utilitaires
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s);
void afficherLigne(char caractere = '=', int longueur = 70);
void afficherTitre(const std::string& titre);
void afficherSousSection(const std::string& titre);
std::string getBarreProgression(int pourcentage, int longueur = 20);
std::string getIconeTemp(float temp);
std::string genererConseils(float temp, int humidite, int nuages, float vent);
WeatherData fetchWeatherData(const std::string& ville, const std::string& apiKey);

// Generer HTML moderne avec conseils
void genererHTML(const std::string& ville, float temp, const std::string& ciel,
                 float ressentie=0, int humidite=0, int vent=0,
                 int nuages=0, const std::string& lever="--:--", const std::string& coucher="--:--");

#endif // WEATHER_H
