// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// e32tools\petran\Szip\compress.cpp
// 
//

#include "deflate.h"

#if defined(__MSVCDOTNET__) || defined(__TOOLS2__)
#include <fstream>
#else //__MSVCDOTNET__
#include <fstream.h>
#endif //__MSVCDOTNET__

#include <assert.h>
#include "h_utl.h"

class TFileOutput : public TBitOutput
	{
	enum {KBufSize=0x1000};
public:
	TFileOutput(ostream& os);
	void FlushL();
	TUint32 iDataCount; 
private:
	void OverflowL();
private:
	ostream& iOutStream;
	TUint8 iBuf[KBufSize];
	};

TFileOutput::TFileOutput(ostream& os)
	:iOutStream(os)
	{
	Set(iBuf,KBufSize);
	}

void TFileOutput::OverflowL()
//
// empty the buffer and reset the pointers
//
	{
	FlushL();
	Set(iBuf,KBufSize);
	}

void TFileOutput::FlushL()
//
// write out the contents of the buffer
//
	{
	TInt len=Ptr()-iBuf;
	if (len)
		{
		iOutStream.write(reinterpret_cast<char *>(iBuf), len); // write extended header
		iDataCount += len;
		}
	}

void DeflateCompress(char *bytes,TInt size,ostream &os)
	{
	TFileOutput* output=new TFileOutput(os);
	output->iDataCount = 0;
	DeflateL((TUint8*)bytes,size,*output);
	output->FlushL();
	delete output;
	}

TUint32 DeflateCompressCheck(char *bytes,TInt size,ostream &os)
	{
	TUint32 r = 0;
	TFileOutput* output=new TFileOutput(os);
	output->iDataCount = 0;
	DeflateL((TUint8*)bytes,size,*output);
	output->FlushL();

	r = output->iDataCount; // Preserve the compressed count
	delete output;
	return r; // Return the compressed size
	}

class TFileInput : public TBitInput
	{
public:
	TFileInput(unsigned char* source,int size);

private:
	void UnderflowL();
private:
	TUint8* iReadBuf;
	TInt iSize;
	};

TFileInput::TFileInput(unsigned char* source,int size)
	:iReadBuf(source),iSize(size)
	{
	Set(source,iSize*8);
	}


void TFileInput::UnderflowL()
	{
	Print(ESevereError,"Buffer underflow on deflate\n");
	}

void InflateUnCompress(unsigned char* source, int sourcesize,unsigned char* dest, int destsize)
	{
	TFileInput* input = new TFileInput(source, sourcesize);
	CInflater* inflater=CInflater::NewLC(*input);
	inflater->ReadL(dest,destsize);
	delete input;
	//delete inflater;
	}


