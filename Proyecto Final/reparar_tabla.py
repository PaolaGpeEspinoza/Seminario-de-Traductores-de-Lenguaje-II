import pandas as pd
import numpy as np

def convertir_accion(valor):
    if pd.isna(valor) or valor == '': return 0
    valor = str(valor).strip()
    if valor.startswith('d'): return int(valor[1:])
    if valor == 'r0' or valor == 'acc' or valor == 'acept': return -1
    if valor.startswith('r'):
        num_regla = int(valor[1:])
        return -(num_regla + 1)
    try: return int(float(valor))
    except: return 0

print("Leyendo compilador.csv...")
df = pd.read_csv('compilador.csv')

if "Unnamed: 0" in df.columns:
    df = df.drop(columns=["Unnamed: 0"])

# =======================================================
# 🚑 CIRUGÍA DE DATOS: Arreglando los bugs del profesor
# =======================================================
print("Aplicando correcciones a la tabla...")

# 1. BUG DE PARAMETROS (El que nos detuvo ahorita)
# En estado 11 ('('), si viene ')' debe ser r10 (vacio), no r11.
print("- Corrigiendo Parametros en main()...")
df.loc[11, ')'] = 'r10' 

# 2. BUG DE GOTO DE PARAMETROS
# Si estamos en 11 y reducimos Parametros, debemos ir a un estado válido (12)
if 'Parametros' in df.columns:
    df.loc[11, 'Parametros'] = '12'

# 3. BUG DEL PUNTO Y COMA (int a;)
# En estado 8, ';' debe cerrar lista vacia (r7)
df.loc[8, ';'] = 'r7'

# 4. BUG DE DEFVAR (Cerrar variable)
# En estado 12, si sigue algo más, cerrar DefVar (r6)
df.loc[12, '$'] = 'r6' 
if 'identificador' in df.columns: df.loc[12, 'identificador'] = 'r6'
if '}' in df.columns: df.loc[12, '}'] = 'r6'

# 5. BUG DEL BUCLE INFINITO (Definiciones)
df.loc[3, '$'] = 'r2'
df.loc[7, '$'] = 'r3'

# 6. BUG DE CIERRE DE BLOQUE '}'
# En estado 17, '}' debe desplazar (d23)
df.loc[17, '}'] = 'd23'

# 7. BUG FINAL (Cierre de funciones)
df.loc[23, '$'] = 'r14' # BloqFunc
df.loc[15, '$'] = 'r9'  # DefFunc

# =======================================================

filas, columnas = df.shape
print(f"Dimensiones finales: {filas} filas x {columnas} columnas")

with open('compilador.lr', 'w') as f:
    # Escribimos las reglas estáticas (estas no cambian)
    reglas = """52
24 1 programa
25 0 Definiciones
25 2 Definiciones
26 1 Definicion
26 1 Definicion
27 4 DefVar
28 0 ListaVar
28 3 ListaVar
29 6 DefFunc
30 0 Parametros
30 3 Parametros
31 0 ListaParam
31 4 ListaParam
32 3 BloqFunc
33 0 DefLocales
33 2 DefLocales
34 1 DefLocal
34 1 DefLocal
35 0 Sentencias
35 2 Sentencias
36 4 Sentencia
36 6 Sentencia
36 5 Sentencia
36 3 Sentencia
36 2 Sentencia
37 0 Otro
37 2 Otro
38 3 Bloque
39 0 ValorRegresa
39 1 ValorRegresa
40 0 Argumentos
40 2 Argumentos
41 0 ListaArgumentos
41 3 ListaArgumentos
42 1 Termino
42 1 Termino
42 1 Termino
42 1 Termino
42 1 Termino
43 4 LlamadaFunc
44 1 SentenciaBloque
44 1 SentenciaBloque
45 3 Expresion
45 2 Expresion
45 2 Expresion
45 3 Expresion
45 3 Expresion
45 3 Expresion
45 3 Expresion
45 3 Expresion
45 3 Expresion
45 1 Expresion"""
    f.write(reglas + "\n")
    f.write(f"{filas} {columnas}\n")
    
    # Escribimos la tabla corregida
    for i in range(filas):
        fila_ints = []
        for col in df.columns:
            val = convertir_accion(df.iloc[i][col])
            fila_ints.append(str(val))
        f.write("\t".join(fila_ints) + "\n")

print("¡Listo! Archivo 'compilador.lr' CORREGIDO generado.")