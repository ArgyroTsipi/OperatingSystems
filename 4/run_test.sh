#!/bin/bash

# Author: Aggelos Kolaitis
# File: run_test.sh
# Usage: ./run_test.sh

if [[ $(sha256sum tests.c) != "f01cfb9cee05599822fea130ec797b76ed541639d01adfa4d41950778b677ca6  tests.c" ]]; then
  echo "The tests.c file has been tampered. This is not allowed."
  echo "Revert any changes you made in tests.c, the re-run:"
  echo ""
  echo "    make test"
  exit 1
fi

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD

FAIL=0

for test_name in test_single_process test_multi_process test_many_signals test_liveness test_loop test_wait_without_signal test_wait_without_enough_signal; do
  printf "./tests %-32s... " $test_name
  timeout 5s ./tests $test_name >/dev/null 2>/dev/null
  RET=$?
  if [ $RET == 0 ]; then
    echo "SUCCESS"
  elif [ $RET == 124 ]; then
    echo "FAILED (TIMED OUT)"
    FAIL=1
  else
    echo "FAILED"
    FAIL=1
  fi
done

if [[ $FAIL == 1 ]]; then
  echo ""
  echo "One or more tests failed. Lookup the test descriptions in tests.c"
  echo "and update your semaphore implementation accordingly, then re-run:"
  echo ""
  echo "    make test"
fi
