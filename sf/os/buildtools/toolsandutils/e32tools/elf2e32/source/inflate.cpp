// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of the Inflater for e32 image dump for the elf2e32 tool
// @internalComponent
// @released
// 
//

#include "huffman.h"
#include "inflate.h"
#include <cpudefs.h>
#include "farray.h"
#include "errorhandler.h"
#include<iostream>
using std::cout;

/*
Inline constructor for CInflater
@param aInput
@internalComponent
@released
*/
inline CInflater::CInflater(TBitInput& aInput):iBits(&aInput),iEncoding(0),iOut(0)
{
}

/*
Function for the 2nd phase construction.
@param 
@internalComponent
@released
*/
void CInflater::ConstructL()
{
	iEncoding=new TEncoding;
	InitL();
	iLen=0;
	iOut=new TUint8[KDeflateMaxDistance];
	iAvail=iLimit=iOut;
}

/*
Function NewLC
@Leave OutOfMemory
@param aInput
@return pointer to self
@internalComponent
@released
*/
CInflater* CInflater::NewLC(TBitInput& aInput)
{
	CInflater* self=new CInflater(aInput);
	self->ConstructL();

	return self;
}

/*
Destructor for CInflater
@internalComponent
@released
*/
CInflater::~CInflater()
{
	delete iEncoding;
	delete [] iOut;
}

/*
Function ReadL
@Leave 
@param aBuffer
@param aLength
@internalComponent
@released
*/
TInt CInflater::ReadL(TUint8* aBuffer,TInt aLength)
{
	TInt tfr=0;
	for (;;)
	{
		TInt len;
		if(aLength > (iLimit-iAvail))
			len=iLimit-iAvail;
		else
			len=aLength;

		if (len && aBuffer)
		{
			memcpy((void * const)aBuffer,(const void * const)iAvail,(size_t)len);
			aBuffer+=len;
		}
		aLength-=len;
		iAvail+=len;
		tfr+=len;
		if (aLength==0)
			return tfr;
		len=InflateL();
		if (len==0)
			return tfr;
		iAvail=iOut;
		iLimit=iAvail+len;
	}
}

/*
Function InitL
@Leave
@internalComponent
@released
*/
void CInflater::InitL()
{
	// read the encoding
	Huffman::InternalizeL(*iBits,iEncoding->iLitLen,KDeflationCodes);
	// validate the encoding
	if (!Huffman::IsValid(iEncoding->iLitLen,TEncoding::ELitLens) ||
		!Huffman::IsValid(iEncoding->iDistance,TEncoding::EDistances))	
	{
		throw E32ImageCompressionError(HUFFMANINVALIDCODINGERROR);
	}

	// convert the length tables into huffman decoding trees
	Huffman::Decoding(iEncoding->iLitLen,TEncoding::ELitLens,iEncoding->iLitLen);
	Huffman::Decoding(iEncoding->iDistance,TEncoding::EDistances,iEncoding->iDistance,KDeflateDistCodeBase);
}

/*
Consume all data lag in the history buffer, then decode to fill up the output buffer
Return the number of available bytes in the output buffer. This is only ever less than the
buffer size if the end of stream marker has been read.
@internalComponent
@released
*/
TInt CInflater::InflateL()
{
	// empty the history buffer into the output
	TUint8* out=iOut;
	TUint8* const end=out+KDeflateMaxDistance;
	const TUint32* tree=iEncoding->iLitLen;
	if (iLen<0)	// EOF
		return 0;
	if (iLen>0)
		goto useHistory;

	while (out<end)
	{
		// get a huffman code
		{
			TInt val=iBits->HuffmanL(tree)-TEncoding::ELiterals;
			if (val<0)
			{
				*out++=TUint8(val);
				continue;			// another literal/length combo
			}
			if (val==TEncoding::EEos-TEncoding::ELiterals)
			{	// eos marker. we're done
				iLen=-1;
				break;
			}
		
			// get the extra bits for the code
			TInt code=val&0xff;
			if (code>=8)
			{	// xtra bits
				TInt xtra=(code>>2)-1;
				code-=xtra<<2;
				code<<=xtra;
				code|=iBits->ReadL(xtra);
			}
			if (val<KDeflateDistCodeBase-TEncoding::ELiterals)
			{	// length code... get the code
				iLen=code+KDeflateMinLength;
				tree=iEncoding->iDistance;
				continue;			// read the huffman code
			}
			// distance code
			iRptr=out-(code+1);
			if (iRptr+KDeflateMaxDistance<end)
			iRptr+=KDeflateMaxDistance;
		}
		useHistory:
			TInt tfr;
			if(iLen > (end-out))
				tfr=end-out;
			else
				tfr=iLen;

			iLen-=tfr;
			const TUint8* from=iRptr;
			do
			{
				*out++=*from++;
				if (from==end)
					from-=KDeflateMaxDistance;
			}while (--tfr!=0);
			iRptr=from;
			tree=iEncoding->iLitLen;
	};
	
	return out-iOut;
}

/*
TFileInput Constructor
@param source
@param size
@internalComponent
@released
*/
TFileInput::TFileInput(unsigned char* source,int size):iReadBuf(source),iSize(size)
{
	Set(source,iSize*8);
}

/*
TFileInput Destructor
@internalComponent
@released
*/
TFileInput::~TFileInput()
{

}

/*
Function UnderFlowL
@Leave E32ImageCompressionError
@internalComponent
@released
*/
void TFileInput::UnderflowL()
{
	throw E32ImageCompressionError(HUFFMANBUFFERUNDERFLOWERROR);
}

/*
Function InflateUncompress
@param source
@param sourcesize
@param dest
@param destsize
@internalComponent
@released
*/
void InflateUnCompress(unsigned char* source, int sourcesize,unsigned char* dest, int destsize)
{
	TFileInput* input = new TFileInput(source, sourcesize);
	CInflater* inflater=CInflater::NewLC(*input);
	inflater->ReadL(dest,destsize);
	delete input;
	delete inflater;
}

