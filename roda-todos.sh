#!/bin/bash
echo "USAGE: ./rodaTodos.sh <nElements> <k>"
    
for i in {1..8}
do
    echo "Executando 10 vezes para achar os $2 menores entre $1 elementos e $i threads:"
    for j in {1..10}
    do
        ./acharKMenores $1 $2 $i | grep -oP '(?<=total_time_in_seconds: )[^ ]*'
    done
done





