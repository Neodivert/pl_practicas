#!/bin/bash
#Substitute " and ' so they do not mess with xargs
./"$1" "$2" | sed 's/"/\\"/g' | sed s/\'/\\\\\'/g | xargs ruby parser.rb
