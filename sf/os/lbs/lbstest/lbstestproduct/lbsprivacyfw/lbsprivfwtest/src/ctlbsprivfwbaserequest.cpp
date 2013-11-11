// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file ctlbsprivfwbaserequest.cpp
*/


#include "ctlbsprivfwbaserequest.h"
#include "ctlbsprivfwstepbase.h"

CT_LbsPrivFwBaseRequest::~CT_LbsPrivFwBaseRequest()
/**
 * 	Destructor
 */
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwBaseRequest::~CT_LbsPrivFwBaseRequest"));
	delete iRequestInfo;
	}
	

CT_LbsPrivFwBaseRequest::CT_LbsPrivFwBaseRequest()
/**
 * 	Constructor - will not leave
 */
 	{ 	

	}


void CT_LbsPrivFwBaseRequest::ConstructL(CPrivFwProxy* aProxy, CPrivFwApiWrapper* aNetPrivacy, CT_LbsPrivFwApiDataObserver* aPrivacyObserver, TPrivacyHandlerType aPrivacyHandler, CT_LbsPrivFwStepBase* aStep)
	{
	iProxy = aProxy;
	iNetPrivacy = aNetPrivacy;
	iApiObserver = aPrivacyObserver;
	iPrivacyHandler = aPrivacyHandler;
	iStep = aStep; 
	INFO_PRINTF1(_L("CT_LbsPrivFwBaseRequest::ConstructL()"));
	}


void CT_LbsPrivFwBaseRequest::ProcessPrivApiData(TInt /*aRequestId*/, TInt /*aCompletionCode*/)
	{
	// Any General Validation of Response returned to API
	}


bool CT_LbsPrivFwBaseRequest::CompareRequestInfoType(TInt aExternalType, TInt aCposType)
	{
	  //EFormatUnknown
	if(aExternalType == 0)
		{
		return EFalse;
		}
	//EFormatLogicalName     EIdTypeLogicalName
	else if(aExternalType == 1 && aCposType == 0)
		{
		return ETrue;
		}
	//EFormatEmailAddress        EIdTypeEmailAddress
	else if(aExternalType == 2 && aCposType == 2)
		{
		return ETrue;
		}
	//EFormatMSISDN				  EIdTypeMSISDN
	else if(aExternalType == 3 && aCposType == 1)
		{
		return ETrue;
		}
	//EFormatURL				  EIdTypeURL
	else if(aExternalType == 4 && aCposType == 3)
		{
		return ETrue;
		}
	//EFormatSIPURL				  EIdTypeSIPURL
	else if(aExternalType == 5 && aCposType == 4)
		{
		return ETrue;
		}
	//EFormatIMSPublicIdentity				  EIdTypeIMSPublicIdentity
	else if(aExternalType == 6 && aCposType == 5)
		{
		return ETrue;
		}
	//EFormatMIN				  EIdTypeMIN
	else if(aExternalType == 7 && aCposType == 6)
		{
		return ETrue;
		}
	//EFormatMDN				  EIdTypeMDN
	else if(aExternalType == 8 && aCposType == 7)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

void CT_LbsPrivFwBaseRequest::SetResponseDelay(TUint aResponseDelay)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwBaseRequest::SetResponseDelay()"));
	iResponseDelay = aResponseDelay;
	}

void CT_LbsPrivFwBaseRequest::ProcessPrivNotifierData(TInt /*aRequestId*/, TLbsTestRequestInfo& aPrivNotifierData)
	{
	// General Validation of data given to Notifier side
	INFO_PRINTF1(_L("CT_LbsPrivFwBaseRequest::ProcessPrivNotifierData()"));
	VerifyPrivNotifierData(iRequestInfo, aPrivNotifierData);
	}

void CT_LbsPrivFwBaseRequest::ProcessPrivRequestComplete(TInt /*aRequestId*/, TInt /*aCompletionCode*/)
	{
	
	}

