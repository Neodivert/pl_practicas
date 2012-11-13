# Programa de ejemplo para probrar la E/S y el if.

puts "Introduzca su nota: "
numero = getc

puts "Resultado: "
if( numero >= 5)
	puts "Aprobado con"
	
	if (numero < 7)
	   puts "   Suficiente\n"
	   	   
	if (numero < 9)
	   puts "   Notable\n"
	   
	if (numero < 10)
	   puts "   Sobresaliente\n"
	   
	if (numero >= 10)
	   puts "   Matrícula de honor\n"
else
	puts "Supendido\n"
end

