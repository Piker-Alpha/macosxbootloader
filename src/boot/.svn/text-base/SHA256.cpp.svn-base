//********************************************************************
//	created:	20:9:2012   18:20
//	filename: 	SHA256.cpp
//	author:		tiamo	
//	purpose:	sha 256
//********************************************************************

#include "StdAfx.h"

#define GET_UINT32(n, b, i)													{(n) = ((UINT32)(b)[(i)] << 24) | ((UINT32)(b)[(i) + 1] << 16) | ((UINT32)(b)[(i) + 2] << 8) | ((UINT32)(b)[(i) + 3]);}
#define PUT_UINT32(n, b, i)													{(b)[(i)] = (UINT8)((n) >> 24); (b)[(i) + 1] = (UINT8)((n) >> 16); (b)[(i) + 2] = (UINT8) ((n) >> 8); (b)[(i) + 3] = (UINT8)((n));}
#define SHR(x, n)															((x & 0xffffffff) >> n)
#define ROTR(x, n)															(SHR(x,n) | (x << (32 - n)))
#define S0(x)																(ROTR(x,  7) ^ ROTR(x, 18) ^  SHR(x,  3))
#define S1(x)																(ROTR(x, 17) ^ ROTR(x, 19) ^  SHR(x, 10))
#define S2(x)																(ROTR(x,  2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define S3(x)																(ROTR(x,  6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define F0(x, y, z)															((x & y) | (z & (x | y)))
#define F1(x, y, z)															(z ^ (x & (y ^ z)))
#define R(t)																(W[t] = S1(W[t -  2]) + W[t -  7] + S0(W[t - 15]) + W[t - 16])
#define P(a, b, c, d, e, f, g, h, x, K)										{temp1 = h + S3(e) + F1(e, f, g) + K + x; temp2 = S2(a) + F0(a, b, c); d += temp1; h = temp1 + temp2;}