void CT_LbsPrivFwBaseRequest::ProcessPrivNotiferCtrlCloseDown()
	{
	
	}

void CT_LbsPrivFwBaseRequest::VerifyPrivNotifierData(CPosNetworkPrivacyRequestInfo* aNetPrivReqInfo, const TLbsTestRequestInfo& aExtReqInfo)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwBaseRequest::VerifyPrivNotifierData()"));

	TBuf<KLbsMaxClientNameSize2> testNotifClientName;
	TLbsTestRequestInfo::TFormatIndicator testNotifClientId;
	aExtReqInfo.GetClientName(testNotifClientName);
	testNotifClientId = aExtReqInfo.ClientNameFormat();
	
	TBuf<KLbsMaxRequesterIdSize2> testNotifRequesterName;
	TLbsTestRequestInfo::TFormatIndicator testNotifRequesterId;
	aExtReqInfo.GetRequesterId(testNotifRequesterName);
	testNotifRequesterId = aExtReqInfo.RequesterIdFormat();

	TLbsTestRequestInfo::TRequestType storedRequestType;
	TLbsTestRequestInfo::TRequestType receivedRequestType;

	receivedRequestType = aExtReqInfo.RequestType();
	
	TPtrC testApiClient;
	TPtrC testApiRequestor;
	
	if(iRequestInfoType == ERequestInfoTypeSupl)
		{
		CPosSUPLPrivacyRequestInfo* suplReqInfo = static_cast<CPosSUPLPrivacyRequestInfo*>(aNetPrivReqInfo);
		CPosSUPLPrivacyRequestInfo::TIdType testApiRequestorId;
		CPosSUPLPrivacyRequestInfo::TIdType testApiClientId;	
		
		suplReqInfo->GetLCSClient(testApiClient, testApiClientId);
		suplReqInfo->GetRequestor(testApiRequestor, testApiRequestorId);
		storedRequestType = suplReqInfo->RequestType();
		
		if(storedRequestType != receivedRequestType)
			{
			ERR_PRINTF3(_L("CT_LbsPrivFwBaseRequest::VerifyPrivNotifierData() - Failed Client RequestTypes Differ, sent value %d, recieved value %d."), storedRequestType, receivedRequestType);
			iStep->SetTestStepResult(EFail);
			}
		
		if(!CompareRequestInfoType(testNotifClientId, testApiClientId))
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwBaseRequest::VerifyPrivNotifierData() - Failed Client Ids Differ"));
			iStep->SetTestStepResult(EFail);
			}
		if(!CompareRequestInfoType(testNotifRequesterId, testApiRequestorId))
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwBaseRequest::VerifyPrivNotifierData() - Failed Requester Ids Differ"));
			iStep->SetTestStepResult(EFail);
			}
		}
	else if(iRequestInfoType == ERequestInfoTypeGsm)
		{
		CPosGSMPrivacyRequestInfo* gsmReqInfo = static_cast<CPosGSMPrivacyRequestInfo*>(aNetPrivReqInfo);
		CPosGSMPrivacyRequestInfo::TIdType testApiRequestorId;
		CPosGSMPrivacyRequestInfo::TIdType testApiClientId;
		
		gsmReqInfo->GetLCSClient(testApiClient, testApiClientId);
		gsmReqInfo->GetRequestor(testApiRequestor, testApiRequestorId);
		storedRequestType = gsmReqInfo->RequestType();
		
		if(storedRequestType != receivedRequestType)
			{
			ERR_PRINTF3(_L("CT_LbsPrivFwBaseRequest::VerifyPrivNotifierData() - Failed Client RequestTypes Differ, sent value %d, recieved value %d."), storedRequestType, receivedRequestType);
			iStep->SetTestStepResult(EFail);
			}
			
		if(!CompareRequestInfoType(testNotifClientId, testApiClientId))
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwBaseRequest::VerifyPrivNotifierData() - Failed Client Ids Differ"));
			iStep->SetTestStepResult(EFail);
			}
		if(!CompareRequestInfoType(testNotifRequesterId, testApiRequestorId))
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwBaseRequest::VerifyPrivNotifierData() - Failed Requester Ids Differ"));
			iStep->SetTestStepResult(EFail);
			}
		}
	// Notifier Names may be truncated as a shorter buffer is used.
	if(iPrivacyHandler != EPrivacyHandleByNotifier)
		{
		if(testApiClient.CompareC(testNotifClientName) != 0)
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwBaseRequest::VerifyPrivNotifierData() - Failed Client Names Differ"));
			iStep->SetTestStepResult(EFail);
			}
		if(testApiRequestor.CompareC(testNotifRequesterName) != 0)
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwBaseRequest::VerifyPrivNotifierData() - Failed Requester Names Differ"));
			iStep->SetTestStepResult(EFail);
			}
		}
	else
		{
		/**
		 * Notifier validation here
		 * -Requestor Id should be max32 characters
		 * -Client name should be max64 Characters
		 * -The first 32/64 or less characters of each should match.
		 * testApiClient is longest, testNotifClientName comes back from notifier and SHOULD be truncated
		 */
		

		TPtrC rwClientName(testNotifClientName);
		TPtrC rwRequesterName(testNotifRequesterName);
		
		TInt numOfCharactersInClientName = rwClientName.Length();
		TInt numOfCharactersInRequesterName = rwRequesterName.Length();
		
		if(	numOfCharactersInClientName > KLbsMaxClientNameSize)
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwBaseRequest::VerifyPrivNotifierData() - Failed, Client too long."));
			iStep->SetTestStepResult(EFail);
			}
		
		if(	numOfCharactersInRequesterName > KLbsMaxRequesterIdSize)
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwBaseRequest::VerifyPrivNotifierData() - Failed, Requestor too long."));
			iStep->SetTestStepResult(EFail);
			}
		
		if( testApiClient.Find(testNotifClientName) == KErrNotFound ||
			testApiClient.Find(testNotifClientName) > 0	)
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwBaseRequest::VerifyPrivNotifierData() - Failed, Client name mismatch."));
			iStep->SetTestStepResult(EFail);
			}
		
		if( testApiRequestor.Find(testNotifRequesterName) == KErrNotFound ||
			testApiRequestor.Find(testNotifRequesterName) > 0 )
			{
			ERR_PRINTF1(_L("CT_LbsPrivFwBaseRequest::VerifyPrivNotifierData() - Failed, Requester name mismatch."));
			iStep->SetTestStepResult(EFail);
			}
		
		}
	}


