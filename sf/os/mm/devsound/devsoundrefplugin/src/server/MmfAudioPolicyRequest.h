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

#ifndef __MMF_AUDIOPOLICYREQUEST_H__
#define __MMF_AUDIOPOLICYREQUEST_H__

#include <e32base.h>
#include "MmfPolicyClientServer.h" 

enum TPolicyResponse
{
	EProceed,			// resource is available for immediate use
	EDenied,			// request is denied
	EStopThenProceed,	// someone must be stopped to honor this request
	EResume,			
	EMix
};
class CMMFAudioPolicyRequest; // declared here
/******************************************************************************
* Class Name:	CMMFAudioPolicyRequest
* 
* Description:	This class carries request information from the client to the
*				server, upon making a request to proceed with a user's command.
*
******************************************************************************/


NONSHARABLE_CLASS( CMMFAudioPolicyRequest ) : public CBase
/**
*@internalTechnology
*/
	{
public:
	CMMFAudioPolicyRequest();
	~CMMFAudioPolicyRequest();

	// Accessor methods:

	inline void SetPrioritySettings(const TMMFAudioPolicyPrioritySettings& aPrioritySettings);

	inline TInt Priority() const;
	inline void SetPriority(TInt aPriority);

	inline TMMFAudioPolicyState State() const;
	inline void SetState(TMMFAudioPolicyState aState);

	inline TMdaPriorityPreference Pref() const;
	inline void SetPref(TMdaPriorityPreference aPref);

	inline TInt PolicySessionId() const;

	inline TUid NotificationEvent() const;
	inline void SetNotificationEvent(TUid aNotificationEventUid);
	inline void ResetNotificationEvent(TUid aNotificationEventUid);
	inline void SetEventFlag(TBool aEventFlag);
	inline TBool IsEventNotified() const;
	
	inline TBool SendEventPending() const;
	inline void SetSendEventPending(TBool aValue);
	
	void SetRequestDataL(const TDesC8& aRequestData);
	inline const TDesC8& RequestData() const;

	inline TBool Capabilities() const;
	inline void SetCapabilities(TBool aCaps);

private:

	TInt							iPriority;
	TMdaPriorityPreference			iPref;
	TMMFAudioPolicyState			iState;
	TInt							iPolicySessionId;
	TMMFAudioPolicyPrioritySettings	iPrioritySettings;
	TUid							iNotificationEventUid;
	TBool							iEventNotified;
	TBool							iSendEventPending;
	HBufC8*							iRequestData;
	TPtrC8							iReqDataPtr;
	
	TBool							iCapabilities;

	friend class CMMFAudioPolicySession;  // Allow this class to use SetSessionId()
	inline void SetPolicySessionId(TInt aPolicySessionId);
	};

#include "MmfAudioPolicyRequest.inl"

#endif //__MMF_AUDIOPOLICYREQUEST_H__
