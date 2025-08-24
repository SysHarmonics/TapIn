#!/bin/bash 

# Ignore the src/main.c file
LINKER_FILES=$(find src -name "*.c" ! -name "main.c")
CFLAGS="-Wall -O2 -g -std=c11 -I src -I src/lib/colorize"
LDFLAGS="-lsodium -lpthread"

# For this to work it needs to be a .c file
SINGLE_TEST=$1

TEST_FILES=$(grep -l "main(" tests/test_*.c)

if [[ "$SINGLE_TEST" != "" ]]; then
  TEST_FILES="tests/${SINGLE_TEST}"
fi

# Now loop through all the test files that have a main
for TEST_FILE in $TEST_FILES; do
  printf "............START..............\n"
  printf "[+] Running %s\n" "$TEST_FILE"
  gcc "$TEST_FILE" $LINKER_FILES tests/test_helpers.c $CFLAGS $LDFLAGS -o tmp_test_runner 
  ./tmp_test_runner

  TEST_STATUS=$?

  if [ $TEST_STATUS -ne 0 ]; then
    echo "Since test failed exiting..."
  fi
  printf "............END................\n\n"
done

