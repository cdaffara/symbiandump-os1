/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "c32cmiutils.h"

/** A class that differs from TLitC8 only in that it does not have a
dereference operator defined. Assists in building constant arrays
of literal strings, constructed at compile time so no dll data is needed.
@internalComponent
*/
template <TInt S>
class TStLitC8
    {
public:
    inline operator const TDesC8&() const;
    inline const TDesC8& operator()() const;
    inline operator const __TRefDesC8() const;
public:
    TUint iTypeLength;
    TText8 iBuf[__Align8(S)];
    };

template <TInt S>
inline const TDesC8& TStLitC8<S>::operator()() const
    {
    return *REINTERPRET_CAST(const TDesC8*,this);
    }

template <TInt S>
inline TStLitC8<S>::operator const TDesC8&() const
    {
    return *REINTERPRET_CAST(const TDesC8*,this);
    }

template <TInt S>
inline TStLitC8<S>::operator const __TRefDesC8() const
    {
    return *REINTERPRET_CAST(const TDesC8*,this);
    }

/**
@internalComponent
*/
#define _STLIT8(name,s) static const TStLitC8<sizeof(s)> name={sizeof(s)-1,s}

// List of possible priorities
_STLIT8(KEPriorityMuchLess,					"EPriorityMuchLess");
_STLIT8(KEPriorityLess,						"EPriorityLess");
_STLIT8(KEPriorityNormal,					"EPriorityNormal");
_STLIT8(KEPriorityMore,						"EPriorityMore");
_STLIT8(KEPriorityMuchMore,					"EPriorityMuchMore");
_STLIT8(KEPriorityRealTime,					"EPriorityRealTime");
_STLIT8(KEPriorityAbsoluteVeryLow,			"EPriorityAbsoluteVeryLow");
_STLIT8(KEPriorityAbsoluteLowNormal,		"EPriorityAbsoluteLowNormal");
_STLIT8(KEPriorityAbsoluteLow,				"EPriorityAbsoluteLow");
_STLIT8(KEPriorityAbsoluteBackgroundNormal,	"EPriorityAbsoluteBackgroundNormal");
_STLIT8(KEPriorityAbsoluteBackground, 		"EPriorityAbsoluteBackground");
_STLIT8(KEPriorityAbsoluteForegroundNormal,	"EPriorityAbsoluteForegroundNormal");
_STLIT8(KEPriorityAbsoluteForeground,		"EPriorityAbsoluteForeground");
_STLIT8(KEPriorityAbsoluteHighNormal,		"EPriorityAbsoluteHighNormal");
_STLIT8(KEPriorityAbsoluteHigh,				"EPriorityAbsoluteHigh");
_STLIT8(KEPriorityAbsoluteRealTime1,		"EPriorityAbsoluteRealTime1");
_STLIT8(KEPriorityAbsoluteRealTime2,		"EPriorityAbsoluteRealTime2");
_STLIT8(KEPriorityAbsoluteRealTime3,		"EPriorityAbsoluteRealTime3");
_STLIT8(KEPriorityAbsoluteRealTime4,		"EPriorityAbsoluteRealTime4");
_STLIT8(KEPriorityAbsoluteRealTime5,		"EPriorityAbsoluteRealTime5");
_STLIT8(KEPriorityAbsoluteRealTime6,		"EPriorityAbsoluteRealTime6");
_STLIT8(KEPriorityAbsoluteRealTime7,		"EPriorityAbsoluteRealTime7");
_STLIT8(KEPriorityAbsoluteRealTime8,		"EPriorityAbsoluteRealTime8");

#undef _STLIT8

/** priority name-value pair
@internalComponent
*/
struct TPriorityEntry
	{
	const TDesC8* iPriorityName;
	TThreadPriority iPriority;
	};

