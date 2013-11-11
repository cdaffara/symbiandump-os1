// DynamicBuffer.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "Misc.h"
#include "stdafx.h"
#include "console_host.h"
#include "DynamicBuffer.h"


CDynamicReadBuffer* CDynamicReadBuffer::New(int aInitialLength)
	{
	std::auto_ptr<CDynamicReadBuffer> self(new(EThrow) CDynamicReadBuffer());
	self->Construct(aInitialLength);
	return self.release();
	}

CDynamicReadBuffer::~CDynamicReadBuffer()
	{
	delete iBuf;
	}

const char* CDynamicReadBuffer::Buffer() const
	{
	return iBuf;
	}

char* CDynamicReadBuffer::Buffer(int aLength)
	{
	if (aLength > iMaxBufLength)
		{
		GrowBuf(aLength);
		}
	return iBuf;
	}

CDynamicReadBuffer::CDynamicReadBuffer() : iBuf(NULL), iMaxBufLength(0)
	{
	}

void CDynamicReadBuffer::Construct(int aInitialLength)
	{
	GrowBuf(aInitialLength);
	}

void CDynamicReadBuffer::GrowBuf(int aNewLength)
	{
	ASSERT(aNewLength > iMaxBufLength);
	char* newBuf = new(EThrow) char[aNewLength];
	delete iBuf;
	iBuf = newBuf;
	iMaxBufLength = aNewLength;
	}


CDynamicWriteBuffer* CDynamicWriteBuffer::New(int aInitialLength)
	{
	std::auto_ptr<CDynamicWriteBuffer> self(new(EThrow) CDynamicWriteBuffer());
	self->Construct(aInitialLength);
	return self.release();
	}

CDynamicWriteBuffer::~CDynamicWriteBuffer()
	{
	delete iBuf;
	}

void CDynamicWriteBuffer::Add(char* aBuf, int aLength)
	{
	if (aLength > (iMaxBufLength - iCurrentBufLength))
		{
		GrowBuf(iCurrentBufLength + aLength);
		}
	memcpy(iBuf + iCurrentBufLength, aBuf, aLength);
	iCurrentBufLength += aLength;
	}

void CDynamicWriteBuffer::Remove(int aLength)
	{
	ASSERT(aLength <= iCurrentBufLength);
	memmove(iBuf, iBuf + aLength, iCurrentBufLength - aLength);
	iCurrentBufLength -= aLength;
	}

const char* CDynamicWriteBuffer::Buffer() const
	{
	return iBuf;
	}

int CDynamicWriteBuffer::Length() const
	{
	return iCurrentBufLength;
	}

CDynamicWriteBuffer::CDynamicWriteBuffer() : iBuf(NULL), iMaxBufLength(0), iCurrentBufLength(0)
	{
	}

void CDynamicWriteBuffer::Construct(int aInitialLength)
	{
	GrowBuf(aInitialLength);
	}

void CDynamicWriteBuffer::GrowBuf(int aNewLength)
	{
	ASSERT(aNewLength > iMaxBufLength);
	char* newBuf = new(EThrow) char[aNewLength];
	if (iBuf)
		{
		memcpy(newBuf, iBuf, iCurrentBufLength);
		delete iBuf;
		}
	iBuf = newBuf;
	iMaxBufLength = aNewLength;
	}
