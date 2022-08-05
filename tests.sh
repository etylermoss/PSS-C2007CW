#!/bin/env sh

FIRST_PASS=true

for file in "$@"
do
	if [ "$FIRST_PASS" = true ]; then
		echo -e "Testing $file with Valgrind\n"
	else
		echo -e "\nTesting $file with Valgrind\n"
	fi
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes "$file"

	sleep 3

	echo -e "\nTesting $file with Helgrind\n"
	valgrind --tool=helgrind "$file"

	FIRST_PASS=false
done

exit 0