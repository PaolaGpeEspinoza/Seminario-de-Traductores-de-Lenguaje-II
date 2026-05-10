#include "tablaSimbolos.h"
#include "arbolSintactico.h"


void TablaSimbolos::agrega( DefVar *defVar){
    char tipo = defVar->tipo->dimeTipo();

    // Recorre la lista de identificadores (ej. int a, b, c)
    Identificador *p = defVar->listaVar;
    while (p != NULL) {

        // Revisa si ya está definida (como pide el PDF)
        bool yaExiste = false;
        if (Nodo::ambito == "") { // Ámbito global
            yaExiste = varGlobalDefinida(p->simbolo);
        } else { // Ámbito local
            yaExiste = varLocalDefinida(p->simbolo, Nodo::ambito);
        }

        if (yaExiste) {
            // Agrega error a la lista (como pide el PDF)
            string error = "Error: Variable '" + p->simbolo + "' redefinida en el ambito '" + Nodo::ambito + "'.";
            listaErrores->push_back(error);
        } else {
            // Agrega la nueva variable a la tabla
            ElementoTabla *elem = new Variable(tipo, p->simbolo, Nodo::ambito);
            agrega(elem); // Llama al 'agrega' base
        }

        p = (Identificador*)p->sig; // Avanza al siguiente identificador
    }
}

void TablaSimbolos::agrega( Parametro *parametros){
    // Recorre la lista de parámetros
    Parametro *p = parametros;
    while (p != NULL) {

        // Revisa si ya está definido (como variable local)
        if (varLocalDefinida(p->id->simbolo, Nodo::ambito)) {
            // Agrega error a la lista
            string error = "Error: Parametro '" + p->id->simbolo + "' redefinido en la funcion '" + Nodo::ambito + "'.";
            listaErrores->push_back(error);
        } else {
            // Agrega el parámetro como una variable local
            ElementoTabla *elem = new Variable(p->tipo->dimeTipo(), p->id->simbolo, Nodo::ambito);
            agrega(elem);
        }

        p = (Parametro*)p->sig; // Avanza al siguiente parámetro
    }
}


void TablaSimbolos::agrega( DefFunc *defFunc){
    // Revisa si la función ya está definida
    if (funcionDefinida(defFunc->id->simbolo)) {
        string error = "Error: Funcion '" + defFunc->id->simbolo + "' redefinida.";
        listaErrores->push_back(error);
    } else {
        // Agrega la nueva función a la tabla
        char tipo = defFunc->tipo->dimeTipo();
        string nombre = defFunc->id->simbolo;

        // Obtiene la "cadena de tipos" de los parámetros
        string tiposParams = "";
        if(defFunc->parametros) {
            tiposParams = defFunc->parametros->cadTipos();
        }

        ElementoTabla *elem = new Funcion(tipo, nombre, tiposParams);
        agrega(elem);
    }
}

int TablaSimbolos::dispersion(string simbolo){
	unsigned int h=0, g;
	unsigned char c;
	unsigned char alfa= 4;

	for (int i=0; i<simbolo.length(); i++){
		c= simbolo[i];
//		h= (h << 4) + c;
		h += alfa*c;
		h= h % TAM;
	}

	return h%TAM;
}

void TablaSimbolos::agrega( ElementoTabla *elemento){
	int ind= dispersion(elemento->simbolo);
	tabla[ind].push_back(elemento);
}

void TablaSimbolos::muestra(){
	list<ElementoTabla*>::iterator it;
	ElementoTabla *elem;

	cout << endl << "*** Tabla de Simbolos ***" << endl;

	for (int i=0; i< TAM; i++){
		if (tabla[i].size() > 0){

			for (it= tabla[i].begin(); it != tabla[i].end(); it++){
				elem= (ElementoTabla *)*it;
				cout << "Lista= " << i << endl;
				elem->muestra();
			}
		}
	}
}

bool TablaSimbolos::funcionDefinida(string funcion){
	int ind= dispersion(funcion);
	list<ElementoTabla*>::iterator it;
	ElementoTabla *elem;

	for (it= tabla[ind].begin(); it != tabla[ind].end(); it++){
		elem= (ElementoTabla *)*it;
		if (elem->esFuncion()) {
			if (elem->simbolo.compare(funcion) == 0) return true;
		}
	}

	return false;
}

bool TablaSimbolos::varGlobalDefinida(string variable){
	int ind= dispersion(variable);
	list<ElementoTabla*>::iterator it;
	ElementoTabla *elem;

	for (it= tabla[ind].begin(); it != tabla[ind].end(); it++){
		elem= (ElementoTabla *)*it;
		if (elem->esVariable() && !elem->esVarLocal()) {
			if (elem->simbolo.compare(variable) == 0) return true;
		}
	}

	return false;
}

bool TablaSimbolos::varLocalDefinida(string variable, string funcion){
	int ind= dispersion(variable);
	list<ElementoTabla*>::iterator it;
	ElementoTabla *elem;

	for (it= tabla[ind].begin(); it != tabla[ind].end(); it++){
		elem= (ElementoTabla *)*it;
		if (elem->esVariable() && elem->esVarLocal()) {
			if (((Variable*)elem)->ambito.compare(funcion) ==0  && elem->simbolo.compare(variable) == 0) return true;
		}
	}

	return false;
}

void TablaSimbolos::buscaIdentificador(string simbolo){
	int ind= dispersion(simbolo);
	list<ElementoTabla*>::iterator it;
	ElementoTabla *elem;

	varGlobal= NULL;
	varLocal= NULL;
	funcion= NULL;

	for (it= tabla[ind].begin(); it != tabla[ind].end(); it++){
		elem= (ElementoTabla *)*it;

		if (elem->simbolo.compare(simbolo) == 0){
			if (elem->esVariable()){
				if (elem->esVarLocal()) varLocal= (Variable*)elem;
				else
					varGlobal= (Variable*)elem;
			}
			else
				funcion= (Funcion*)elem;
		}

	}

}


void TablaSimbolos::buscaFuncion(string simbolo){
	int ind= dispersion(simbolo);
	list<ElementoTabla*>::iterator it;
	ElementoTabla *elem;

	varGlobal= NULL;
	varLocal= NULL;
	funcion= NULL;

	for (it= tabla[ind].begin(); it != tabla[ind].end(); it++){
		elem= (ElementoTabla *)*it;

		if (elem->simbolo.compare(simbolo) == 0 && elem->esFuncion()){
				funcion= (Funcion*)elem;
				return;
		}
	}

}

