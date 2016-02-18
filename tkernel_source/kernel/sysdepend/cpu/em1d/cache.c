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
 */

/*
 *	cache.c (EM1-D512)
 *	Cache Operation
 */

#include <basic.h>
#include <tk/tkernel.h>
#include <tk/sysdef.h>

#define	CacheLineSZ	32

/*
 * Obtain cache line size
 */
EXPORT INT GetCacheLineSize( void )
{
	return CacheLineSZ;
}

/*
 * Flush cache
 *      Flush cache for an area that starts at laddr for len bytes.
 *      cache is written back and invalidated.
 *
 *      mode := [TCM_ICACHE] | [TCM_DCACHE]
 */
EXPORT void FlushCacheM( CONST void *laddr, INT len, UINT mode )
{
	CONST VB	*p, *ep;

	ep = (VB*)laddr + len;

	if ( (mode & TCM_DCACHE) != 0 ) {
		p = (VB*)((UINT)laddr & ~(CacheLineSZ - 1));
		while ( p < ep ) {
			/* Clean and Invalidate data cache to PoC */

			p += CacheLineSZ;
		}
	}
	if ( (mode & TCM_ICACHE) != 0 ) {
		p = (VB*)((UINT)laddr & ~(CacheLineSZ - 1));
		while ( p < ep ) {
			/* Invalidate instruction cache to PoC */

			p += CacheLineSZ;
		}

	}
	DSB();
}

EXPORT void FlushCache( CONST void *laddr, INT len )
{
	FlushCacheM(laddr, len, TCM_ICACHE|TCM_DCACHE);
}

/*
 * Control cache
 *	mode := [CC_FLUSH] | [CC_INVALIDATE]
 */
EXPORT ER ControlCacheM( void *laddr, INT len, UINT mode )
{
	VB	*p, *ep;

	if ( (mode & ~(CC_FLUSH|CC_INVALIDATE)) != 0 ) return E_PAR;

	ep = (VB*)laddr + len;

	p = (VB*)((UINT)laddr & ~(CacheLineSZ - 1));
	while ( p < ep ) {
		switch ( mode ) {
		  case CC_FLUSH:
			/* Clean data cache to PoC */

			break;
		  case CC_INVALIDATE:
			/* Invalidate data cache to PoC */

			break;
		  default:
			/* Clean and Invalidate data cache to PoC */

		}

		/* Invalidate instruction cache to PoC */


		p += CacheLineSZ;
	}

	DSB();

	return E_OK;
}
