#!/bin/bash

PROJECT_ROOT="$(realpath "$(dirname "$0")/../")"

if [ -n "$(which clang-tidy-13)" ]; then
    # NOTE: Using clang-tidy-13 in CI system, too
    LINTER=clang-tidy-13
elif [ -n "$(which clang-tidy)" ]; then
    echo "Did not find clang-tidy-13. Trying clang-tidy. Results may not"
    echo "match formatting in GitHub CI process."
    LINTER=clang-tidy
else
    echo "Could not find clang-tidy. Please make sure it is installed" 1>&2
    exit 2
fi

usage() {
	echo "$(basename "$0") path/to/compile_commands.json [source_to_lint]" 1>&2
	echo 1>&2
	echo "    compile_commands.json" 1>&2
	echo "        Produced during a cmake build when configured with the" 1>&2
	echo "        -DCMAKE_EXPORT_COMPILE_COMMANDS=yes flag" 1>&2
	echo 1>&2
	echo "    source_to_lint (optional)" 1>&2
	echo "        Source file to run clang-tidy against. If not specified," 1>&2
	echo "        all source files in the repo will be scanned." 1>&2
}

if [ "$1" == "-h" ] || [ "$1" == "--help" ] || [ "$1" == "/h" ] || [ "$1" == "/?" ]; then
	usage
	exit
fi

compile_database="$1"

if [ -z "$compile_database" ]; then
	echo "No compile database specified. Make sure cmake was configured" 1>&2
	echo "with '-DCMAKE_EXPORT_COMPILE_COMMANDS=yes' and re-run the build" 1>&2
	echo 1>&2
	echo "Usage:" 1>&2
	usage
	exit 1
elif [ ! -f "$compile_database" ]; then
	echo "Compile database file not found. Make sure cmake was configured" 1>&2
	echo "with '-DCMAKE_EXPORT_COMPILE_COMMANDS=yes' and re-run the build" 1>&2
	echo 1>&2
	echo "Usage:" 1>&2
	usage
	exit 1
fi

compile_database="$(realpath "$compile_database")"

target_source="$2"

if [ -z "$target_source" ]; then
	echo "Running $LINTER on all files in '$PROJECT_ROOT'"
	shopt -s globstar

	pushd "$PROJECT_ROOT" > /dev/null
	for f in include/**/*.h src/**/*.cpp; do # test/coverage/**/*.cpp test/extra/**/*.cpp test/include/**/*.h; do
		if [[ ! ("$f" =~ "build/") ]]; then
			echo
			echo "Checking file '$f'"
			$LINTER -p "$(dirname "$compile_database")" "$f"
		fi
	done
	popd > /dev/null
	exit
fi

if [ ! -f "$target_source" ]; then
	echo "Target source file '$target_source' not found." 1>&2
	echo 1>&2
	echo "Usage:" 1>&2
	usage
	exit 1
fi

echo "Running $LINTER on '$target_source'"
$LINTER -p "$(dirname "$compile_database")" "$target_source"
