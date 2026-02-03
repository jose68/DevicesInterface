# DevicesInterface - Zebra Printer Native Bridge

Cette bibliothèque C++ permet de communiquer directement

- avec une imprimante Zebra via USB en utilisant l'API Windows `SetupAPI`. Elle inclut un pont **JNI** pour une utilisation fluide avec Java.
- avec une scale via le port COM.
  Elle inclut un pont **JNI** pour une utilisation fluide avec Java.

## 🚀 Caractéristiques

- **Performance** : Utilise un système de cache pour éviter de rescanner le bus USB inutilement.
- **Auto-guérison** : Rescane automatiquement le matériel si l'imprimante est débranchée/rebranchée.
- **Sécurité** : Intègre des timeouts (500ms) pour éviter de geler l'application appelante.
- **Zéro Driver** : Pas besoin de spooler Windows, communique directement avec l'ID matériel USB. (la méthode avec spooler est toujours dedans)

## 📁 Structure du Projet

- `src/` : Fichiers sources C++ et bridge JNI.
- `include/` : En-têtes (.hpp).
- `tests/` : Utilitaire de test autonome (C++).
- `build/` : (Ignoré par Git) Contient les fichiers de compilation.

## 🛠 Prérequis

- **Compilateur** : MinGW-w64 (via MSYS2) ou tout compilateur supportant C++17.
- **CMake** : Version 3.10 ou supérieure.
- **JDK** : Défini dans la variable d'environnement `JAVA_HOME`.

## ⚙️ Compilation

### Via VS Code (Recommandé)

1. Ouvrez le dossier dans VS Code.
2. Installez l'extension **CMake Tools**.
3. Sélectionnez votre "Kit" (MinGW-w64).
4. Appuyez sur **F7** pour compiler.
