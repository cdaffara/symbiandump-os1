// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "readers.h"

// Descriptors support a maximum of 2^28 bytes.
// The highest 4 bits are used for other purposes.
static const TInt KMaxDesLen = (2 << 28) - 1;
static const TInt KMinDesLen = -(2 << 28);		

//
//
//
CBufferReader::CBufferReader(const TDesC8& aBuffer)
 :	CReader(aBuffer, CReader::EBuffer)
	{
	}
	
//
//
//
CBufferReader::CBufferReader(const TDesC8& aBuffer, TReaderType aType)
 :	CReader(aBuffer, aType)
	{
	}
	
//
//
//
CBufferReader::~CBufferReader()
	{
	}

//
//
//
CBufferReader* CBufferReader::NewLC(const TDesC8& aBuffer)
	{
	CBufferReader* self = new(ELeave) CBufferReader(aBuffer);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
//
//
//
void CBufferReader::ConstructL()
	{
	}

//
//
//
void CBufferReader::Reset()
	{
	CReader::Reset();
	}

//
//
//
void CBufferReader::SeekL(TInt64 aOffset)
	{
	// Symbian OS descriptors support a maximum size of 2^28 bytes.
	if (aOffset > (TInt64)KMaxDesLen)
		{
		User::Leave(KErrNotSupported);
		}
		
	if (aOffset < (TInt64)KMinDesLen)
		{
		User::Leave(KErrNotSupported);
		}
	
	TInt low = (TInt)I64LOW(aOffset);
	SeekL(low);
	}

//
//
//
void CBufferReader::SeekL(TInt aOffset)
	{
	User::LeaveIfError(CReader::Seek(aOffset));
	}

