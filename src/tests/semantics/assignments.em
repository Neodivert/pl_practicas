###############################################################################
# Emerald - semantic test
# Try to do a lot of llegal and illegal assignments.
# Language's tested features: type checking in assignments.
###############################################################################

# ERROR: float * integer is not permited.
a = 15.4 * 4	

# ERROR: not applied on integer is not permitted.
b = not 14		

# ERROR: integer + boolean is not permited.
a = 4 / 12.5 - 23 + true

# ERROR: All elements in array must be the same type
b = [1, 2.3 ,3]	

# ERROR: [] operator can not be applied on variable a
a[15] = 65		

# ERROR: a is not a class.	
a.a = 'a'		

# ERROR: A is not a class.	
A.a = 15

# good: a is declared as an integer.
a = 5

# ERROR: A is not a class.	
A.b = 16

# good: a is an integer.
a = 15 + 40

# ERROR: a is not an array.
a[15] = 65 / (3 - 2)

# ERROR: a is not a class. z and b are not defined.
a.a = 65 * a
a = 5 * 4 < z and true or b

# ERROR: A is not a class. z and b are not defined.
A.a = (15) and 4 
A.b = (5 * 4 < z) and (true and b)
