// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "graphics/WSGRAPHICMSGBUF.H"
#include "W32STDGRAPHIC.H"

//so sue us; fixing a very nasty-to-track-down unsigned/signed conversion defect in code below
#define KSizeOfTInt (TInt)sizeof(TInt)

// TWsGraphicMsgBufParser \\\\\\\\\\\\\\\\\\\\\\\\

EXPORT_C TWsGraphicMsgBufParser::TWsGraphicMsgBufParser(const TDesC8& aData): iData(aData)
/** Initialise a parser for the specified message buffer */
	{
	}

EXPORT_C TInt TWsGraphicMsgBufParser::Verify() const
/** Verifies that the message buffer is properly formed
	@return KErrNone if buffer is ok, else a system-wide error code */
	{
	const TInt length = iData.Length();
	TInt ofs = 0;
	const TInt tmp = (length - sizeof(TInt));
 	while(ofs < tmp)
		{
		ofs += IntAt(ofs+KSizeOfTInt) + (KSizeOfTInt*2);
		}
	if(ofs == length)
		{
		return KErrNone;
		}
	return KErrCorrupt;
	}

EXPORT_C TInt TWsGraphicMsgBufParser::Count() const
/** Returns the number of elements in the buffer.
@return the count of elements.
*/
	{
	const TInt length = iData.Length();
	TInt ofs = 0, count = 0;
	while(ofs < length)
		{
		count++;
		ofs += IntAt(ofs+KSizeOfTInt) + (KSizeOfTInt*2);
		}
	return count;
	}

EXPORT_C TUid TWsGraphicMsgBufParser::Uid(TInt aIndex) const
/** Returns the UID if the message, or null UID if the buffer is empty.
@return KNullUid if the buffer is empty, otherwise the stored Uid
*/
	{
	const TInt length = iData.Length();
	TInt ofs = 0, count = 0;
	while(ofs < length)
		{
		if(count == aIndex)
			{
			return TUid::Uid(IntAt(ofs));
			}
		count++;
		ofs += IntAt(ofs+KSizeOfTInt) + (KSizeOfTInt*2);
		}
	return KNullUid;
	}

EXPORT_C TPtrC8 TWsGraphicMsgBufParser::Data(TInt aIndex) const
/**Returns the buffer contents at a perticular offset (aIndex).
@param aIndex - the index into of the buffer element required.
@return KNullDesC8 if index is more than the element count, otherwise, the element at index aIndex.
*/
	{
	const TInt length = iData.Length();
	TInt ofs = 0, count = 0;
	while(ofs < length)
		{
		if(count == aIndex)
			{
			return iData.Mid(ofs+(KSizeOfTInt*2),IntAt(ofs+KSizeOfTInt));
			}
		count++;
		ofs += IntAt(ofs+KSizeOfTInt) + (KSizeOfTInt*2);
		}
	return TPtrC8(KNullDesC8());
	}

EXPORT_C TInt TWsGraphicMsgBufParser::Find(TUid aUid,TInt aStartingFrom) const
/** Finds the element equal to the aUid, and returns the index of the element in the buffer.
@param aUid - the search item to be found in the buffer.
@param aStartingFrom - the starting index.
@return the position (index) of the found element, or KErrNotFound
*/
	{
	const TInt length = iData.Length();
	TInt ofs = 0, count = 0;
	while(ofs < length)
		{
		if((count >= aStartingFrom) && (aUid == TUid::Uid(IntAt(ofs))))
			{
			return count;
			}
		count++;
		ofs += IntAt(ofs+KSizeOfTInt) + (KSizeOfTInt*2);
		}
	return KErrNotFound;
	}

