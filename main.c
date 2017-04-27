#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>

#include "gpio.h"

#define WHICH_BIT_ON(x) (x & (1 << 3))?4:(x & (1 << 2))?3:(x & (1 << 1))?2:(x & 1)?1:0
#define I2C_DEVICE_ADDR         0x20

static const char KEYS[][5] = {{'\0', '\0', '\0', '\0'}, 
							  {'\0', '1', '2', '3', 'A'}, 
							  {'\0', '4', '5', '6', 'B'}, 
							  {'\0', '7', '8', '9', 'C'}, 
							  {'\0', '*', '0', '#', 'D'}};

int main(void)
{
    int fd;
	uint8_t cmd, row, col, state;
    
    fd = open("/dev/i2c-1", O_RDWR|O_SYNC);
    if(fd < 0){
        printf("Error opening file: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    if(ioctl(fd, I2C_SLAVE, I2C_DEVICE_ADDR) < 0){
        printf("ioctl error");
        return EXIT_FAILURE;
    }

	setup_gpio(4, GPIO_IN);

	while(1){
		while((state = gpio_read(4)) == 1);


		//Probing rows
		cmd = 0x0F;
		write(fd, &cmd, 1);
		usleep(1000);	
		read(fd, &row, 1);
		usleep(1000);
		row = ~row;
		row = row & 0x0F;
		row = WHICH_BIT_ON(row);
		
		//Probing cols
		cmd = 0xF0;
		write(fd, &cmd, 1);
		usleep(1000);	
		read(fd, &col, 1);
		usleep(1000);
		col = ~col;
		col = col & 0xF0;
		col = col >> 4;
		col = WHICH_BIT_ON(col);

		//col = 0;

		//printf("0x%02X\n", row);
		//printf("0x%02X\n", col);
		//row = ~row;
		printf("%c\n", KEYS[row][col]);

		usleep(1000);
	}

	close(fd);

	return 0;
}
