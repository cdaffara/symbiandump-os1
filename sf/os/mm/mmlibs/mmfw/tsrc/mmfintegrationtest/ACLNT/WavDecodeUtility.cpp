// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mmfwavformat.h"

#include "WavDecodeUtility.h" 


CWavDecodeUtility::CWavDecodeUtility()
	{
	}

void CWavDecodeUtility::ConstructL(TDesC8& aBuffer)
	{
	iBuffer = &aBuffer;
	FindRiffChunksL();
	ProcessFormatHeaderL();
	}

CWavDecodeUtility* CWavDecodeUtility::NewL(TDesC8& aBuffer)
	{
	CWavDecodeUtility* self = new (ELeave) CWavDecodeUtility();
	CleanupStack::PushL(self);
	self->ConstructL(aBuffer);
	CleanupStack::Pop();
	return self;
	}


CWavDecodeUtility::~CWavDecodeUtility()
	{
	}

TUint16 CWavDecodeUtility::Read16(const TUint8* aPtr)

    {
	TUint16 ret = *(REINTERPRET_CAST(const TUint16*, aPtr));
	return ret;
    }

TUint32 CWavDecodeUtility::Read32(const TUint8* aPtr)
    {
    TUint32 x = *aPtr++;
    x |= *aPtr++ << 8;
    x |= *aPtr++ << 16;
    x |= *aPtr++ << 24;
    return x;
    }
void CWavDecodeUtility::AssignChunkTo(TMdaRiffChunk* aAssignedChunk)
	{
	Mem::Copy(REINTERPRET_CAST(TUint8*, aAssignedChunk), REINTERPRET_CAST(TUint8*, &iCurrent),sizeof(TMdaRiffChunk));
	aAssignedChunk->iFound=ETrue;
	}

void CWavDecodeUtility::ReadChunk(TMdaRiffChunk* aChunk)
	{
	Mem::FillZ(REINTERPRET_CAST(TUint8*, aChunk),sizeof(TMdaRiffChunk)); // Zero data
	aChunk->iPosition=iPos + KRiffChunkHeaderLength;
	aChunk->iName = Read32(iStartPtr + iPos - iLastReadPosition);
	aChunk->iLength = Read32(iStartPtr + iPos - iLastReadPosition + KRiffChunkDataLength);
	}
void CWavDecodeUtility::FindRiffChunksL()
	{
	if (!iFoundChunks)
		{
		iFoundChunks=ETrue;
//		iStartPtr=iBuffer->Data().Ptr();
		iStartPtr=iBuffer->Ptr();
		iPos=0;
		iLastReadPosition=0;//Set by CBase, but what the heck
		iDone=EFalse;
		}
	else
		{//We've done another read. If there's < chunk in the buffer then something's wrong
		if (iBuffer->Length() < STATIC_CAST(TInt, KRiffChunkHeaderLength))
			{
			if ((iFormatChunk.iFound) && (iDataChunk.iFound)) 
				{
				iDone = ETrue; //it should be ok to exit loop
				return;
				}
			else
				{
				User::Leave(KErrCorrupt);
				}
			}
		}
	
	while (!iDone)
		{
		TInt advance=0;
		
		ReadChunk(&iCurrent);
		
		if (iCurrent.iName == KRiffChunkNameRiff)//we need to look INSIDE the RIFF chunk
			{
			if(iBuffer->Length() < STATIC_CAST(TInt, KRiffContainerChunkHeaderLength))
				User::Leave(KErrCorrupt);
			iRiffChunkLength=iCurrent.iLength + KRiffChunkHeaderLength;
			advance=KRiffContainerChunkHeaderLength;
			}
		else
			{
			advance=iCurrent.iLength + KRiffChunkHeaderLength;		//... and skip all others
			}
		
		if (iCurrent.iName == KRiffChunkNameFmt_)
			AssignChunkTo(&iFormatChunk);
		
		else if (iCurrent.iName == KRiffChunkNameFact)
			AssignChunkTo(&iFactChunk);
		
		else if (iCurrent.iName == KRiffChunkNameData)
			AssignChunkTo(&iDataChunk);
		
		if (iDataChunk.iFound && iFormatChunk.iFound && iFactChunk.iFound)
			{
			iDone=ETrue;
			}
		else
			{//still have chunks to look for
			iPos+=advance;
			if (iPos & 1)
				iPos++;
			
			if ((TUint)iPos>=(TUint)iRiffChunkLength)
				{
				iDone=ETrue;//end of file
				iClipLength = iRiffChunkLength;
				}
			else
				{//make sure we have at least a chunk's worth left in the buffer
				if ((TUint)(iPos-iLastReadPosition) > 
					(TUint)(iBuffer->Length() -KRiffChunkHeaderLength))
					{
					iLastReadPosition=iPos;
					//DoReadL(iLastReadPosition);
					return;
					}	
				}
			}
		}
	
	iClipLength = iRiffChunkLength;
	if (iClipLength == 0) User::Leave(KErrNotFound);
	else if (!(iDataChunk.iFound && iFormatChunk.iFound))
		User::Leave(KErrCorrupt);

	}

void CWavDecodeUtility::ProcessFormatHeaderL()
	{
	TMdaRiffChunk* chunk = &iFormatChunk;
	
	if (!chunk)
		User::Leave(KErrCorrupt);
	
	iLastReadPosition = chunk->iPosition; // Should be beginning of fmt block
	//DoReadL(iLastReadPosition);
	
	// Set the real format
	const TUint8* rawform = iBuffer->Ptr() + iLastReadPosition;	//skip _fmt & length
	iCodecId = Read16(rawform); rawform+=2;
	iChannels = Read16(rawform); rawform+=2;
	if ((iChannels != 1)&&(iChannels != 2))		//only 1 or 2 channels allowed
		User::Leave(KErrCorrupt); 
	
	iSampleRate = Read32(rawform); rawform+=4; // Skip bytes per second estimate
	if (!iSampleRate) 	User::Leave(KErrCorrupt);
	
	iAverageBytesPerSecond = Read32(rawform); rawform+=4;
	iBlockAlign = Read16(rawform); rawform+=2;
	
	iBitsPerSample = Read16(rawform);
	rawform+=2;
	
	switch (iCodecId)
		{
		case KMMFWavFormatTypePcm:
			{
			}
			break;
		case KMMFWavFormatTypeImaAdpcm:
			{
			}
			break;
		case KMMFWavFormatTypeAlaw:
			{
			}
			break;
		case KMMFWavFormatTypeMulaw:
			{
			}
			break;
		case KMMFWavFormatTypeGSM610:
			{
			}
			break;
		default:
			User::Leave(KErrNotSupported);
		}
	
	if (iCodecId == KMMFWavFormatTypeImaAdpcm)
		{
		TUint16 extraData = Read16(rawform);
		if (extraData == 2)
			{
			rawform+=2;
			iSamplesPerBlock = Read16(rawform);
			rawform+=2;
			}
		}
	
	// Is there a fact chunk?
	if (iFactChunk.iFound)			
		iHasFactChunk = ETrue;
	
	// Find the data block
	chunk=&iDataChunk;
	iStartPosition = chunk->iPosition; 
	iDataLength = chunk->iLength;
	}
