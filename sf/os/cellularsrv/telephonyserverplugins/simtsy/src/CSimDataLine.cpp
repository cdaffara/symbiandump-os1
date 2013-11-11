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
#include "CSimDataLineTraces.h"
#endif

#include "CSimPhone.h"
#include "CSimDataCall.h"
#include "et_struct.h"

_LIT(KCommonCallName,"DataCall%d");	// < Data call name template.
const TInt KCallListGranularity=2;		// < The granularity of the call list array.

/**
 * @file
 *
 * This file contains the implementation of the Similator TSY Data line functionality.  
 * The line classes process the line-based requests made by ETel clients 
 * and passed down to the TSY by the ETel Server.
 */

CSimDataLine* CSimDataLine::NewL(CSimPhone* aPhone,const TDesC& aName)
/**
 * Standard two phase constructor.
 *
 * @param aPhone pointer to the phone object.
 * @param aName name of the line to be constructed
 * @return CSimDataLine  pointer to the voice line object created
 * @leave Leaves if no memory or object is not created for any reason
 */
	{
	CSimDataLine* dataLine=new(ELeave) CSimDataLine(aPhone);
	TCleanupItem newLineDataClose(CloseLine,dataLine);
	CleanupStack::PushL(newLineDataClose);
	dataLine->ConstructL(aName);
	CleanupStack::Pop();
	return dataLine;
	}

CSimDataLine::CSimDataLine(CSimPhone* aPhone)
	:CSimLine(aPhone)
/**
 *	Trivial constructor. Calls CSimLine to initialise its members
 */
	{
	iICProperty.iCategory = KUidPSSimTsyCategory;
	iICProperty.iKey = KPSSimTsyIncomingDataCall;
	iICProperty.iType = KPSSimTsyIncomingDataCallKeyType;

	iRHProperty.iCategory = KUidPSSimTsyCategory;
	iRHProperty.iKey = KPSSimTsyRemoteHangup;
	iRHProperty.iType = KPSSimTsyRemoteHangupKeyType;
	}

void CSimDataLine::ConstructL(const TName& aName)
/**
* Pre-allocate a pool of data calls and allocate one to be the default to
* contain information about an incoming call.
*
* @param aName name of the voice line to be constructed
*/
	{
	iCaps=Caps();
	CSimLine::ConstructL(aName);
	iCalls=new(ELeave) CArrayFixFlat<CSimCall*>(KCallListGranularity);
	TName callName;
	iSpareCall=CreateNewCallL(callName,ECallTypeSpareCall);
	iAnswerNextIncomingCall=iSpareCall;
	}

CSimDataLine::~CSimDataLine()
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

CTelObject* CSimDataLine::OpenNewObjectByNameL(const TDesC& aName)
/**
* Opens a data call by name. 
* This will be called if the user opens a pre-alloc'ed call by name.
*
* @param aName			name of call to be opened
* @return CTelObject	pointer to the object opened by name
* @leave			Leaves if name given does not match the required name
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATALINE_OPENNEWOBJECTBYNAMEL_1, ">>CSimDataLine::OpenNewObjectByNameL");
	TInt i;
	for(i=0;i<iCalls->Count();i++)
		{
		if(iCalls->At(i)->iName.MatchF(aName)==0)
			{
			(void)iCalls->At(i)->Open();
			return iCalls->At(i);
			}
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATALINE_OPENNEWOBJECTBYNAMEL_2, "<<CSimDataLine::OpenNewObjectByNameL");
	User::Leave(KErrNotFound);
	return NULL;
	}

CTelObject* CSimDataLine::OpenNewObjectL(TDes& aNewName)
/**
* Open a data call and return a name. 
* This function creates a new call object and returns its pointer.
*
* @param aNewName name of call to be opened
* @return CTelObject pointer to the object allocated
* @leave Leaves if no memory available
*/
	{
	return CreateNewCallL(aNewName,ECallTypeNormalCall);
	}

CSimCall* CSimDataLine::CreateNewCallL(TDes& aNewName,TCallType aCallType)
	{
	aNewName.Format(KCommonCallName,iCallCnt++);
	CSimDataCall* newCall=CSimDataCall::NewL(this,aNewName,iPhone);
	CleanupStack::PushL(newCall);
	iCalls->AppendL(newCall);
	if(aCallType!=ECallTypeSpareCall)
		{
		HandleNewCallAddedNotification(aNewName);
		}
	iNameOfLastCallAdded.Copy(aNewName);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATALINE_CREATENEWCALLL_1, ">>CSimDataLine::CreateNewCallL 0x%08x",newCall);
	CleanupStack::Pop(newCall);
	return newCall;
	}

TInt CSimDataLine::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage)
/**
* ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request 
* for the TSY to process
* A request handle, request type and request data are passed to the TSY
*
* @param aTsyReqHandle
* @param aIpc IPc number representing the request
* @param aPackage data for the request
* @return Symbian system wide error code.
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

TInt CSimDataLine::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
/**
 * Cancel an outstanding request.
 * @param aIpc	The IPC number of the request that must be cancelled.  Note: this is not the
 *				IPC number of the cancel request itself.
 * @param aTsyReqHandle The TSY Request Handle of the request to be cancelled.
 */
	{
	switch(aIpc)
		{
	case EEtelLineNotifyStatusChange:
		return NotifyStatusChangeCancel(aTsyReqHandle);

	case EEtelLineNotifyIncomingCall:
		return NotifyIncomingCallCancel(aTsyReqHandle);

	case EEtelLineNotifyHookChange:
		return NotifyHookChangeCancel(aTsyReqHandle);

	case EEtelLineNotifyCallAdded:
		return NotifyCallAddedCancel(aTsyReqHandle);

	case EMobileLineNotifyMobileLineStatusChange:
		return NotifyMobileLineStatusChangeCancel(aTsyReqHandle);

	default:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDATALINE_CANCELSERVICE_1, "CSimDataLine::CancelService: No IPC match in SIM TSY, defering to core cancelservice function.");
		break;
		}

	return CLineBase::CancelService(aIpc,aTsyReqHandle);
	}

TInt CSimDataLine::EnumerateCall(const TTsyReqHandle aTsyReqHandle,TInt* aParams)
/**
* Count and return the number of data calls used.
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

TInt CSimDataLine::GetCallInfo(const TTsyReqHandle aTsyReqHandle,TCallInfoIndex* aCallInfoIndex)
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
	if((i<0)||(i>iCalls->Count()))
		{
		ReqCompleted(aTsyReqHandle,KErrArgument);
		return KErrNone;
		}

	aCallInfoIndex->iInfo.iCallName.Copy(iCalls->At(i)->iName);
	aCallInfoIndex->iInfo.iStatus=(RCall::TStatus)iCalls->At(i)->iState;
	aCallInfoIndex->iInfo.iCallCapsFlags=iCalls->At(i)->Caps();
	ReqCompleted(aTsyReqHandle,KErrNone);
	return KErrNone;
	}

TUint CSimDataLine::Caps()
/**
 * Return the current capabilities of this line.
 * @return TUint	Current line capabilities.
 */
	{
	TUint caps=RLine::KCapsData;
	if(iState==RMobileCall::EStatusIdle)
		caps|=RLine::KCapsEventIncomingCall;
	return caps;
	}

TInt CSimDataLine::FindActiveCall(CSimCall*& aCall)
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
			aCall=(CSimDataCall*)iCalls->At(i);
			return KErrNone;
			}
		}
	return KErrNotFound;
	}
