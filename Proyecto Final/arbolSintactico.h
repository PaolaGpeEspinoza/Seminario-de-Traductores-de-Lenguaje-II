#ifndef _ARBOLSINTACTICO
#define _ARBOLSINTACTICO

#include <string>
#include <iostream>
#include "tablaSimbolos.h"

using namespace std;

class Generador;

class Nodo{
public:
	string simbolo;
	Nodo *sig;

	static int sangria;
	void muestraSangria(){
		for (int i=0; i < sangria; i++)
		cout << " ";
	}

	char tipoDato;
    static TablaSimbolos *tablaSimbolos;
    static string ambito;

	virtual void muestra() {};

	virtual void validaTipos() {
        tipoDato= 'v';
        if (sig != NULL) sig->validaTipos();
    };

    virtual void generaCodigo(Generador* gen) {
        // Por defecto, solo sigue al siguiente nodo
        if (sig) sig->generaCodigo(gen);
        }
    virtual void generaDatos(Generador* gen) {
        if (sig) sig->generaDatos(gen);
    }


};

class Tipo: public Nodo{
public:
	Tipo(string simbolo){
		this->simbolo= simbolo;
		this->sig=NULL;
	}

	void muestra(){
		muestraSangria();
		cout << "<Tipo> " << simbolo << endl;
	}
    char dimeTipo(){
		if ( simbolo.compare("int") == 0) return 'i';
		if ( simbolo.compare("float") == 0) return 'f';
		if ( simbolo.compare("string") == 0) return 's';
		if ( simbolo.compare("void") == 0) return 'v';
        return 'e'; // 'e' de error por si acaso
	}


};

class Expresion: public Nodo{
public:
	Expresion *izq, *der;
		string guardaArbol(){
		return "";
	}

};

class Identificador: public Expresion{
public:
	Identificador(string simbolo, Nodo *sig=NULL){
		this->simbolo= simbolo;
		this->sig=sig;
	}

	void muestra(){
		muestraSangria();
		cout << "<Identificador> " << simbolo << endl;
		if (sig != NULL) sig->muestra();
	}

	void validaTipos(){
		// Busca el ID en la tabla para saber su tipo
		tablaSimbolos->buscaIdentificador(simbolo);

		// ¿En qué ámbito lo encontró?
		if (tablaSimbolos->varLocal && tablaSimbolos->varLocal->ambito == Nodo::ambito) {
			tipoDato = tablaSimbolos->varLocal->tipo; // Es local
		} else if (tablaSimbolos->varGlobal) {
			tipoDato = tablaSimbolos->varGlobal->tipo; // Es global
		} else {
			// No lo encontró
			tipoDato = 'e'; // 'e' de error
			string error = "Error: Variable '" + simbolo + "' no ha sido declarada en el ambito '" + Nodo::ambito + "'.";
			Nodo::tablaSimbolos->listaErrores->push_back(error);
		}
	}

	void generaCodigo(Generador* gen);

};

class DefVar: public Nodo{
public:
	Tipo *tipo;
	Identificador *listaVar;
	friend class TablaSimbolos;

	DefVar(Tipo *tipo, Identificador *listaVar, Nodo *sig){
		this->tipo= tipo;
		this->listaVar= listaVar;
		this->sig = sig;
	}

	void muestra(){
		muestraSangria();
		cout << "<DefVar> " << endl;

		Nodo::sangria++;
		tipo->muestra();
		listaVar->muestra();
		Nodo::sangria--;

		if (sig != NULL) sig->muestra();
	}
	void validaTipos(){
        tipoDato = tipo->dimeTipo();
        tablaSimbolos->agrega(this); // 'this' es el propio DefVar

        // Continúa la validación con el resto de la lista
        if (sig != NULL) sig->validaTipos();
    }

    void generaCodigo(Generador* gen);

    void generaDatos(Generador* gen);

};

class Parametro: public Nodo{
protected:
	Tipo *tipo;
	Identificador *id;
	friend class TablaSimbolos;

public:

	Parametro (Tipo *tipo, Identificador *id, Nodo *sig){
		this->tipo= tipo;
		this->id= id;
		this->sig= sig;
	}

	void muestra(){
		muestraSangria();
		cout << "<Parametro> " << endl;

		Nodo::sangria++;
		tipo->muestra();
		id->muestra();
		Nodo::sangria--;

		if (sig != NULL) sig->muestra();
	}

