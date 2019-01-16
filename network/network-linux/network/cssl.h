/***************************************************************************
                          cssl.h  -  description
                             -------------------
    begin                : Áù  4ÔÂ 17 2010
    copyright            : (C) 2010 by root
    email                : root@localhost.localdomain
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 /* Copyright 2003 Marcin Siennicki <m.siennicki@cloos.pl>
 * see COPYING file for details */

#ifndef __CSSL_H__
#define __CSSL_H__

#include <stdint.h>
#include <signal.h>
#include <termios.h>


typedef void (*cssl_callback_t)(int id,  /* id passed to callback */
				uint8_t *buffer, /* data received */
				int len); /* length of data in bytes */

typedef struct __cssl_t {

    uint8_t buffer[255];      /* input buffer */

    int fd;                   /* tty file descriptor */

    struct termios tio;       /* termios structure for the port */
    struct termios oldtio;    /* old termios structure */

    cssl_callback_t callback; /* callback function */

    int id;                   /* id which would be passed to callback */

    struct __cssl_t *next;

} cssl_t;

typedef enum {
    CSSL_OK,                 /* everything is all right */
    CSSL_ERROR_NOSIGNAL,     /* there's no free signal */
    CSSL_ERROR_NOTSTARTED,   /* you should first start cssl */
    CSSL_ERROR_NULLPOINTER,  /* you gave a null pointer to the function */
    CSSL_ERROR_OOPS,         /* internal error, something's erong */
    CSSL_ERROR_MEMORY,       /* there's no memory for cssl_t structure */
    CSSL_ERROR_OPEN          /* file doesnt exist or you aren't good user */
} cssl_error_t;

/* get the error message */
const char *cssl_geterrormsg();

/* get the error code */
int cssl_geterror();

/* start the cssl */
void cssl_start();

/* finish all jobs, clear memory, etc. */
void cssl_stop();

/* alloc new cssl_t struct and open the port */
cssl_t *cssl_open(const char *fname, /* pathname of port file,
				      * for example "/dev/ttyS0" */
		  cssl_callback_t callback, /* callback function
					     * If you dont want
					     * event driven reading - set
					     * it to NULL */
		  int id,     /* your own id for the port, it can help
			       * to identify the port in callback f.*/
		  int baud,   /* baudrate, integer, for example 19200 */
		  int bits,   /* data bits: 7 or 8 */
		  int parity, /* parity: 0 - none, 1-odd, 2-even */
		  int stop);  /* stop bits: 1 or 2 */

/* closes the port, and frees its cssl_t struct */
void cssl_close(cssl_t *serial);

/* setups the port, look at cssl_open */
void cssl_setup(cssl_t *serial,
		   int baud,
		   int bits,
		   int parity,
		   int stop);

void cssl_setflowcontrol(cssl_t *serial,
			 int rtscts,   /* Boolean:
					* 0 - no rts/cts control,
					* 1 - rts/cts control
					*/
			 int xonxoff); /* Boolean:
					* 0 - no xon/xoff,
					* 1 - xon/xoff
					*/

/* sends a char via serial port */
void cssl_putchar(cssl_t *serial,
		     char c);

/* sends a null terminated string */
void cssl_putstring(cssl_t *serial,
		       char *str);

/* sends a data of known size */
void cssl_putdata(cssl_t *serial,
		  uint8_t *data, /* data */
		  int datalen);  /* length of data */

/* waits until all data has been transmited */

void cssl_drain(cssl_t *serial);

/*======================================
 * Blocking mode
 */

/* Sets port timeout (deciseconds) in blocking mode */
void cssl_settimeout(cssl_t *serial, int timeout);

/* reads a char in blocking mode */
int cssl_getchar(cssl_t *serial);

/* reads a data to a buffer in blocking mode*/
int cssl_getdata(cssl_t *serial,
		 uint8_t *buffer,  /* buffer for data */
		 int size);        /* buffer size */


#endif /* __CSSL_H__ */
