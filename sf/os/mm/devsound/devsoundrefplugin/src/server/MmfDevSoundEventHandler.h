// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __MMFDEVSOUNDEVENTHANDLER_H
#define __MMFDEVSOUNDEVENTHANDLER_H

#include <e32base.h>
#include <e32std.h>
#include "MmfPolicyClientServer.h"

class CMMFDevSoundSessionXtnd;

class RMMFAudioPolicyProxy;


class TMMFAudioPolicyEvent
	{
public:

/**
	Default constructor.

	Provided so this class can be packaged in a TPckgBuf<>.
	@since 7.0s
*/
	TMMFAudioPolicyEvent() : iEventType(EMMFAudioPolicyNoEvent), iErrorCode(KErrNone) {};
	
	enum TAudioPolicyEventType
		{
		EMMFAudioPolicyNoEvent = 0,
		EMMFAudioPolicySwitchToIdle,
		EMMFAudioPolicyPriorityTooLow,
		EMMFAudioPolicyResourceNotification
		};
		
	TMMFAudioPolicyEvent(TAudioPolicyEventType aType, TInt aError, TMMFAudioPolicyState aState) : 
							iEventType(aType), iErrorCode(aError), iState(aState) {};

	TAudioPolicyEventType iEventType;
	TInt iErrorCode;
	TMMFAudioPolicyState iState;				

	};

typedef TPckgBuf<TMMFAudioPolicyEvent> TMMFAudioPolicyEventPckg;

class CMMFDevSoundEventHandler : public CActive
/**
*@internalTechnology
*/
	{
public:
	static CMMFDevSoundEventHandler* NewL(RMMFAudioPolicyProxy* aAudioPolicyProxy);
	~CMMFDevSoundEventHandler();
	void ReceiveEvents();
	void CancelReceiveEvents();
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	void SetDevSoundInfo(CMMFDevSoundSessionXtnd* aDevSound);

private:
	void ConstructL();
	CMMFDevSoundEventHandler(RMMFAudioPolicyProxy* aAudioPolicyProxy);
private:
	enum {EGranularity=8};
	RMMFAudioPolicyProxy*  iAudioPolicyProxy;
	TMMFAudioPolicyEventPckg	iAudioPolicyEventPckg;
	TRequestStatus	iRequestStatus;
	TInt			iDevSoundId;
	CMMFDevSoundSessionXtnd*	iDevSound;
	};




#endif
