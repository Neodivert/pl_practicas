###############################################################################
# Emerald - lexical test
# String and char literals
###############################################################################

# Escape sequences
###############################################################################
"\t '  \\\" \" \n  \\n" 

# Strings
###############################################################################
"a"
"abc"

# ERROR: unclosed string.
"a 
# Error: unopen string.		
a"		

"a#{a}"
# ERROR: Unclosed interpolation.
"#{"		
"#{
# ERROR: only simple variables allowed in interpolation.
"#{a[1]}"
"#{a} \" dfd \t #{b}"
# ERROR: illegal escape sequences (\u,\q).
"3 df} \u#\\ {\q"

# Characters
###############################################################################
'a'
# ERROR: Unclosed character.
'a
''
# ERROR: Unopen character
a'
# ERROR: Undefined escape sequence.
'\'
'\n'
'\s'
# ERROR: Only one character allowed.
'aa'
# ERROR: Only one character allowed.
'\nk'
'\''
'"'
