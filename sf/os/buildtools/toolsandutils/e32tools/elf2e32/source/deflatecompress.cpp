// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <fstream>
#include <cassert>

#include "e32imagedefs.h"
#include "errorhandler.h"
#include "farray.h"
#include "huffman.h"

const TInt KDeflateMinLength=3;
const TInt KDeflateMaxLength=KDeflateMinLength-1 + (1<<KDeflateLengthMag);
const TInt KDeflateMaxDistance=(1<<KDeflateDistanceMag);

// hashing
const TUint KDeflateHashMultiplier=0xAC4B9B19u;
const TInt KDeflateHashShift=24;

#define COMPILE_TIME_ASSERT(e)	\
	switch (0)					\
	{							\
	case 0:						\
	case e:						\
		;						\
	}

/**
Class HDeflateHash
@internalComponent
@released
*/
class HDeflateHash
{
	public:
		inline static HDeflateHash* NewLC(TInt aLinks);
//
		inline TInt First(const TUint8* aPtr,TInt aPos);
		inline TInt Next(TInt aPos,TInt aOffset) const;
	private:
		inline HDeflateHash();
		inline static TInt Hash(const TUint8* aPtr);
	private:
		typedef TUint16 TOffset;
	private:
		TInt iHash[256];
		TOffset iOffset[1];	// or more
};

/**
Class MDeflater
@internalComponent
@released
*/
class MDeflater
{
	public:
		void DeflateL(const TUint8* aBase,TInt aLength);
	private:
		const TUint8* DoDeflateL(const TUint8* aBase,const TUint8* aEnd,HDeflateHash& aHash);
		static TInt Match(const TUint8* aPtr,const TUint8* aEnd,TInt aPos,HDeflateHash& aHas);
		void SegmentL(TInt aLength,TInt aDistance);
		virtual void LitLenL(TInt aCode) =0;
		virtual void OffsetL(TInt aCode) =0;
		virtual void ExtraL(TInt aLen,TUint aBits) =0;
};

/**
Class TDeflateStats
@internalComponent
@released
*/
class TDeflateStats : public MDeflater
{
	public:
		inline TDeflateStats(TEncoding& aEncoding);
	private:
		// from MDeflater
		void LitLenL(TInt aCode);
		void OffsetL(TInt aCode);
		void ExtraL(TInt aLen,TUint aBits);
	private:
		TEncoding& iEncoding;
};

/**
Class TDeflater
@internalComponent
@released
*/
class TDeflater : public MDeflater
{
	public:
		inline TDeflater(TBitOutput& aOutput,const TEncoding& aEncoding);
	private:
		// from MDeflater
		void LitLenL(TInt aCode);
		void OffsetL(TInt aCode);
		void ExtraL(TInt aLen,TUint aBits);
	private:
		TBitOutput& iOutput;
		const TEncoding& iEncoding;
};


/**
Constructor for class HDeflateHash
@internalComponent
@released
*/
inline HDeflateHash::HDeflateHash()
{TInt* p=iHash+256;do *--p=-KDeflateMaxDistance-1; while (p>iHash);}

/**
@Leave - OutOfMemory
This function allocates memory for HDeflateHash
@param aLinks
@return pointer to allocated memory
@internalComponent
@released
*/
inline HDeflateHash* HDeflateHash::NewLC(TInt aLinks)
{
#if __GNUC__ >= 4
	// Try to detect if the class' layout has changed.
	COMPILE_TIME_ASSERT( sizeof(HDeflateHash) == 1028 );
	COMPILE_TIME_ASSERT( sizeof(TOffset) == 2 );
	COMPILE_TIME_ASSERT( offsetof(HDeflateHash, iHash) < offsetof(HDeflateHash, iOffset) );

	// Compute the size of the class, including rounding it up to a multiple of 4
	// bytes.

	unsigned n = sizeof(TInt) * 256 + sizeof(TOffset) * Min(aLinks, KDeflateMaxDistance);

	while (n & 0x1f)
	{
		n++;	
	}

	// Allocate the raw memory ...
	void* p = ::operator new(n);

	// ... And create the object in that memory.
	return new(p) HDeflateHash;
#else
	return new(new char[_FOFF(HDeflateHash,iOffset[Min(aLinks,KDeflateMaxDistance)])]) HDeflateHash;
#endif
}

/**
Hash function for HDeflateHash
@param aPtr
@return Hash value
@internalComponent
@released
*/
inline TInt HDeflateHash::Hash(const TUint8* aPtr)
{
	TUint x=aPtr[0]|(aPtr[1]<<8)|(aPtr[2]<<16);
	return (x*KDeflateHashMultiplier)>>KDeflateHashShift;
}

