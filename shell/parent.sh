#!/bin/bash 
# 
# Show how simple vars in a parent could be modified 
# in a child 

set -x 

parent_var="parentName"

echo "Value in parent_var ${parent_var}" 

. $PWD/child.sh  

echo "Value parent_var in parent ${parent_var}" 

