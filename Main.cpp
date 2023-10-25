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
#include <tuple>
#include <cstdlib>
#include <unordered_map>
#include <unordered_set>
#include <set>

#ifdef _WIN32
#include <cstdlib>
#endif

std::vector<std::string> alfabeto;
std::vector<std::string> estados;
std::string estadoInicial;
std::vector<std::string> estadosFinales;
std::map<std::string, std::map<std::string, std::string>> transiciones;

bool isXML(const std::string& filename) {
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

void AFNGraph(const std::vector<std::tuple<std::string, std::string, std::string>>& transiciones,
                  const std::string& estadoInicial,
                  const std::vector<std::string>& estadosFinales) {
    std::ofstream dotFile("AFN.dot");
    if (!dotFile.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo 'AFN.dot'." << std::endl;
        return;
    }

    dotFile << "digraph G {\n";
    dotFile << "  rankdir=LR;\n";

    std::set<std::string> estadosFinalesSet(estadosFinales.begin(), estadosFinales.end());

    std::unordered_map<std::string, std::string> redirigirTransiciones;

    for (const auto& transicion : transiciones) {
        std::string estadoActual = std::get<0>(transicion);
        std::string simbolo = std::get<1>(transicion);
        std::string estadoSiguiente = std::get<2>(transicion);

        dotFile << "  \"" << estadoActual << "\" [shape=ellipse];\n";
        dotFile << "  \"" << estadoSiguiente << "\" [shape=ellipse];\n";

        if (estadosFinalesSet.count(estadoActual) != 0 && estadosFinalesSet.count(estadoSiguiente) != 0) {
            std::string estadoRedireccionado = estadoActual;
            while (estadosFinalesSet.count(estadoRedireccionado) != 0 || estadoRedireccionado == estadoInicial) {
                for (const auto& t : transiciones) {
                    if (std::get<2>(t) == estadoRedireccionado) {
                        estadoRedireccionado = std::get<0>(t);
                        break;
                    }
                }
            }

            redirigirTransiciones[estadoActual] = estadoRedireccionado;
        }

        if (redirigirTransiciones.count(estadoActual) != 0) {
            dotFile << "  \"" << redirigirTransiciones[estadoActual] << "\" -> \"" << estadoSiguiente << "\" [label=\"" << simbolo << "\"];\n";
        } else {
            dotFile << "  \"" << estadoActual << "\" -> \"" << estadoSiguiente << "\" [label=\"" << simbolo << "\"];\n";
        }
    }

    dotFile << "  start [shape=none, label=\"\", width=0, height=0];\n";
    dotFile << "  start -> \"" << estadoInicial << "\" [label=\"Inicio\", dir=none];\n";

    for (const std::string& estadoFinal : estadosFinales) {
        if (redirigirTransiciones.count(estadoFinal) != 0) {
            dotFile << "  \"" << estadoFinal << "\" [shape=doublecircle];\n";
        } else {
            dotFile << "  \"" << estadoFinal << "\" [shape=doublecircle];\n";
        }
    }

    dotFile << "}\n";
    dotFile.close();

    int result = std::system("dot -Tpng AFN.dot -o AFN.png");

    if (result != 0) {
        std::cerr << "Error al generar el grafico." << std::endl;
    } else {
        std::cout << "Se ha generado el grafico 'AFN.png'." << std::endl;
    }
}

void AFN() {
    std::ifstream tokensFile("vitacora_tokens.html");

    if (!tokensFile.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo 'vitacora_tokens.html'." << std::endl;
        return;
    }

    std::ofstream outputFile("dibujo.html");

    if (!outputFile.is_open()) {
        std::cerr << "Error: No se pudo crear el archivo 'dibujo.html'." << std::endl;
        tokensFile.close();
        return;
    }

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
        size_t found = line.find("|");
        if (found != std::string::npos) {
            std::string lineData = line.substr(found + 1);

            if (lineData.size() > 0) {
                if (lineData.find("<ALFABETO>") != std::string::npos) {
                    while (std::getline(tokensFile, line)) {
                        if (line.find("</ALFABETO>") != std::string::npos) {
                            break;
                        }
                        size_t start = line.find("|");
                        if (start != std::string::npos) {
                            start++; 
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
                            start++; 
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
                            start++; 
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
                            start++; 
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
                            start++; 
                            std::string estadoActual = line.substr(start);
                            std::getline(tokensFile, line); 
                            size_t startSimbolo = line.find("|");
                            if (startSimbolo != std::string::npos) {
                                startSimbolo++; 
                                std::string simbolo = line.substr(startSimbolo);
                                std::getline(tokensFile, line); 
                                size_t startEstadoSiguiente = line.find("|");
                                if (startEstadoSiguiente != std::string::npos) {
                                    startEstadoSiguiente++; 
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

    AFNGraph(transiciones, estadoInicial, estadosFinales);

    outputFile << "<img src='AFN.png' />";
    outputFile << "</body>\n";
    outputFile << "</html>\n";

    tokensFile.close();
    outputFile.close();

    std::cout << "Se ha creado el archivo 'dibujo.html' con la tabla de transiciones." << std::endl;
}

void AFD() {
    std::ifstream entrada("dibujo.html");
    if (!entrada.is_open()) {
        std::cerr << "No se pudo abrir el archivo de entrada." << std::endl;
        return;
    }

    std::ofstream salida("conversion.html");
    if (!salida.is_open()) {
        std::cerr << "No se pudo abrir el archivo de salida." << std::endl;
        entrada.close();
        return;
    }

    std::string linea;

    bool enTablaDeTransiciones = false;
    bool enEstadosFinales = false;

    while (std::getline(entrada, linea)) {
        if (linea.find("Tabla de Transiciones:") != std::string::npos) {
            enTablaDeTransiciones = true;
            salida << linea << "\n";
        } else if (enTablaDeTransiciones) {
            if (linea.find("</table>") != std::string::npos) {
                enTablaDeTransiciones = false;
                salida << linea << "\n";
            } else if (linea.find("<tr><td>") != std::string::npos) {
                std::smatch match;
                if (std::regex_search(linea, match, std::regex("<td> (\\d+)</td><td> ([^<]+)</td><td> (\\d+)</td>"))) {
                    std::string estadoActual = "q" + match[1].str();
                    std::string simbolo = match[2].str();
                    std::string estadoSiguiente = "q" + match[3].str();

                    if (simbolo != "&") {
                        salida << "<tr><td>" << estadoActual << "</td><td>" << simbolo << "</td><td>" << estadoSiguiente << "</td></tr>" << "\n";
                    }
                }
            } else {
                salida << linea << "\n";
            }
        } else if (linea.find("<h2>Alfabeto:") != std::string::npos) {
            enTablaDeTransiciones = false;
            salida << linea << "\n";
            while (std::getline(entrada, linea)) {
                if (linea.find("</ul>") != std::string::npos) {
                    break;
                } else if (linea.find("<li>") != std::string::npos) {
                    std::smatch match;
                    if (std::regex_search(linea, match, std::regex("<li> ([^<]+)</li>"))) {
                        std::string simbolo = match[1].str();
                        salida << "<li>" << simbolo << "</li>" << "\n";
                    }
                }
            }
        } else if (linea.find("<h2>Estado Inicial:") != std::string::npos) {
            enTablaDeTransiciones = false;
            std::getline(entrada, linea); 
            std::smatch match;
            if (std::regex_search(linea, match, std::regex("<p> (\\d+)</p>")) || std::regex_search(linea, match, std::regex("<p>q(\\d+)</p>"))) {
                std::string estadoInicial = "q" + match[1].str();
                salida << "<h2>Estado Inicial:</h2>" << "\n";
                salida << "<p>" << estadoInicial << "</p>" << "\n";
            }
        } else if (linea.find("<h2>Estados:") != std::string::npos ||
                   linea.find("<h2>Estados Finales:") != std::string::npos) {
            enEstadosFinales = true;
            salida << linea << "\n";
        } else if (enEstadosFinales) {
            if (linea.find("</ul>") != std::string::npos) {
                enEstadosFinales = false;
                salida << linea << "\n";
            } else if (linea.find("<li>") != std::string::npos) {
                std::smatch match;
                if (std::regex_search(linea, match, std::regex("<li> (\\d+)</li>")) || std::regex_search(linea, match, std::regex("<li>q(\\d+)</li>"))) {
                    std::string estado = "q" + match[1].str();
                    salida << "<li>" << estado << "</li>" << "\n";
                }
            } else {
                salida << linea << "\n";
            }
        } else if (linea.find("<img src='AFN.png' />") != std::string::npos) {
        } else {
            salida << linea << "\n";
        }
    }

    salida << "<img src='AFD.png' />" << "\n";

    entrada.close();
    salida.close();

    std::cout << "Se ha procesado la conversion de AFN a AFD excitosamente." << std::endl;
}

void AFDGraph() {
    std::ifstream inputFile("conversion.html");

    if (!inputFile.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo 'conversion.html'." << std::endl;
        return;
    }

    std::ofstream dotFile("AFD.dot");
    if (!dotFile.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo 'AFD.dot'." << std::endl;
        inputFile.close();
        return;
    }

    dotFile << "digraph G {\n";
    dotFile << "  rankdir=LR;\n";

    std::string initialState;
    std::vector<std::string> finalStates;

    std::string line;

    bool inTransitionTable = false;

    while (std::getline(inputFile, line)) {
        if (line.find("<h2>Estado Inicial:") != std::string::npos) {
            std::getline(inputFile, line); 
            if (line.find("<p>") != std::string::npos) {
                initialState = line.substr(line.find("<p>") + 3, 2);
            }
        } else if (line.find("<h2>Estados Finales:") != std::string::npos) {
            std::getline(inputFile, line); 
            while (line.find("<li>") != std::string::npos) {
                finalStates.push_back(line.substr(line.find("<li>") + 4, 2));
                std::getline(inputFile, line); 
            }
        } else if (line.find("<h2>Tabla de Transiciones:") != std::string::npos) {
            inTransitionTable = true;
            std::getline(inputFile, line); 
            std::getline(inputFile, line); 
        } else if (inTransitionTable && std::regex_search(line, std::regex("<tr><td>(q\\d+)</td><td>([a-zA-Z])</td><td>(q\\d+)</td>"))) {
            std::smatch match;
            if (std::regex_search(line, match, std::regex("<td>(q\\d+)</td><td>([a-zA-Z])</td><td>(q\\d+)</td>"))) {
                std::string currentState = match[1].str();
                std::string symbol = match[2].str();
                std::string nextState = match[3].str();
                dotFile << "  \"" << currentState << "\" -> \"" << nextState << "\" [label=\"" << symbol << "\"];\n";
            }
        }
    }

    dotFile << "  start [shape=none, label=\"\", width=0, height=0];\n";
    dotFile << "  start -> \"" << initialState << "\" [label=\"Inicio\", dir=none];\n";
    for (const std::string& finalState : finalStates) {
        dotFile << "  \"" << finalState << "\" [shape=doublecircle];\n";
    }

    dotFile << "}\n";
    dotFile.close();
    inputFile.close();

    int result = std::system("dot -Tpng AFD.dot -o AFD.png");

    if (result != 0) {
        std::cerr << "Error al generar el grafico." << std::endl;
    } else {
        std::cout << "Se ha generado el grafico 'AFD.png'." << std::endl;
    }
}

int main() {
    std::cout << "Bienvenido al programa de manejo de archivos y operaciones AFN." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

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
        std::cout << "3. Mostrar AFN\n";
        std::cout << "4. Pasar de AFN a AFD\n";
        std::cout << "5. Mostrar AFD\n";
        std::cout << "6. Salir\n";
        std::cout << "Seleccione una opcion: ";

        int choice;
        std::cin >> choice;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
    int result = system("build_parser.bat");

    if (result == 0) {
        std::cout << "El parser se construyo y ejecuto con exito." << std::endl;
    } else {
        std::cerr << "Error al construir o ejecutar el parser." << std::endl;
    }
    break;
    }
            case 2: {
                AFN();
                break;
            }
            case 3: {
    int result = system("start dibujo.html");

    if (result != 0) {
        std::cerr << "Error al abrir el archivo en el navegador." << std::endl;
    }
    break;
    }
            case 4: {
                AFD();
                AFDGraph();
                break;
            }
            case 5: {
    int result = system("start conversion.html");

    if (result != 0) {
        std::cerr << "Error al abrir el archivo en el navegador." << std::endl;
    }
    break;
            }
            case 6: {
                std::cout << "Saliendo del programa. Hasta luego!" << std::endl;
                return 0;
            }
            default: {
                std::cerr << "Opcion invalida. Por favor, selecciona una opcion valida." << std::endl;
                break;
            }
        }

        std::cout << "Presiona Enter para continuar...";
        std::cin.get();
    }

    return 0;
}