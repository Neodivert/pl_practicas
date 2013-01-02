###############################################################################
# Emerald - lexical test
# Variable identifiers
###############################################################################

# Global variables
###############################################################################
$a
$_a
$__a__1
$1			# ERROR: numbers not allowed at the beginning.
$abc_
$_

# Instance variables
###############################################################################
@1			# ERROR: numbers not allowed at the beginning.
@____
@abc
@_abc

# Constants
###############################################################################
ABC
_ABC 		# ERROR: '_' not allowed at the beginning. This will be read as a variable identifier.
A_B_C
