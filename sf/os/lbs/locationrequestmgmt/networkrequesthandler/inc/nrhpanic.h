// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalTechnology
 @released
*/

#ifndef NRHPANIC_H_
#define NRHPANIC_H_

#include <e32base.h>

_LIT(KNrhPanic, "LBS-NRH");

enum TNrhPanic
	{
	ENrhPanicIdleUnknownExitReason = 1,
	ENrhPanicWaitLocReqUnknownExitReason = 2,
	ENrhPanicWaitPrivRespUnknownExitReason = 3,
	ENrhPanicWaitLocUpdateUnknownExitReason = 4,
	ENrhPanicNoQualityProfile = 5,
	ENrhPanicBadParamType = 6,
	ENrhPanicLocationTimerError = 7,
	ENrhPanicX3pTimerError = 8,
	ENrhPanicExceededMaxNumExternalLocateRequest = 9,
	ENrhPanicUnexpectedNetChannelId = 10,
	ENrhPanicInvalidExternalRequestInfoType = 11,
	ENrhPanicInvalidPowerModeAdviceIndex = 12,
	ENrhPanicInvalidGpsModeCombination = 13,
	ENrhPanicInvalidServerPointer = 14,
	ENrhPanicInvalidStandalonPrivacySessionType = 15,
	ENrhPanicOOMWhenProcessingEmergency = 16

	};
	
void Panic(TNrhPanic aCategory);

#endif // NRHPANIC_H_