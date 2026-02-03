#include <iostream>
#include <string>
#include "Scale.hpp"

int main() {
    Scale scale;
    
    // Paramètres à adapter selon votre balance
    std::string port = "COM3";      // Vérifiez dans le gestionnaire de périphériques
    int baudrate = 9600;
    int dataBits = 8;
    int stopBits = 1;               // 1 pour ONESTOPBIT
    int parity = 2;                 // 0 pour NOPARITY  // 1 odd // 2 even 
    std::string request = "S";      // Commande standard pour demander le poids (souvent 'W' ou 'P')
    int timeout = 2000;             // 2 secondes
    
    char buffer[256] = {0};

    std::cout << "--- Test de la Balance ---" << std::endl;
    std::cout << "Ouverture du port : " << port << " (" << baudrate << " baud)" << std::endl;
    std::cout << "Envoi de la commande : '" << request << "'" << std::endl;

    int result = scale.readWeight(port, baudrate, dataBits, stopBits, parity, request, buffer, 256, timeout);

    if (result > 0) {
        std::cout << "Succes !" << std::endl;
        std::cout << "Octets recus : " << result << std::endl;
        std::cout << "Donnees brutes : [" << buffer << "]" << std::endl;
    } else {
        std::cerr << "Erreur de lecture (Code : " << result << ")" << std::endl;
        if (result == -2) {
            std::cerr << "Impossible d'ouvrir le port. Est-il deja utilise ?" << std::endl;
        } else {
            std::cerr << "Pas de reponse de la balance (Timeout)." << std::endl;
        }
    }

    std::cout << "\nAppuyez sur Entree pour quitter..." << std::endl;
    std::cin.get();
    return 0;
}