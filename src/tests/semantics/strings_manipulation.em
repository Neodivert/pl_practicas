string = "abcdefg"
i = 0

while( i < string.length ) do
	var = string[i]
	puts "Quito\t #{var}"
	string[i] = 48 + i
	var = string[i]
	puts "Y pongo\t #{var}"
	i = i + 1
end
