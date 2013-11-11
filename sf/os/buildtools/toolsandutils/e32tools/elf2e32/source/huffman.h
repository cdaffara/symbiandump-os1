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
// Huffman Class for deflate and inflate
// @internalComponent
// @released
// 
//

#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

#include "e32defwrap.h"
#include <fstream>

/**
class Bit Output stream
@internalComponent
@released
*/
class TBitOutput
{
	public:
		TBitOutput();
		TBitOutput(TUint8* aBuf,TInt aSize);
		inline void Set(TUint8* aBuf,TInt aSize);
		inline const TUint8* Ptr() const;
		inline TInt BufferedBits() const;
		void WriteL(TUint aValue, TInt aLength);
		void HuffmanL(TUint aHuffCode);
		void PadL(TUint aPadding);
	private:
		void DoWriteL(TUint aBits, TInt aSize);
		virtual void OverflowL();
	private:
		TUint iCode;		// code in production
		TInt iBits;
		TUint8* iPtr;
		TUint8* iEnd;
};

/*
Set the memory buffer to use for output

Data will be written to this buffer until it is full, at which point OverflowL() will be
called. This should handle the data and then can Set() again to reset the buffer for further
output.
	
@param "TUint8* aBuf" The buffer for output
@param "TInt aSize" The size of the buffer in bytes
@internalComponent
@released
*/
inline void TBitOutput::Set(TUint8* aBuf,TInt aSize)
{
	iPtr=aBuf;
	iEnd=aBuf+aSize;
}

/*
Get the current write position in the output buffer

In conjunction with the address of the buffer, which should be known to the caller, this
describes the data in the bitstream.
@internalComponent
@released
*/
inline const TUint8* TBitOutput::Ptr() const
{
	return iPtr;
}

/*
Get the number of bits that are buffered

This reports the number of bits that have not yet been written into the output buffer.It will
always lie in the range 0..7. Use PadL() to pad the data out to the next byte and write it to
the buffer.
@internalComponent
@released
*/
inline TInt TBitOutput::BufferedBits() const
{
	return iBits+8;
}

/**
This class is derived from TBitOutput
@internalComponent
@released
*/
class TFileOutput : public TBitOutput
{
	enum {KBufSize=0x1000};
	public:
		TFileOutput(std::ofstream & os);
		void FlushL();
		TUint32 iDataCount; 
	private:
		void OverflowL();
	private:
		std::ofstream & iOutStream;
		TUint8 iBuf[KBufSize];
};

/**
Class for Bit input stream.
Good for reading bit streams for packed, compressed or huffman data algorithms.
@since 8.0
@library euser.lib
@internalComponent
@released
*/
class TBitInput
{
	public:
		TBitInput();
		TBitInput(const TUint8* aPtr, TInt aLength, TInt aOffset=0);
		void Set(const TUint8* aPtr, TInt aLength, TInt aOffset=0);
		TUint ReadL();
		TUint ReadL(TInt aSize);
		TUint HuffmanL(const TUint32* aTree);
	private:
		virtual void UnderflowL();
	private:
		TInt iCount;
		TUint iBits;
		TInt iRemain;
		const TUint32* iPtr;
};

/**
Class derived from TBitInput
@internalComponent
@released
*/
class TFileInput : public TBitInput
{
	public:
		TFileInput(unsigned char* source,int size);
		~TFileInput();
	private:
		void UnderflowL();
	private:
		TUint8* iReadBuf;
		TInt iSize;
};

/*
Class for Huffman code toolkit.

This class builds a huffman encoding from a frequency table and builds a decoding tree from a
code-lengths table.

The encoding generated is based on the rule that given two symbols s1 and s2, with code
length l1 and l2, and huffman codes h1 and h2:
	if l1<l2 then h1<h2 when compared lexicographically
	if l1==l2 and s1<s2 then h1<h2 ditto

This allows the encoding to be stored compactly as a table of code lengths

@since 8.0
@library euser.lib
@internalComponent
@released
*/
class Huffman
{
	public:
		enum {KMaxCodeLength=27};
		enum {KMetaCodes=KMaxCodeLength+1};
		enum {KMaxCodes=0x8000};
	public:
		static void HuffmanL(const TUint32 aFrequency[],TInt aNumCodes,TUint32 aHuffman[]);
		static void Encoding(const TUint32 aHuffman[],TInt aNumCodes,TUint32 aEncodeTable[]);
		static TBool IsValid(const TUint32 aHuffman[],TInt aNumCodes);
		static void ExternalizeL(TBitOutput& aOutput,const TUint32 aHuffman[],TInt aNumCodes);
		static void Decoding(const TUint32 aHuffman[],TInt aNumCodes,TUint32 aDecodeTree[],TInt aSymbolBase=0);
		static void InternalizeL(TBitInput& aInput,TUint32 aHuffman[],TInt aNumCodes);
};

// local definitions used for Huffman code generation
typedef TUint16 THuff;		/** @internal */
const THuff KLeaf=0x8000;	/** @internal */
struct TNode
/** @internal */
{
	TUint iCount;
	THuff iLeft;
	THuff iRight;
};

const TInt KDeflateLengthMag=8;
const TInt KDeflateDistanceMag=12;

/**
class for TEncoding
@internalComponent
@released
*/
class TEncoding
{
	public:
		enum {ELiterals=256,ELengths=(KDeflateLengthMag-1)*4,ESpecials=1,EDistances=(KDeflateDistanceMag-1)*4};
		enum {ELitLens=ELiterals+ELengths+ESpecials};
		enum {EEos=ELiterals+ELengths};
	public:
		TUint32 iLitLen[ELitLens];
		TUint32 iDistance[EDistances];
};

const TInt KDeflationCodes=TEncoding::ELitLens+TEncoding::EDistances;

#endif

