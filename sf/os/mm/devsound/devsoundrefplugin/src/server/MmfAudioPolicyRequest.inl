// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MMFAUDIOPOLICYREQUEST_INL
#define MMFAUDIOPOLICYREQUEST_INL

inline TInt CMMFAudioPolicyRequest::Priority()  const
{ return (iPriority);}

inline void CMMFAudioPolicyRequest::SetPriority(TInt aPriority)
{ iPriority = aPriority;}

inline TMdaPriorityPreference CMMFAudioPolicyRequest::Pref()  const
{ return (iPref);}

inline void CMMFAudioPolicyRequest::SetPref(TMdaPriorityPreference aPref)
{ iPref = aPref;}

inline TMMFAudioPolicyState CMMFAudioPolicyRequest::State()  const
{ return (iState);}

inline void CMMFAudioPolicyRequest::SetState(TMMFAudioPolicyState aState)
{ iState = aState;}

inline TInt CMMFAudioPolicyRequest::PolicySessionId() const
{ return (iPolicySessionId);}

inline void CMMFAudioPolicyRequest::SetPolicySessionId(TInt aPolicySessionId)
{ iPolicySessionId = aPolicySessionId;}

inline void CMMFAudioPolicyRequest::SetPrioritySettings(const TMMFAudioPolicyPrioritySettings& aPrioritySettings)
{ iPrioritySettings = aPrioritySettings;}

inline TUid CMMFAudioPolicyRequest::NotificationEvent() const
{ return (iNotificationEventUid);}

inline void CMMFAudioPolicyRequest::SetNotificationEvent(TUid aNotificationEventUid)
	{ 
	iNotificationEventUid.iUid = aNotificationEventUid.iUid; 
	SetEventFlag(EFalse);
	}

inline void CMMFAudioPolicyRequest::ResetNotificationEvent(TUid /*aNotificationEventUid*/)
	{ 
	iNotificationEventUid = KNullUid;
	}

inline void CMMFAudioPolicyRequest::SetEventFlag(TBool aEventFlag)
	{ 
	iEventNotified = aEventFlag;
	}

inline TBool CMMFAudioPolicyRequest::IsEventNotified() const
	{ 
	return iEventNotified;
	}

inline TBool CMMFAudioPolicyRequest::Capabilities()  const
	{
	return iCapabilities;
	}
inline void CMMFAudioPolicyRequest::SetCapabilities(TBool aCaps)
	{
	iCapabilities = aCaps;
	}

inline TBool CMMFAudioPolicyRequest::SendEventPending() const
	{
	return iSendEventPending;
	}
	
inline void CMMFAudioPolicyRequest::SetSendEventPending(TBool aValue)
	{
	iSendEventPending = aValue;
	}

inline const TDesC8& CMMFAudioPolicyRequest::RequestData() const
	{
	return iReqDataPtr;
	}

#endif // MMFAUDIOPOLICYREQUEST_INL

