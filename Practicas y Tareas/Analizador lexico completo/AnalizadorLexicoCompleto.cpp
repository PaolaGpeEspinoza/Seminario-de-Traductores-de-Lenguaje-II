#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <map>

using namespace std;

// Definición de Tokens según la tabla del profesor
enum TokenType {
    IDENTIFICADOR = 0,
    ENTERO = 1,
    REAL = 2,
    CADENA = 3,
    TIPO = 4,       // int, float, void
    OP_SUMA = 5,    // +, -
    OP_MUL = 6,     // *, /
    OP_RELAC = 7,   // <, <=, >, >=
    OP_OR = 8,      // ||
    OP_AND = 9,     // &&
    OP_NOT = 10,    // !
    OP_IGUALDAD = 11, // ==, !=
    PUNTO_COMA = 12, // ;
    COMA = 13,      // ,
    PARENT_IZQ = 14, // (
    PARENT_DER = 15, // )
    LLAVE_IZQ = 16,  // {
    LLAVE_DER = 17,  // }
    ASIGNACION = 18, // =
    IF = 19,
    WHILE = 20,
    RETURN = 21,
    ELSE = 22,
    FIN = 23
};

struct Token {
    int tipo;
    string valor;
};

// Función auxiliar para verificar si es palabra reservada o tipo
int identificarPalabra(string s) {
    if (s == "int" || s == "float" || s == "void") return TIPO;
    if (s == "if") return IF;
    if (s == "while") return WHILE;
    if (s == "return") return RETURN;
    if (s == "else") return ELSE;
    return IDENTIFICADOR; // Si no es reservada, es ID (0)
}

void analizadorLexico(string entrada) {
    int i = 0;
    int n = entrada.length();

    cout << "Analizando entrada: " << entrada << endl;
    cout << "---------------------------------------------------" << endl;
    cout << "LEXEMA\t\tTIPO (ID)" << endl;
    cout << "---------------------------------------------------" << endl;

    while (i < n) {
        // Ignorar espacios
        if (isspace(entrada[i])) {
            i++;
            continue;
        }

        // 1. Identificadores y Palabras Reservadas
        if (isalpha(entrada[i])) {
            string lexema = "";
            while (i < n && (isalnum(entrada[i]))) {
                lexema += entrada[i];
                i++;
            }
            int tipo = identificarPalabra(lexema);
            cout << lexema << "\t\t" << tipo << endl;
        }
        // 2. Números (Enteros y Reales)
        else if (isdigit(entrada[i])) {
            string num = "";
            while (i < n && isdigit(entrada[i])) {
                num += entrada[i];
                i++;
            }
            // Revisar si es Real
            if (i < n && entrada[i] == '.') {
                num += '.';
                i++;
                while (i < n && isdigit(entrada[i])) {
                    num += entrada[i];
                    i++;
                }
                cout << num << "\t\t" << REAL << endl;
            } else {
                cout << num << "\t\t" << ENTERO << endl;
            }
        }
        // 3. Operadores y Símbolos
        else {
            switch (entrada[i]) {
                case '+': cout << "+\t\t" << OP_SUMA << endl; i++; break;
                case '-': cout << "-\t\t" << OP_SUMA << endl; i++; break;
                case '*': cout << "*\t\t" << OP_MUL << endl; i++; break;
                case '/': cout << "/\t\t" << OP_MUL << endl; i++; break;
                case ';': cout << ";\t\t" << PUNTO_COMA << endl; i++; break;
                case ',': cout << ",\t\t" << COMA << endl; i++; break;
                case '(': cout << "(\t\t" << PARENT_IZQ << endl; i++; break;
                case ')': cout << ")\t\t" << PARENT_DER << endl; i++; break;
                case '{': cout << "{\t\t" << LLAVE_IZQ << endl; i++; break;
                case '}': cout << "}\t\t" << LLAVE_DER << endl; i++; break;
                // Casos dobles
                case '=': 
                    if (i+1 < n && entrada[i+1] == '=') { cout << "==\t\t" << OP_IGUALDAD << endl; i+=2; }
                    else { cout << "=\t\t" << ASIGNACION << endl; i++; }
                    break;
                case '!':
                    if (i+1 < n && entrada[i+1] == '=') { cout << "!=\t\t" << OP_IGUALDAD << endl; i+=2; }
                    else { cout << "!\t\t" << OP_NOT << endl; i++; }
                    break;
                case '<':
                    if (i+1 < n && entrada[i+1] == '=') { cout << "<=\t\t" << OP_RELAC << endl; i+=2; }
                    else { cout << "<\t\t" << OP_RELAC << endl; i++; }
                    break;
                case '>':
                    if (i+1 < n && entrada[i+1] == '=') { cout << ">=\t\t" << OP_RELAC << endl; i+=2; }
                    else { cout << ">\t\t" << OP_RELAC << endl; i++; }
                    break;
                case '&':
                    if (i+1 < n && entrada[i+1] == '&') { cout << "&&\t\t" << OP_AND << endl; i+=2; }
                    else { cout << "&\t\tERROR" << endl; i++; }
                    break;
                case '|':
                    if (i+1 < n && entrada[i+1] == '|') { cout << "||\t\t" << OP_OR << endl; i+=2; }
                    else { cout << "|\t\tERROR" << endl; i++; }
                    break;
                default:
                    cout << entrada[i] << "\t\tERROR" << endl;
                    i++;
                    break;
            }
        }
    }
    cout << "$\t\t" << FIN << endl; // Fin de entrada
    cout << endl;
}

int main() {
    // PRUEBA COMPLETA CON TODOS LOS CASOS DE LA LISTA
    string codigoFuente = "int main() { float x = 3.14; if (x >= 0) return 1; else return -1; }";
    
    analizadorLexico(codigoFuente);
    return 0;
}