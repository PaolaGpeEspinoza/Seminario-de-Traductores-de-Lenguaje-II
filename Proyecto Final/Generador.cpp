#include "generador.h"

Generador::Generador(const char* nombreArchivo) {
    archivo.open(nombreArchivo);
}

Generador::~Generador() {
    if (archivo.is_open()) {
        archivo.close();
    }
}

void Generador::emit(string codigo) {
    archivo << "    " << codigo << endl;
}

void Generador::emitData(string codigo) {
    archivo << "    "<<codigo << endl;
}

void Generador::generar(Nodo* arbol) {
    // 1. Encabezado para archivos .COM
    archivo << "org 100h" << endl;

    // 2. Sección de Datos
    archivo << ".data" << endl;
    if (arbol) {
        arbol->generaDatos(this);
    }

    // 3. Sección de Código
    archivo << ".code" << endl;
    archivo << "start:" << endl;

    // NOTA: En formato .COM NO inicializamos DS (@DATA).
    // El código va directo.

    if (arbol) {
        arbol->generaCodigo(this);
    }

    // Fin del programa
    archivo << "    MOV AX, 4C00h" << endl; // Salir al sistema
    archivo << "    INT 21h" << endl;
    archivo << "end start" << endl;

    cout << endl << "------ GENERACION DE CODIGO ------" << endl;
    cout << "¡Archivo 'programa.asm' generado con exito!" << endl;
}
