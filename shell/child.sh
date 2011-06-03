#!/bin/bash 
# Modify the parent variable here 
#
echo "executing $1" ; 
child_id=$$

# Append the childpid to parent variable 
parent_var=${parent_var}_${child_id}	  

echo "In child loc: $parent_var " 

