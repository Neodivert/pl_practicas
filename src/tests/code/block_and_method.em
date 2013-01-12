###############################################################################
# Emerald - semantic test
# Multiply a vector's elements by a constant.
# Language's tested features: block and method calling, arrays.
###############################################################################

# FIXME: Al ejecutar da:
#Q.h: Acceso a memoria fuera de l�mites (error 2)
#48�		R0 = I(R0 + 8); //Loading value of var i
# Y si el vector y el bloque se mete en un metodo, se obtiene un fallo de
# de segmentacion.

# Multiplication's method.
def multiply( a, b )
  res = a * b
end

# Vector definition.
v = [1, 2, 3, 4]

# (Block) Iterate over the elements of vector and call Multiply.
v.each do |i|
	b = 4
	c = multiply( i, b )
	puts( "#{c}" )
end
