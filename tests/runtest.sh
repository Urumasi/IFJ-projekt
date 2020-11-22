#!bin/bash

TESTS=0

while getopts "hps" opt; do
    case "${opt}" in
		s)
            TESTS=1
            ;;
        p)
            echo "parser"
            ;;
        h)
			echo "Usage: -s Test scanner, -p Test parser or nothing to run all tests" ; exit 0
            ;;
    esac
done
shift $((OPTIND-1))

if [ "$#" -ne "0" ]; then
	echo "Usage: -a: Run all tests, -s Test scanner, -p Test parser" >&2 ; exit 1
fi

test_scanner(){
	gcc print_token.c ../src/str.c ../src/scanner.c -o print_token
	count=$(ls -d expected.output* | wc -l)

	for i in $(eval echo "{1..$count}")
	do
		./print_token test"$i".txt > output"$i"
	
		DIFF=$(diff output"$i" expected.output"$i")
		if [ "$DIFF" == "" ]; then
			    echo "Test"$i" OK"
		else
			echo "Test"$i" FAILED"
			echo "Test"$" diff:" > Test.log
			echo "$DIFF" > Test.log
		fi

		rm output*
	done

	rm print_token 
}

if [ "$TESTS" -eq "0" ]; then
	test_scanner
elif [ "$TESTS" -eq "1" ]; then
	test_scanner
fi

#gcc test_parser.c ../src/parser.c ../src/scanner.c -o test_parser
#test_parser