void CT_LbsPrivFwBaseRequest::SetRequestInfo(CPosNetworkPrivacyRequestInfo* aRequestInfo, TPosNetworkPrivacyRequestInfoType aPrivacyInfoType)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwBaseRequest::SetRequestInfo()"));
	iRequestInfo = aRequestInfo;
	iRequestInfoType = aPrivacyInfoType;
	iNetPrivacy->SetRequestInfoType(iRequestInfoType);
	if(iRequestInfo->RequestType() == CPosNetworkPrivacyRequestInfo::ERequestTypeUnknown)
		{
		iRequestInfo->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		}
	}

void CT_LbsPrivFwBaseRequest::SetRequestId(TInt aRequestId)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwBaseRequest::SetRequestId()"));
	iRequestId = aRequestId;
	}

void CT_LbsPrivFwBaseRequest::SetResponseDelay(TTimeIntervalMicroSeconds aResponseDelay)
	{
	INFO_PRINTF1(_L("CT_LbsPrivFwTimeoutRequest::SetResponseDelay()"));
	iResponseDelay = aResponseDelay;
	}

// Getters
TInt CT_LbsPrivFwBaseRequest::RequestId()
	{
	//INFO_PRINTF1(_L("CT_LbsPrivFwBaseRequest::RequestId()"));
	return iRequestId;
	}

