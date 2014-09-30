//********************************************************************
//	created:	8:11:2009   20:05
//	filename: 	MD5.cpp
//	author:		tiamo
//	purpose:	md5
//********************************************************************

#include "stdafx.h"

//
// define
//
#define	FETCH_32(P)															(*(UINT32 CONST*)(P))
#define	F(X, Y, Z)															((((Y) ^ (Z)) & (X)) ^ (Z))
#define	G(X, Y, Z)															((((X) ^ (Y)) & (Z)) ^ (Y))
#define	H(X, Y, Z)															((X) ^ (Y) ^ (Z))
#define	I(X, Y, Z)															(((~(Z)) | (X)) ^ (Y))
#define	ROTATE_LEFT(x, n)													(((x) << (n)) | ((x) >> (32 - (n))))

#define	FF(A, B, C, D, X, S, AC)											{(A) += F((B), (C), (D)) + (X) + (UINT64)(AC); (A) = ROTATE_LEFT((A), (S)); (A) += (B);}
#define	GG(A, B, C, D, X, S, AC)											{(A) += G((B), (C), (D)) + (X) + (UINT64)(AC); (A) = ROTATE_LEFT((A), (S)); (A) += (B);}
#define	HH(A, B, C, D, X, S, AC)											{(A) += H((B), (C), (D)) + (X) + (UINT64)(AC); (A) = ROTATE_LEFT((A), (S));	(A) += (B);}
#define	II(A, B, C, D, X, S, AC)											{(A) += I((B), (C), (D)) + (X) + (UINT64)(AC); (A) = ROTATE_LEFT((A), (S));	(A) += (B);}

//
// transform
//
STATIC VOID MD5pTransform(UINT32 a, UINT32 b, UINT32 c, UINT32 d, UINT8 CONST* block, MD5_CONTEXT* md5Context)
{
	UINT32 x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15;

	x15																		= FETCH_32(block + 60);
	x14																		= FETCH_32(block + 56);
	x13																		= FETCH_32(block + 52);
	x12																		= FETCH_32(block + 48);
	x11																		= FETCH_32(block + 44);
	x10																		= FETCH_32(block + 40);
	x9																		= FETCH_32(block + 36);
	x8																		= FETCH_32(block + 32);
	x7																		= FETCH_32(block + 28);
	x6																		= FETCH_32(block + 24);
	x5																		= FETCH_32(block + 20);
	x4																		= FETCH_32(block + 16);
	x3																		= FETCH_32(block + 12);
	x2																		= FETCH_32(block +  8);
	x1																		= FETCH_32(block +  4);
	x0																		= FETCH_32(block +  0);

#define	S11 7
#define	S12 12
#define	S13 17
#define	S14 22
	FF(a, b, c, d, x0,  S11, 0xd76aa478); /* 1 */
	FF(d, a, b, c, x1,  S12, 0xe8c7b756); /* 2 */
	FF(c, d, a, b, x2,  S13, 0x242070db); /* 3 */
	FF(b, c, d, a, x3,  S14, 0xc1bdceee); /* 4 */
	FF(a, b, c, d, x4,  S11, 0xf57c0faf); /* 5 */
	FF(d, a, b, c, x5,  S12, 0x4787c62a); /* 6 */
	FF(c, d, a, b, x6,  S13, 0xa8304613); /* 7 */
	FF(b, c, d, a, x7,  S14, 0xfd469501); /* 8 */
	FF(a, b, c, d, x8,  S11, 0x698098d8); /* 9 */
	FF(d, a, b, c, x9,  S12, 0x8b44f7af); /* 10 */
	FF(c, d, a, b, x10, S13, 0xffff5bb1); /* 11 */
	FF(b, c, d, a, x11, S14, 0x895cd7be); /* 12 */
	FF(a, b, c, d, x12, S11, 0x6b901122); /* 13 */
	FF(d, a, b, c, x13, S12, 0xfd987193); /* 14 */
	FF(c, d, a, b, x14, S13, 0xa679438e); /* 15 */
	FF(b, c, d, a, x15, S14, 0x49b40821); /* 16 */

#define	S21 5
#define	S22 9
#define	S23 14
#define	S24 20
	GG(a, b, c, d, x1,  S21, 0xf61e2562); /* 17 */
	GG(d, a, b, c, x6,  S22, 0xc040b340); /* 18 */
	GG(c, d, a, b, x11, S23, 0x265e5a51); /* 19 */
	GG(b, c, d, a, x0,  S24, 0xe9b6c7aa); /* 20 */
	GG(a, b, c, d, x5,  S21, 0xd62f105d); /* 21 */
	GG(d, a, b, c, x10, S22, 0x02441453); /* 22 */
	GG(c, d, a, b, x15, S23, 0xd8a1e681); /* 23 */
	GG(b, c, d, a, x4,  S24, 0xe7d3fbc8); /* 24 */
	GG(a, b, c, d, x9,  S21, 0x21e1cde6); /* 25 */
	GG(d, a, b, c, x14, S22, 0xc33707d6); /* 26 */
	GG(c, d, a, b, x3,  S23, 0xf4d50d87); /* 27 */
	GG(b, c, d, a, x8,  S24, 0x455a14ed); /* 28 */
	GG(a, b, c, d, x13, S21, 0xa9e3e905); /* 29 */
	GG(d, a, b, c, x2,  S22, 0xfcefa3f8); /* 30 */
	GG(c, d, a, b, x7,  S23, 0x676f02d9); /* 31 */
	GG(b, c, d, a, x12, S24, 0x8d2a4c8a); /* 32 */

#define	S31 4
#define	S32 11
#define	S33 16
#define	S34 23
	HH(a, b, c, d, x5,  S31, 0xfffa3942); /* 33 */
	HH(d, a, b, c, x8,  S32, 0x8771f681); /* 34 */
	HH(c, d, a, b, x11, S33, 0x6d9d6122); /* 35 */
	HH(b, c, d, a, x14, S34, 0xfde5380c); /* 36 */
	HH(a, b, c, d, x1,  S31, 0xa4beea44); /* 37 */
	HH(d, a, b, c, x4,  S32, 0x4bdecfa9); /* 38 */
	HH(c, d, a, b, x7,  S33, 0xf6bb4b60); /* 39 */
	HH(b, c, d, a, x10, S34, 0xbebfbc70); /* 40 */
	HH(a, b, c, d, x13, S31, 0x289b7ec6); /* 41 */
	HH(d, a, b, c, x0,  S32, 0xeaa127fa); /* 42 */
	HH(c, d, a, b, x3,  S33, 0xd4ef3085); /* 43 */
	HH(b, c, d, a, x6,  S34, 0x04881d05); /* 44 */
	HH(a, b, c, d, x9,  S31, 0xd9d4d039); /* 45 */
	HH(d, a, b, c, x12, S32, 0xe6db99e5); /* 46 */
	HH(c, d, a, b, x15, S33, 0x1fa27cf8); /* 47 */
	HH(b, c, d, a, x2,  S34, 0xc4ac5665); /* 48 */

#define	S41 6
#define	S42 10
#define	S43 15
#define	S44 21
	II(a, b, c, d, x0,  S41, 0xf4292244); /* 49 */
	II(d, a, b, c, x7,  S42, 0x432aff97); /* 50 */
	II(c, d, a, b, x14, S43, 0xab9423a7); /* 51 */
	II(b, c, d, a, x5,  S44, 0xfc93a039); /* 52 */
	II(a, b, c, d, x12, S41, 0x655b59c3); /* 53 */
	II(d, a, b, c, x3,  S42, 0x8f0ccc92); /* 54 */
	II(c, d, a, b, x10, S43, 0xffeff47d); /* 55 */
	II(b, c, d, a, x1,  S44, 0x85845dd1); /* 56 */
	II(a, b, c, d, x8,  S41, 0x6fa87e4f); /* 57 */
	II(d, a, b, c, x15, S42, 0xfe2ce6e0); /* 58 */
	II(c, d, a, b, x6,  S43, 0xa3014314); /* 59 */
	II(b, c, d, a, x13, S44, 0x4e0811a1); /* 60 */
	II(a, b, c, d, x4,  S41, 0xf7537e82); /* 61 */
	II(d, a, b, c, x11, S42, 0xbd3af235); /* 62 */
	II(c, d, a, b, x2,  S43, 0x2ad7d2bb); /* 63 */
	II(b, c, d, a, x9,  S44, 0xeb86d391); /* 64 */

	md5Context->State[0]													+= a;
	md5Context->State[1]													+= b;
	md5Context->State[2]													+= c;
	md5Context->State[3]													+= d;
}