//
// transform
//
STATIC VOID SHA256_Transform(UINT8 CONST* dataBuffer, SHA256_CONTEXT* sha256Context)
{
	UINT32 W[64]															= {0};
	GET_UINT32(W[ 0], dataBuffer,  0);
	GET_UINT32(W[ 1], dataBuffer,  4);
	GET_UINT32(W[ 2], dataBuffer,  8);
	GET_UINT32(W[ 3], dataBuffer, 12);
	GET_UINT32(W[ 4], dataBuffer, 16);
	GET_UINT32(W[ 5], dataBuffer, 20);
	GET_UINT32(W[ 6], dataBuffer, 24);
	GET_UINT32(W[ 7], dataBuffer, 28);
	GET_UINT32(W[ 8], dataBuffer, 32);
	GET_UINT32(W[ 9], dataBuffer, 36);
	GET_UINT32(W[10], dataBuffer, 40);
	GET_UINT32(W[11], dataBuffer, 44);
	GET_UINT32(W[12], dataBuffer, 48);
	GET_UINT32(W[13], dataBuffer, 52);
	GET_UINT32(W[14], dataBuffer, 56);
	GET_UINT32(W[15], dataBuffer, 60);

	UINT32 A																= sha256Context->State[0];
	UINT32 B																= sha256Context->State[1];
	UINT32 C																= sha256Context->State[2];
	UINT32 D																= sha256Context->State[3];
	UINT32 E																= sha256Context->State[4];
	UINT32 F																= sha256Context->State[5];
	UINT32 G																= sha256Context->State[6];
	UINT32 H																= sha256Context->State[7];
	UINT32 temp1															= 0;
	UINT32 temp2															= 0;

	P(A, B, C, D, E, F, G, H, W[ 0], 0x428a2f98);
	P(H, A, B, C, D, E, F, G, W[ 1], 0x71374491);
	P(G, H, A, B, C, D, E, F, W[ 2], 0xb5c0fbcf);
	P(F, G, H, A, B, C, D, E, W[ 3], 0xe9b5dba5);
	P(E, F, G, H, A, B, C, D, W[ 4], 0x3956c25b);
	P(D, E, F, G, H, A, B, C, W[ 5], 0x59f111f1);
	P(C, D, E, F, G, H, A, B, W[ 6], 0x923f82a4);
	P(B, C, D, E, F, G, H, A, W[ 7], 0xab1c5ed5);
	P(A, B, C, D, E, F, G, H, W[ 8], 0xd807aa98);
	P(H, A, B, C, D, E, F, G, W[ 9], 0x12835b01);
	P(G, H, A, B, C, D, E, F, W[10], 0x243185be);
	P(F, G, H, A, B, C, D, E, W[11], 0x550c7dc3);
	P(E, F, G, H, A, B, C, D, W[12], 0x72be5d74);
	P(D, E, F, G, H, A, B, C, W[13], 0x80deb1fe);
	P(C, D, E, F, G, H, A, B, W[14], 0x9bdc06a7);
	P(B, C, D, E, F, G, H, A, W[15], 0xc19bf174);
	P(A, B, C, D, E, F, G, H, R(16), 0xe49b69c1);
	P(H, A, B, C, D, E, F, G, R(17), 0xefbe4786);
	P(G, H, A, B, C, D, E, F, R(18), 0x0fc19dc6);
	P(F, G, H, A, B, C, D, E, R(19), 0x240ca1cc);
	P(E, F, G, H, A, B, C, D, R(20), 0x2de92c6f);
	P(D, E, F, G, H, A, B, C, R(21), 0x4a7484aa);
	P(C, D, E, F, G, H, A, B, R(22), 0x5cb0a9dc);
	P(B, C, D, E, F, G, H, A, R(23), 0x76f988da);
	P(A, B, C, D, E, F, G, H, R(24), 0x983e5152);
	P(H, A, B, C, D, E, F, G, R(25), 0xa831c66d);
	P(G, H, A, B, C, D, E, F, R(26), 0xb00327c8);
	P(F, G, H, A, B, C, D, E, R(27), 0xbf597fc7);
	P(E, F, G, H, A, B, C, D, R(28), 0xc6e00bf3);
	P(D, E, F, G, H, A, B, C, R(29), 0xd5a79147);
	P(C, D, E, F, G, H, A, B, R(30), 0x06ca6351);
	P(B, C, D, E, F, G, H, A, R(31), 0x14292967);
	P(A, B, C, D, E, F, G, H, R(32), 0x27b70a85);
	P(H, A, B, C, D, E, F, G, R(33), 0x2e1b2138);
	P(G, H, A, B, C, D, E, F, R(34), 0x4d2c6dfc);
	P(F, G, H, A, B, C, D, E, R(35), 0x53380d13);
	P(E, F, G, H, A, B, C, D, R(36), 0x650a7354);
	P(D, E, F, G, H, A, B, C, R(37), 0x766a0abb);
	P(C, D, E, F, G, H, A, B, R(38), 0x81c2c92e);
	P(B, C, D, E, F, G, H, A, R(39), 0x92722c85);
	P(A, B, C, D, E, F, G, H, R(40), 0xa2bfe8a1);
	P(H, A, B, C, D, E, F, G, R(41), 0xa81a664b);
	P(G, H, A, B, C, D, E, F, R(42), 0xc24b8b70);
	P(F, G, H, A, B, C, D, E, R(43), 0xc76c51a3);
	P(E, F, G, H, A, B, C, D, R(44), 0xd192e819);
	P(D, E, F, G, H, A, B, C, R(45), 0xd6990624);
	P(C, D, E, F, G, H, A, B, R(46), 0xf40e3585);
	P(B, C, D, E, F, G, H, A, R(47), 0x106aa070);
	P(A, B, C, D, E, F, G, H, R(48), 0x19a4c116);
	P(H, A, B, C, D, E, F, G, R(49), 0x1e376c08);
	P(G, H, A, B, C, D, E, F, R(50), 0x2748774c);
	P(F, G, H, A, B, C, D, E, R(51), 0x34b0bcb5);
	P(E, F, G, H, A, B, C, D, R(52), 0x391c0cb3);
	P(D, E, F, G, H, A, B, C, R(53), 0x4ed8aa4a);
	P(C, D, E, F, G, H, A, B, R(54), 0x5b9cca4f);
	P(B, C, D, E, F, G, H, A, R(55), 0x682e6ff3);
	P(A, B, C, D, E, F, G, H, R(56), 0x748f82ee);
	P(H, A, B, C, D, E, F, G, R(57), 0x78a5636f);
	P(G, H, A, B, C, D, E, F, R(58), 0x84c87814);
	P(F, G, H, A, B, C, D, E, R(59), 0x8cc70208);
	P(E, F, G, H, A, B, C, D, R(60), 0x90befffa);
	P(D, E, F, G, H, A, B, C, R(61), 0xa4506ceb);
	P(C, D, E, F, G, H, A, B, R(62), 0xbef9a3f7);
	P(B, C, D, E, F, G, H, A, R(63), 0xc67178f2);

	sha256Context->State[0]													+= A;
	sha256Context->State[1]													+= B;
	sha256Context->State[2]													+= C;
	sha256Context->State[3]													+= D;
	sha256Context->State[4]													+= E;
	sha256Context->State[5]													+= F;
	sha256Context->State[6]													+= G;
	sha256Context->State[7]													+= H;
}

