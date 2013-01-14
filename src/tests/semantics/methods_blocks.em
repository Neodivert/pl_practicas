###############################################################################
# Emerald - semantic test
# Random method that accepts an array as an argument.
# Language's tested features: passing an array to a method / block, nested 
# blocks.
###############################################################################

# Random method, don't try to find any logic in it.
def method ( array, arg2 )
	d = [1, 2, 3]
	array.each do |i|
		d = 5		# ERROR: d is an array.
		d.each { |j|
			d = 5.4 # ERROR: d is an array.
			h = d
		}	
	end
	b = arg2
end

# Array creation and passing to a method.
c = Array.new( 2, 1.1 )
method( c, 1.1 )
