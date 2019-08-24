default: c

c:  check.c colony.c life.c
	gcc -std=c99 check.c -o check
	gcc -std=c99 colony.c -o colony
	gcc $(X) -std=c99 -O3 -pedantic -Wall life.c canvas.c player.c -o life -lm -lSDL2

test:
	./colony 0 | ./life | ./check 0
	./colony 1 | ./life | ./check 1
	./colony 2 | ./life | ./check 2
	./colony 3 | ./life | ./check 3
	./colony 4 | ./life | ./check 4
	./colony 5 | ./life | ./check 5
	./colony 6 | ./life | ./check 6

clean:
	rm -f *.hi *.o
	rm check colony life
