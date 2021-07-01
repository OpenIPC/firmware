#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include <unistd.h>             /* add by paulerr*/


#define CHIP			"/dev/i2c-0"
#define CHIP_ADDR		0x40			//HTU21D地址
#define READ_TEMP		0XE3			//温度转换命令
#define READ_HUM		0XE5			//湿度转换命令

/* More info: https://github.com/kingmacth/Openwet-SDK/tree/master/package/HTU21D  */

unsigned char HTU21D_Check_CRC(unsigned short int message_from_sensor,unsigned char check_value_from_sensor);

int main()

{
         printf("hello,this is HTU21D tester \n");
         int fd =open(CHIP, O_RDWR);
         if (fd< 0) {
                   printf("open"CHIP"failed \n");
                   goto exit;
         }

         if (ioctl(fd,I2C_SLAVE_FORCE, CHIP_ADDR) < 0) {           /*设置芯片地址   */
                   printf("oictl:setslave address failed\n");
                   goto close;
         }

	struct		i2c_msg msg;

	unsigned char      rddata[3];
	unsigned char      rdaddr[2] = {0x10, 0};     /* 将要读取的数据在芯片中的偏移量  */
	unsigned char      TempAddr[2] = {0xe3, 0};   /* 发送E3读取温度    */
	unsigned char      HumAddr[2] = {0xE5, 0};    /* 发送E5读取湿度    */
	unsigned short int tmp_dat,CRC_Result;
	float Temp,Hum;
         /*printf("inputa char you want to write to E2PROM\n");
         wrbuf[2]= getchar();
         printf("writereturn:%d, write data:%x\n", write(fd, wrbuf, 1), wrbuf[2]);
         sleep(5);*/
         //printf("Read temp!\n",write(fd, TempAddr, 1));      /* 开启温度转换    */
	write(fd, TempAddr, 1);
	 usleep(100000);
         read(fd, &rddata, 3);		//读取两字节温度一字节校验
         
	tmp_dat=rddata[0];
	tmp_dat=tmp_dat<<8;
	tmp_dat=tmp_dat+rddata[1];
	//printf("rddata:%x%02x,%x\n", rddata[0], rddata[1],rddata[2]);
	CRC_Result=HTU21D_Check_CRC(tmp_dat,rddata[2]);
	//printf("CRC=%x,%x\n",CRC_Result ,tmp_dat);//CRC校验
	if(CRC_Result==0)
	{
		Temp=(tmp_dat*175.72)/65536-46.85;
		printf("Temp=%.2f\n",Temp);
	}
	 //printf("Read hum!\n",write(fd, HumAddr, 1));      /* 开启湿度转换    */
	write(fd, HumAddr, 1);
	 usleep(100000);
         read(fd, &rddata, 3);		//读取两字节湿度一字节校验
         
	tmp_dat=rddata[0];
	tmp_dat=tmp_dat<<8;
	tmp_dat=tmp_dat+rddata[1];
	//printf("rddata:%x%02x,%x\n", rddata[0], rddata[1],rddata[2]);
	CRC_Result=HTU21D_Check_CRC(tmp_dat,rddata[2]);
	//printf("CRC=%x,%x\n",CRC_Result ,tmp_dat);//CRC校验
	if(CRC_Result==0)
	{
		Temp=(tmp_dat*125)/65536-6;
		printf("Hum=%.2f%%RH\n",Temp);
	}

close:
         close(fd);
exit:
         return 0;

}

//**************************************************************************************************************
//两个参数一个是传感器的源数据，另一个是传感器得出的CRC校验值
//如果校验通过返回0，不通过返回非0
//From: http://www.nongnu.org/avr-libc/user-manual/group__util__crc.html
//POLYNOMIAL = 0x0131 = x^8 + x^5 + x^4 + 1 : http://en.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks
#define SHIFTED_DIVISOR 0x988000 //This is the 0x0131 polynomial shifted to farthest left of three bytes
//**************************************************************************************************************
unsigned char HTU21D_Check_CRC(unsigned short int message_from_sensor,unsigned char check_value_from_sensor)
{
unsigned char i;
//Test cases from datasheet:
//message = 0xDC, checkvalue is 0x79
//message = 0x683A, checkvalue is 0x7C
//message = 0x4E85, checkvalue is 0x6B
unsigned int remainder = (unsigned int)message_from_sensor << 8; //Pad with 8 bits because we have to add in the check value
unsigned int divsor = (unsigned int)SHIFTED_DIVISOR;
remainder |= check_value_from_sensor; //Add on the check value
for (i = 0 ; i < 16 ; i++) //Operate on only 16 positions of max 24. The remaining 8 are our remainder and should be zero when we're done.
{
if( remainder & (unsigned int)1<<(23 - i) ) //Check if there is a one in the left position
remainder ^= divsor;
divsor>>=1;//Rotatethedivsormax16timessothatwehave8bitsleftofaremainder
}
return(unsigned char)remainder;
}
