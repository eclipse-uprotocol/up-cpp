#!/bin/bash
echo "Unit Test Coverage report generation.  Must compile and run ctest before running this."

mkdir ./build/Coverage

gcovr -r . --html --html-details -o ./build/Coverage/index.html -e '.*test.*' 

xdg-open ./build/Coverage/index.html
cd ..
echo "Coverage report can be found here: ./build/Coverage/index.html"

