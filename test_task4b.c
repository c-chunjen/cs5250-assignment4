#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
//needed for IO things. Attention that this is different from kernel mode

int lcd;

#define SCULL_MESSAGE_SIZE 256
#define SCULL_IOC_MAGIC 'k'
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC, 1)
#define SCULL_IOW _IOW(SCULL_IOC_MAGIC, 2, unsigned long)
#define SCULL_IOR _IOR(SCULL_IOC_MAGIC, 3, unsigned long)
#define SCULL_IOWR _IOWR(SCULL_IOC_MAGIC, 4, unsigned long)

void test()
{
	int k;
	char s[3];
	char to_dev_msg[SCULL_MESSAGE_SIZE], to_user_msg[SCULL_MESSAGE_SIZE], just_msg[SCULL_MESSAGE_SIZE];

	strcpy(to_dev_msg, "0");
	strcpy(to_user_msg, "0");
	strcpy(just_msg, "0");
	printf("initial values: to_dev_msg=\"%s\", to_user_msg=\"%s\", just_msg=\"%s\"\n",to_dev_msg ,to_user_msg ,just_msg);

	memset(s, '2', sizeof(s));
	printf("test begin!\n");

	k = write(lcd, s, sizeof(s));
	printf("written = %d\n", k);
	
	sprintf(to_dev_msg, "1 - Hello World, Hello Chiu Chun-Jen");
	k=ioctl(lcd, SCULL_IOW, to_dev_msg);
	printf("ioctl(lcd, SCULL_IOW, to_dev_msg):\tto_dev_msg=\"%s\", result=%d\n", to_dev_msg, k);
	k=ioctl(lcd, SCULL_IOR, to_user_msg);
	printf("ioctl(lcd, SCULL_IOR, to_user_msg):\tto_user_msg=\"%s\", result=%d\n", to_user_msg, k);
}

int main(int argc, char **argv)
{
	lcd = open("/dev/fourMegaBytes", O_RDWR);
	
	if (lcd == -1) {
		perror("unable to open lcd");
		exit(EXIT_FAILURE);
	}
	test();

	close(lcd);
	
	return 0;
}
