// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __MMF_BT_AUDIOCODEC_H__
#define __MMF_BT_AUDIOCODEC_H__
#include <e32std.h>

/*
*
* class TMmfAudioAudioCodecPanics provides a name space for audio codec
* panics
*
**/
class TMmfAudioCodecPanicsNameSpace
{
public:
	/*
	 A list of panic codes for Codecs
	@internalTechnology
	 EBadArgument indicates a bad argument
	 EBadState indicates a state viaolation
	 EBadInvariant indicates an invariant violation
	 EPostConditionViolation indicates a post condition violation
	*/
	enum TMmfAudioCodecPanics
		{
		EBadArgument,
		EBadState,
		EBadInvariant,
		EBadReset,
		EPostConditionViolation
		};
public:
	inline static void Panic(const TMmfAudioCodecPanicsNameSpace::TMmfAudioCodecPanics aPanic);
};

/*
*
* Panic
* @param aPanic is the panic code for the codec
*
*/
inline void TMmfAudioCodecPanicsNameSpace::Panic(const TMmfAudioCodecPanicsNameSpace::TMmfAudioCodecPanics aPanic)
	{
	_LIT(KAudioCodecPanicCategory, "AudioCodec");
	User::Panic(KAudioCodecPanicCategory, aPanic);
	}

#endif //__MMF_BT_AUDIOCODEC_H__
