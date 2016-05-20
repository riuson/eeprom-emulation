#!/bin/sh
PROJECT_DIR_PATH=$1

ASTYLE=astyle

if command -v $ASTYLE >/dev/null 2; then
  echo "AStyle detected."
else
  echo "AStyle not found. Try included astyle.exe."

  ASTYLE=$PROJECT_DIR_PATH/AStyle.exe

  if command -v $ASTYLE >/dev/null 2; then
    echo "AStyle detected at $ASTYLE. Try using it."
  else
    echo "AStyle not found. Aborting."
    exit 1;
  fi
fi

$ASTYLE --options=$PROJECT_DIR_PATH/astyle.conf "$PROJECT_DIR_PATH/src/*.c*"
$ASTYLE --options=$PROJECT_DIR_PATH/astyle.conf "$PROJECT_DIR_PATH/src/*.h"
$ASTYLE --options=$PROJECT_DIR_PATH/astyle.conf "$PROJECT_DIR_PATH/wrapper/*.cpp"
$ASTYLE --options=$PROJECT_DIR_PATH/astyle.conf "$PROJECT_DIR_PATH/wrapper/*.h"
