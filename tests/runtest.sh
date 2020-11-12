#!bin/bash

gcc print_token.c ../src/str.c ../src/scanner.c -o print_token

count=$(ls -d expected.output* | wc -l)

for i in $(eval echo "{1..$count}")
do
	./print_token test"$i".txt > output"$i"
	
	DIFF=$(diff output"$i" expected.output"$i")
	if [ "$DIFF" == "" ] 
	then
		    echo "Test"$i" OK"
	else
		echo "Test"$i" FAILED"
		echo "Test"$" diff:" > Test.log
		echo "$DIFF" > Test.log
	fi

	rm output*
done

rm print_token
