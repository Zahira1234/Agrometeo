#include "weather.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <thread>
#include <chrono>

using namespace std;

int main() {
    string ville;
    string apiKey = "018d3b8332871db65c0e6f7b365b485a";

    afficherTitre("ASSISTANT AGRICOLE INTELLIGENT");

    while(true) {
        cout << "\nBienvenue dans votre assistant meteo agricole" << endl;
        afficherLigne('-', 70);

        cout << "\nEntrez le nom de la ville (ou 'exit' pour quitter) : ";
        cin >> ville;

        if(ville == "exit") break;

        cout << "\nRecherche des donnees meteorologiques";
        for(int i=0; i<3; i++){
            cout << ".";
            cout.flush();
            this_thread::sleep_for(chrono::milliseconds(500));
        cout << endl;

        WeatherData data = fetchWeatherData(ville, apiKey);

        if(!data.valide){
            cout << "\nERREUR : Ville introuvable ou probleme API" << endl;
            continue;
        }

        // Generer HTML
        genererHTML(data.nomVille, data.temp, data.ciel, data.ressentie, data.humidite,
                    data.vent, data.nuages, data.leverSoleil, data.coucherSoleil);

        cout << "\n[FICHIER HTML] index.html genere avec succes !" << endl;

        // Rapport console
        afficherTitre("RAPPORT METEO");
        cout << "\nLieu : " << data.nomVille << ", " << data.pays << endl;
        cout << "Date et heure : " << data.rechercheTime << endl;

        afficherSousSection("CONDITIONS ACTUELLES");
        cout << "Temperature : " << fixed << setprecision(1) << data.temp << " C" << endl;
        cout << "Humidite : " << data.humidite << " %" << endl;
        cout << "Vent : " << data.vent << " m/s" << endl;
        cout << "Nuages : " << data.nuages << " %" << endl;
        cout << "Description : " << data.ciel << endl;
        cout << "Lever du soleil : " << data.leverSoleil;
        cout << "Coucher du soleil : " << data.coucherSoleil;

        genererConseils(data.temp, data.humidite, data.nuages, data.vent);

        // Sauvegarder rapport
        ofstream rapport("rapport_meteo.txt", ios::app);
        rapport << "========================================\n";
        rapport << "RAPPORT METEO AGRICOLE\n";
        rapport << "Date : " << data.rechercheTime;
        rapport << "Lieu : " << data.nomVille << " (" << data.pays << ")\n";
        rapport << "Temperature : " << data.temp << " C\n";
        rapport << "Humidite : " << data.humidite << " %\n";
        rapport << "Vent : " << data.vent << " m/s\n";
        rapport << "Nuages : " << data.nuages << " %\n";
        rapport << "Description : " << data.ciel << "\n";
        rapport.close();

        cout << "\nRapport enregistre dans rapport_meteo.txt" << endl;

        system("start index.html"); // Windows

        cout << "\nVoulez-vous rechercher une autre ville ? (oui/non) : ";
        string reponse;
        cin >> reponse;
        if(reponse != "oui") break;

        system("cls");
    }

    cout << "\nMerci d'avoir utilise l'Assistant Agricole !" << endl;
    return 0;
}}
