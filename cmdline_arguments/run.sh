#!/bin/bash

g++ -std=c++17 task0.cpp -o a.out

echo 'Test #1'

echo '1 2 3 4 -1' | ./a.out 3 2 
echo $'\nans: 16'

echo $'\nTest #2'
echo '4 2 5 -1 1 0 7 9 2 0 -1 -3 -1 -2 0 0 -3 1 9 25 3 7 -4 -5 -6 1 3 5 -2 2 2 -1 -2 2 8 -2 0 1 3 5 5 3 -3 2 1 2 2 1 -3 -5 -3 4 1 1 3 -2' | ./a.out 5 12 8 5 3 7 6 4 6
echo $'\nans: -310500'
