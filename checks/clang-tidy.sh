#!/bin/bash

PROJECT_ROOT="$(realpath "$(dirname "$0")/../")"

echo "Running clang-tidy on all files in '$PROJECT_ROOT'"
shopt -s globstar

pushd -n "$PROJECT_ROOT"
for f in **/*.h **/*.cpp; do
	echo
	echo "Checking file '$f'"
	clang-tidy "$f" -- -I"include" -std=c++17 -x c++ > "$f.tidy" || vim -o "$f" "$f.tidy"
	rm "$f.tidy"
done
popd -n
