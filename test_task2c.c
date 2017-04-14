#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define DEVICE_SIZE 4*1024*1024
#define TEST_SIZE 5*1024*1024

int lcd, file;

int test()
{
	int k;
	char s[TEST_SIZE];
	
	k = 0;
	memset(s, 'T', TEST_SIZE);
	memset(s, '9', TEST_SIZE-10);
	memset(s, '1', DEVICE_SIZE);
	memset(s, '0', DEVICE_SIZE-1);
	memset(s, 'H', 10);

	write(file, s, TEST_SIZE);
	k = write(lcd, s, TEST_SIZE);  

	printf("TEST_SIZE=%d\n", TEST_SIZE);
	printf("actual size written to device: %d\n", k);
 	
}

int main(int argc, char **argv)
{
	lcd = open("/dev/fourMegaBytes", O_RDWR);
	file = open("/home/chiuchunjen/cs5250/assignment4/5mbfile", O_RDWR);
	
	if (lcd == -1) {
		perror("unable to open lcd");
		exit(EXIT_FAILURE);
	}
	test();

	close(file);
	close(lcd);
	
	return 0;
}
