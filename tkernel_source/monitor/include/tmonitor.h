/*
 *----------------------------------------------------------------------
 *    T-Kernel 2.0 Software Package
 *
 *    Copyright 2011 by Ken Sakamura.
 *    This software is distributed under the latest version of T-License 2.x.
 *----------------------------------------------------------------------
 *
 *    Released by T-Engine Forum(http://www.t-engine.org/) at 2011/05/17.
 *    Modified by T-Engine Forum at 2013/03/01.
 *    Modified by TRON Forum(http://www.tron.org/) at 2015/06/01.
 *
 *----------------------------------------------------------------------
 * it seems use printk or printf is more friendly than these DSP_* macros.
 * remove DSP_* macros.
 * Copyright 2014-2016 Du Huanpeng <u74147@gmail.com>
 */

/*
 *	tmonitor.h
 *
 *       T-Monitor internal common definitions
 */

#ifndef __MONITOR_TMONITOR_H__
#define	__MONITOR_TMONITOR_H__

#include <basic.h>
#include <tk/errno.h>
#include "device.h"

/* CPU-dependent definitions */
#ifdef __ARM__
#  include <asm/cpudepend.h>
#endif

/*
 * error code
 *       common error codes with T-Kernel (the following is used)
 *	E_OK, E_MACV, E_PAR, E_LIMIT, E_NOSPT, E_NOEXS, E_IO, E_RONLY
 */
        /* original error code */
#define	E_END		(-1000)
#define	E_LESS		(-1001)
#define	E_CMD		(-1002)
#define	E_RANGE		(-1003)
#define	E_EMPTY		(-1004)
#define	E_ILREG		(-1005)
#define	E_PC		(-1006)
#define	E_BOOT		(-1007)
#define	E_ROM		(-1008)
#define	E_PROTECT	(-1009)
        /* LOAD command and friends */
#define	E_PROTO		(-1010)
#define	E_NOADDR	(-1011)
#define	E_LOADFMT	(-1012)
#define	E_LOAD		(-1013)
#define	E_CANCEL	(-1014)
#define	E_XMODEM	(-1015)
        /* BREAK command and friends */
#define	E_BPATR		(-1020)
#define	E_BPBAD		(-1021)
#define	E_BPDSLT	(-1022)
#define	E_BPROM		(-1023)
#define	E_BPCMD		(-1024)
#define	E_BPUDF		(-1025)
#define	E_HBPOVR	(-1026)
#define	E_SBPOVR	(-1027)

#define	E_ONOEXS	(-1030)

IMPORT char const Version[];		/* version number */
IMPORT char const * const Title[];	 /* boot message */

/* ------------------------------------------------------------------------ */

/*
 * service call function code
 */
#define	TM_MONITOR	0
#define	TM_GETCHAR	(TM_MONITOR + 1)
#define	TM_PUTCHAR	(TM_MONITOR + 2)
#define	TM_GETLINE	(TM_MONITOR + 3)
#define	TM_PUTSTRING	(TM_MONITOR + 4)
#define	TM_COMMAND	(TM_MONITOR + 5)
#define	TM_READDISK	(TM_MONITOR + 6)
#define	TM_WRITEDISK	(TM_MONITOR + 7)
#define	TM_INFODISK	(TM_MONITOR + 8)
#define	TM_EXIT		(TM_MONITOR + 9)

#define	TM_EXTSVC	255

/* ======================================================================== */
/*
 *       hardware dependent processing (hwdepend)function
 */

/*
 * system basic set up
 *       boot    1 : boot is in progress
 *               0 : reset is in progress
 */
IMPORT void resetSystem( W boot );

/*
 * system termination
 *       reset    0 : power off
 *               -1 : reboot
 *               other: machine-dependent
 */
IMPORT void sysExit( W reset );

/*
 * processing at monitor entry and exit
 *       info and return value is defined in machine-dependent manner.
 */
IMPORT W enterMonitor( UW info );	/* entry */
IMPORT W leaveMonitor( UW info );	/* exit */

/*
 * EIT processing
 *       return value      0 : monitor should keep on running
 *                         1 : return from the interrupt handler
 */
IMPORT W procEIT( UW vec );

/*
 * Obtain boot selection information
 */
IMPORT W bootSelect( void );

/* bootSelect() return value */
#define	BS_MONITOR	0	/* boot monitor */
#define	BS_AUTO		1	/* automatic boot */