//
// init
//
VOID MD5Init(MD5_CONTEXT* md5Context)
{
	md5Context->Count[0]													= 0;
	md5Context->Count[1]													= 0;
	md5Context->State[0]													= 0x67452301;
	md5Context->State[1]													= 0xefcdab89;
	md5Context->State[2]													= 0x98badcfe;
	md5Context->State[3]													= 0x10325476;
}

//
// update
//
VOID MD5Update(MD5_CONTEXT* md5Context, VOID CONST* inputBuffer, UINT32 bufferLength)
{
	UINT8 CONST* byteBuffer													= static_cast<UINT8 CONST*>(inputBuffer);
	UINT32 index															= (md5Context->Count[0] >> 3) & 0x3f;

	md5Context->Count[0]													+= bufferLength << 3;
	if(md5Context->Count[0] < (bufferLength << 3))
		md5Context->Count[1]												+= 1;

	md5Context->Count[1]													+= bufferLength >> 29;

	UINT32 partLen															= 64 - index;
	UINT32 i																= 0;
	if(bufferLength >= partLen)
	{
		if(index)
		{
			memcpy(&md5Context->InputBuffer[index], byteBuffer, partLen);
			MD5pTransform(md5Context->State[0], md5Context->State[1], md5Context->State[2], md5Context->State[3], md5Context->InputBuffer, md5Context);
			i															= partLen;
		}

		for(; i + 63 < bufferLength; i += 64)
			MD5pTransform(md5Context->State[0], md5Context->State[1], md5Context->State[2], md5Context->State[3], byteBuffer + i, md5Context);

		if(bufferLength == i)
			return;

		index																= 0;
	}

	memcpy(&md5Context->InputBuffer[index], byteBuffer +i, bufferLength - i);
}

//
// finish
//
VOID MD5Final(UINT8* md5Result, MD5_CONTEXT* md5Context)
{
	UINT32 index															= (md5Context->Count[0] >> 3) & 0x3f;
	UINT8 bits[8]															= {0};
	STATIC UINT8 CONST paddingBuffer[64]									= {0x80};
	memcpy(bits, md5Context->Count, 8);
	MD5Update(md5Context, paddingBuffer, ((index < 56) ? 56 : 120) - index);
	MD5Update(md5Context, bits, 8);
	memcpy(md5Result, md5Context->State, 16);
	memset(md5Context, 0, sizeof(MD5_CONTEXT));
}