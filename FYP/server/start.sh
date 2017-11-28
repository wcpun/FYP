#!/bin/bash
serv="./server";
if [ -e "$serv" ]; then
	./server 1100 &
	./server 1110 &
	./server 1120 &
else
	echo "$serv not found."
fi
