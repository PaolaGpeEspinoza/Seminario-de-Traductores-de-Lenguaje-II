#include <iostream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

// Función para clasificar tokens
void analizar(string entrada) {
    int i = 0;
    int n = entrada.length();

    cout << "Analizando: " << entrada << endl;
    cout << "-----------------------------------------" << endl;

    while (i < n) {
        // Ignorar espacios en blanco
        if (isspace(entrada[i])) {
            i++;
            continue;
        }

        // 1. REGLA: Identificadores = letra(letra|digito)*
        if (isalpha(entrada[i])) {
            string id = "";
            while (i < n && (isalnum(entrada[i]))) {
                id += entrada[i];
                i++;
            }
            cout << "[TOKEN] Identificador: " << id << endl;
        }
        // 2. REGLA: Real = entero.entero+
        else if (isdigit(entrada[i])) {
            string num = "";
            bool tienePunto = false;

            while (i < n && (isdigit(entrada[i]) || entrada[i] == '.')) {
                if (entrada[i] == '.') {
                    if (tienePunto) break; // Ya tenía punto, rompemos (error o fin)
                    tienePunto = true;
                }
                num += entrada[i];
                i++;
            }

            // Validamos si cumple la regla estricta (debe tener punto para ser Real según la tarea)
            if (tienePunto && num.back() != '.') {
                 cout << "[TOKEN] Numero Real:   " << num << endl;
            } else {
                // Si no tiene punto, técnicamente es entero, pero la tarea pide "Real".
                // Lo clasificamos como entero o error según prefieras, aquí lo dejo como entero simple.
                cout << "[INFO]  Numero Entero: " << num << " (No es Real segun regla)" << endl;
            }
        }
        // Caracteres desconocidos
        else {
            cout << "[!] Caracter no reconocido: " << entrada[i] << endl;
            i++;
        }
    }
    cout << endl;
}

int main() {
    // Casos de prueba
    string test1 = "variable1  3.1416  radio  50.5"; 
    string test2 = "area = 45.2 * base";
    
    analizar(test1);
    analizar(test2);

    return 0;
}