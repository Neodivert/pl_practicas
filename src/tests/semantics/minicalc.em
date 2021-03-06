###############################################################################
# Emerald - code test
# Interactive aritmetic expressions interpreter.
# Language's tested features: methods, ifs, I/O
###############################################################################

# FIXME: still doesnt work.

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
  if (c < '0' or c > '9' ) 
    error()
  end
  
 # resultado = 10 * resultado + c- '0'
  c = getc
  while (c >= '0' and c<= '9') do 
   # resultado = 10 * resultado + c - '0'
    c = getc
  end 
  resultado = resultado
end

def factor()
  if (c=='(') 
    c = getc
    resultado = expresion()
    if (c==')') 
      c = getc
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
    c = getc
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
    c=getc
  end
  resultado=termino()
  if (op=='-') 
    resultado = -resultado
  end
  while (c=='+' or c=='-') do
    op=c
    c=getc
    aux=termino()
    if op == '+' 
      resultado = resultado+aux
    else
        if op == '-'
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