EXPORT_C TBool TWsGraphicMsgBufParser::LoadFixed(TUid aUid,TAny* aMsg,TInt aMsgSize,TInt aStartingFrom) const
/** Loads the buffer of aMsg with the contents of the buffer, based on the aUid and aMsgSize.
@param aUid - the Uid to match after which the loading is performed.
@param aMsg - the pointer to the output buffer.
@param aMsgSize - the size of the output buffer.
@param aStartingFrom - the starting position to be used in the search of the buffer.
@return ETrue if loaded, EFalse otherwise.
*/
	{
	const TInt length = iData.Length();
	TInt ofs = 0, count = 0;
	while(ofs < length)
		{
		if((count >= aStartingFrom) && (aUid == TUid::Uid(IntAt(ofs))))
			{
			// found it?  return it
			const TInt len = IntAt(ofs+KSizeOfTInt);
			if(len == aMsgSize)
				{
				TPtr8 msg(reinterpret_cast<TUint8*>(aMsg),aMsgSize);
				msg = iData.Mid(ofs+(KSizeOfTInt*2),len);
				return ETrue;
				}
			else // message was not the expected size!
				{
				return EFalse;
				}
			}
		count++;
		ofs += IntAt(ofs+KSizeOfTInt) + (KSizeOfTInt*2);
		}
	return EFalse;
	}

TInt TWsGraphicMsgBufParser::IntAt(TInt aOfs) const
/** @internalComponent @released */
	{
	if((aOfs < 0) || ((aOfs+KSizeOfTInt) > iData.Length()))
		{
		return 0;
		}
	TInt ret;
	memcpy(&ret,iData.Ptr()+aOfs,KSizeOfTInt);
	return ret;
	}

// TWsGraphicMsgAnimation \\\\\\\\\\\\\\\\\\\\\\\\

EXPORT_C TWsGraphicMsgAnimation::TWsGraphicMsgAnimation(): iFlags(EStopped)
	{
	}

EXPORT_C TInt TWsGraphicMsgAnimation::Load(const TWsGraphicMsgBufParser& aData)
	{
	const TInt index = aData.Find(TUid::Uid(TWsGraphicAnimation::ETypeId));
	if(index >= 0)
		{
		return Load(aData,index);
		}
	return index;
	}

EXPORT_C TInt TWsGraphicMsgAnimation::Load(const TWsGraphicMsgBufParser& aData,TInt aIndex)
	{
	if(aData.Uid(aIndex).iUid != TWsGraphicAnimation::ETypeId)
		{
		return KErrArgument;
		}
	const TPtrC8 pckg = aData.Data(aIndex);
	if(pckg.Size() != sizeof(TWsGraphicMsgAnimation))
		{
		return KErrCorrupt;
		}
	memcpy(this,pckg.Ptr(),sizeof(TWsGraphicMsgAnimation));
	return KErrNone;
	}

EXPORT_C TTimeIntervalMicroSeconds TWsGraphicMsgAnimation::AnimationTime(const TTime& aNow,const TTimeIntervalMicroSeconds& aAnimationLength) const
	{
	// an animation to time?
	if(aAnimationLength <= 0LL)
		{
		return 0LL;
		}
	switch(iFlags & EStateMask)
		{
		case EPaused:
			return ((iPauseOrStopping.Int64() - iPlay.Int64()) % aAnimationLength.Int64());
		case EStopping:
			{
			const TInt64 elapsed = (aNow.Int64() - iPlay.Int64());
			if(elapsed <= aAnimationLength.Int64())
				{
				return (elapsed % aAnimationLength.Int64());
				}
			return 0LL;
			}
		case EStopped:
			return 0LL;
		case EPlaying:
			{
			const TInt64 elapsed = (aNow.Int64() - iPlay.Int64());
			if((iFlags & ELoop) || (elapsed <= aAnimationLength.Int64()))
				{
				return (elapsed % aAnimationLength.Int64());
				}
			return 0LL;
			}
		default:
			return 0LL;
		}
	}

EXPORT_C TBool TWsGraphicMsgAnimation::IsPlaying(const TTime& aNow,const TTimeIntervalMicroSeconds& aAnimationLength) const
	{
	// an animation to time?
	if(aAnimationLength <= 0LL)
		{
		return EFalse;
		}
	switch(iFlags & EStateMask)
		{
		case EPaused:
			return EFalse;
		case EStopping:
			{
			const TInt64 elapsed = (aNow.Int64() - iPlay.Int64());
			if(elapsed <= aAnimationLength.Int64())
				{
				return ETrue;
				}
			return EFalse;
			}
		case EStopped:
			return EFalse;
		case EPlaying:
			{
			const TInt64 elapsed = (aNow.Int64() - iPlay.Int64());
			if((iFlags & ELoop) || (elapsed <= aAnimationLength.Int64()))
				{
				return ETrue;
				}
			return EFalse;
			}
		default:
			return EFalse;
		}
	}

