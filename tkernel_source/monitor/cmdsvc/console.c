/*
 *----------------------------------------------------------------------
 *    T-Kernel 2.0 Software Package
 *
 *    Copyright 2011 by Ken Sakamura.
 *    This software is distributed under the latest version of T-License 2.x.
 *----------------------------------------------------------------------
 *
 *    Released by T-Engine Forum(http://www.t-engine.org/) at 2011/05/17.
 *    Modified by TRON Forum(http://www.tron.org/) at 2015/06/01.
 *
 *----------------------------------------------------------------------
 * use printk instead of many putHex*, putDec...
 * Copyright 2014-2016, Du Huanpeng<u74147@gmail.com>
 */

/*
 *	console.c
 *
 *       console I/O
 */

#include <cmdsvc.h>

/* control characters */
#define CTRL(ch)	(ch - '@')

#define	BS		CTRL('H')
#define	CAN		CTRL('X')
#define	CTLC		CTRL('C')
#define	DEL		(0x7f)
#define	CR		(0x0d)
#define	LF		(0x0a)
#define	XOFF		CTRL('S')
#define	XON		CTRL('Q')
#define	ERASE		CTRL('K')
#define	CAN2		CTRL('U')
#define	TAB		CTRL('I')
#define	ESC		CTRL('[')
#define	CUR_UP		CTRL('P')		/* or	ESC [ A */
#define	CUR_DWN		CTRL('N')		/* or	ESC [ B */
#define	CUR_FWD		CTRL('F')		/* or	ESC [ C */
#define	CUR_BWD		CTRL('B')		/* or	ESC [ D */

#define	HISTBUF_SZ	1024		/* history buffer size */

LOCAL	UB	hist[HISTBUF_SZ];	/* history buffer */
LOCAL	W	CTRL_C_IN;		/* CTRL-C input flag */
LOCAL	W	XOFF_IN;		/* XOFF input flag */
LOCAL	const UB	Digit[] = "0123456789ABCDEF";

/*
 * detect CTRL-C
 *       check if there is a history of control-C input to the console
 *       history is cleared
 *       return value      TRUE  : CTRL-C input exists
 *                         FALSE : CTRL-C input is absent
 */
EXPORT BOOL checkAbort( void )
{
	if (CTRL_C_IN) {CTRL_C_IN = 0; return TRUE;}
	return FALSE;
}

/*
 * console output (one character)
 *       XON/XOFF flow control
 *       check for CTRL-C input
 *       return value       0 : normal
 *                         -1 : CTRL-C input exists
 */
EXPORT W putchar( W c )
{
	W	ch;
	
	if (XOFF_IN || (ch = getSIO(0)) == XOFF) {
		while ((ch = getSIO(1)) != XON && ch != CTLC);
		XOFF_IN = 0;
	}
	if (ch == CTLC) {CTRL_C_IN++; return -1;}
	if (c == LF) putSIO(CR);
	putSIO(c);
	return 0;
}

/*
 * console output (character string)
 *       XON/XOFF flow control
 *       check for CTRL-C input
 *       return value       0 : normal
 *                         -1 : CTRL-C input exists
 */
EXPORT W puts( const UB *str )
{
	UB	c;

	while ((c = *str++)) {
		if (putchar(c) < 0) return -1;
	}
	return 0;
}

/*
 * console input (one character)
 *       if wait = TRUE, wait for input if FALSE, do not wait.
 *       return value       >= 0 : character
 *                            -1 : no input
 */
EXPORT W getChar( BOOL wait )
{
	W	c;

	for (;;) {
		if ((c = getSIO(0)) == XOFF) XOFF_IN = 1;
		else if (c != -1 || !wait) break;
	}
	return c;
}

/*
 * consle input (character string)
 *       line input with editing
 *       return value      >= 0 : number of input characters
 *                           -1 : CTRL-C was detected
 */