	string cadTipos(){
		string cad;
		cad += tipo->dimeTipo();
		Parametro *p= (Parametro*)sig;

		while (p != NULL){
			cad += p->tipo->dimeTipo();
			p= (Parametro*)p->sig;
		}

		return cad;
	}

	void validaTipos(){
		tipoDato = tipo->dimeTipo();
        // La validación real (revisar duplicados) se hace en tablaSimbolos->agrega(Parametro*)
		if (sig != NULL) sig->validaTipos();
	}

};



class DefFunc: public Nodo{
protected:
	Tipo *tipo;
	Identificador *id;
	Parametro *parametros;
	Nodo *bloqueFunc;
	friend class TablaSimbolos;

public:

	DefFunc(	Tipo *tipo, Identificador *id, Parametro *parametros, Nodo *bloqueFunc, Nodo *sig){
		this->tipo= tipo;
		this->id= id;
		this->parametros= parametros;
		this->bloqueFunc= bloqueFunc;
		this->sig= sig;
	}

	void muestra(){
		muestraSangria();
		cout << "<DefFunc> " << endl;

		Nodo::sangria++;
		tipo->muestra();
		id->muestra();
		if (parametros) parametros->muestra();
		if (bloqueFunc) bloqueFunc->muestra();
		Nodo::sangria--;

		if (sig != NULL) sig->muestra();
	}

	void validaTipos(){
		tipoDato = tipo->dimeTipo();

        // 1. Agrega la función al ámbito global
        Nodo::ambito = ""; // Asegura estar en ámbito global
        tablaSimbolos->agrega(this);

        // 2. Cambia al ámbito local (el nombre de la función)
        Nodo::ambito = id->simbolo;

        // 3. Valida y agrega los parámetros (si existen)
        if (parametros) {
            parametros->validaTipos();
            tablaSimbolos->agrega(parametros);
        }

        // 4. Valida el bloque de la función (si existe)
        if (bloqueFunc) {
            bloqueFunc->validaTipos();
        }

        // 5. Regresa al ámbito global
        Nodo::ambito = "";

        if (sig != NULL) sig->validaTipos();
	}
	void generaCodigo(Generador* gen);


};


class Asignacion: public Nodo{
public:
	Identificador *id;
	Expresion *expresion;
	friend class TablaSimbolos;

	Asignacion(	Identificador *id, Expresion *expresion, Nodo *sig= NULL){
		this->id= id;
		this->expresion= expresion;
		this->sig= sig;

	}

	void muestra(){
		muestraSangria();
		cout << "<Asignacion> " << endl;

		Nodo::sangria++;
		id->muestra();
		expresion->muestra();
		Nodo::sangria--;

		if (sig != NULL) sig->muestra();
	}

	void validaTipos(){
		id->validaTipos();
		expresion->validaTipos();

        // Regla: El tipo del ID debe ser igual al tipo de la Expresión
		if (id->tipoDato != expresion->tipoDato) {
			tipoDato = 'e';
            string error = "Error: Tipo incompatible en la asignacion a '" + id->simbolo + "'.";
            Nodo::tablaSimbolos->listaErrores->push_back(error);
		} else {
            tipoDato = 'v'; // 'v' de void (la asignación no retorna valor)
        }
		if (sig != NULL) sig->validaTipos();
	}
	void generaCodigo(Generador* gen);


};

class Regresa: public Nodo{
protected:
	Expresion *expresion;
	friend class TablaSimbolos;

public:

	Regresa(Expresion *expresion, Nodo *sig= NULL){
		this->expresion= expresion;
		this->sig= sig;
	}

	void muestra(){
		muestraSangria();
		cout << "<Regresa> " << endl;

		Nodo::sangria++;
			expresion->muestra();
		Nodo::sangria--;

		if (sig != NULL) sig->muestra();
	}

	void validaTipos(){
        // Valida la expresión que está retornando
		if (expresion) {
            expresion->validaTipos();
        }
        // (La lógica de si el tipo de retorno es correcto
        //  se haría aquí, pero es más avanzado)

        tipoDato = 'v'; // 'v' de void
		if (sig != NULL) sig->validaTipos();
	}

};