CPosNetworkPrivacyRequestInfo& CT_LbsPrivFwBaseRequest::RequestInfo()
	{
	//INFO_PRINTF1(_L("CT_LbsPrivFwBaseRequest::RequestInfo()"));
	return *iRequestInfo;
	}

TPosNetworkPrivacyRequestInfoType CT_LbsPrivFwBaseRequest::RequestInfoType()
	{
	//INFO_PRINTF1(_L("CT_LbsPrivFwBaseRequest::RequestInfoType()"));
	return iRequestInfoType;
	}

CT_LbsPrivFwBaseRequest::TCurReqState CT_LbsPrivFwBaseRequest::State()
	{
	//INFO_PRINTF1(_L("CT_LbsPrivFwBaseRequest::GetState()"));
	return iState;
	}

// Enables TEF PRINT macros
CTestExecuteLogger& CT_LbsPrivFwBaseRequest::Logger()
	{
	return iStep->Logger();
	}

TBool CT_LbsPrivFwBaseRequest::IsRequestComplete()
	{
	return (iState >= EReqEndOfRange);
	}


//OMA SUPL Pos Handler
const TUid KOMASuplHandlerUid = {0x102871EF};


// Start and finish a positioning session, to end the session with LBS	
void CT_LbsPrivFwBaseRequest::TerminateSessionL(TInt aRequestId)
	{
    __ASSERT_ALWAYS(iStep->NeedSessionTerminate(), User::Invariant());
    
	iStep->INFO_PRINTF2(_L("CT_LbsPrivFwBaseRequest::TerminateSessionL(0x%x)++"), aRequestId);
	COMASuplPosHandlerBase* posHandler = COMASuplPosHandlerBase::NewL(KOMASuplHandlerUid);
	if(posHandler)
		{
		iStep->INFO_PRINTF1(_L("posHandler"));
		CleanupStack::PushL(posHandler);
		
		TRequestStatus status;
		posHandler->InitializeL(status);
		User::WaitForRequest(status);
		if (status.Int() != KErrNone)
			{
			iStep->INFO_PRINTF1(_L("InitializeL() failed!\n"));
			iStep->SetTestStepResult(EFail);
			}	
		else
			{
			COMASuplPosSessionBase* posSession = posHandler->CreateNewSessionL(this);
			if(posSession == NULL)
				{
				iStep->INFO_PRINTF1(_L("CreateNewSessionL() failed!\n"));
				iStep->SetTestStepResult(EFail);
				}
			else
				{
				CleanupStack::PushL(posSession);
				posSession->InitializeL(aRequestId, status);
				CleanupStack::Pop(posSession);
				User::WaitForRequest(status);
				posSession->SessionEnd();	
				iStep->SetTestStepResult(EPass);
				delete posSession;
				}
			}	
		iStep->INFO_PRINTF1(_L("pop and delete posHandler"));
		CleanupStack::Pop(posHandler);
		delete posHandler;	
		}
	else
		{
		iStep->SetTestStepResult(EPass);
		}
	iStep->INFO_PRINTF1(_L("CT_LbsPrivFwBaseRequest::TerminateSessionL()--"));
	}

//-----------------------------------------------------
// Functions from MOMASuplObserver
//-----------------------------------------------------

/** FROM MOMASuplObserver
 *   Returns the version of SUPL supported by the framework
 *   
 *   @return TReal value containing the version of SUPL. 
 */
TReal CT_LbsPrivFwBaseRequest::SuplVersion()
	{
	//Always claim to support SUPL version 2
	return 2.0;
	}

/** FROM MOMASuplObserver
 *   Called by the SPPM to terminate the current session
 */
void CT_LbsPrivFwBaseRequest::TerminateSession()
	{
	//Not expected to ever be received from the SPPM, fail the test
	//INFO_PRINTF1(_L("Unexpected TerminateSession"));
	iStep->SetTestStepResult(EFail);	
	}

