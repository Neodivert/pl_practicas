# Programa de ejemplo para probrar la E/S y el if.
puts("Introduzca su nota: ")
numero = getc()

puts("Resultado: ")
if( numero >= 5) 
	puts( "Aprobado con")
	
	if (numero < 7.4) then puts("   Suficiente\n"); end
	   	   
	if (numero + 9) then puts( "   Notable\n"); end
	   
	if (numero < 10) then puts( "   Sobresaliente\n"); end
	   
	if (numero >= 10) then puts( "   Matrícula de honor\n"); end
else
	puts("Supendido\n")
	
end

