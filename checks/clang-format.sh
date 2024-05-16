#!/bin/bash

PROJECT_ROOT="$(realpath "$(dirname "$0")/../")"

echo "Running clang-format on all files in '$PROJECT_ROOT'"
shopt -s globstar

pushd -n "$PROJECT_ROOT"
for f in **/*.h **/*.cpp; do
	echo
	echo "Checking file '$f'"
	clang-format "$f" > "$f.format" ; meld "$f" "$f.format"
	rm "$f.format"
done
popd -n
