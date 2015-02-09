//********************************************************************
//	created:	4:11:2009   10:04
//	filename: 	StdAfx.cpp
//	author:		Pike R. Alpha (tiamo just used an emty file)
//	purpose:	StdAfx
//********************************************************************

#include "StdAfx.h"

#ifdef __APPLE__

#include <stdint.h>

extern VOID BdTrap01();

extern uint64_t ___udivdi3(uint64_t num, uint64_t den);
extern uint64_t ___umoddi3(uint64_t num, uint64_t den);

//====================================================================
// Copied from: syslinux

uint64_t __udivmoddi4(uint64_t num, uint64_t den, uint64_t *rem)
{
	uint64_t quot = 0, qbit = 1;

	if (den == 0)
	{
		BdTrap01();

		return 0;	/* In case BdTrap01() returns... */
	}

	/* Left-justify denominator and count shift */
	while ((int64_t)den >= 0)
	{
		den <<= 1;
		qbit <<= 1;
	}

	while (qbit)
	{
		if (den <= num)
		{
			num -= den;
			quot += qbit;
		}

		den >>= 1;
		qbit >>= 1;
	}

	if (rem)
	{
		*rem = num;
	}

	return quot;
}

//====================================================================

extern "C"
{
	// Copied from: arch/i386/libgcc/__divdi3.c
	uint64_t __udivdi3(uint64_t num, uint64_t den)
	{
		return __udivmoddi4(num, den, (uint64_t *)0);
	}

	// Copied from: arch/i386/libgcc/__umoddi3.c
	uint64_t __umoddi3(uint64_t num, uint64_t den)
	{
		uint64_t v;

	(void) __udivmoddi4(num, den, &v);
		return v;
	}

	void __bzero(void *b, UINTN length)
	{
		char *ptr = (char *)b;
		
		while (length--)
		{
			*ptr++ = 0;
		}
	}
}
#endif // if __APPLE__
