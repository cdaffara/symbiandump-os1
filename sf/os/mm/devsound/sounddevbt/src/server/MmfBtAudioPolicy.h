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

#ifndef __MMFBTAUDIOPOLICY_H__
#define __MMFBTAUDIOPOLICY_H__

#include <e32base.h>
#include <f32file.h>
#include "MmfBtAudioPolicyRequest.h"	
#include <mmf/server/sounddevice.h>			
#include "MdaBtHwInfo.h"
#include "MmfBtDevSoundEventHandler.h"
#include "mmfstandardcustomcommands.h"
#include "mmfcontrollerframeworkbase.h"

/** 
Panic category and codes for the AudioPolicy  
@internalTechnology
*/
_LIT(KMMFAudioPolicyPanicCategory, "MMFAudioPolicy");
enum TMMFAudioPolicyPanicCodes
	{
	EMMFAudioPolicyRequestArrayOverflow
	};
	
class CAudioPolicy; // declared here.
/******************************************************************************
* Class Name:	CAudioPolicy
* 
* Description:	This class is ultimately implemented by the product team based
*				on their requirements for audio behavior in the face of 
*				multiple audio requests simultaneously.  The reference 
*				implementation is a simple priority scheme.  If a request is 
*				a higher priority, it gets played, even if termination of a 
*				running request of lower prioirity is required.
******************************************************************************/

class CMMFAudioPolicyServer;

NONSHARABLE_CLASS( CAudioPolicy ): public CBase
/**
*@internalTechnology
*/
	{
public:

	IMPORT_C static CAudioPolicy* NewL(CMMFAudioPolicyServer* aAudioPolicyServer);
	void RemoveSessionFromList(CMMFAudioPolicyRequest* aAudioPolicyRequest);
	~CAudioPolicy();
	void MakeRequest(CMMFAudioPolicyRequest* aAudioPolicyRequest);
	void ModifyEntry(TInt aPolicySessionId, CMMFAudioPolicyRequest* aAudioPolicyRequest);
	void RemoveFromList(TInt aPolicySessionId);
	void LaunchRequest();
	CArrayFixFlat<CMMFAudioPolicyRequest>* AudioPolicyRequestArray();
	void DoSendNotification();
	TInt SetNotification(TInt aSessionId,TUid aEventType);
	inline CMdaHwInfo* MdaHwInfo(void) {return iMdaHwInfo;} 
protected:
	CAudioPolicy(CMMFAudioPolicyServer* aAudioPolicyServer);
private:
	enum {EGranularity=8};
	CArrayFixFlat<CMMFAudioPolicyRequest>* iAudioPolicyRequestArray; 
	TInt iSessionToAlert;
	TInt iSessionToBeLaunched;
	CMMFAudioPolicyServer* iAudioPolicyServer;
	CMdaHwInfo* iMdaHwInfo;
	TMMFAudioPolicyEvent iAudioPolicyEventToLaunch;
	TMMFAudioPolicyEvent iAudioPolicyEvent;

	void ConstructL();
	TPolicyResponse ProcessRequest(CMMFAudioPolicyRequest* aAudioPolicyRequest);
	TPolicyResponse ComparePriorities(CMMFAudioPolicyRequest* aAudioPolicyRequest/*,TMMFAudioPolicyEvent& aEvent*/);
	void HandlePreferences(CMMFAudioPolicyRequest* aAudioPolicyRequest, TInt aPref, TPolicyResponse& aResponse);
	void SetSessionToAlert(TInt aSessionToAlert, TInt aSessionToBeLaunched, TMMFAudioPolicyEvent::TAudioPolicyEventType aEventType, TMMFAudioPolicyState aState );
	TBool IsRegisteredNotification(TInt aSessionId);
	TBool IsNotified();
	void ResetNotification();
	TInt CheckSessionToNotify();
	};

/**
The function panic raises a panic from within the MmfAudioPolicy library
@param The argument is a member of the enumeration TMMFAudioPolicyPanicCodes
*/
GLDEF_C void Panic(TMMFAudioPolicyPanicCodes aError);

#endif