EXPORT W getString( UB *str )
{
	W	i, c, c1;
	W	cp, ep, hp, esc;
	W	len;

	CTRL_C_IN = 0;
	c = c1 = 0;
	cp = ep = esc = 0;
	hp = -1;

	while (ep < L_LINE - 2) {
		if ((c = getSIO(0)) <= 0) continue;
		len = 1;
		if (c & 0x80) {		/* EUC 2 bytes characters */
			if (c1 == 0) {c1 = c; continue;}
			c |= c1 << 8;
			c1 = 0;
			len = 2;
		}
		if (c == ESC) {esc = 1; continue;}

		if (esc) {	/* ESC sequence */
			if (esc == 1) {
				esc = (c == '[') ? 2 : 0;
				continue;
			}
			esc = 0;
			if (c == 'A')		c = CUR_UP;
			else if (c == 'B')	c = CUR_DWN;
			else if (c == 'C')	c = CUR_FWD;
			else if (c == 'D')	c = CUR_BWD;
			else			continue;
		}
		if (c == CUR_FWD) {
			if (cp < ep) {
				if (str[cp] & 0x80) putSIO(str[cp++]);
				putSIO(str[cp++]);
			}
			continue;
		}
		if (c == CUR_BWD) {
			if (cp > 0) {
				if (str[--cp] & 0x80) {putSIO(BS); cp--;}
				putSIO(BS);
			}
			continue;
		}
		if (c == CUR_UP || c == CUR_DWN) {	/* history is recalled */
			if (c == CUR_DWN) {
				if (hp <= 0) continue;
				for (hp--; (--hp) > 0 && hist[hp];);
				if (hp) hp++;
			} else {
				i = hp < 0 ? 0 : (strlen(&hist[hp]) + hp + 1);
				if (hist[i] == '\0') continue;
				hp = i;
			}
			for (; cp > 0; cp--) putSIO(BS);
			for (i = strlen(&hist[hp]); cp < i; cp++)
				putSIO(str[cp] = hist[hp + cp]);
			c = ERASE;
		}
		if (c == BS || c == DEL) {
			if (cp <= 0) continue;
			len = (str[cp - 1] & 0x80) ? 2 : 1;
			if (cp < ep) memcpy(&str[cp - len], &str[cp], ep - cp);
			for (i = 0; i < len; i++)
				{str[--ep] = ' '; putSIO(BS);}
			cp -= len;
			for (i = cp; i < ep + len; i++) putSIO(str[i]);
			for (; i > cp; i--) putSIO(BS);
			continue;
		}
		if (c == CAN || c == CAN2) {
			for (; cp > 0; cp--) putSIO(BS);
			c = ERASE;
		}
		if (c == ERASE) {
			for (i = cp; i < ep; i++) putSIO(' ');
			for (; i > cp; i--) putSIO(BS);
			ep = cp;
			continue;
		}

		if (c == CR || c == LF) break;

		if (c == CTLC) {
			CTRL_C_IN++;
			break;
		}

		if (c < ' ' && c != TAB) continue;

		if (cp < ep) memmove(&str[cp + len], &str[cp], ep - cp);
		if (len == 2) {
			str[cp + 1] = c & 0xff;
			c = (c >> 8) & 0xff;
		}
		str[cp] = c;
		for (ep += len, i = cp; i < ep; i++) putSIO(str[i]);
		for (cp += len; i > cp; i--) putSIO(BS);
	}
	putSIO(CR);		/* echo back */
	putSIO(LF);
	str[ep] = '\0';
	if (c == CTLC)	return -1;

	if (ep) {		/* add to history buffer */
		i = ep + 1;
		memmove(&hist[i], hist, HISTBUF_SZ - i);
		memcpy(hist, str, i);
		hist[HISTBUF_SZ - 2] = '\0';
		hist[HISTBUF_SZ - 1] = '\0';
	}
	return ep;
}