/*
 * boot device following the standard boot order
 *       return the device name that is the 'no'-th device in the standard boot order.
 *
 *       if no such device name exists (when 'no' is given as a value larger or equal to the last number), it is NULL.
 */
IMPORT const UB* bootDevice( W no );

IMPORT const UH BootSignature;	/* boot block signature */
IMPORT UB* const PBootAddr;	/* primary boot loader address */

/*
 * list of disk drives
 *       returns the disk drive device name, indicated by 'no' ( 0 - : a consecutive number )
 *       if no such device name exists (when 'no' is given as a value larger or equal to the last number), it is NULL.
 *       if attr is not NULL, disk driver attribute returns in `attr' )
 */
IMPORT const UB* diskList( W no, UW *attr );

/*
 * obtain switch status
 */
IMPORT UW getDipSw( void );

/*
 * set LED
 *       lower 16 bits of val value (1:ON 0:OFF)
 *       upper 16 bits of val mask (1: keep, 0: change)
 */
IMPORT void cpuLED( UW val );

/*
 * micro wait for a small amount of time
 *       wait time is not that accurate.
 */
IMPORT void waitMsec( UW msec );	/* milliseconds */
IMPORT void waitUsec( UW usec );	/* microseconds */
IMPORT void waitNsec( UW nsec );	/* nanoseconds */

/*
 *       cache control
 *       acts on the whole address space.
 */
IMPORT void FlushCache( void );		/* writeback and invalidate */
IMPORT void EnableCache( void );	/* enable cache */
IMPORT void DisableCache( void );	/* disable cache */

/* ------------------------------------------------------------------------ */

/*
 *       memory region definition
 *       the location of end is NOT included in the region. ((end - top) is the region size)
 *       end = 0x00000000, by the way, means 0x100000000.
 */
typedef struct {
	UW	top;		/* area start address */
	UW	end;		/* area end address */
	UW	attr;		/* attribute */
#if CPU_ARM && VIRTUAL_ADDRESS
	UW	pa;		/* physical address | page attribute */
#endif
} MEMSEG;

/*
 * attribute attr
 *       if MSA_WRK is defined, make it so that it is found before MSA_OS.
 */
#define	MSA_ROM		(1<<0)	/* ROM       (read-only) */
#define	MSA_FROM	(1<<1)	/* Flash ROM (write_enabled) */
#define	MSA_RAM		(1<<2) 	/* RAM */
#define	MSA_ERAM	(1<<3) 	/* extended RAM */
#define	MSA_IO		(1<<4) 	/* I/O */
#define	MSA_SRAM	(1<<5) 	/* SRAM */
#define	MSA_HW		(0xFFF)	/* attribute related to hardware */

#define	MSA_MON		(1<<12) /* monitor area        (area inside MSA_ROM/FROM ) */
#define	MSA_OS		(1<<13) /* OS area        (area in MSA_RAM ) */
#define	MSA_WRK		(1<<14) /* special work area (used by LH7A400) */

#define	MSA_RDA		(1<<16)	/* ROM disk area (area in MSA_FROM ) */
#define	MSA_RDB		(1<<17)	/* RAM disk area */

/* page attribute (ARM) 1st level page table */
#ifdef __ARM__

#define	PGA_RW		0x00402	/* Kernel/RW (effective section, AP0='1') */
#define	PGA_RO		0x08402	/* Kernel/RO (effective section) AP0='1') */
#define	PGA_XN		0x00010	/* code execution prohibited */
#define	PGA_C		0x0100c	/* TEX0:C:B='111', Normal, WB/WA  */
#define	PGA_NC		0x01000	/* TEX0:C:B='100', Normal, CacheOff */
#define	PGA_D		0x00004	/* TEX0:C:B='001', Device, CacheOff */
#define	PGA_S		0x10000	/* shareable */

#endif

/*
 * obtaining memory region information
 *       no = 1 - (and up)
 *       'no'-th information in the region specified by the attr is returned.
 *       if attr = 0, no matter what the attribute is, 'no'-th information is returned unconditionally.
 *       if no such information is found, return NULL.
 */
IMPORT MEMSEG* MemArea( UW attr, W no );

/*
 * obtaining memory region information (specify address)
 *       within the region specified by `attr', return the information that surrounds the position specified by `addr'.
 *
 *       if no such information is found, return NULL.
 */
