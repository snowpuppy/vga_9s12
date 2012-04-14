#!/bin/bash

for i in ./*.png
do
    ./convert.py $i
done
