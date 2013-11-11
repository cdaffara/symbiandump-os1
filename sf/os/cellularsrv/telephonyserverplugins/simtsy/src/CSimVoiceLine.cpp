// Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimVoiceLineTraces.h"
#endif

#include "CSimPhone.h"
#include "CSimVoiceCall.h"
#include "et_struct.h"

_LIT(KCommonCallName,"VoiceCall%d");	// < Voice call name template.
const TInt KVoiceCallGranularity=2;		// < Granularity of voice call list array.

/**
* @file
*
* This file contains the implementation of the Similator TSY Voice line functionality.  
* The line classes process the line-based requests made by ETel clients 
* and passed down to the TSY by the ETel Server.
*/

CSimVoiceLine* CSimVoiceLine::NewL(CSimPhone* aPhone,const TDesC& aName)
/**
* Standard two phase constructor.
*
* @param aPhone pointer to the phone object.
* @param aName name of the line to be constructed
* @return CSimVoiceLine  pointer to the voice line object created
* @leave Leaves if no memory or object is not created for any reason
*/
	{
	CSimVoiceLine* voiceLine=new(ELeave) CSimVoiceLine(aPhone);
	TCleanupItem newLineVoiceClose(CloseLine,voiceLine);
	CleanupStack::PushL(newLineVoiceClose);
	voiceLine->ConstructL(aName);
	CleanupStack::Pop();
	return voiceLine;
	}

CSimVoiceLine::CSimVoiceLine(CSimPhone* aPhone)
	:CSimLine(aPhone)
/**
*	Trivial constructor. Calls CSimLine to initialise its members
*/
	{
	iICProperty.iCategory = KUidPSSimTsyCategory;
	iICProperty.iKey = KPSSimTsyIncomingVoiceCall;
	iICProperty.iType = KPSSimTsyIncomingVoiceCallKeyType;

	iRHProperty.iCategory = KUidPSSimTsyCategory;
	iRHProperty.iKey = KPSSimTsyRemoteHangup;
	iRHProperty.iType = KPSSimTsyRemoteHangupKeyType;
	}

void CSimVoiceLine::ConstructL(const TName& aName)
/**
* Pre-allocate a pool of voice calls and allocate one to be the default to
* contain information about an incoming call.
*
* @param aName name of the voice line to be constructed
*/
	{
	iCaps=Caps();
	CSimLine::ConstructL(aName);
	iCalls=new(ELeave) CArrayFixFlat<CSimCall*>(KVoiceCallGranularity);
	TName callName;
	iSpareCall=CreateNewCallL(callName,ECallTypeSpareCall);
	iAnswerNextIncomingCall=iSpareCall;
	}

CSimVoiceLine::~CSimVoiceLine()
/**
* Destroy the spare call - all the others will be closed by the client (or server).
*/
	{
	if ((iAnswerNextIncomingCall!=iSpareCall) && (iAnswerNextIncomingCall))
		{
		iAnswerNextIncomingCall->Close();
		}
	if(iSpareCall)
		{
		iSpareCall->Close();
		}
	if(iCalls)
		{
		iCalls->Delete(0,iCalls->Count());
		delete iCalls;
		}
	}

CTelObject* CSimVoiceLine::OpenNewObjectByNameL(const TDesC& aName)
/**
* Opens a voice call by name. 
* This will be called if the user opens a pre-alloc'ed call by name.
*
* @param aName name of call to be opened
* @return CTelObject pointer to the object opened by name
* @leave Leaves if name given does not match the required name
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICELINE_OPENNEWOBJECTBYNAMEL_1, ">>CSimVoiceLine::OpenNewObjectByNameL");
	TInt i;
	TInt count=iCalls->Count();

	for(i=0;i<count;i++)
		{
		OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICELINE_OPENNEWOBJECTBYNAMEL_2, ">>CSimVoiceLine::OpenNewObjectByNameL %S", iCalls->At(i)->iName);
		if(iCalls->At(i)->iName.MatchF(aName)==0)
			{
			iCalls->At(i)->Open();			
			return iCalls->At(i);
			}
		}

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICELINE_OPENNEWOBJECTBYNAMEL_3, "<<CSimVoiceLine::OpenNewObjectByNameL");
	User::Leave(KErrNotFound);
	return NULL;
	}

CTelObject* CSimVoiceLine::OpenNewObjectL(TDes& aNewName)
/**
* Open a voice call and return a name. 
* This function creates a new call object and returns its pointer.
*
* @param aNewName name of call to be opened
* @return CTelObject pointer to the object allocated
* @leave Leaves if no memory available
*/
	{
	return CreateNewCallL(aNewName,ECallTypeNormalCall);
	}

CSimCall* CSimVoiceLine::CreateNewCallL(TDes& aNewName,TCallType aCallType)
	{
	aNewName.Format(KCommonCallName,iCallCnt++);
	CSimVoiceCall* newCall=CSimVoiceCall::NewL(this,aNewName,iPhone);	
	CleanupStack::PushL(newCall);
	iCalls->AppendL(newCall);
	if(aCallType!=ECallTypeSpareCall)
		{
		HandleNewCallAddedNotification(aNewName);
		}
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMVOICELINE_CREATENEWCALLL_1, ">>CSimVoiceLine::CreateNewCallL 0x%08x",newCall);
	CleanupStack::Pop(newCall);
	return newCall;
	}

