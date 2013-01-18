###############################################################################
# Emerald - semantic test
# Iterate over the elements of an array and show them.
# Language's tested features: block calling, arrays.
###############################################################################

v = ['a', 'b', 'c', 'd']
$w = [1,2,3]
x = [1.1, 2.2, 3.3]

v.each do |i|
	puts( "#{i}\n" )
	$w.each do |j|
		puts("\t#{j}\n")
		x.each do |k|
			puts("\t\t#{k}\n")
		end
	end		
end

