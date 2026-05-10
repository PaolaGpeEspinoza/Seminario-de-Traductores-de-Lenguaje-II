## Introducción
El analizador léxico es la primera fase de un compilador o intérprete. Su papel es fundamental: recibe el código fuente como texto plano y lo transforma en una secuencia de **tokens**, que son unidades con significado dentro del lenguaje de programación. Gracias a este proceso, el analizador sintáctico puede trabajar con estructuras de más alto nivel sin preocuparse por los detalles de los caracteres individuales.

## ¿Qué es un analizador léxico?
Un **analizador léxico** (también llamado *scanner* o *tokenizer*) es un programa que:
1. Lee el código fuente carácter por carácter.  
2. Agrupa los caracteres en lexemas de acuerdo con reglas formales (expresiones regulares o autómatas finitos).  
3. Clasifica cada lexema en un tipo de token (ejemplo: identificador, número entero, operador de suma).  
4. Entrega una secuencia de tokens al analizador sintáctico.

## Componentes principales
- **Alfabeto:** conjunto de símbolos válidos (ej. letras, dígitos, signos).  
- **Lexema:** secuencia de caracteres concreta en el código (ej. `x`, `10`).  
- **Token:** par formado por el tipo de símbolo y su lexema (ej. `<ENTERO, 10>`).  
- **Errores léxicos:** ocurren cuando se encuentra un carácter o secuencia no válida dentro del lenguaje.  

## Implementación en este proyecto
En este trabajo se desarrolló un **analizador léxico en Python** que reconoce:
- Identificadores: letra seguida de letras o dígitos.  
- Constantes: enteras y reales.  
- Palabras reservadas: `if`, `while`, `return`, `else`, `int`, `float`, `void`.  
- Operadores: aritméticos (+, -, *, /), relacionales (<, <=, >, >=, ==, !=), lógicos (&&, ||, !), asignación (=).  
- Delimitadores: paréntesis, llaves, punto y coma, coma.  
- Símbolo de fin de entrada: `$`.  

Los tokens fueron codificados de acuerdo con la tabla entregada por el profesor.

## Conclusión
El analizador léxico es indispensable para la construcción de compiladores y lenguajes de programación, ya que traduce el código fuente a una forma estructurada (tokens) comprensible para las fases posteriores. Su correcta implementación garantiza que el analizador sintáctico pueda operar de manera confiable y que los errores se detecten en la etapa más temprana posible.

## Bibliografía
- Aho, A. V., Lam, M. S., Sethi, R., & Ullman, J. D. (2007). *Compilers: Principles, Techniques, and Tools* (2nd ed.). Addison Wesley.  
- Grune, D., Bal, H. E., Jacobs, C. J. H., & Langendoen, K. (2012). *Modern Compiler Design* (2nd ed.). Springer.  
- Loukas, A. (2019). *Lexical Analysis in Compiler Design*. Journal of Computer Science.  
