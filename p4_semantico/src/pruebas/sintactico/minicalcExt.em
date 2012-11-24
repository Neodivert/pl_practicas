#  Pequeño deféprete deferactivo de expresiones aritméticas enteras.

#  El usuario teclea cada vez una expresión, y se le muestra el
#  resultado. Termina al recibir una línea en blanco.  El lenguaje de
#  una expresión corresponde a la siguiente gramática EBNF:

#  expresion -> [ '+' | '-' ] termino  ( '+' | '-' ) termino end
#  termino -> factor  ( '*' | '/' ) factor end
#  factor -> numero | '(' expresion ')'
#  numero -> '0' | .. | '9'  '0' | .. | '9' end 



def error()
  puts("expresión errónea\n")
  exit()
end

def numero() 
  resultado = 0
  if (c < '0' or c> '9' ) 
    error()
  end
  
  resultado = 10 * resultado + c- '0'
  c = getc()
  while (c >= '0' and c<= '9') do 
    resultado = 10 * resultado + c - '0'
    c = getc()
  end 
  resultado = resultado
end

def factor()
  if (c=='(') 
    c = getchar()
    resultado = expresion()
    if (c==')') 
      c = getchar()
    else 
      error()
    end
    resultado = resultado
  else 
   numero()
  end
end

def termino()
  resultado = factor()
  while (c=='*' or c=='/') do
    op = c
    c = getc()
    aux = factor()
    if op == '*' 
      resultado = resultado*aux
    else
        if '/'
          resultado = resultado/aux
        end
    end
  end
 resultado = resultado
end

def expresion()
  op= '+'
  if (c=='+' or c=='-') 
    op=c
    c=getc()
  end
  resultado=termino()
  if (op=='-') 
    resultado = -resultado
  end
  while (c=='+' or c=='-') do
    op=c
    c=getc()
    aux=termino()
    if op == '+' 
      resultado = resultado+aux
    else
        if '-'
          resultado = resultado-aux
        end
    end
  end
  resultado = resultado
end

c = getc()
while (c!='\n') do
  resultado = expresion()
  if (c=='\n') 
    puts resultado
    c=getc()
  else 
    error()
  end   
end

class Punto
	@x = 0
	@y = 0
end

a = [1]
p = Punto.new
p.x = 2.1
p.y = p.x + 1
puts("p.x = #{p.x} p.y = #{p.y}")

d = Array.new(4,1)
a = Array.new(6,1.2)

c = [1.1 , 2.1, 1.1] 
b = z + d[1] + p[2]
z = 1

$pe = 1
arg1 = 4
i = 4.5
def metodo ( arg1, arg2 )
	i = 1
	$pa = 1
	arg1.each do |i|
		d = 5
		$pi = 1
		ar2.each do |j|
			$po = 1
			d = 5.4
			h = d
			x = Array.new(1,3)
		end	
	end
	$pu = 1
	b = 5
end
$paa = 1

def multiplicar(a, b)
  res = a * b
end

a = [1, 2, 3, 4]

a.each do |i|
	b = 4
	c = multiplicar(i, b)
	puts(c)
end
