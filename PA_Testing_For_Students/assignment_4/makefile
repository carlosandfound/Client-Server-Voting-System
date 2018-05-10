CC=gcc
CFLAGS=-std=c99
DBFLAGS=-g

make: client server

client: client.c
	$(CC) $(CFLAGS) -pthread -o client client.c

server: server.c
	$(CC) $(CFLAGS) -pthread -o server server.c

clean:
	rm client server assignment_4.zip
	rm -rf ./PA_Testing_For_Students/assignment_4

build: make
	zip assignment_4.zip client.c server.c list.h map.h utils.h makefile README

test: clean build
	cp ./assignment_4.zip ./PA_Testing_For_Students/
	unzip ./PA_Testing_For_Students/assignment_4.zip -d ./PA_Testing_For_Students/assignment_4/
	cd ./PA_Testing_For_Students; python3 ./prepare_individual.py ./assignment_4/; python ./grade_individual.py ./assignment_4/
	echo ./PA_Testing_For_Students/assignment_4/GRADE.txt
