/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#ifndef __MMF_TMMFTABLEAUDIOCODEC_H__
#define __MMF_TMMFTABLEAUDIOCODEC_H__

#include <e32std.h>
#include "MMFAudioCodecBase.h"

class TMMFTableAudioCodec : public TMMFAudioCodecBase
	{
public:
	inline TMMFTableAudioCodec();
	//
	inline TInt TableSize();
	inline void SetTable(TPtr8 aTable);
	//
	virtual void FillTable(TPtr8 aTable)=0;
	IMPORT_C	virtual void Convert(TUint8* aSrc, TUint8* aDst, TInt aSamples);
protected:
	virtual void ConvertSlow(TUint8* aSrc, TUint8* aDst, TInt aSamples)=0;
	virtual void ConvertFast(TUint8* aSrc, TUint8* aDst, TInt aSamples)=0;
protected:
	TInt iTableSize;
	TUint8* iTablePtr;
	};

/**
* Inline functions
**/
inline TMMFTableAudioCodec::TMMFTableAudioCodec()
	: iTablePtr(0) 
	{
	}

inline TInt TMMFTableAudioCodec::TableSize()
	{ 
	return iTableSize; 
	}

inline void TMMFTableAudioCodec::SetTable(TPtr8 aTable)
	{ 
	ASSERT(aTable.Length()>iTableSize); 
	iTablePtr = &aTable[0]; 
	}

#endif

