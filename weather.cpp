#include "weather.h"
#include <iostream>
#include <fstream>
#include "include/curl/curl.h"
#pragma comment(lib, "ws2_32")
using json = nlohmann::json;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try { s->append((char*)contents, newLength); }
    catch (std::bad_alloc&) { return 0; }
    return newLength;
}

void afficherLigne(char caractere, int longueur) {
    for (int i = 0; i < longueur; i++) std::cout << caractere;
    std::cout << std::endl;
}

void afficherTitre(const std::string& titre) {
    afficherLigne('=');
    std::cout << "  " << titre << std::endl;
    afficherLigne('=');
}

void afficherSousSection(const std::string& titre) {
    std::cout << "\n";
    afficherLigne('-', 70);
    std::cout << " " << titre << std::endl;
    afficherLigne('-', 70);
}

std::string getBarreProgression(int pourcentage, int longueur) {
    int rempli = (pourcentage * longueur) / 100;
    std::string barre = "[";
    for (int i = 0; i < longueur; i++)
        barre += (i < rempli) ? "#" : "-";
    barre += "] " + std::to_string(pourcentage) + "%";
    return barre;
}

std::string getIconeTemp(float temp) {
    if (temp < 0) return "❄";/*emojipedia*/
    else if (temp < 10) return "🧥";
    else if (temp < 20) return "🌤";
    else if (temp < 30) return "☀";
    else return "🔥";
}

// Conseils simples
std::string genererConseils(float temp, int humidite, int nuages, float vent) {
    std::cout << "\n";
    afficherSousSection("CONSEILS AGRICOLES");

    if(temp < 5) std::cout << " > Risque de gel, proteger les cultures sensibles\n";
    else if(temp < 15) std::cout << " > Temps frais, ideal pour les legumes feuilles\n";
    else if(temp < 25) std::cout << " > Temperature optimale pour la plupart des cultures\n";
    else if(temp < 35) std::cout << " > Temperature elevee, irriguer regulierement\n";
    else std::cout << " > Chaleur extreme, reduire le travail en plein air\n";

    if(humidite < 30) std::cout << " > Air sec, arroser les cultures\n";
    else if(humidite > 70) std::cout << " > Humidite elevee, surveiller les maladies\n";

    if(nuages > 70) std::cout << " > Ciel tres couvre, verifier les besoins en soleil\n";

    if(vent > 10) std::cout << " > Vent fort, proteger les jeunes plantes\n";

    return "";
}

WeatherData fetchWeatherData(const std::string& ville, const std::string& apiKey) {
    WeatherData data;
    std::string url = "https://api.openweathermap.org/data/2.5/weather?q=" + ville +
                      "&appid=" + apiKey + "&units=metric&lang=fr";

    CURL* curl = curl_easy_init();
    std::string readBuffer;
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK) std::cerr << "\n ERREUR : " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
    }

    try {
        json jsonData = json::parse(readBuffer);
        if (jsonData.contains("main")) {
            data.nomVille = jsonData["name"];
            data.pays = jsonData["sys"]["country"];
            data.temp = jsonData["main"]["temp"];
            data.ressentie = jsonData["main"]["feels_like"];
            data.tempMin = jsonData["main"]["temp_min"];
            data.tempMax = jsonData["main"]["temp_max"];
            data.humidite = jsonData["main"]["humidity"];
            data.vent = jsonData["wind"]["speed"];
            data.nuages = jsonData["clouds"]["all"];
            data.ciel = jsonData["weather"][0]["description"];

            time_t lever = jsonData["sys"]["sunrise"];
            time_t coucher = jsonData["sys"]["sunset"];
            char leverBuf[26], coucherBuf[26];
            ctime_s(leverBuf, sizeof(leverBuf), &lever);
            ctime_s(coucherBuf, sizeof(coucherBuf), &coucher);
            data.leverSoleil = leverBuf;
            data.coucherSoleil = coucherBuf;

            time_t now = time(0);
            char nowBuf[26];
            ctime_s(nowBuf, sizeof(nowBuf), &now);
            data.rechercheTime = nowBuf;

            data.valide = true;
        }
    } catch (...) { std::cerr << "\n ERREUR : Reponse invalide du serveur\n"; }

    return data;
}