TInt CSimVoiceLine::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage)
/**
* ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request 
* for the TSY to process
* A request handle, request type and request data are passed to the TSY
*
* @param aTsyReqHandle
* @param aIpc IPc number representing the request
* @param aPackage data for the request
* @return KErrNone
*/
	{

	TAny* dataPtr=aPackage.Ptr1();

	// The request data has to extracted from TDataPackage and the TAny* pointers have to
	// be "cast" to the expected request data type

	switch (aIpc)
		{
//
// No Flow Control OR Multiple Completion
//
	case EMobileLineGetMobileLineStatus:
		return GetMobileLineStatus(aTsyReqHandle,
			REINTERPRET_CAST(RMobileCall::TMobileCallStatus*,dataPtr));

//
// Multiple Completion Services with Immediate Server Repost
// (Usually Notifications)
//
	case EMobileLineNotifyMobileLineStatusChange:
		return NotifyMobileLineStatusChange(aTsyReqHandle,
			REINTERPRET_CAST(RMobileCall::TMobileCallStatus*, dataPtr));

	default:
		return KErrNotSupported;
		}
	}

TInt CSimVoiceLine::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
/**
 * Cancel an outstanding request.
 * @param aIpc The IPC number of the request that must be cancelled.  Note: this is not the
 *             IPC number of the cancel request itself.
 * @param aTsyReqHandle The TSY Request Handle of the request to be cancelled.
 */
	{
	switch(aIpc)
		{
	case EEtelLineNotifyStatusChange:
		// Always returns KErrNone
		(void)NotifyStatusChangeCancel(aTsyReqHandle);
		return KErrNone;

	case EEtelLineNotifyIncomingCall:
		// Always returns KErrNone
		(void)NotifyIncomingCallCancel(aTsyReqHandle);
		return KErrNone;

	case EEtelLineNotifyHookChange:
		// Always returns KErrNone
		(void)NotifyHookChangeCancel(aTsyReqHandle);
		return KErrNone;

	case EEtelLineNotifyCallAdded:
		// Always returns KErrNone
		(void)NotifyCallAddedCancel(aTsyReqHandle);
		return KErrNone;

	case EMobileLineNotifyMobileLineStatusChange:
		// Always returns KErrNone
		(void)NotifyMobileLineStatusChangeCancel(aTsyReqHandle);
		return KErrNone;

	default:
		break;
		}

	return CLineBase::CancelService(aIpc,aTsyReqHandle);
	}

TInt CSimVoiceLine::EnumerateCall(const TTsyReqHandle aTsyReqHandle,TInt* aParams)
/**
* Count and return the number of voice calls used.
*
* @param aTsyReqHandle
* @return KErrNone
* @param aParams pointer to the number of calls used
*/
	{
	*aParams=iCalls->Count();
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimVoiceLine::GetCallInfo(const TTsyReqHandle aTsyReqHandle,TCallInfoIndex* aCallInfoIndex)
/**
* Retrieve the Call Information relevant to the numbered call.
* Returns the name, the current call status and the call capabilities
* TCallInfoIndex specifies which call info is requested.
*
* @param aTsyReqHandle
* @param aCallInfoIndex pointer to the call info
* @return KErrNone
*/
	{
	TInt& i=aCallInfoIndex->iIndex;
	aCallInfoIndex->iInfo.iCallName.Copy(iCalls->At(i)->iName);
	aCallInfoIndex->iInfo.iStatus=(RCall::TStatus)iCalls->At(i)->iState;
	aCallInfoIndex->iInfo.iCallCapsFlags=iCalls->At(i)->Caps();
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimVoiceLine::FindActiveVoiceCall(CSimVoiceCall*& aCall)
/**
 * Find an active voice call.  Return KErrNotFound if no voice calls active.
 * @param aCall		Pointer to active voice call.
 * @return TInt		Standard return error.
 */
	{
	TInt i;
	for(i=0;i<iCalls->Count();i++)
		{
		if(iCalls->At(i)->iState==RMobileCall::EStatusConnected)
			{
			aCall=(CSimVoiceCall*)iCalls->At(i);
			return KErrNone;
			}
		}
	return KErrNotFound;
	}

TInt CSimVoiceLine::FindActiveCall(CSimCall*& aCall)
/**
 * Find an active voice call.  Return KErrNotFound if no voice calls active.
 * @param aCall		Pointer to active voice call.
 * @return TInt		Standard return error.
 */
	{
	TInt i;
	for(i=0;i<iCalls->Count();i++)
		{
		if(iCalls->At(i)->iState==RMobileCall::EStatusConnected)
			{
			aCall=(CSimVoiceCall*)iCalls->At(i);
			return KErrNone;
			}
		}
	return KErrNotFound;
	}

TUint CSimVoiceLine::Caps()
/**
 * Return the current capabilities of this line.
 * @return TUint	Current line capabilities.
 */
	{
	TUint caps=RLine::KCapsVoice;
	if(iState==RMobileCall::EStatusIdle)
		caps|=RLine::KCapsEventIncomingCall;
	return caps;
	}
