#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <limits>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cstring>
#include <sstream>
#include <regex>

// No es necesario incluir el archivo generado por Bison
// #include "parser.tab.h" 

// No es necesario declarar funciones relacionadas con el analizador léxico y sintáctico
// extern int yylex();
// extern FILE *yyin;

#ifdef _WIN32
#include <cstdlib>
#endif

// No es necesario reiniciar el analizador léxico
// extern void yyrestart(FILE*);

bool isXML(const std::string& filename) {
    // Verifica si el archivo tiene extensión .xml
    if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".xml") {
        std::ifstream fileStream(filename);
        if (fileStream.is_open()) {
            bool containsOpeningTag = false;
            bool containsClosingTag = false;
            std::string line;
            while (std::getline(fileStream, line)) {
                if (line.find('<') != std::string::npos) {
                    containsOpeningTag = true;
                }
                if (line.find('>') != std::string::npos) {
                    containsClosingTag = true;
                }
            }
            return containsOpeningTag && containsClosingTag;
        }
    }
    return false;
}

std::string extractFileName(const std::string& filePath) {
    size_t lastSlash = filePath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        return filePath.substr(lastSlash + 1);
    }
    return filePath;
}

void createHTMLWithTransitionTable() {
    // Abre el archivo "vitacora_tokens.html" para lectura
    std::ifstream tokensFile("vitacora_tokens.html");

    if (!tokensFile.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo 'vitacora_tokens.html'." << std::endl;
        return;
    }

    // Abre un nuevo archivo HTML para escritura
    std::ofstream outputFile("output_with_table.html");

    if (!outputFile.is_open()) {
        std::cerr << "Error: No se pudo crear el archivo 'output_with_table.html'." << std::endl;
        tokensFile.close();  // Importante: Cerrar el archivo antes de salir
        return;
    }

    // Agrega las etiquetas HTML iniciales al archivo de salida
    outputFile << "<!DOCTYPE html>\n";
    outputFile << "<html>\n";
    outputFile << "<head>\n";
    outputFile << "<title>El dibujo</title>\n";
    outputFile << "</head>\n";
    outputFile << "<body>\n";

    std::string line;
    std::string alfabeto;
    std::vector<std::string> estados;
    std::string estadoInicial;
    std::vector<std::string> estadosFinales;
    std::vector<std::tuple<std::string, std::string, std::string>> transiciones;

    while (std::getline(tokensFile, line)) {
        // Encuentra la posición del primer '|'
        size_t found = line.find("|");
        if (found != std::string::npos) {
            // Obtiene la información después del '|'
            std::string lineData = line.substr(found + 1);

            // Comprueba que la cadena sea lo suficientemente larga antes de procesarla
            if (lineData.size() > 0) {
                // Procesa la información basada en el contenido después del '|'
                if (lineData.find("<ALFABETO>") != std::string::npos) {
                    while (std::getline(tokensFile, line)) {
                        if (line.find("</ALFABETO>") != std::string::npos) {
                            break;
                        }
                        size_t start = line.find("|");
                        if (start != std::string::npos) {
                            start++; // Avanza un carácter para omitir el '|'
                            alfabeto += line.substr(start) + ", ";
                        }
                    }
                } else if (lineData.find("<ESTADO>") != std::string::npos) {
                    while (std::getline(tokensFile, line)) {
                        if (line.find("</ESTADO>") != std::string::npos) {
                            break;
                        }
                        size_t start = line.find("|");
                        if (start != std::string::npos) {
                            start++; // Avanza un carácter para omitir el '|'
                            estados.push_back(line.substr(start));
                        }
                    }
                } else if (lineData.find("<INICIAL>") != std::string::npos) {
                    while (std::getline(tokensFile, line)) {
                        if (line.find("</INICIAL>") != std::string::npos) {
                            break;
                        }
                        size_t start = line.find("|");
                        if (start != std::string::npos) {
                            start++; // Avanza un carácter para omitir el '|'
                            estadoInicial = line.substr(start);
                        }
                    }
                } else if (lineData.find("<FINAL>") != std::string::npos) {
                    while (std::getline(tokensFile, line)) {
                        if (line.find("</FINAL>") != std::string::npos) {
                            break;
                        }
                        size_t start = line.find("|");
                        if (start != std::string::npos) {
                            start++; // Avanza un carácter para omitir el '|'
                            estadosFinales.push_back(line.substr(start));
                        }
                    }
                } else if (lineData.find("<TRANSICIONES>") != std::string::npos) {
                    while (std::getline(tokensFile, line)) {
                        if (line.find("</TRANSICIONES>") != std::string::npos) {
                            break;
                        }
                        size_t start = line.find("|");
                        if (start != std::string::npos) {
                            start++; // Avanza un carácter para omitir el '|'
                            std::string estadoActual = line.substr(start);
                            std::getline(tokensFile, line); // Lee la siguiente línea
                            size_t startSimbolo = line.find("|");
                            if (startSimbolo != std::string::npos) {
                                startSimbolo++; // Avanza un carácter para omitir el '|'
                                std::string simbolo = line.substr(startSimbolo);
                                std::getline(tokensFile, line); // Lee la siguiente línea
                                size_t startEstadoSiguiente = line.find("|");
                                if (startEstadoSiguiente != std::string::npos) {
                                    startEstadoSiguiente++; // Avanza un carácter para omitir el '|'
                                    std::string estadoSiguiente = line.substr(startEstadoSiguiente);
                                    transiciones.push_back(std::make_tuple(estadoActual, simbolo, estadoSiguiente));
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Genera la estructura del HTML con los datos procesados
    outputFile << "<h2>Alfabeto:</h2>\n";
    outputFile << "<ul>\n";
    std::istringstream alfabetoStream(alfabeto);
    std::string token;
    while (std::getline(alfabetoStream, token, ',')) {
        outputFile << "<li>" << token << "</li>\n";
    }
    outputFile << "</ul>\n";

    outputFile << "<h2>Estados:</h2>\n";
    outputFile << "<ul>\n";
    for (const std::string& estado : estados) {
        outputFile << "<li>" << estado << "</li>\n";
    }
    outputFile << "</ul>\n";

    outputFile << "<h2>Estado Inicial:</h2>\n";
    outputFile << "<p>" << estadoInicial << "</p>\n";

    outputFile << "<h2>Estados Finales:</h2>\n";
    outputFile << "<ul>\n";
    for (const std::string& estadoFinal : estadosFinales) {
        outputFile << "<li>" << estadoFinal << "</li>\n";
    }
    outputFile << "</ul>\n";

    outputFile << "<h2>Tabla de Transiciones:</h2>\n";
    outputFile << "<table border=\"1\">\n";
    outputFile << "<tr><th>Estado Actual</th><th>Simbolo</th><th>Estado Siguiente</th></tr>\n";
    for (const auto& transicion : transiciones) {
        outputFile << "<tr><td>" << std::get<0>(transicion) << "</td><td>" << std::get<1>(transicion) << "</td><td>" << std::get<2>(transicion) << "</td></tr>\n";
    }
    outputFile << "</table>\n";

    // Agrega las etiquetas HTML finales al archivo de salida
    outputFile << "</body>\n";
    outputFile << "</html>\n";

    // Cierra ambos archivos
    tokensFile.close();
    outputFile.close();

    std::cout << "Se ha creado el archivo 'output_with_table.html' con la tabla de transiciones." << std::endl;
}




void createAFN() {
    // Aquí puedes agregar la lógica para crear un AFN (Automaton Finite State Network)
    // Por ahora, esta función está vacía
    std::cout << "Opcion 'Crear AFN' seleccionada. Funcionalidad en desarrollo." << std::endl;
}

void showAFD() {
    // Función para mostrar el AFD (Automaton Finite Deterministic) - Implementación pendiente
    std::cout << "Opcion 'Mostrar AFD' seleccionada. Funcionalidad en desarrollo." << std::endl;
}

void convertAFNtoAFD() {
    // Función para convertir de AFN a AFD - Implementación pendiente
    std::cout << "Opcion 'Pasar de AFN a AFD' seleccionada. Funcionalidad en desarrollo." << std::endl;
}

int main() {
    std::cout << "Bienvenido al programa de manejo de archivos y operaciones AFN." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Espera de 2 segundos

    std::string currentFile;
    std::string currentFilePath;

    while (true) {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif

        std::cout << "\nMenu:\n";
        std::cout << "1. Seleccionar Archivo XML y Parsearlo\n";
        std::cout << "2. Crear AFN\n";
        std::cout << "3. Mostrar AFD (en desarrollo)\n";
        std::cout << "4. Pasar de AFN a AFD (en desarrollo)\n";
        std::cout << "5. Salir\n";
        std::cout << "Seleccione una opcion: ";

        int choice;
        std::cin >> choice;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            /*case 1: {
                std::string filePath;
                std::cout << "Introduce la ruta del archivo: ";
                std::getline(std::cin, filePath);

                if (!isXML(filePath)) {
                    std::cerr << "Error: El archivo no es valido, por favor vuelva a intentarlo." << std::endl;
                } else {
                    currentFile = extractFileName(filePath);
                    currentFilePath = filePath;
                    std::cout << "Archivo XML seleccionado: " << currentFile << std::endl;
                }
                break;
            }*/
            case 1: {
    // Ejecutar el archivo batch que construye el parser
    int result = system("build_parser.bat");

    if (result == 0) {
        std::cout << "El parser se construyo y ejecuto con exito." << std::endl;
    } else {
        std::cerr << "Error al construir o ejecutar el parser." << std::endl;
    }
    break;
}
            case 2: {
                // Llama a tu función para crear el AFN aquí
                createHTMLWithTransitionTable();
                break;
            }
            case 3: {
    // Abre el archivo "output_with_table.html" en el navegador predeterminado
    int result = system("start output_with_table.html");

    if (result != 0) {
        std::cerr << "Error al abrir el archivo en el navegador." << std::endl;
    }
    break;
}
            case 4: {
                // Llama a la función para convertir de AFN a AFD (en desarrollo)
                convertAFNtoAFD();
                break;
            }
            case 5: {
                std::cout << "Saliendo del programa. Hasta luego!" << std::endl;
                return 0;
            }
            default: {
                std::cerr << "Opcion invalida. Por favor, selecciona una opcion valida." << std::endl;
                break;
            }
        }

        std::cout << "Presiona Enter para continuar...";
        std::cin.get(); // Espera hasta que el usuario presione Enter
    }

    return 0;
}