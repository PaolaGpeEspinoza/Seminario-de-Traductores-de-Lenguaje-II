#ifndef _LEXICO
#define _LEXICO

#include <iostream>
#include <string>
#include <map>        // <-- ¡ESTE FALTABA!
#include <fstream>
#include <sstream>

using namespace std;

// --- Clase TipoSimbolo (del profesor) ---
class TipoSimbolo{
public:
    static const int ID= 0;
    static const int ENTERO= 50;
    static const int OPSUMA= 51;
    static const int PESOS= 1;
};


class Lexico{
public:
    // --- Miembros del profesor ---
    char simbolo;
    int tipo;

    // --- Constructores y métodos ---
    Lexico(string fuente);
    Lexico();

    void entrada(string fuente);
    int sigSimbolo();
    bool terminado();

    string getBuffer();

private:
    // --- Miembros del profesor ---
    string fuente;
    int ind;

    // --- NUESTRAS ADICIONES ---
    bool continua;
    std::map<std::string, int> tablaSimbolos; // <-- ¡ESTE NECESITA <map> y <string>!
    string buffer;
};

#endif
