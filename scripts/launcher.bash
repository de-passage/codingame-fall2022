#!/usr/bin/env bash

java -jar "$HOME/workspace/cg-brutaltester/target/cg-brutaltester-1.0.0-SNAPSHOT.jar" \
  -r "java -jar /home/depassage/workspace/codingame-fall2022/ais/runner.jar" \
  "$@"
