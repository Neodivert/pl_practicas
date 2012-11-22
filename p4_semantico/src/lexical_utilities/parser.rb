tokens = ['INTEGER',
'FLOAT',
'CHAR',
'ID_GLOBAL_VARIABLE',
'ID_INSTANCE_VARIABLE',
'ID_CONSTANT',
'IDENTIF',
'DEF',
'END',
'IF',
'THEN',
'ELSE',
'WHILE',
'DO',
'CLASS',
'AND',
'OR',
'BEGIN_COMPLEX_STRING',
'END_COMPLEX_STRING',
'START_STRUCT',
'END_STRUCT',
'SUBSTRING',
'END_LINE',
'NOT_EQUAL',
'EQUAL_EQUAL',
'LESS_EQUAL',
'GREATER_EQUAL',
'NIL',
'BOOL',
'SEC_SCAPE',
'NOT',
'EACH',
'NEW',
'ARRAY']

ARGV.each do |token|
  token_i = token.to_i 
  if token_i > 256
    puts tokens[token_i - 257]
  else
		if token_i != 0
			puts token_i.chr 
		else
			puts token
		end   
  end 
end
