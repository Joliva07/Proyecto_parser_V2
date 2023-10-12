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

// No es necesario incluir el archivo generado por Bison
// #include "parser.tab.h" 

// No es necesario declarar funciones relacionadas con el analizador léxico y sintáctico
// extern int yylex();
// extern FILE *yyin;

#ifdef _WIN32
#include <cstdlib>
#endif

char* tokens_alfabetos[100];
int num_tokens_alfabetos = 0;
char* tokens_estados[100];
int num_tokens_estados = 0;
char* tokens_inicial[100];
int num_tokens_inicial = 0;
char* tokens_final[100];
int num_tokens_final = 0;
char tokens_transicional[100];
int num_tokens_transicional = 0;
char tokens_afn[100];
int num_tokens_afn = 0;
char* tokens_epsilon[100];
int num_tokens_epsilon = 0;

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

void processParserFile(const std::string& parserFilePath) {
    std::ifstream parserFile(parserFilePath);
    if (!parserFile.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo 'parser.y'." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(parserFile, line)) {
        if (line.find("tokens_alfabetos[num_tokens_alfabetos++] = ") != std::string::npos) {
            size_t start = line.find("\"") + 1;
            size_t end = line.find("\";", start);
            std::string token = line.substr(start, end - start);
            tokens_alfabetos[num_tokens_alfabetos++] = strdup(token.c_str());
        } else if (line.find("tokens_estados[num_tokens_estados++] = ") != std::string::npos) {
            size_t start = line.find("\"") + 1;
            size_t end = line.find("\";", start);
            std::string token = line.substr(start, end - start);
            tokens_estados[num_tokens_estados++] = strdup(token.c_str());
        } else if (line.find("tokens_inicial[num_tokens_inicial++] = ") != std::string::npos) {
            size_t start = line.find("\"") + 1;
            size_t end = line.find("\";", start);
            std::string token = line.substr(start, end - start);
            tokens_inicial[num_tokens_inicial++] = strdup(token.c_str());
        } else if (line.find("tokens_final[num_tokens_final++] = ") != std::string::npos) {
            size_t start = line.find("\"") + 1;
            size_t end = line.find("\";", start);
            std::string token = line.substr(start, end - start);
            tokens_final[num_tokens_final++] = strdup(token.c_str());
        } else if (line.find("tokens_transicional[num_tokens_transicional++] = ") != std::string::npos) {
            size_t start = line.find("\"") + 1;
            size_t end = line.find("\";", start);
            std::string token = line.substr(start, end - start);
            strcpy(tokens_transicional, token.c_str());
        } else if (line.find("tokens_afn[num_tokens_afn++] = ") != std::string::npos) {
            size_t start = line.find("\"") + 1;
            size_t end = line.find("\";", start);
            std::string token = line.substr(start, end - start);
            strcpy(tokens_afn, token.c_str());
        } else if (line.find("tokens_epsilon[num_tokens_epsilon++] = ") != std::string::npos) {
            size_t start = line.find("\"") + 1;
            size_t end = line.find("\";", start);
            std::string token = line.substr(start, end - start);
            tokens_epsilon[num_tokens_epsilon++] = strdup(token.c_str());
        }
        // Agrega más bloques 'else if' para procesar otros tipos de tokens si es necesario.
    }
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
        return;
    }

    // Agrega las etiquetas HTML iniciales al archivo de salida
    outputFile << "<!DOCTYPE html>\n";
    outputFile << "<html>\n";
    outputFile << "<head>\n";
    outputFile << "<title>Tabla de Transiciones</title>\n";
    outputFile << "</head>\n";
    outputFile << "<body>\n";

    std::string line;
    std::string alfabeto;
    std::vector<std::string> estados;
    std::string estadoInicial;
    std::vector<std::string> estadosFinales;
    std::vector<std::tuple<std::string, std::string, std::string>> transiciones;

    while (std::getline(tokensFile, line)) {
        size_t found = line.find("TOKEN FOUND: ");
        if (found != std::string::npos) {
            size_t start = found + strlen("TOKEN FOUND: ");
            size_t end = line.find_first_of(" |", start);
            std::string token = line.substr(start, end - start);

            if (token == "<ALFABETO>") {
                // Procesa el alfabeto
                while (std::getline(tokensFile, line)) {
                    if (line.find("</ALFABETO>") != std::string::npos) {
                        break;
                    }
                    size_t start = line.find("TOKEN FOUND: ");
                    if (start != std::string::npos) {
                        start += strlen("TOKEN FOUND: ");
                        alfabeto += line.substr(start) + ", ";
                    }
                }
            } else if (token == "<ESTADO>") {
                // Procesa los estados
                while (std::getline(tokensFile, line)) {
                    if (line.find("</ESTADO>") != std::string::npos) {
                        break;
                    }
                    size_t start = line.find("TOKEN FOUND: ");
                    if (start != std::string::npos) {
                        start += strlen("TOKEN FOUND: ");
                        estados.push_back(line.substr(start));
                    }
                }
            } else if (token == "<INICIAL>") {
                // Procesa el estado inicial
                while (std::getline(tokensFile, line)) {
                    if (line.find("</INICIAL>") != std::string::npos) {
                        break;
                    }
                    size_t start = line.find("TOKEN FOUND: ");
                    if (start != std::string::npos) {
                        start += strlen("TOKEN FOUND: ");
                        estadoInicial = line.substr(start);
                    }
                }
            } else if (token == "<FINAL>") {
                // Procesa los estados finales
                while (std::getline(tokensFile, line)) {
                    if (line.find("</FINAL>") != std::string::npos) {
                        break;
                    }
                    size_t start = line.find("TOKEN FOUND: ");
                    if (start != std::string::npos) {
                        start += strlen("TOKEN FOUND: ");
                        estadosFinales.push_back(line.substr(start));
                    }
                }
            } else if (token == "<TRANSICIONES>") {
                // Procesa las transiciones
                while (std::getline(tokensFile, line)) {
                    if (line.find("</TRANSICIONES>") != std::string::npos) {
                        break;
                    }
                    size_t start = line.find("TOKEN FOUND: ");
                    if (start != std::string::npos) {
                        start += strlen("TOKEN FOUND: ");
                        std::string estadoActual = line.substr(start);
                        std::getline(tokensFile, line);  // Lee la siguiente línea
                        start = line.find("TOKEN FOUND: ");
                        if (start != std::string::npos) {
                            start += strlen("TOKEN FOUND: ");
                            std::string simbolo = line.substr(start);
                            std::getline(tokensFile, line);  // Lee la siguiente línea
                            start = line.find("TOKEN FOUND: ");
                            if (start != std::string::npos) {
                                start += strlen("TOKEN FOUND: ");
                                std::string estadoSiguiente = line.substr(start);
                                transiciones.push_back(std::make_tuple(estadoActual, simbolo, estadoSiguiente));
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