class Entero: public Expresion{
public:
    Entero(string simbolo){
        this->simbolo= simbolo;
        this->sig=NULL;
    }

    void muestra(){
        muestraSangria();
        cout << "<Entero> " << simbolo << endl;
    }

    void validaTipos(){ tipoDato = 'i'; }

    // Solo la promesa (declaración), sin el código
    void generaCodigo(Generador* gen);
};
class Real: public Expresion{
public:
	Real(string simbolo){
		this->simbolo= simbolo;
		this->sig=NULL;
	}

	void muestra(){
		muestraSangria();
		cout << "<Real> " << simbolo << endl;
	}
	void validaTipos(){ tipoDato = 'f'; }
};

class Cadena: public Expresion{
public:
	Cadena(string simbolo){
		this->simbolo= simbolo;
		this->sig=NULL;
	}

	void muestra(){
		muestraSangria();
		cout << "<Cadena> " << simbolo << endl;
	}

	void validaTipos(){ tipoDato = 's'; }
};

class Signo: public Expresion{
protected:
public:

	Signo(string simbolo, Expresion *izq){
		this->simbolo = simbolo;
		this->izq= izq;
		sig= NULL;
	}

	void muestra(){
		muestraSangria();
		cout << "<Signo> " << endl;

		Nodo::sangria++;
			izq->muestra();
		Nodo::sangria--;

		if (sig != NULL) sig->muestra();
	}

	void validaTipos(){
        // Valida la expresión interna
		izq->validaTipos();
        // El tipo del nodo 'Signo' es el mismo que el de su hijo
		tipoDato = izq->tipoDato;
		if (sig != NULL) sig->validaTipos();
	}

};

class Mult: public Expresion{
protected:
public:

	Mult(string simbolo, Expresion *izq, Expresion *der){
		this->der= der;
		this->simbolo= simbolo;
		this->izq= izq;
		sig= NULL;

	}

	void muestra(){
		muestraSangria();
		cout << "<Multiplicacion> " << simbolo << endl;

		Nodo::sangria++;
			izq->muestra();
			der->muestra();
		Nodo::sangria--;

		if (sig != NULL) sig->muestra();
	}

	void validaTipos(){
		izq->validaTipos();
		der->validaTipos();

        // Regla: int * int = int | float * float = float
		if (izq->tipoDato == 'i' && der->tipoDato == 'i') tipoDato = 'i';
		else if (izq->tipoDato == 'f' && der->tipoDato == 'f') tipoDato = 'f';
		else {
			tipoDato = 'e'; // 'e' de error
            string error = "Error: Tipos incompatibles en la operacion '" + simbolo + "'.";
            Nodo::tablaSimbolos->listaErrores->push_back(error);
		}
		if (sig != NULL) sig->validaTipos();
	}

	string guardaArbol(){
		return "new Mult(" + simbolo + "," + izq->guardaArbol() + ", " +  der->guardaArbol() + ") ";
	}
	void generaCodigo(Generador* gen);
};

class Suma: public Expresion{
protected:
public:

	Suma(string simbolo, Expresion *izq, Expresion *der){
		this->der= der;
		this->simbolo= simbolo;
		this->izq= izq;
		sig= NULL;

	}

	void muestra(){
		muestraSangria();
		cout << "<Suma> " << simbolo << endl;

		Nodo::sangria++;
			izq->muestra();
			der->muestra();
		Nodo::sangria--;

		if (sig != NULL) sig->muestra();
	}

	void validaTipos(){
		izq->validaTipos();
		der->validaTipos();

        // Regla: int + int = int | float + float = float
		if (izq->tipoDato == 'i' && der->tipoDato == 'i') tipoDato = 'i';
		else if (izq->tipoDato == 'f' && der->tipoDato == 'f') tipoDato = 'f';
		else {
			tipoDato = 'e'; // 'e' de error
            string error = "Error: Tipos incompatibles en la operacion '" + simbolo + "'.";
            Nodo::tablaSimbolos->listaErrores->push_back(error);
		}
		if (sig != NULL) sig->validaTipos();
	}


	string guardaArbol(){
		return "new Suma(" + simbolo + "," + izq->guardaArbol() + ", " +  der->guardaArbol() + ") ";
	}
	void generaCodigo(Generador* gen);

};


#endif
