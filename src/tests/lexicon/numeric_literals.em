###############################################################################
# Emerald - lexical test
# Numeric literals
###############################################################################

# Integers
###############################################################################
+1
+2
566
451
45a	# ERROR: not letters allowed.
-5
-99
-45
-65

# Floats
###############################################################################
1.
.1
01.01
1.1.2		# ERROR: more than one '.'
0.3e		# ERROR: not exponent specified.
0.3E		# ERROR: not exponent specified.
0.3e1.3	# ERROR: not float allowed in the exponent.
0.3e25
0.3E25
.3e25
.03e25
1.e25

