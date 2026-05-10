#ifndef SEMANTICO_H
#define SEMANTICO_H

#include <list>
#include <string>
#include "arbolSintactico.h"
#include "tablaSimbolos.h" // Se necesita para la Tabla

using namespace std;

class Semantico {
public:
    Nodo* arbol;
    list<string> listaErrores; // Lista para guardar errores

    // --- Atributo pedido por el PDF ---
    TablaSimbolos *tablaSimbolos;

    // --- Métodos pedidos por el PDF ---
    Semantico(); // Constructor

    void analiza(Nodo *arbol);

    // (Helper para mostrar errores)
    void muestraErrores() {
        if (listaErrores.empty()) {
            cout << "------ ANALISIS SEMANTICO: Sin errores ------" << endl;
        } else {
            cout << "------ ERRORES SEMANTICOS ------" << endl;
            for (list<string>::iterator it = listaErrores.begin(); it != listaErrores.end(); ++it) {
                cout << *it << endl;
            }
        }
    }
};

#endif
