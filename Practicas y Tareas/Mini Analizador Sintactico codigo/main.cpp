#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

// --- CLASE PILA ---
// Implementada con vector para poder mostrar el contenido fácilmente
class Pila {
private:
    vector<int> elementos;
public:
    void push(int x) {
        elementos.push_back(x);
    }
    
    int pop() {
        if (elementos.empty()) return -1;
        int top = elementos.back();
        elementos.pop_back();
        return top;
    }
    
    int top() {
        if (elementos.empty()) return -1;
        return elementos.back();
    }
    
    void muestra() {
        cout << "Pila: ";
        for (int e : elementos) {
            cout << e << " ";
        }
        cout << endl;
    }
};

// --- CLASE LEXICO ---
// Simula el analizador léxico devolviendo columnas para la tabla
class Lexico {
private:
    string entrada;
    int indice;
public:
    int tipo; // Columna en la tabla: 0=id, 1=+, 2=$, 3=E (No terminal)
    char simbolo; // El caracter actual

    Lexico(string in) {
        entrada = in;
        indice = 0;
        tipo = -1;
    }

    void sigSimbolo() {
        if (indice >= entrada.length()) {
            simbolo = '$';
            tipo = 2; // Fin de cadena
            return;
        }

        simbolo = entrada[indice];
        indice++;

        // Mapeo según instrucciones del Ejercicio 2:
        // id (letras) = 0
        // + = 1
        // $ = 2
        if (isalpha(simbolo)) {
            tipo = 0;
        } else if (simbolo == '+') {
            tipo = 1;
        } else {
            tipo = 2; // Por defecto o error
        }
    }
    
    // Método para resetear el puntero (útil si hicieras múltiples pruebas)
    bool terminado() {
        return simbolo == '$';
    }
};

// --- TABLA LR Y LOGICA PRINCIPAL ---
int main() {
    // TABLA LR DEL EJERCICIO 2
    // Filas 0-4. Columnas: 0(id), 1(+), 2($), 3(E)
    // Reglas:
    // -1 = Aceptación
    // -2 = Reducción regla 1 (E -> id + E)
    // -3 = Reducción regla 2 (E -> id)
    // Positivos = Desplazamientos
    
    int tablaLR[5][4] = {
        { 2,  0,  0,  1}, // Estado 0
        { 0,  0, -1,  0}, // Estado 1 (Aceptación)
        { 0,  3, -3,  0}, // Estado 2 (OJO: Texto dice r2 en $, aquí puse -3 para r2)
        { 2,  0,  0,  4}, // Estado 3
        { 0,  0, -2,  0}  // Estado 4 (Texto dice r1 en $, aquí puse -2 para r1)
    };

    // Inicialización
    Pila pila;
    Lexico lexico("a+b+c"); // Entrada del ejercicio
    int fila, columna, accion;
    bool aceptacion = false;

    cout << "--- INICIO DEL ANALISIS SINTACTICO LR ---" << endl;
    cout << "Entrada a analizar: a+b+c" << endl << endl;

    // Estado inicial
    pila.push(2); // $ representado como 2 en algunos contextos, o simplemente marcador de fondo
    // Nota: El PDF dice push(TipoSimbolo::PESOS) y luego push(0). 
    // Asumiremos que el fondo de pila no afecta si la tabla está bien hecha, 
    // pero seguiremos la lógica estándar LR: 
    // Limpiamos pila y metemos estado inicial 0.
    
    // Reiniciamos pila para hacerlo limpio:
    while(pila.top() != -1) pila.pop(); 
    pila.push(2); // $ (Simbolo Pesos)
    pila.push(0); // Estado inicial
    
    lexico.sigSimbolo();

    // CICLO PRINCIPAL
    while (!aceptacion) {
        fila = pila.top();
        columna = lexico.tipo;
        
        // Validación de índices para evitar crash
        if (fila < 0 || fila > 4 || columna < 0 || columna > 3) {
             cout << "Error: indices fuera de rango o celda vacia." << endl;
             break;
        }

        accion = tablaLR[fila][columna];

        pila.muestra();
        cout << "Entrada: " << lexico.simbolo << " (" << columna << ")" << endl;
        cout << "Accion: " << accion << endl;
        cout << "-------------------------" << endl;

        if (accion == 0) {
            cout << "Error de sintaxis: Celda vacia en tabla." << endl;
            break;
        }

        // DESPLAZAMIENTO (SHIFT)
        if (accion > 0) {
            pila.push(lexico.tipo); // Metemos el símbolo (id o +)
            pila.push(accion);      // Metemos el siguiente estado
            lexico.sigSimbolo();    // Avanzamos al siguiente caracter
        }
        // REDUCCION (REDUCE)
        else if (accion < 0) {
            if (accion == -1) {
                cout << "Cadena ACEPTADA exitosamente." << endl;
                aceptacion = true;
                break;
            }
            
            int id_regla = accion; // -2 o -3
            int simbolo_NoTerminal_E = 3; // Columna de E
            
            // Regla 1 (código -2): E -> id + E
            // Quitamos 3 símbolos de la gramática (id, +, E) -> 3 * 2 (estado+simbolo) = 6 pops
            if (id_regla == -2) {
                cout << "REDUCCION R1: E -> id + E (pop 6 elementos)" << endl;
                for(int k=0; k<6; k++) pila.pop();
            }
            // Regla 2 (código -3): E -> id
            // Quitamos 1 símbolo (id) -> 1 * 2 (estado+simbolo) = 2 pops
            else if (id_regla == -3) {
                cout << "REDUCCION R2: E -> id (pop 2 elementos)" << endl;
                pila.pop(); 
                pila.pop();
            }

            // GOTO (Transición con el No Terminal E)
            fila = pila.top(); // Estado que quedó en la cima
            columna = simbolo_NoTerminal_E; 
            accion = tablaLR[fila][columna]; // Buscar a dónde ir con E
            
            cout << "GOTO(" << fila << ", E) -> " << accion << endl << endl;
            
            pila.push(simbolo_NoTerminal_E);
            pila.push(accion);
        }
    }

    return 0;
}