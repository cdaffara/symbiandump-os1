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

#ifndef __MMFAUDIOPOLICY_H__
#define __MMFAUDIOPOLICY_H__

#include <e32base.h>
#include <f32file.h>
#include "MmfAudioPolicyRequest.h"	
#include <mmf/server/sounddevice.h>			
#include "MdaHwInfo.h"
#include "MmfDevSoundEventHandler.h"
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>

/** 
Panic category and codes for the AudioPolicy  
@internalTechnology
*/
_LIT(KMMFAudioPolicyPanicCategory, "MMFAudioPolicy");
enum TMMFAudioPolicyPanicCodes
	{
	EMMFAudioPolicyRequestArrayOverflow,
	EMMFAudioPolicySessionEventQueueOverflow
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
	void ModifyEntry(TInt aPolicySessionId,const TMMFAudioPolicyState aNewState);
	void RemoveFromList(TInt aPolicySessionId, TBool aAllowTimerRestart=ETrue);
	void LaunchRequest(TInt aSessionId);
	void ReserveClientNumL(TInt aNum);
	void NotifyNextClient();
	TInt SetNotification(TInt aSessionId,TUid aEventType);
	inline CMdaHwInfo* MdaHwInfo(void) {return iMdaHwInfo;} 
protected:
	CAudioPolicy(CMMFAudioPolicyServer* aAudioPolicyServer);
private:
	enum {EGranularity=8};
	typedef CMMFAudioPolicyRequest* PtrPolicyRequest;
	typedef CArrayFixFlat<PtrPolicyRequest> CPolicyReqPtrArray;
	CPolicyReqPtrArray* iAudioPolicyRequestArray; 

	CMMFAudioPolicyServer* iAudioPolicyServer;
	CMdaHwInfo* iMdaHwInfo;
	TInt iNotifiedSessionId;  // latest attemped session Id
	TInt iSessionIdAwaitingForDevsound;
	TInt iStopHandledFromSessId;

	void ConstructL();

	CMMFAudioPolicyRequest* FindPolicyRequestById(TInt aSessionId) const;
	TPolicyResponse ProcessRequest(CMMFAudioPolicyRequest* aAudioPolicyRequest);
	void HandlePreferences(CMMFAudioPolicyRequest* aAudioPolicyRequest, TInt aPref, TPolicyResponse& aResponse);
	void NotifySessionWithTimeout(TInt aPolicySessionIdx, TMMFAudioPolicyEvent::TAudioPolicyEventType aEvent);
	TBool IsRegisteredNotification(TInt aSessionId) const;
	TInt CheckSessionToNotify();
	};

/**
The function panic raises a panic from within the MmfAudioPolicy library
@param The argument is a member of the enumeration TMMFAudioPolicyPanicCodes
*/
GLDEF_C void Panic(TMMFAudioPolicyPanicCodes aError);

	
#endif


