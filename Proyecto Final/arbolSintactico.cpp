#include "arbolSintactico.h"
#include "generador.h"

int Nodo::sangria = 0;

// --- RECETAS DE IDENTIFICADOR ---
void Identificador::generaCodigo(Generador* gen) {
    gen->emit("MOV AX, [" + simbolo + "]");
}

// --- RECETAS DE DEFVAR ---

void DefVar::generaCodigo(Generador* gen) {
    if (sig) sig->generaCodigo(gen);
}

void DefVar::generaDatos(Generador* gen) {
    Identificador *p = listaVar;
    while (p != NULL) {
        gen->emitData(p->simbolo + " DW 0");
        p = (Identificador*)p->sig;
    }
    if (sig) sig->generaDatos(gen);
}

// --- RECETAS DE ENTERO ---
void Entero::generaCodigo(Generador* gen) {
    gen->emit("MOV AX, " + simbolo);
}


// --- Asignacion (Escribir valor) ---
void Asignacion::generaCodigo(Generador* gen) {
    expresion->generaCodigo(gen);
    // Usamos corchetes para asegurar que escribimos en MEMORIA
    gen->emit("MOV [" + id->simbolo + "], AX");

    if (sig) sig->generaCodigo(gen);
}

// --- RECETA PARA SUMA ---
void Suma::generaCodigo(Generador* gen) {
    // 1. Genera código del hijo izquierdo (resultado en AX)
    izq->generaCodigo(gen);
    // 2. Guarda AX en la pila
    gen->emit("PUSH AX");

    // 3. Genera código del hijo derecho (resultado en AX)
    der->generaCodigo(gen);
    // 4. Mueve el resultado derecho a BX
    gen->emit("MOV BX, AX");

    // 5. Recupera el resultado izquierdo de la pila a AX
    gen->emit("POP AX");

    // 6. Suma: AX = AX + BX
    gen->emit("ADD AX, BX");
}

// --- RECETA PARA MULT ---
void Mult::generaCodigo(Generador* gen) {
    izq->generaCodigo(gen);
    gen->emit("PUSH AX");
    der->generaCodigo(gen);
    gen->emit("MOV BX, AX");
    gen->emit("POP AX");

    // Multiplicación (MUL usa AX y BX, resultado en AX)
    gen->emit("MUL BX");
}

// --- RECETA PARA FUNCIONES ---
void DefFunc::generaCodigo(Generador* gen) {
    // 1. Si es main, ponemos la etiqueta especial, si no, usamos el nombre
    if (id->simbolo == "main") {
        // En nuestro formato .COM, el start: ya está en el generador.
        // Solo necesitamos que el código fluya aquí.
        // Podemos poner un comentario o nada.
        gen->emit("; INICIO DE MAIN");
    } else {
        gen->emit(id->simbolo + ":"); // Etiqueta para otras funciones
    }

    // 2. Generar el código del cuerpo de la función
    if (bloqueFunc) bloqueFunc->generaCodigo(gen);

    // 3. Si hay más definiciones después, seguir
    if (sig) sig->generaCodigo(gen);
}
