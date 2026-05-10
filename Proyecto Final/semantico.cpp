#include "semantico.h"

// --- Constructor (Como pide el PDF) ---
Semantico::Semantico(){
    // Inicializa la tabla de símbolos y la "conecta"
    // a la variable estática de la clase Nodo.
    Nodo::tablaSimbolos = tablaSimbolos = new TablaSimbolos(&listaErrores);
}

// --- Método 'analiza' (Como pide el PDF) ---
void Semantico::analiza(Nodo *arbol){
    this->arbol = arbol;

    // 1. Inicia el recorrido del árbol
    if (arbol) { // Solo si el árbol no está vacío
        arbol->validaTipos();
    }

    // 2. Muestra la tabla de símbolos
    cout << endl << "------ TABLA DE SIMBOLOS ------" << endl;
    tablaSimbolos->muestra();

    // 3. Muestra los errores (si los hay)
    cout << endl;
    muestraErrores();
}
