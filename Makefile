TARGET=mybarrier.o
CFLAGS=-Wall -pthread -D PIPE_BUF_MAX=1024
CC=gcc

all: $(TARGET)
	
mybarrier.o: mybarrier.c mybarrier.h
	$(CC) $(CFLAGS) -c $*.c
<<<<<<< HEAD

=======
	
>>>>>>> ddd7f05bc5f35db669a3bca20d2c59f27002315c

clean:
	rm -rf *.o *.a $(TARGET)
