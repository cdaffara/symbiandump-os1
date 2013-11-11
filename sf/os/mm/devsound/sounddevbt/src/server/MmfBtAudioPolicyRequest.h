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

#ifndef __MMFBTAUDIOPOLICYREQUEST_H__
#define __MMFBTAUDIOPOLICYREQUEST_H__

#include <e32base.h>
#include "MmfBtPolicyClientServer.h" 

enum TPolicyResponse
{
	EProceed,
	EDenied,
	EStopThenProceed,
	EResume,
	EMix
};

/******************************************************************************
* Class Name:	CMMFAudioPolicyRequest
* 
* Description:	This class carries request information from the client to the
*				server, upon making a request to proceed with a user's command.
*
******************************************************************************/


class CMMFAudioPolicyRequest : public CBase
/**
*@internalTechnology
*/
	{
public:
	inline CMMFAudioPolicyRequest();
	~CMMFAudioPolicyRequest(){};

	// Accessor methods:

	inline void SetPrioritySettings(const TMMFAudioPolicyPrioritySettings& aPrioritySettings);

	inline TInt Priority();
	inline void SetPriority(TInt aPriority);

	inline TMMFAudioPolicyState State();
	inline void SetState(TMMFAudioPolicyState aState);

	inline TMdaPriorityPreference Pref();
	inline void SetPref(TMdaPriorityPreference aPref);

	inline TInt PolicySessionId();

	inline TBool DeviceInUse();
	inline void SetDeviceInUse(TBool aDeviceInUse);
	
	inline TUid NotificationEvent() const;
	inline void SetNotificationEvent(TUid aNotificationEventUid);
	inline void ResetNotificationEvent(TUid aNotificationEventUid);
	inline void SetEventFlag(TBool aEventFlag);
	inline TBool IsEventNotified() const;

	inline TBool Capabilities();
	inline void SetCapabilities(TBool aCaps);

private:

	TInt							iPriority;
	TMdaPriorityPreference			iPref;
	TMMFAudioPolicyState			iState;
	TInt							iPolicySessionId;
	TMMFAudioPolicyPrioritySettings	iPrioritySettings;
	TBool							iDeviceInUse;
	TUid							iNotificationEventUid;
	TBool							iEventNotified;

	TBool							iCapabilities;

	friend class CMMFAudioPolicySession;  // Allow this class to use SetSessionId()
	inline void SetPolicySessionId(TInt aPolicySessionId);
	};

#include "MmfBtAudioPolicyRequest.inl"

#endif //__MMFBTAUDIOPOLICYREQUEST_H__
