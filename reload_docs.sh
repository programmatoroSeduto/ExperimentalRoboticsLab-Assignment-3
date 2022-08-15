#! /bin/bash

make clean && make html
firefox _build/html/index.html
