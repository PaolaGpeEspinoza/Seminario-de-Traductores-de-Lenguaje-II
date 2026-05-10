#ifndef SINTACTICO_H
#define SINTACTICO_H

#include <vector>
#include <fstream>
#include <iostream>
#include <string>

#include "lexico.h"
#include "arbolSintactico.h"
#include "pila.h"

using namespace std;


class Sintactico {
public:
    Sintactico(); // Constructor
    ~Sintactico(); // <-- LÍNEA NUEVA 1: El destructor

    void analiza();
    void muestraAccion(int accion);
    void analizaGramatica0();

    Lexico lexico;
    Pila pila;
    Nodo* arbolSintactico;
    bool aceptacion;

private:
    struct InfoRegla {
        int idNoTerminal;
        int longitud;
        std::string nombre;
    };

    int numReglas;
    int filas;
    int columnas;
    std::vector<InfoRegla> infoReglas;
    std::vector<std::vector<int>> tablaLR;

    // --- LÍNEA NUEVA 2: La función para crear el árbol ---
    Nodo* crearNodoAST(int indiceRegla, int longitud);
};

#endif // SINTACTICO_H
