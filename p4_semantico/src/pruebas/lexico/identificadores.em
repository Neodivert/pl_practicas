# Prueba de identificadores de variables.

# Variables globales
$a
$_a
$__a__1
$1		# ERROR: al principio no se admiten numeros.
$abc_
$_

# Variables de instancia
@1		# ERROR: al principio no se admiten numeros.
@____		# Nombre perfectamente explicativo y legal.
@abc
@_abc

# Constantes
ABC
_ABC 		# ERROR: al principio no se admite _. Se leera como un identificador
A_B_C
