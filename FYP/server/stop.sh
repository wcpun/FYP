#!/bin/bash
fuser -k 1100/tcp
fuser -k 1110/tcp
fuser -k 1120/tcp
make clean
