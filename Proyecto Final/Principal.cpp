#include <cstdlib>
#include <iostream>
#include "pila.h"
#include "lexico.h"
#include "sintactico.h"
#include "semantico.h"
#include "generador.h"
#include <fstream>
#include <sstream>

using namespace std;

// Función auxiliar para leer del archivo
string leerArchivo(string nombreArchivo) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "Error critico: No se pudo abrir el archivo '" << nombreArchivo << "'" << endl;
        exit(1);
    }
    stringstream buffer;
    buffer << archivo.rdbuf();
    return buffer.str();
}

TablaSimbolos * Nodo::tablaSimbolos= NULL;
string Nodo::ambito= "";

int main(int argc, char *argv[]){

    // 1. LEEMOS EL ARCHIVO DE TEXTO (NUEVO)
    // Asegúrate de crear "fuente.txt" en la misma carpeta del ejecutable
    string codigoFuente = leerArchivo("prueba.txt");

    cout << "--- ANALIZANDO ARCHIVO: prueba.txt ---" << endl;
    cout << codigoFuente << endl;
    cout << "--------------------------------------" << endl;

    Sintactico sintactico;

    // 2. PASAMOS EL CONTENIDO DEL ARCHIVO AL LEXICO (CORREGIDO)
    // Antes tenías: sintactico.lexico.entrada("int a; ...");
    // Ahora usamos la variable:
    sintactico.lexico.entrada(codigoFuente);

    sintactico.analiza();

    if (sintactico.aceptacion) {
        Semantico semantico;
        semantico.analiza(sintactico.arbolSintactico);

        if (semantico.listaErrores.empty()) {
            Generador generador("programa.asm");
            generador.generar(sintactico.arbolSintactico);
        }
    }

    system("PAUSE");
    return EXIT_SUCCESS;
}
