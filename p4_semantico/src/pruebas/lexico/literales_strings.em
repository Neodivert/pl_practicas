# Prueba de literales string y caracter

#Pruba de las secuencias de escape
"\t '  \\\" \" \n  \\n" 

# Strings
"a"
"abc"

# Error: String sin cerrar.
"a 
# Error: String sin abrir.		
a"		

"a#{a}"
# Error: Interpolación sin cerrar.
"#{"		
"#{
# Error: solo se permiten variables simples
"#{a[1]}"
"#{a} \" dfd \t #{b}"
# Error secuencias de escape \u y \q
"3 df} \u#\\ {\q"

# Caracteres
'a'
# Error: caracter sin cerrar
'a
''
# Error: caracter sin abrir
a'
# Error: \ sin valor
'\'
'\n'
'\s'
# Error: solo se permite un caracter
'aa'
# Error: solo se permite un caracter
'\nk'
'\''
'"'
