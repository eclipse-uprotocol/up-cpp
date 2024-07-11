#!/bin/bash

PROJECT_ROOT="$(realpath "$(dirname "$0")/../")"

if [ -n "$(which clang-tidy-12)" ]; then
    # NOTE: Using clang-tidy-12 in CI system, too
    LINTER=clang-tidy-12
elif [ -n "$(which clang-tidy)" ]; then
    echo "Did not find clang-tidy-12. Trying clang-tidy. Results may not"
    echo "match formatting in GitHub CI process."
    LINTER=clang-tidy
else
    echo "Could not find clang-tidy. Please make sure it is installed" 1>&2
    exit 2
fi

compile_database="$1"

if [ -z "$compile_database" ]; then
	echo "No compile database specified. Make sure cmake was configured" 1>&2
	echo "with '-DCMAKE_EXPORT_COMPILE_COMMANDS=yes' and re-run the build" 1>&2
	exit 1
elif [ ! -f "$compile_database" ]; then
	echo "Compile database file not found. Make sure cmake was configured" 1>&2
	echo "with '-DCMAKE_EXPORT_COMPILE_COMMANDS=yes' and re-run the build" 1>&2
	exit 1
fi

compile_database="$(realpath "$compile_database")"

echo "Running $LINTER on all files in '$PROJECT_ROOT'"
shopt -s globstar

pushd "$PROJECT_ROOT" > /dev/null
for f in **/*.h **/*.cpp; do
	if [[ ! ("$f" =~ "build/") ]]; then
		echo
		echo "Checking file '$f'"
		$LINTER -p "$(dirname "$compile_database")" "$f"
	fi
done
popd > /dev/null
