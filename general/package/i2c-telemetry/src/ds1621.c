/* Test DS1621 sur interface I2C GPIO Fonera (3/2008) 					*/
/* Inspir de "Un bus i2c pour La Fonera"						*/
/* http://www.lefinnois.net/wp/index.php/2007/05/05/un-bus-i2c-pour-la-fonera/		*/

/* Affichage de la t									*/
/* Par domos domos78<at>free<point>fr 							*/
/* http://vesta.homelinux.net/								*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#define I2C_SLAVE       0x0703  	// Change slave address

static int i2c_fd ;
int res ;

#define DEVICE "/dev/i2c-0"
#define ds1621_addr 0x9E >> 1		// 0x4f - Address (A0+A1+A2 to Vcc)


/*------------------------------------------------------------------------------*/
/* Fonctions i2c 								*/
/*------------------------------------------------------------------------------*/
void i2c_init()
{
	if ((i2c_fd = open(DEVICE, O_RDWR)) < 0) 
	{
		fprintf(stderr, "Erreur ouverture port: %s (%d)\n", strerror(errno), errno);
		exit(EXIT_FAILURE);
	}
}

//-----------------------------------------------------------------------------
void SelectSlave(unsigned char slaveaddr)
{
	if (ioctl(i2c_fd, I2C_SLAVE, slaveaddr) < 0) 
	{
		fprintf(stderr, "Erreur selection esclave i2c: %s (%d)\n", strerror(errno), errno);
		close ( i2c_fd ) ;
		exit(EXIT_FAILURE) ;
	} 
}

/*------------------------------------------------------------------------------*/
/* Fonctions DS1621 								*/
/*------------------------------------------------------------------------------*/
int i2c_init_ds1621(unsigned char addr)
{
	int res ;
	char buff[10] ;
	
	SelectSlave(addr) ;
  	buff[0] = 0xAC ;
  	buff[1] = 0x01 ;
 	res = write( i2c_fd, buff, 2 );
	if ( res < 0 )
	{
		printf("Erreur init DS1621 addr 0x%x: '%s', Abandon programme !", addr, strerror(errno));
		close ( i2c_fd );
		exit(1) ;
	}
  	/* start temperature conversion */
	errno = 0 ;	
  	buff[0] = 0xEE ;
 	write( i2c_fd, buff, 1 );
	sleep(1) ;
	return 1 ;
}

//-----------------------------------------------------------------------------
double i2c_gettemp_ds1621(unsigned char addr)
{
	int k, count, slope, temp;
	char buff[10] ;
	
	SelectSlave(addr) ;
	/* stop conversion */
	errno = 0 ;	
	buff[0] = 0x22 ;

	if ( write( i2c_fd, buff, 1 ) < 0 ) return 255 ; // Write retourne -1 et strerror(errno))='Remote I/O error' si adr. i2c non connecte.
	else
	{
		/* Temperature reading (1 Celsius degree precision) */
		buff[0] = 0xAA ;
		write( i2c_fd, buff, 1 );
		read(i2c_fd, buff, 1) ;
		temp = buff[0] ;
		/* Counter reading (fraction of degree) ) */
		buff[0] = 0xA8 ;
		write( i2c_fd, buff, 1 );
		read(i2c_fd, buff, 1) ;
		count = buff[0] ;
		/* slope reading */
		buff[0] = 0xA9 ;
		write( i2c_fd, buff, 1 );
		read(i2c_fd, buff, 1) ;
		slope = buff[0] ;
		k = temp;
		if (slope != 0) 
		{
			k = temp*100-25+(100*(slope-count))/slope;
		}
	  	/* start temperature conversion */
		buff[0] = 0xEE ;
		write( i2c_fd, buff, 1 );
		return (float)k/100 ;
	}
}

/*------------------------------------------------------------------------------*/
int main ( int argc, char ** argv )
{
	// Init i2c.
	i2c_init() ;

	// Init ds1621.
	i2c_init_ds1621(ds1621_addr) ;

	// Affiche la t.
	printf("%2.1f\n", i2c_gettemp_ds1621(ds1621_addr) ) ;

	close ( i2c_fd );
}

/*------------------------------------------------------------------------------*/

