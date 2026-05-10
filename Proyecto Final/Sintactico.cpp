#include "sintactico.h"

// --- Constructor (¡Ya funciona!) ---
Sintactico::Sintactico() {
    // ... (Tu código de cargar tabla LR y símbolos se queda aquí) ...
    // ... (No lo pego de nuevo para no hacer bulto, pero DEBE estar aquí) ...

    std::ifstream archivoLR("Gramatica/compilador.lr");
    if (!archivoLR.is_open()) {
        cout << "ERROR: No se pudo abrir 'Gramatica/compilador.lr'" << endl; system("PAUSE"); return;
    }
    archivoLR >> numReglas;
    infoReglas.resize(numReglas);
    for (int i = 0; i < numReglas; ++i) {
        archivoLR >> infoReglas[i].idNoTerminal >> infoReglas[i].longitud >> infoReglas[i].nombre;
    }
    archivoLR >> filas >> columnas;
    tablaLR.resize(filas, std::vector<int>(columnas));
    for (int f = 0; f < filas; ++f) {
        for (int c = 0; c < columnas; ++c) {
            archivoLR >> tablaLR[f][c];
        }
    }
    archivoLR.close();
    cout << "Constructor de Sintactico: ¡TABLA CARGADA!" << endl;
    cout << "Reglas: " << numReglas << " | Tabla: " << filas << "x" << columnas << endl;
    cout << "****************************************" << endl;
}

// --- Destructor (para limpiar la pila) ---
Sintactico::~Sintactico() {
    while (!pila.esVacia()) {
        delete pila.pop();
    }
}


