#include <iostream>
#include "ZebraPrinter.hpp"

int main() {
    ZebraPrinter printer;
    std::string testLabel = "^XA^FO50,50^A0N,50,50^FDTest Autonome C++^FS^XZ";

    std::cout << "Tentative d'impression directe..." << std::endl;
    
    if (printer.sendZPL(testLabel)) {
        std::cout << "Succes ! L'etiquette doit sortir." << std::endl;
    } else {
        std::cerr << "Erreur : Imprimante introuvable ou echec d'ecriture." << std::endl;
        std::cerr << "Verifiez le branchement USB." << std::endl;
    }

    std::cout << "Appuyez sur Entree pour quitter..." << std::endl;
    std::cin.get();
    return 0;
}