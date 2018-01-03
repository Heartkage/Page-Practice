all: main

main: project6_0416250.c
	gcc project6_0416250.c -lm -o main

clean:
	rm -f main 0416250_address.txt 0416250_value.txt

