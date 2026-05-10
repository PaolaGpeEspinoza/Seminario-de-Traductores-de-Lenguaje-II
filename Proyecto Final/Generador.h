#ifndef GENERADOR_H
#define GENERADOR_H

#include <fstream>
#include <string>
#include "arbolSintactico.h"

using namespace std;

class Generador {
private:
    ofstream archivo; // El stream para escribir el archivo

public:
    Generador(const char* nombreArchivo);
    ~Generador();

    void emit(string codigo);     // Escribe en la sección .text
    void emitData(string codigo); // Escribe en la sección .data

    void generar(Nodo* arbol);
};

#endif