IMPORT MEMSEG* AddrMatchMemArea( UW addr, UW attr );

/*
 * Decide whether two memory regions are included in another.
 *      if the region, from `top' to `end', is completely included in the region specified by `attr',
 *      TRUE
 *       the location of end is NOT included in the region (end - top) is the region size
 *       end = 0x00000000, by the way, means 0x100000000.
 */
IMPORT BOOL inMemArea( UW top, UW end, UW attr );

/*
 * Decide whether two memory regions overlap with each other
 *       if the area, from top to end, is included even partially in the region specified by `attr' - 'end',
 *       it is TRUE
 *       the location of end is NOT included in the region. ((end - top) is the region size)
 *       end = 0x00000000, by the way, means 0x100000000.
 */
IMPORT BOOL isOverlapMemArea( UW top, UW end, UW attr );

/* ======================================================================== */
/*
 *       command / SVC processing (cmdsvc) function
 */

/*
 * console output
 *       XON/XOFF flow control
 *       check for CTRL-C input
 *       return value       0 : normal
 *                         -1 : CTRL-C input exists
 */
IMPORT W putchar( W c );
IMPORT W puts( const UB *str );

/*
 * console output (hexadecimal: 2, 4, or 8 columns)
 *       XON/XOFF flow control
 *       check for CTRL-C input
 *       return value       0 : normal
 *                         -1 : CTRL-C input exists
 */
/*
 * console output (decimal: 10 columns/zero-suppress supported)
 *       XON/XOFF flow control
 *       check for CTRL-C input
 *       return value       0 : normal
 *                         -1 : CTRL-C input exists
 */
 /*
 * console display
 */
/*
 * use printk instead most of console output routines.
 */
IMPORT int printf(const char *format, ...);
IMPORT int printk(const char *format, ...);


/*
 * console input (one character)
 *       if wait = TRUE, wait for input if FALSE, do not wait.
 *       return value       >= 0 : character
 *                            -1 : no input
 */
IMPORT W getChar( BOOL wait );

/*
 * console input (character string)
 *       line input with editing
 *       return value      >= 0 : number of input characters
 *                           -1 : CTRL-C was detected
 */
IMPORT W getString( UB *str );

/*
 * detect CTRL-C
 *       check if there is a history of control-C input to the console
 *       history is cleared
 *      return value      TRUE  : CTRL-C input exists
 *                        FALSE : CTRL-C input is absent
 */
IMPORT BOOL checkAbort( void );

/*
 *       memory & I/O access
 *       len     number of bytes
 *       unit    access unit (1=B 2=H 4=W)
 *       return value number of bytes really accessed
 *                    return 0 when there was an error or exception
 *
 *       writeMem() becomes fill operation if (unit | 0x10) is given. (only writeMem)
 *       in this case, the only leading data in buf is used.
 */
IMPORT W readMem( UW addr, void *buf, W len, W unit );
IMPORT W writeMem( UW addr, void *buf, W len, W unit );
IMPORT W readIO( UW addr, UW *data, W unit );
IMPORT W writeIO( UW addr, UW data, W unit );

/*
 * read character string
 *       read byte string up to maximum len bytes from addr to buf.
 *       return the length read (excluding the terminating '\0').
 *       if an error occurs (including the string longer than len bytes), return -1.
 */
IMPORT W readMemStr( UW addr, void *buf, W len );

/*
 * initialize breakpoint
 */
IMPORT void initBreak( void );

/*
 * Invoking user reset initialization routine
 */
IMPORT void callUserResetInit( void );

/* ======================================================================== */
/*
 *	Standard string / memory operations used for t-monitor
 */

IMPORT	void*	memset( void *s, int c, size_t n );
IMPORT	int	memcmp( const void *s1, const void *s2, size_t n );
IMPORT	void*	memcpy( void *dst, const void *src, size_t n );
IMPORT	void*	memmove( void *dst, const void *src, size_t n );
IMPORT	size_t	strlen( const char *s );
IMPORT	int	strcmp( const char *s1, const char *s2 );
IMPORT	int	strncmp( const char *s1, const char *s2, size_t n );
IMPORT	char*	strcpy( char *dst, const char *src );
IMPORT	char*	strncpy( char *dst, const char *src, size_t n );

/* ======================================================================== */

#endif /* __MONITOR_TMONITOR_H__ */
