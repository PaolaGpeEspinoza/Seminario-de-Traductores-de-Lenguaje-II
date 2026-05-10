#include "lexico.h"

// --- Constructor (Carga el .inf) ---
Lexico::Lexico() {
    std::ifstream archivo("Gramatica/compilador.inf");
    if (!archivo.is_open()) {
        cout << "ERROR: No se pudo abrir 'Gramatica/compilador.inf'" << endl; system("PAUSE"); return;
    }
    std::string palabra;
    int id;
    while (archivo >> palabra >> id) {
        tablaSimbolos[palabra] = id;
    }
    archivo.close();
    cout << "Constructor de Lexico: ¡TABLA DE SIMBOLOS CARGADA!" << endl;
    cout << "Se cargaron " << tablaSimbolos.size() << " simbolos." << endl;
    cout << "****************************************" << endl;
    ind = 0;
    continua = false;
    buffer = ""; // Inicializa el buffer
}

// --- Constructor alternativo ---
Lexico::Lexico(string fuente) : Lexico() {
    entrada(fuente);
}

// --- Método para poner el string de entrada ---
void Lexico::entrada(string fuente) {
    this->fuente = fuente + "$";
    this->ind = 0;
    this->continua = true;
}

// --- ¡FUNCIÓN SIGSIMBOLO() COMPLETA Y CORREGIDA! ---
int Lexico::sigSimbolo() {
    buffer = ""; // Limpia el buffer

    if (!continua) {
        buffer = "$";
        return tablaSimbolos["$"];
    }

    // Ignorar espacios en blanco
    while (isspace(fuente[ind])) {
        ind++;
    }

    // 1. Revisa si es una letra (inicio de palabra)
    if (isalpha(fuente[ind])) {
        while (isalnum(fuente[ind])) {
            buffer += fuente[ind];
            ind++;
        }
    }
    // 2. Revisa si es un número
    else if (isdigit(fuente[ind])) {
        while (isdigit(fuente[ind])) {
            buffer += fuente[ind];
            ind++;
        }
    }
    // 3. Revisa SÍMBOLOS ESPECIALES (¡Aquí agregamos los que faltaban!)
    else if (fuente[ind] == '$') {
        buffer = "$"; continua = false; ind++;
    }
    else if (fuente[ind] == ';') { buffer = ";"; ind++; }
    else if (fuente[ind] == '=') { buffer = "="; ind++; }
    else if (fuente[ind] == '(') { buffer = "("; ind++; } // <--- NUEVO
    else if (fuente[ind] == ')') { buffer = ")"; ind++; } // <--- NUEVO
    else if (fuente[ind] == '{') { buffer = "{"; ind++; } // <--- NUEVO
    else if (fuente[ind] == '}') { buffer = "}"; ind++; } // <--- NUEVO
    else if (fuente[ind] == ',') { buffer = ","; ind++; }
    else if (fuente[ind] == '+') { buffer = "+"; ind++; } // <--- Por si acaso
    else if (fuente[ind] == '*') { buffer = "*"; ind++; } // <--- Por si acaso

    // 4. Si no es nada de eso, es un error
    else {
        cout << "LEXICO: Error, simbolo no reconocido: " << fuente[ind] << endl;
        continua = false; return -1;
    }

    // --- BÚSQUEDA EN EL MAPA ---

    // 5. ¿La palabra es un "tipo" especial?
    if (buffer == "int" || buffer == "float" || buffer == "char") {
        cout << "LEXICO: Envio '" << buffer << "' como 'tipo' (4)" << endl;
        tipo = tablaSimbolos["tipo"];
        simbolo = buffer[0];
        return tipo;
    }

    // 6. ¿Es otra palabra clave o símbolo que está en el mapa?
    if (tablaSimbolos.count(buffer)) {
        cout << "LEXICO: Envio '" << buffer << "' (" << tablaSimbolos[buffer] << ")" << endl;
        tipo = tablaSimbolos[buffer];
        simbolo = buffer[0];
        return tipo;
    }

    // 7. Si es número
    if (isdigit(buffer[0])) {
        cout << "LEXICO: Envio '" << buffer << "' como 'entero' (1)" << endl;
        tipo = tablaSimbolos["entero"];
        simbolo = buffer[0];
        return tipo;
    }

    // 8. Si no, es un "identificador"
    cout << "LEXICO: Envio '" << buffer << "' como 'identificador' (0)" << endl;
    tipo = tablaSimbolos["identificador"];
    simbolo = buffer[0];
    return tipo;
}

// --- NUEVA FUNCIÓN ---
string Lexico::getBuffer(){
    return buffer;
}

// --- Método terminado() ---
bool Lexico::terminado() {
    return !continua;
}