//
// init
//
VOID SHA256_Init(SHA256_CONTEXT* sha256Context)
{
	sha256Context->TotalLength[0]											= 0;
	sha256Context->TotalLength[1]											= 0;
	sha256Context->State[0]													= 0x6a09e667;
	sha256Context->State[1]													= 0xbb67ae85;
	sha256Context->State[2]													= 0x3c6ef372;
	sha256Context->State[3]													= 0xa54ff53a;
	sha256Context->State[4]													= 0x510e527f;
	sha256Context->State[5]													= 0x9b05688c;
	sha256Context->State[6]													= 0x1f83d9ab;
	sha256Context->State[7]													= 0x5be0cd19;
}

//
// update
//
VOID SHA256_Update(VOID CONST* dataBuffer, UINTN dataLength, SHA256_CONTEXT* sha256Context)
{
	if(!dataLength)
		return;

	UINT32 left																= sha256Context->TotalLength[0] & 0x3f;
	UINT32 fill																= 64 - left;

	sha256Context->TotalLength[0]											+= static_cast<UINT32>(dataLength);
	sha256Context->TotalLength[0]											&= 0xffffffff;

	if(sha256Context->TotalLength[0] < dataLength)
		sha256Context->TotalLength[1]										+= 1;

	if(left && dataLength >= fill)
	{
		memcpy(sha256Context->Buffer + left, dataBuffer, fill);
		SHA256_Transform(sha256Context->Buffer, sha256Context);
		dataLength															-= fill;
		dataBuffer															= Add2Ptr(dataBuffer, fill, VOID CONST*);
		left																= 0;
	}

	while(dataLength >= 64)
	{
		SHA256_Transform(static_cast<UINT8 CONST*>(dataBuffer), sha256Context);
		dataLength															-= 64;
		dataBuffer															= Add2Ptr(dataBuffer, 64, VOID CONST*);
	}

	if(dataLength)
		memcpy(sha256Context->Buffer + left, dataBuffer, dataLength);
}

//
// final
//
VOID SHA256_Final(UINT8* resultBuffer, SHA256_CONTEXT* sha256Context)
{
	STATIC UINT8 padding[64] =
	{
		0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};

	UINT32 high																= (sha256Context->TotalLength[0] >> 29) | (sha256Context->TotalLength[1] << 3);
	UINT32 low																= (sha256Context->TotalLength[0] <<  3);

	UINT8 messageLength[8]													= {0};
	PUT_UINT32(high, messageLength, 0);
	PUT_UINT32(low, messageLength, 4);

	UINT32 last																= sha256Context->TotalLength[0] & 0x3f;
	UINT32 padCount															= (last < 56) ? (56 - last) : (120 - last);

	SHA256_Update(padding, padCount, sha256Context);
	SHA256_Update(messageLength, sizeof(messageLength), sha256Context);

	PUT_UINT32(sha256Context->State[0], resultBuffer,  0);
	PUT_UINT32(sha256Context->State[1], resultBuffer,  4);
	PUT_UINT32(sha256Context->State[2], resultBuffer,  8);
	PUT_UINT32(sha256Context->State[3], resultBuffer, 12);
	PUT_UINT32(sha256Context->State[4], resultBuffer, 16);
	PUT_UINT32(sha256Context->State[5], resultBuffer, 20);
	PUT_UINT32(sha256Context->State[6], resultBuffer, 24);
	PUT_UINT32(sha256Context->State[7], resultBuffer, 28);
}

//
// sha256 buffer
//
VOID SHA256(VOID CONST* dataBuffer, UINTN dataLength, UINT8* resultBuffer)
{
	SHA256_CONTEXT sha256Context;
	SHA256_Init(&sha256Context);
	SHA256_Update(dataBuffer, dataLength, &sha256Context);
	SHA256_Final(resultBuffer, &sha256Context);
}
