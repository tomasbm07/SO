#!/bin/bash

FILE_NAME=$1
PROJECT=$2
NAME=$3

if [ $# -eq 0 ]
      then
            echo "No arguments supplied"
      else
            echo "// $3" >> $1.c
            echo "// Project: $2" >> $1.c
            echo "// ---------------------" >> $1.c

fi