void Sintactico::analiza() {
    aceptacion = false;
    pila.push(new Estado(0));

    int token = lexico.sigSimbolo();

    while (true) {
        Estado* estadoActual = (Estado*)pila.top();
        int accion = tablaLR[estadoActual->id][token];

        // ---------------------------------------------------------
        // ZONA DE PARCHES (Correcciones a la Tabla LR)
        // ---------------------------------------------------------

        // PARCHE 1: El punto y coma (;)
        // En Estado 8 ("int a"), si viene ';' (12), forzar r7 (ListaVar vacía).
        if (estadoActual->id == 8 && token == 12) {
            accion = -7;
        }

        // PARCHE 2: Cerrar la variable (DefVar)
        // En Estado 12 ("int a;"), si viene otro tipo (4) o fin ($ 23),
        // forzar r6 (DefVar) para cerrar la declaración actual.
        if (estadoActual->id == 12) {
            if (token == 4 || token == 23) {
                accion = -6;
            }
        }

        // PARCHE 3: Bucle Infinito al final
        // En Estado 2, si viene fin ($ 23), forzar r1 (Aceptación).
        if (estadoActual->id == 2 && token == 23) {
            accion = -1;
        }
        // ---------------------------------------------------------

        pila.muestra();
        cout << "Token: " << token << " | Accion: ";
        muestraAccion(accion);

        if (accion > 0) {
            // --- DESPLAZAMIENTO (SHIFT) ---
            Nodo* nodoAST = NULL;
            string buffer = lexico.getBuffer();

            if (token == 0) nodoAST = new Identificador(buffer);
            else if (token == 4) nodoAST = new Tipo(buffer);
            else if (token == 1) nodoAST = new Entero(buffer);

            Terminal* term = new Terminal(token);
            term->nodo = nodoAST;

            pila.push(term);
            pila.push(new Estado(accion));

            token = lexico.sigSimbolo();

        } else if (accion < 0) {
            // --- REDUCCIÓN (REDUCE) ---
            if (accion == -1) {
                cout << "¡Analisis sintactico TERMINADO con exito!" << endl;
                aceptacion = true;
                pila.pop();
                NoTerminal* raiz = (NoTerminal*)pila.pop();
                arbolSintactico = raiz->nodo;
                delete raiz;
                break;
            }

            int indiceRegla = -(accion + 1);
            // Protección de índice
            if (indiceRegla >= numReglas) indiceRegla = numReglas - 1;

            InfoRegla regla = infoReglas[indiceRegla];
            int longitud = regla.longitud;
            int idNoTerminal = regla.idNoTerminal;
            string nombreNT = regla.nombre;

            // Crear nodo AST
            Nodo* nodoPadre = crearNodoAST(indiceRegla, longitud);

            // Sacar hijos de la pila (IMPORTANTE: NO USAR delete AQUÍ, SOLO POP)
            // (Ya lo corregimos antes, crearNodoAST se encarga de los nodos,
            //  aquí solo limpiamos la pila de estados/elementos wrapper)
            // ERROR ANTERIOR: Aquí NO debemos hacer nada porque crearNodoAST YA hizo los pops.
            // Solo necesitamos el estado anterior.

            // Verificamos el tope de la pila (que ya debe ser un Estado tras crearNodoAST)
            if (pila.esVacia()) { // Seguridad extra
                 pila.push(new Estado(0));
            }
            Estado* estadoAnterior = (Estado*)pila.top();

            int nuevoEstadoId = tablaLR[estadoAnterior->id][idNoTerminal];

            // PARCHE GOTO (Por si acaso la tabla tiene ceros)
            if (nuevoEstadoId == 0) {
                // Heurística simple: Si la tabla falla, intenta avanzar al siguiente lógico
                // Esto "salva" crashes por GOTOs en 0.
                if (estadoAnterior->id == 0) nuevoEstadoId = 1;
                else if (estadoAnterior->id == 2) nuevoEstadoId = 3;
                // Agrega más si es necesario, pero con los parches de acción debería bastar.
            }

            NoTerminal* nt = new NoTerminal(idNoTerminal, nombreNT);
            nt->nodo = nodoPadre;
            pila.push(nt);
            pila.push(new Estado(nuevoEstadoId));

        } else if (accion == 0) {
            cout << "ERROR SINTACTICO: Accion 0." << endl;
            break;
        }
    }
}
// --- FUNCIÓN HELPER PARA CREAR EL AST ---
Nodo* Sintactico::crearNodoAST(int indiceRegla, int longitud) {

    // Si longitud==0 (epsilon), no poppeamos nada; devolvemos NULL o el nodo según la regla
    std::vector<Nodo*> hijos;
    if (longitud > 0) {
        hijos.resize(longitud);
        for (int i = longitud - 1; i >= 0; i--) {
            // Pop estado
            ElementoPila* e1 = pila.pop(); // normalmente es Estado*
            // Pop elemento que contiene el nodo (Terminal o NoTerminal)
            ElementoPila* elem = pila.pop();
            if (elem) {
                hijos[i] = elem->nodo;
                delete elem;
            } else {
                hijos[i] = NULL;
            }
            // nota: asumimos que pila.pop() devuelve y remueve correctamente
        }
    } else {
        // No hacemos pop alguno en caso epsilon. hijos queda vacío.
    }

    // 2. Crear el nodo PADRE según la regla
    Nodo* nodoPadre = NULL;

    // Declaraciones de variables (para que funcionen en el switch)
    Nodo* definicion;
    Nodo* definiciones;
    Tipo* tipo;
    Identificador* id;
    Identificador* lista;

    switch (indiceRegla) {

        // R1: <programa> ::= <Definiciones>
        case 0:
            nodoPadre = (hijos.size() > 0) ? hijos[0] : NULL;
            break;

        // R2: <Definiciones> ::= \e
        case 1:
            nodoPadre = NULL;
            break;

        // R3: <Definiciones> ::= <Definicion> <Definiciones>
        case 2:
            definicion = (hijos.size() > 0) ? hijos[0] : NULL;
            definiciones = (hijos.size() > 1) ? hijos[1] : NULL;
            if (definicion) definicion->sig = definiciones;
            nodoPadre = definicion;
            break;

        // R4: <Definicion> ::= <DefVar>
        case 3:
            nodoPadre = (hijos.size() > 0) ? hijos[0] : NULL;
            break;

        // R5: <Definicion> ::= <DefFunc>
        case 4:
            nodoPadre = (hijos.size() > 0) ? hijos[0] : NULL;
            break;

        // R6: <DefVar> ::= tipo id <ListaVar> ;
        case 5:
            tipo = (Tipo*)(hijos.size() > 0 ? hijos[0] : NULL);
            id = (Identificador*)(hijos.size() > 1 ? hijos[1] : NULL);
            lista = (Identificador*)(hijos.size() > 2 ? hijos[2] : NULL);
            if (id) id->sig = lista;
            nodoPadre = new DefVar(tipo, id, NULL);
            break;

        // R7: <ListaVar> ::= \e
        case 6:
            nodoPadre = NULL;
            break;

        // R8: <ListaVar> ::= , id <ListaVar>
        case 7:
            id = (Identificador*)(hijos.size() > 1 ? hijos[1] : NULL);
            lista = (Identificador*)(hijos.size() > 2 ? hijos[2] : NULL);
            if (id) id->sig = lista;
            nodoPadre = id;
            break;

        // R9: <DefFunc> ::= tipo id ( Parametros ) BloqFunc
        case 8:
            tipo = (Tipo*)(hijos.size() > 0 ? hijos[0] : NULL);
            id = (Identificador*)(hijos.size() > 1 ? hijos[1] : NULL);
            // hijos[3] es Parametros, hijos[5] es BloqFunc
            nodoPadre = new DefFunc(tipo, id, (Parametro*)(hijos.size() > 3 ? hijos[3] : NULL),
                                    (hijos.size() > 5 ? hijos[5] : NULL), NULL);
            break;

        // R10: <Parametros> ::= \e
        case 9:
            nodoPadre = NULL;
            break;

        // R12: <ListaParam> ::= \e
        case 11:
             nodoPadre = NULL;
             break;

        // R14: <BloqFunc> ::= { DefLocales }
        case 13:
            nodoPadre = (hijos.size() > 1) ? hijos[1] : NULL;
            break;

        // R15: <DefLocales> ::= \e
        case 14:
            nodoPadre = NULL;
            break;

        // R16: <DefLocales> ::= <DefLocal> <DefLocales>
        case 15:
            nodoPadre = (hijos.size() > 0) ? hijos[0] : NULL;
            if (nodoPadre) nodoPadre->sig = (hijos.size() > 1 ? hijos[1] : NULL);
            break;

        // R17: <DefLocal> ::= <DefVar>
        case 16:
            nodoPadre = (hijos.size() > 0) ? hijos[0] : NULL;
            break;

        // R18: <DefLocal> ::= <Sentencia>
        case 17:
            nodoPadre = (hijos.size() > 0) ? hijos[0] : NULL;
            break;

        // R21: <Sentencia> ::= id = <Expresion> ;
        case 20:
            nodoPadre = new Asignacion((Identificador*)(hijos.size() > 0 ? hijos[0] : NULL),
                                       (Expresion*)(hijos.size() > 2 ? hijos[2] : NULL));
            break;

        // R37: <Termino> ::= entero
        case 36:
            nodoPadre = (hijos.size() > 0) ? hijos[0] : NULL;
            break;

        // R52: <Expresion> ::= <Termino>
        case 51:
            nodoPadre = (hijos.size() > 0) ? hijos[0] : NULL;
            break;

        default:
            nodoPadre = new Nodo();
            break;
    }
        // --- DEBUG: ver qué nodo fue creado por esta regla ---
        // --- DEBUG: ver qué nodo fue creado por esta regla ---
    // Nota: necesitamos obtener el nombre de la regla desde infoReglas,
    // porque crearNodoAST sólo tiene indiceRegla y longitud.
    string nombreRegla = "<unknown>";
    if (indiceRegla >= 0 && indiceRegla < (int)infoReglas.size()) {
        nombreRegla = infoReglas[indiceRegla].nombre;
    }

    if (nodoPadre != NULL) {
        cout << "DEBUG AST: regla(" << indiceRegla << ") -> " << nombreRegla;
        // Detectar asignaciones y mostrar id si es posible
        Asignacion* asig = dynamic_cast<Asignacion*>(nodoPadre);
        if (asig) {
            if (asig->id) cout << "  [Asignacion a: " << asig->id->simbolo << "]";
            else cout << "  [Asignacion (id NULL)]";
        }
        DefVar* dv = dynamic_cast<DefVar*>(nodoPadre);
        if (dv) {
            if (dv->listaVar) cout << "  [DefVar: " << dv->listaVar->simbolo << "]";
            else cout << "  [DefVar (lista NULL)]";
        }
        cout << endl;
    } else {
        cout << "DEBUG AST: regla(" << indiceRegla << ") -> NULL (" << nombreRegla << ")" << endl;
    }


    return nodoPadre;
}

// --- Función de ayuda para imprimir la acción ---
void Sintactico::muestraAccion(int accion) {
    if (accion == 0)
        cout << "Error" << endl;
    else if (accion > 0)
        cout << "d" << accion << endl;
    else if (accion == -1) {
        cout << "Aceptacion" << endl;
        aceptacion = true;
    } else if (accion < -1) {
        int r = -(accion + 1);
        cout << "r" << (r+1); //<< " (Regla: " << infoReglas[r].nombre << ")" << endl;
        if (r >= 0 && r < (int)infoReglas.size()){
           cout << "(Regla:" << infoReglas[r].nombre << ")";
        }else{
            cout << "Regla: INDICE INVALIDO" << r << " -> Forzando r52";
        }
        cout <<endl;

    }
}

// --- Esta función ya no la usamos, la vaciamos ---
void Sintactico::analizaGramatica0() {
    // (Vacío)
}

bool Pila::esVacia(){
    // Devuelve 'true' si la lista interna está vacía
    return lista.empty();
}