/**
Look-up table of thread priority name-enum pairs.
Ordered to match the most common comms priorities first.
@internalComponent
*/
static const TPriorityEntry priorityNamePairs[] =
	{
	reinterpret_cast<const TDesC8*>(&KEPriorityMore),						EPriorityMore,
	reinterpret_cast<const TDesC8*>(&KEPriorityMuchMore),					EPriorityMuchMore,
	reinterpret_cast<const TDesC8*>(&KEPriorityRealTime),					EPriorityRealTime,
	reinterpret_cast<const TDesC8*>(&KEPriorityNormal),						EPriorityNormal,
	reinterpret_cast<const TDesC8*>(&KEPriorityLess),						EPriorityLess,
	reinterpret_cast<const TDesC8*>(&KEPriorityMuchLess),					EPriorityMuchLess,
	reinterpret_cast<const TDesC8*>(&KEPriorityAbsoluteHigh),				EPriorityAbsoluteHigh,
	reinterpret_cast<const TDesC8*>(&KEPriorityAbsoluteHighNormal),			EPriorityAbsoluteHighNormal,
	reinterpret_cast<const TDesC8*>(&KEPriorityAbsoluteForeground),			EPriorityAbsoluteForeground,
	reinterpret_cast<const TDesC8*>(&KEPriorityAbsoluteForegroundNormal),	EPriorityAbsoluteForegroundNormal,
	reinterpret_cast<const TDesC8*>(&KEPriorityAbsoluteRealTime1),			EPriorityAbsoluteRealTime1,
	reinterpret_cast<const TDesC8*>(&KEPriorityAbsoluteRealTime2),			EPriorityAbsoluteRealTime2,
	reinterpret_cast<const TDesC8*>(&KEPriorityAbsoluteRealTime3),			EPriorityAbsoluteRealTime3,
	reinterpret_cast<const TDesC8*>(&KEPriorityAbsoluteRealTime4),			EPriorityAbsoluteRealTime4,
	reinterpret_cast<const TDesC8*>(&KEPriorityAbsoluteRealTime5),			EPriorityAbsoluteRealTime5,
	reinterpret_cast<const TDesC8*>(&KEPriorityAbsoluteRealTime6),			EPriorityAbsoluteRealTime6,
	reinterpret_cast<const TDesC8*>(&KEPriorityAbsoluteRealTime7),			EPriorityAbsoluteRealTime7,
	reinterpret_cast<const TDesC8*>(&KEPriorityAbsoluteRealTime8),			EPriorityAbsoluteRealTime8,
	reinterpret_cast<const TDesC8*>(&KEPriorityAbsoluteBackground),			EPriorityAbsoluteBackground,
	reinterpret_cast<const TDesC8*>(&KEPriorityAbsoluteBackgroundNormal),	EPriorityAbsoluteBackgroundNormal,
	reinterpret_cast<const TDesC8*>(&KEPriorityAbsoluteLow),				EPriorityAbsoluteLow,
	reinterpret_cast<const TDesC8*>(&KEPriorityAbsoluteLowNormal),			EPriorityAbsoluteLowNormal,
	reinterpret_cast<const TDesC8*>(&KEPriorityAbsoluteVeryLow),			EPriorityAbsoluteVeryLow
	};

/**
Converts a thread priority name to the corresponding enum value.
@param aPriorityName The input priority name as a descriptor, either a TThreadPriority
enum value such as "10" or a full TThreadPriority enum name such a "EPriorityMore",
case insensitive.
@param aPriority On success, contains the thread priority enumerator value
@return KErrNone if successful, KErrCorrupt if aPriorityName is unrecognised. If priority
is passed in as a number it will not be checked for validity, and KErrNone always returned:
calling code must beware that passing invalid enum values to RThread will result in a panic.
@internalComponent
*/
TInt C32CmiUtils::ThreadPriorityNameToEnum(const TDesC8& aPriorityName, TThreadPriority& aPriority)
	{
	TLex8 name(aPriorityName);
	TInt priorityValue;
	TInt valResult = name.Val(priorityValue);
	if ((valResult == KErrNone) && name.Eos())
		{
		aPriority = (TThreadPriority)priorityValue;
		return KErrNone;
		}
	const TInt numPriorities = sizeof(priorityNamePairs) / sizeof(TPriorityEntry);
	for (TInt i = 0; i < numPriorities; ++i)
		{
		if (aPriorityName.CompareF(*priorityNamePairs[i].iPriorityName) == 0)
			{
			aPriority = priorityNamePairs[i].iPriority;
			return KErrNone;
			}
		}
	return KErrCorrupt;
	}

