#!/bin/bash

test_binary=$1

if [ ! -x "$test_binary" ]; then
	if [ -e "$test_binary" ]; then
		echo "Specified test binary is not executable" 1>&2
	else
		echo "Specified test binary does not exist" 1>&2
	fi
fi

echo "LEAK CHECK" 1>&2
echo "==========" 1>&2
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$test_binary --gtest_brief=1 > /dev/null
echo 1>&2

echo "DRD" 1>&2
echo "===" 1>&2
valgrind --tool=drd ./$test_binary --gtest_brief=1 > /dev/null
echo 1>&2

echo "HELGRIND" 1>&2
echo "========" 1>&2
valgrind --tool=helgrind ./$test_binary --gtest_brief=1 > /dev/null
echo 1>&2

echo "DHAT" 1>&2
echo "====" 1>&2
valgrind --tool=dhat ./$test_binary --gtest_brief=1 > /dev/null
