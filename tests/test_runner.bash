#!/bin/bash 

# Ignore the src/main.c file
linker_files=`find src -name "*.c" ! -name "main.c"`
CFLAGS="-Wall -O2 -g -std=c11 -Isrc"
LDFLAGS="-lsodium -lpthread"

# Now loop through all the test files that have a main
for test_file in `grep -l "main(" tests/test_*.c`; do
  printf "............START..............\n"
  printf "[+] Running %s\n" $test_file
  gcc $test_file $linker_files tests/test_helpers.c $CFLAGS $LDFLAGS -o tmp_test_runner 
  ./tmp_test_runner

  test_status=$?

  if [ $test_status -ne 0 ]; then
    echo "Since test failed exiting..."
  fi
  printf "............END................\n\n"
done