/**
Function First
@param aPtr
@param aPos
@internalComponent
@released
*/
inline TInt HDeflateHash::First(const TUint8* aPtr,TInt aPos)
{
	TInt h=Hash(aPtr);
	TInt offset=Min(aPos-iHash[h],KDeflateMaxDistance<<1);
	iHash[h]=aPos;
	iOffset[aPos&(KDeflateMaxDistance-1)]=TOffset(offset);
	return offset;
}

/**
Function Next
@param aPtr
@param aPos
@internalComponent
@released
*/
inline TInt HDeflateHash::Next(TInt aPos,TInt aOffset) const
{return aOffset+iOffset[(aPos-aOffset)&(KDeflateMaxDistance-1)];}


// Class TDeflater
//
// generic deflation algorithm, can do either statistics and the encoder

/**
Function Match
@param aPtr
@param aEnd
@param aPos
@param aHash
@internalComponent
@released
*/
TInt MDeflater::Match(const TUint8* aPtr,const TUint8* aEnd,TInt aPos,HDeflateHash& aHash)
{
	TInt offset=aHash.First(aPtr,aPos);
	if (offset>KDeflateMaxDistance)
		return 0;
	TInt match=0;
	aEnd=Min(aEnd,aPtr+KDeflateMaxLength);
	TUint8 c=*aPtr;
	do
	{
		const TUint8* p=aPtr-offset;
		if (p[match>>16]==c)
		{	// might be a better match
			const TUint8* m=aPtr;
			for (;;)
			{
				if (*p++!=*m++)
					break;
				if (m<aEnd)
					continue;
				return ((m-aPtr)<<16)|offset;
			}
			TInt l=m-aPtr-1;
			if (l>match>>16)
			{
				match=(l<<16)|offset;
				c=m[-1];
			}
		}
		offset=aHash.Next(aPos,offset);
	} while (offset<=KDeflateMaxDistance);
	return match;
}

/*
Apply the deflation algorithm to the data [aBase,aEnd)
Return a pointer after the last byte that was deflated (which may not be aEnd)
@param aBase
@param aEnd
@param aHash
@internalComponent
@released
*/
const TUint8* MDeflater::DoDeflateL(const TUint8* aBase,const TUint8* aEnd,HDeflateHash& aHash)
{
	const TUint8* ptr=aBase;
	TInt prev=0;		// the previous deflation match
	do
	{
		TInt match=Match(ptr,aEnd,ptr-aBase,aHash);
// Extra deflation applies two optimisations which double the time taken
// 1. If we have a match at p, then test for a better match at p+1 before using it
// 2. When we have a match, add the hash links for all the data which will be skipped 
		if (match>>16 < prev>>16)
		{	// use the previous match--it was better
			TInt len=prev>>16;
			SegmentL(len,prev-(len<<16));
			// fill in missing hash entries for better compression
			const TUint8* e=ptr+len-2;
			do
			{
				++ptr;
				if (ptr + 2 < aEnd)
				  aHash.First(ptr,ptr-aBase);
			} while (ptr<e);
			prev=0;
		}
		else if (match<=(KDeflateMinLength<<16))
			LitLenL(*ptr);			// no deflation match here
		else
		{	// save this match and test the next position
			if (prev)	// we had a match at ptr-1, but this is better
				LitLenL(ptr[-1]);
			prev=match;
		}
		++ptr;
	} while (ptr+KDeflateMinLength-1<aEnd);
	if (prev)
	{		// emit the stored match
		TInt len=prev>>16;
		SegmentL(len,prev-(len<<16));
		ptr+=len-1;
	}
	return ptr;
}

/*
The generic deflation algorithm
@param aBase
@param aLength
@internalComponent
@released
*/
void MDeflater::DeflateL(const TUint8* aBase,TInt aLength)
{
	const TUint8* end=aBase+aLength;
	if (aLength>KDeflateMinLength)
	{	// deflation kicks in if there is enough data
		HDeflateHash* hash=HDeflateHash::NewLC(aLength);

		aBase=DoDeflateL(aBase,end,*hash);
		delete hash;
	}
	while (aBase<end)					// emit remaining bytes
		LitLenL(*aBase++);
	LitLenL(TEncoding::EEos);	// eos marker
}