// Generer HTML avec conseils simples
void genererHTML(const std::string& ville, float temp, const std::string& ciel,
                 float ressentie, int humidite, int vent,
                 int nuages, const std::string& lever, const std::string& coucher) {

    std::ofstream file("index.html");
    file << "<!DOCTYPE html>\n<html lang='fr'>\n<head>\n<meta charset='UTF-8'>\n";
    file << "<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n";
    file << "<title>Meteo Agricole</title>\n";
    file << "<link rel='stylesheet' href='style.css'>\n</head>\n";

    std::string bgClass = "temp-mild";
    if(temp < 5) bgClass = "temp-cold";
    else if(temp < 15) bgClass = "temp-fresh";
    else if(temp < 25) bgClass = "temp-mild";
    else if(temp < 35) bgClass = "temp-warm";
    else bgClass = "temp-hot";

    file << "<body class='" << bgClass << "'>\n";
    file << "<div class='container'>\n<header>\n<h1>Assistant Agricole Intelligent</h1>\n";
    file << "<p>Données meteo pour vos cultures</p>\n</header>\n";

    file << "<section class='weather-section'>\n";
    file << "<div class='city-header'><h2>" << ville << "</h2>\n";
    file << "<div class='icon'>" << getIconeTemp(temp) << "</div>\n</div>\n";

    file << "<div class='grid'>\n";

    file << "<div class='card'><h3>🌡 Temperature</h3>\n";
    file << "<p><strong>" << temp << "°C</strong></p>\n";
    file << "<p>Ressentie : " << ressentie << "°C</p></div>\n";

    file << "<div class='card'><h3>💧 Humidite</h3>\n";
    file << "<p><strong>" << humidite << "%</strong></p>\n";
    file << "<h3>🌬 Vent</h3><p>" << vent << " m/s</p></div>\n";

    file << "<div class='card'><h3>⛅ Nuages</h3>\n";
    file << "<p><strong>" << nuages << "%</strong></p>\n";
    file << "<h3>📘 Description</h3><p>" << ciel << "</p></div>\n";

    file << "<div class='card'><h3>🌞 Soleil</h3>\n";
    file << "<p>Lever : " << lever << "</p>\n";
    file << "<p>Coucher : " << coucher << "</p></div>\n";

    file << "</div>\n"; // grid

    // Conseils simples en francais
    file << "<div class='card conseil'><h3>💡 Conseils Simples</h3>\n<ul>\n";
    if(temp < 5) file << "<li>Risque de gel, proteger les cultures sensibles</li>\n";
    else if(temp < 15) file << "<li>Temps frais, ideal pour les legumes feuilles</li>\n";
    else if(temp < 25) file << "<li>Temperature optimale pour la plupart des cultures</li>\n";
    else if(temp < 35) file << "<li>Temperature elevee, irriguer regulierement</li>\n";
    else file << "<li>Chaleur extreme, reduire le travail en plein air</li>\n";

    if(humidite < 30) file << "<li>Air sec, arroser les cultures</li>\n";
    else if(humidite > 70) file << "<li>Humidite elevee, surveiller les maladies</li>\n";

    if(nuages > 70) file << "<li>Ciel tres couvre, verifier les besoins en soleil</li>\n";
    if(vent > 10) file << "<li>Vent fort, proteger les jeunes plantes</li>\n";
    file << "</ul>\n</div>\n";

    file << "<div class='actions'>\n";
    file << "<button onclick='location.reload()'>🔄 Recharger</button>\n";
    file << "<button onclick='window.print()'>🖨 Imprimer</button>\n</div>\n";

    file << "</section>\n<footer>Genere automatiquement par votre Assistant Agricole</footer>\n";
    file << "</div>\n</body>\n</html>";
    file.close();
}