/*
Turn a (length,offset) pair into the deflation codes+extra bits before calling the specific
LitLen(), Offset() and Extra() functions.
@param aLength
@param aDistance
@internalComponent
@released
*/
void MDeflater::SegmentL(TInt aLength,TInt aDistance)
{
	aLength-=KDeflateMinLength;
	TInt extralen=0;
	TUint len=aLength;
	while (len>=8)
	{
		++extralen;
		len>>=1;
	}
	LitLenL((extralen<<2)+len+TEncoding::ELiterals);
	if (extralen)
		ExtraL(extralen,aLength);
//
	aDistance--;
	extralen=0;
	TUint dist=aDistance;
	while (dist>=8)
	{
		++extralen;
		dist>>=1;
	}
	OffsetL((extralen<<2)+dist);
	if (extralen)
		ExtraL(extralen,aDistance);
}

/**
Class TDeflateStats
This class analyses the data stream to generate the frequency tables 
for the deflation algorithm
@internalComponent
@released
*/
inline TDeflateStats::TDeflateStats(TEncoding& aEncoding)
	:iEncoding(aEncoding)
	{}
/*
Function LitLenL
@Leave
@param aCode
@internalComponent
@released
*/
void TDeflateStats::LitLenL(TInt aCode)
	{
	++iEncoding.iLitLen[aCode];
	}

/*
@Leave ArrayIndexOutOfBounds
Finction OffsetL
@param aCode
@internalComponent
@released
*/
void TDeflateStats::OffsetL(TInt aCode)
	{
	++iEncoding.iDistance[aCode];
	}

/*
Function ExtraL
@Leave
@internalComponent
@released
*/void TDeflateStats::ExtraL(TInt,TUint)
	{}

/**
Constructor of Class TDeflater
Extends MDeflater to provide huffman encoding of the output
@internalComponent
@released
*/
inline TDeflater::TDeflater(TBitOutput& aOutput,const TEncoding& aEncoding)
//
// construct for encoding
//
	:iOutput(aOutput),iEncoding(aEncoding)
	{}

/*
Function LitLenL
@Leave
@param aCode 
@internalComponent
@released
*/
void TDeflater::LitLenL(TInt aCode)
	{
	iOutput.HuffmanL(iEncoding.iLitLen[aCode]);
	}

/*
Function OffsetL
@Leave
@param aCdoe 
@internalComponent
@released
*/
void TDeflater::OffsetL(TInt aCode)
	{
	iOutput.HuffmanL(iEncoding.iDistance[aCode]);
	}

/*
Function ExtraL
@Leave
@param  aLen
@param aBits
@internalComponent
@released
*/
void TDeflater::ExtraL(TInt aLen,TUint aBits)
	{
	iOutput.WriteL(aBits,aLen);
	}
/*
Function DoDeflateL
@Leave
@param aBuf
@param aLength
@param aOutput
@param aEncoding
@internalComponent
@released
*/
void DoDeflateL(const TUint8* aBuf,TInt aLength,TBitOutput& aOutput,TEncoding& aEncoding)
	{
// analyse the data for symbol frequency 
	TDeflateStats analyser(aEncoding);
	analyser.DeflateL(aBuf,aLength);
	
// generate the required huffman encodings
	Huffman::HuffmanL(aEncoding.iLitLen,TEncoding::ELitLens,aEncoding.iLitLen);
	Huffman::HuffmanL(aEncoding.iDistance,TEncoding::EDistances,aEncoding.iDistance);

// Store the encoding table
	Huffman::ExternalizeL(aOutput,aEncoding.iLitLen,KDeflationCodes);

// generate the tables
	Huffman::Encoding(aEncoding.iLitLen,TEncoding::ELitLens,aEncoding.iLitLen);
	Huffman::Encoding(aEncoding.iDistance,TEncoding::EDistances,aEncoding.iDistance);

// now finally deflate the data with the generated encoding
	TDeflater deflater(aOutput,aEncoding);
	deflater.DeflateL(aBuf,aLength);
	aOutput.PadL(1);
	}

/*
Function DeflateL
@Leave
@param aBuf
@param aLength
@param aOutput 
@internalComponent
@released
*/
void DeflateL(const TUint8* aBuf, TInt aLength, TBitOutput& aOutput)
	{
	TEncoding* encoding=new TEncoding;
	memset(encoding,0,sizeof(TEncoding));
	DoDeflateL(aBuf,aLength,aOutput,*encoding);
	delete encoding;
	}
/*
Function DeflateCompress
@param bytes
@param size
@param os
@internalComponent
@released
*/
void DeflateCompress(char *bytes,size_t size, std::ofstream & os)
	{
	TFileOutput* output=new TFileOutput(os);
	output->iDataCount = 0;
	DeflateL((TUint8*)bytes,size,*output);
	output->FlushL();
	delete output;
	}

