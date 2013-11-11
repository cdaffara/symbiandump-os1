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
// Implements the functionality required to provide clients with
// callforwarding status and registration information.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimCallForwardingTraces.h"
#endif

#include <testconfigfileparser.h>
#include "CSimCallForwarding.h"
#include "CSimPhone.h"

const TInt KSettingListGranularity=5;	// < The granularity used for parameter list arrays.
const TInt KInvalidTimeout = -1;        // Value used when timeout period does not matter
const TInt KMobServiceIndxStart = 1;
const TInt KMobServiceIndxEnd   = 6;

CSimCallForwarding* CSimCallForwarding::NewL(CSimPhone* aPhone)
/**
 * Standard two-phase constructor.
 * @param aPhone				The parent phone object.
 * @return CSimNetworkStatus	The new network status class.
 */
	{
	CSimCallForwarding* self=new(ELeave) CSimCallForwarding(aPhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSimCallForwarding::CSimCallForwarding(CSimPhone* aPhone)
		: iPhone(aPhone)
/**
 * Trivial first phase construction.
 * @param aPhone				The parent phone object.
 */
	{
	}

void CSimCallForwarding::ConstructL()
	/** 
	Second phase of 2-Phase Constructor
 	Retrieves all the Call forwarding and Identity services tags from the config file
	*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_CONSTRUCTL_1, "Starting to parse Call Forwarding config parameters...");

	iIdentityServiceStatus=new(ELeave) CArrayFixFlat<TIdentityServiceStatus>(KSettingListGranularity);
	
	iGetCFStatus = new(ELeave) CArrayPtrFlat<CListReadAllAttempt>(1);
	FindAndCreateCFListL();
	
	// Read in the Identity services information
	TInt count=CfgFile()->ItemCount(KIdentityServiceStatus);
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;
	for(TInt i=0;i<count;i++)
		{
		item=CfgFile()->Item(KIdentityServiceStatus,i);
		if(!item)
			break;

		TInt status, service;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,service);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_CONSTRUCTL_2, "WARNING - CONFIGURATION FILE PARSING - Reading element SERVICE returned %d (element no. %d) from tag %s.",ret,0,KIdentityServiceStatus);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,status);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_CONSTRUCTL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element STATUS returned %d (element no. %d) from tag %s.",ret,1,KIdentityServiceStatus);
			continue;
			}
			
		TIdentityServiceStatus identityServiceStatus;
		identityServiceStatus.iService = static_cast<RMobilePhone::TMobilePhoneIdService>(service);
		identityServiceStatus.iStatus = static_cast<RMobilePhone::TMobilePhoneIdServiceStatus>(status);
		iIdentityServiceStatus->AppendL(identityServiceStatus);
		}
		
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_CONSTRUCTL_4, "...Finished parsing Call Forwarding config parameters...");
	}

CSimCallForwarding::~CSimCallForwarding()
	/** 
	Destroy all the objects constructed.
	*/
	{
	if (iIdentityServiceStatus != NULL)
		{
		iIdentityServiceStatus->Delete(0,iIdentityServiceStatus->Count());
		delete iIdentityServiceStatus;
		}
	delete iCFList;
	if (iGetCFStatus)
		iGetCFStatus->ResetAndDestroy();
	delete iGetCFStatus;
	}


TInt CSimCallForwarding::ExtFunc(const TTsyReqHandle aReqHandle,const TInt aIpc, const TDataPackage& aPckg)
	{
	/**
	Handles all the Call forwarding APIs
	@param aReqHandle to the request
	@param Ipc unique identifier of the method to call
	@param aPckg contains arguments of the method to call
	*/
	TAny* dataPtr = aPckg.Ptr1();
	TAny* dataPtr2 = aPckg.Ptr2();

	// The following requests can be completed even if the completion of another request is pending.
	switch(aIpc)
		{
	case EMobilePhoneSetCallForwardingStatus:
		return SetCallForwardingStatus(aReqHandle,
			reinterpret_cast<RMobilePhone::TMobilePhoneCFCondition*>(dataPtr),
			reinterpret_cast<RMobilePhone::TMobilePhoneCFChangeV1*>(dataPtr2));
	case EMobilePhoneNotifyCallForwardingStatusChange:
		return NotifyCallForwardingStatusChange(aReqHandle, reinterpret_cast<RMobilePhone::TMobilePhoneCFCondition*>(dataPtr));
	case EMobilePhoneGetIdentityServiceStatus:
		return GetIdentityServiceStatus(aReqHandle, 
			reinterpret_cast<RMobilePhone::TMobilePhoneIdService*>(dataPtr),
			reinterpret_cast<RMobilePhone::TMobilePhoneIdServiceStatus*>(dataPtr2));
	case EMobilePhoneGetCallForwardingStatusPhase1:
		return GetCallForwardingStatusPhase1(aReqHandle, 
			REINTERPRET_CAST(CRetrieveMobilePhoneCFList::TGetCallForwardingRequest*, dataPtr), 
			REINTERPRET_CAST(TInt*, dataPtr2));
	case EMobilePhoneGetCallForwardingStatusPhase2:
		return GetCallForwardingStatusPhase2(aReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), aPckg.Des2n());			
	default:
		break;
		}
	return KErrNotSupported;
	}
	
TInt CSimCallForwarding::CancelService(const TInt aIpc, const TTsyReqHandle aReqHandle)
	/**
	Cancels Call forwarding requests
	@param ipc request to cancel
	@param aReqHandle Handle to the request
	*/
	{
	switch(aIpc)
		{
	case EMobilePhoneSetCallForwardingStatus:
		return SetCallForwardingStatusCancel(aReqHandle);
	case EMobilePhoneNotifyCallForwardingStatusChange:
		return NotifyCallForwardingStatusChangeCancel(aReqHandle);
	case EMobilePhoneGetIdentityServiceStatus:
		return GetIdentityServiceStatusCancel(aReqHandle);
	default:
		break;
		}
	return KErrNone;
	}
	
void CSimCallForwarding::FindAndCreateCFListL()
	{
	/**
	Creates the Call forwarding list from the config file
	*/
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_FINDANDCREATECFLISTL_1, "CSimPhone::FindAndCreateCFListL");
	RMobilePhone::TMobilePhoneCFInfoEntryV1 entry;

	if(iCFList)
		delete iCFList;
	iCFList=NULL;

	iCFList = CMobilePhoneCFList::NewL();
	TInt count=CfgFile()->ItemCount(KCFList);
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_FINDANDCREATECFLISTL_2, "Starting to Load and Parse CFList Config parameters");
	
	TInt i;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KCFList,i);
		if(!item)
			break;
		
		TPtrC8 number;
		TInt condition, serviceGroup, status, timeout; // valid for CFRNy only
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,condition);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_FINDANDCREATECFLISTL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element CONDITION returned %d (element no. %d) from tag %s.",ret,0,KCFList);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,serviceGroup);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_FINDANDCREATECFLISTL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element SERVICEGROUP returned %d (element no. %d) from tag %s.",ret,1,KCFList);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,status);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_FINDANDCREATECFLISTL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element STATUS returned %d (element no. %d) from tag %s.",ret,2,KCFList);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,timeout);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_FINDANDCREATECFLISTL_6, "WARNING - CONFIGURATION FILE PARSING - Reading element TIMEOUT returned %d (element no. %d) from tag %s.",ret,3,KCFList);
			continue;
			}
		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,4,number);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_FINDANDCREATECFLISTL_7, "WARNING - CONFIGURATION FILE PARSING - Reading element NUMBER returned %d (element no. %d) from tag %s.",ret,4,KCFList);
			continue;
			}
		
		entry.iServiceGroup=(RMobilePhone::TMobileService)serviceGroup;
		entry.iNumber.iTelNumber.Copy(number);
		entry.iTimeout=timeout;
		entry.iCondition=(RMobilePhone::TMobilePhoneCFCondition)condition;
		entry.iStatus=(RMobilePhone::TMobilePhoneCFStatus)status;
		iCFList->AddEntryL(entry);
		}//end for
	
	//CFlist must define entries for each condition even if unknown
	//fill up table if it misses conditions:
	
	RArray<TBool> serviceArray;
	//voice, auxVoice, data, packet, fax, sms, all;

	CleanupClosePushL(serviceArray);

	RMobilePhone::TMobilePhoneCFInfoEntryV1 extraEntry;

	for (TInt j=1;j<5;j++)//loop conditions
		{
		TInt serviceFound=EFalse;
		TInt index;
		extraEntry.iCondition=static_cast<RMobilePhone::TMobilePhoneCFCondition>(j);
		serviceArray.Reset();
		for(TInt h=0;h<8;h++)
			{
			serviceArray.Append(EFalse);
			}
		
		for (TInt k=0;k<count;k++)//loop entries
			{
			entry=iCFList->GetEntryL(k);
			if(entry.iCondition==static_cast<RMobilePhone::TMobilePhoneCFCondition>(j))
				{
				index = (TInt)entry.iServiceGroup;
				serviceArray[index]=ETrue;
				}
			}
		
		if (!serviceArray[7])//(all services)
			{
			for (TInt l=1;l<7;l++)
				{
				if (serviceArray[l])
					serviceFound=ETrue;										
				}
			//no service defined for this condition
			if(!serviceFound)
				{
				extraEntry.iServiceGroup=RMobilePhone::EAllServices;
				extraEntry.iStatus=RMobilePhone::ECallForwardingStatusNotRegistered;
				iCFList->AddEntryL(extraEntry);
				}
			//some services defined in table
			else
				{
				for (TInt m=1;m<7;m++)
					{
					if (!serviceArray[m])
						{
						extraEntry.iServiceGroup=static_cast<RMobilePhone::TMobileService>(m);
						extraEntry.iStatus=RMobilePhone::ECallForwardingStatusNotRegistered;
						iCFList->AddEntryL(extraEntry);
						}
					}//end for
				}//end else
			}//end if
		}//end for
		
	CleanupStack::PopAndDestroy(&serviceArray);	//Destroying serviceArray to free the memory
	}

TInt CSimCallForwarding::GetIdentityServiceStatus(const TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhoneIdService* aService, RMobilePhone::TMobilePhoneIdServiceStatus* aStatus)
	/**
	Gets the status of the identity services
	@param aReqHandle handle to the request
	@param aService service that the client is interested in
	@param aStatus of the identity service
	*/
	{
	RMobilePhone::TMobilePhoneIdServiceStatus status;
	status = RMobilePhone::EIdServiceUnknown;
	TInt count = iIdentityServiceStatus->Count();
	for(TInt i=0; i<count; i++)
		{
		if(iIdentityServiceStatus->At(i).iService == *aService)
			{
			status = iIdentityServiceStatus->At(i).iStatus;
			break;
			}
		}
	*aStatus = status;
	iPhone->ReqCompleted(aReqHandle, KErrNone);
	return KErrNone;
	}
	
TInt CSimCallForwarding::GetIdentityServiceStatusCancel(const TTsyReqHandle aReqHandle)
	/**
	Cancels the request to get the status of the identity services
	@param aReqHandle hadle to the request
	*/
	{
	iPhone->ReqCompleted(aReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CSimCallForwarding::NotifyCallForwardingStatusChange(const TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhoneCFCondition* aCF)
	/**
	Requests to be notified of call forwarding status change
	@param aReqHandle handle to the request
	@param aCF condition for which the client wants to be notified
	 */
	{
	__ASSERT_ALWAYS(!iCFNotification.iCFChangeInfoNotificationPending,SimPanic(ENotificationReqAlreadyOutstanding));

	iCFNotification.iCFChangeInfoNotificationPending=ETrue;
	iCFNotification.iCFChangeInfoReqHandle=aReqHandle;
	iCFNotification.iCurrentCFCondition=aCF;
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_NOTIFYCALLFORWARDINGSTATUSCHANGE_1, "Finished CSimCallForwarding::NotifyCallForwardingStatusChange");
	return KErrNone;
	}
	
TInt CSimCallForwarding::NotifyCallForwardingStatusChangeCancel(const TTsyReqHandle aReqHandle)
	/**
	Cancels the request to be notified of call forwarding status change
	@param aReqHandle handle to the request
	*/
	{
	if(iCFNotification.iCFChangeInfoNotificationPending)
		{
		iCFNotification.iCFChangeInfoNotificationPending=EFalse;
		iPhone->ReqCompleted(aReqHandle,KErrCancel);
		}
	return KErrNone;
	}
	
TInt CSimCallForwarding::SetCallForwardingStatus(const TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhoneCFCondition* aCF,  RMobilePhone::TMobilePhoneCFChangeV1* aInfo)
	/**
	Sets the status of call forwarding
	@param aReqHandle handle to the request
	@param aCF condition for which the client wants to set the status
	@param aInfo value of Call forwarding to set
	*/
	{
	TRAPD(err, UpdateCFListL(aCF,aInfo));
	if ( err != KErrNone )
		{ 
		iPhone->ReqCompleted(aReqHandle,err);
		return KErrNone;
		}
		
	if(iCFNotification.iCFChangeInfoNotificationPending)
		{
		*(iCFNotification.iCurrentCFCondition)=*aCF;
		iCFNotification.iCFChangeInfoNotificationPending=EFalse;
		iPhone->ReqCompleted(iCFNotification.iCFChangeInfoReqHandle,KErrNone);
		}
	
	iPhone->ReqCompleted(aReqHandle,err);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_SETCALLFORWARDINGSTATUS_1, "Finished CSimCallForwarding::SetCallForwardingStatus");
	return KErrNone;
	}
	
TInt CSimCallForwarding::SetCallForwardingStatusCancel(const TTsyReqHandle aReqHandle)
	/**
	Cancels the request to set the status of call forwarding
	@param aReqHandle handle to the request
	*/
	{
	iPhone->ReqCompleted(aReqHandle,KErrCancel);
	return KErrNone;
	}
	

TInt CSimCallForwarding::GetCallForwardingStatusPhase1(const TTsyReqHandle aTsyReqHandle, 
												 CRetrieveMobilePhoneCFList::TGetCallForwardingRequest* aReqData, 
												 TInt* aBufSize)
	{
	/**
	1st phase retrieval of the status of call forwarding
	@param aTsyReqHandle handle to the request
	@param aReqData contains details of the request
	@param aBufSize size of the buffer the client needs to allocate for phase 2
	*/
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_GETCALLFORWARDINGSTATUSPHASE1_1, "CSimPhone::GetCallForwardingStatusPhase1");
	TInt ret=KErrNone;

    // for forwarding we cannot querry for all conditions;
    // conditions - specific
    // services  - individual and all
	if ((aReqData->iCondition==RMobilePhone::ECallForwardingUnspecified)
	   || (aReqData->iCondition==RMobilePhone::ECallForwardingAllCases)
	   || (aReqData->iCondition==RMobilePhone::ECallForwardingAllConditionalCases))
	   {
			iPhone->ReqCompleted(aTsyReqHandle,KErrArgument);
		}
	else
		{
		TInt leaveCode=KErrNone;
		TRAP(leaveCode, ret=ProcessGetCallForwardingStatusPhase1L(aTsyReqHandle, aReqData, aBufSize););
		if (leaveCode != KErrNone)
			iPhone->ReqCompleted(aTsyReqHandle,leaveCode);
		}
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_GETCALLFORWARDINGSTATUSPHASE1_2, "CSimPhone::GetCallForwardingStatusPhase1");
	return ret;
	}
	
TInt CSimCallForwarding::ProcessGetCallForwardingStatusPhase1L(const TTsyReqHandle aTsyReqHandle, 
														 CRetrieveMobilePhoneCFList::TGetCallForwardingRequest* aReqData, 
														 TInt* aBufSize)
	{
	/** Retrieve call forwarding status of each line from phone, 
	store each CF status response as a list entry,
	stream the list and then return size of this buffer to client
	@param aReqHandle Handle to the request
	@param aReqData information about the request
	@param aBufSize Size of the buffer the client has to allocate for the 2nd pahase
	*/

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_PROCESSGETCALLFORWARDINGSTATUSPHASE1L_1, "CSimPhone::ProcessGetCallForwardingStatusPhase1L");

	CMobilePhoneCFList* list=CMobilePhoneCFList::NewL();
	CleanupStack::PushL(list);
	
	TInt cnt=0;//Only interested by entries with a particular condition
	TInt maxNum=iCFList->Enumerate();
	TBool isAllGroups = (aReqData->iServiceGroup == RMobilePhone::EAllServices);
	for(TInt i=0;i<maxNum;i++)
		{
		RMobilePhone::TMobilePhoneCFInfoEntryV1 entry = iCFList->GetEntryL(i);
		
		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(entry);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aTsyReqHandle, err);
			return KErrNone;
			}

		if((entry.iCondition == aReqData->iCondition) 
		   &&(entry.iStatus != RMobilePhone::ECallForwardingStatusNotRegistered))
			{
			if ( !isAllGroups && (entry.iServiceGroup != aReqData->iServiceGroup))
				{
				continue;
				}
			list->AddEntryL(entry);
			cnt++;
			}	
		}
	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aReqData->iClient,aTsyReqHandle);
	CleanupStack::PushL(read);
	
	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufBase allocated by StoreLC
	
	iGetCFStatus->AppendL(read);
	CleanupStack::Pop(read); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	CleanupStack::PopAndDestroy(list); // pop&destroy list
	
	// Complete first phase of list retrieval
	iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_PROCESSGETCALLFORWARDINGSTATUSPHASE1L_2, "CSimPhone::ProcessGetCallForwardingStatusPhase1L");
	return KErrNone;	
	}

TInt CSimCallForwarding::GetCallForwardingStatusPhase2(const TTsyReqHandle aTsyReqHandle, 
												 RMobilePhone::TClientId* aClient, TDes8* aBuf)
	/**
	2nd phase retrieval of the status of call forwarding
	@param aTsyRqHandle handle to the request
	@param aClient pointer to the client
	@param aBuf buffer that contains the call forwarding list
	*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_GETCALLFORWARDINGSTATUSPHASE2_1, "CSimPhone::GetCallForwardingStatusPhase2");
	CListReadAllAttempt* read=NULL;
	// Find the get detected network attempt from this client
	for (TInt i=0; i<iGetCFStatus->Count(); ++i)
		{
		read = iGetCFStatus->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuf->Copy(bufPtr);
			delete read;
			iGetCFStatus->Delete(i);
			iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_GETCALLFORWARDINGSTATUSPHASE2_2, "CSimPhone::GetCallForwardingStatusPhase2");
	return KErrNotFound;
	}


TInt CSimCallForwarding::GetCallForwardingStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	/**
	Cancel the request to retrieve the status of call forwarding
	@param aTsyReqHandle handle to the request
	*/
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_GETCALLFORWARDINGSTATUSCANCEL_1, "CSimPhone::GetCallForwardingStatusCancel");
	iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
	// Remove the read all attempt from iGetCFStatus
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iGetCFStatus->Count(); ++i)
		{
		read = iGetCFStatus->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iGetCFStatus->Delete(i);
			break;
			}
		}
	iPhone->ReqCompleted(aTsyReqHandle,KErrCancel);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLFORWARDING_GETCALLFORWARDINGSTATUSCANCEL_2, "CSimPhone::GetCallForwardingStatusCancel");
	return KErrNone;
	}

void CSimCallForwarding::UpdateCFListL(RMobilePhone::TMobilePhoneCFCondition* aCF, 
									   RMobilePhone::TMobilePhoneCFChangeV1* aCFInfo )
	{		
	/**
	Modifies the status of call forwarding
	@param aCF condition
	@param aCFInfo details of the request
	*/
	User::LeaveIfNull(aCF);
	User::LeaveIfNull(aCFInfo);

	if (*aCF == RMobilePhone::ECallForwardingUnspecified)
	    {
	    // not supported - tell user it is a wrong argument 
		User::Leave(KErrArgument);
	    }
	     
	if ((*aCF == RMobilePhone::ECallForwardingAllCases)
		||(*aCF == RMobilePhone::ECallForwardingAllConditionalCases)) 
	    {
	    // not supported - tell user it is a wrong argument 
		User::Leave(KErrNotSupported);
	    }  
	       
	switch (aCFInfo->iAction)
		{
		case RMobilePhone::EServiceActionRegister:
			RegisterL(aCF, aCFInfo);
			break;
			
		case RMobilePhone::EServiceActionActivate:
			ActivateL(aCF, aCFInfo);
			break;
			
		case RMobilePhone::EServiceActionInvoke:
		    // currently not supported
		    // tell user it is a wrong argument 
			User::Leave(KErrArgument);
			break;
			
		case RMobilePhone::EServiceActionDeactivate:
			DeactivateL(aCF, aCFInfo);
			break;
			
		case RMobilePhone::EServiceActionErase:
			EraseL(aCF, aCFInfo);
			break;
			
		case RMobilePhone::EServiceActionUnspecified:
		default:
		    // not supported - tell user it is a wrong argument 
			User::Leave(KErrArgument);
			break;
		}
	}
	

inline void SplitOneCFEntryIntoSeparateGroupsL(CMobilePhoneCFList* aCFList,
											   RMobilePhone::TMobilePhoneCFInfoEntryV1& aOrigEntry,
											   RMobilePhone::TMobileService aExceptServiceGroup)
	{
	/*
	Splits one entry valid for all basic service groups into individual entries 
	and places them into a CMobilePhoneCFList. This excludes one service group.	 
	@param aCBList The list to which to store the entries
	@param aOrigEntry The original entry; details of which should be passed to individual group entries
	@param aExceptServiceGroup The group which has to be avoided
	*/
	// we have to split the entry into entries 
	// per group with the same condition; excluding the specified service group 
	RMobilePhone::TMobilePhoneCFInfoEntryV1 anotherEntry(aOrigEntry);

	RMobilePhone::TMobileService service;
	
	for (TInt i = KMobServiceIndxStart;i <= KMobServiceIndxEnd;i++)
		{
		service=static_cast<RMobilePhone::TMobileService>(i);
		
		if( service != aExceptServiceGroup )
			{
			anotherEntry.iServiceGroup=service;
			aCFList->AddEntryL(anotherEntry);
			}
		}//end for (another)
	}
	
	
void CSimCallForwarding::UpdateCFQuiescentConditionL(TBool aActivate, 
													RMobilePhone::TMobilePhoneCFCondition aCF)
	{
	// only the Call Forward Unconditional can alter any other CF condition
	if(aCF == RMobilePhone::ECallForwardingUnconditional)
		{		
		TBool cFBChangeRequired = EFalse;
		
		RMobilePhone::TMobilePhoneCFInfoEntryV1 theEntry;
		
		const TInt count = iCFList->Enumerate();

		// look for any instances of the ECallForwardingBusy
		for (TInt indx=0; indx < count; indx++)
			{ 
			theEntry = iCFList->GetEntryL(indx);
			
			if(theEntry.iCondition == RMobilePhone::ECallForwardingBusy &&
			  (theEntry.iStatus == RMobilePhone::ECallForwardingStatusActive || 
			   theEntry.iStatus == RMobilePhone::ECallForwardingStatusQuiescent)) 
				{
				cFBChangeRequired = ETrue;
				}												
			}
		
		if(cFBChangeRequired)
			{
			// retain prev version of the list in local variable and in Cleanup stack
			CMobilePhoneCFList* thePrevCFList = CMobilePhoneCFList::NewL();
			CleanupStack::PushL(thePrevCFList);
	
			// copy existing data into the temp storage
			for (TInt indx=0; indx < count; indx++)
				{ 
				thePrevCFList->AddEntryL(iCFList->GetEntryL(indx));
				}

			delete iCFList;
			iCFList = NULL;
	
			// create a new version and hold it in the class member
			iCFList = CMobilePhoneCFList::NewL();
	
			// get a hold of it in a local variable       
			CMobilePhoneCFList* theNewCFList = iCFList;
			
			for(TInt indx=0; indx < count; indx++)
				{
				theEntry = thePrevCFList->GetEntryL(indx);
			
				if(theEntry.iCondition == RMobilePhone::ECallForwardingBusy)
					{
					if(aActivate)
						{
						if(theEntry.iStatus == RMobilePhone::ECallForwardingStatusActive)
							{
							theEntry.iStatus = RMobilePhone::ECallForwardingStatusQuiescent;
							}
						}
					else
						{
						if(theEntry.iStatus == RMobilePhone::ECallForwardingStatusQuiescent)
							{
							theEntry.iStatus = RMobilePhone::ECallForwardingStatusActive;
							}
						}					
					}			
				theNewCFList->AddEntryL(theEntry);
				}					
			
			// now delete the old memory
			CleanupStack::PopAndDestroy(thePrevCFList);
			}						
		}
	}	

void CSimCallForwarding::ActivateL(RMobilePhone::TMobilePhoneCFCondition* aCF, 
								   RMobilePhone::TMobilePhoneCFChangeV1* aCFInfo )
	{
	// NOTE: An assumption is made that the calling method has checked the parameters
	// are not NULL.
	RMobilePhone::TMobilePhoneCFInfoEntryV1 theNewEntry;

	theNewEntry.iCondition = *aCF;
	theNewEntry.iServiceGroup = aCFInfo->iServiceGroup;
	theNewEntry.iStatus= RMobilePhone::ECallForwardingStatusNotActive;
	theNewEntry.iNumber = aCFInfo->iNumber;
	
	if (*aCF ==  RMobilePhone::ECallForwardingNoReply)
		{	
		theNewEntry.iTimeout = aCFInfo->iTimeout;
		}
	else
		{
		theNewEntry.iTimeout = KInvalidTimeout;
		} 


	// retain prev version of the list in local variable and in Cleanup stack
	CMobilePhoneCFList* thePrevCFList = CMobilePhoneCFList::NewL();
	CleanupStack::PushL(thePrevCFList);
	
	const TInt count = iCFList->Enumerate();
	
	TBool CFUActive = EFalse;
	
	// copy existing data into the temp storage
	for (TInt indx = 0; indx < count; indx++)
		{ 	
		thePrevCFList->AddEntryL(iCFList->GetEntryL(indx));
		
		RMobilePhone::TMobilePhoneCFInfoEntryV1 copyEntry = iCFList->GetEntryL(indx);

		if(copyEntry.iCondition == RMobilePhone::ECallForwardingUnconditional &&
		   copyEntry.iStatus == RMobilePhone::ECallForwardingStatusActive)
			{
			CFUActive = ETrue;
			}
		}
	
	delete iCFList;
	iCFList = NULL;
	
	// create a new version and hold it in the class member
	iCFList = CMobilePhoneCFList::NewL();
	
	// get a hold of it in a local variable 
	CMobilePhoneCFList* theNewCFList = iCFList;
	
	TBool callForwardActivated = EFalse;
		
	RMobilePhone::TMobilePhoneCFInfoEntryV1 entry;
	
	for (TInt i = 0; i < count; i++)
		{
		entry = thePrevCFList->GetEntryL(i);
					
		if(theNewEntry.iCondition == entry.iCondition)
			{			
			if(theNewEntry.iServiceGroup == entry.iServiceGroup)
				{
				// only activate service if previously in a not active state
				if(entry.iStatus == RMobilePhone::ECallForwardingStatusNotActive)
					{
					if(entry.iCondition == RMobilePhone::ECallForwardingBusy && CFUActive != EFalse)
						{
						// do nothing, condition's service will not be activated as callforwarding
						// unconditional is active	
						}
					else
						{
						entry.iStatus = RMobilePhone::ECallForwardingStatusActive;
						callForwardActivated = ETrue;									
						}
					}					
				}
			else if(entry.iServiceGroup == RMobilePhone::EAllServices)
				{
				if(entry.iStatus == RMobilePhone::ECallForwardingStatusNotActive)
					{
					SplitOneCFEntryIntoSeparateGroupsL(theNewCFList,
													   entry,
													   theNewEntry.iServiceGroup);
					
					callForwardActivated = ETrue;
					entry = theNewEntry;
					}				
				}					
			}		

		theNewCFList->AddEntryL(entry);				
		}
	
	if(callForwardActivated)
		{
		// only need to check quiescent status if an activation has occurred
		UpdateCFQuiescentConditionL(ETrue, theNewEntry.iCondition);	
		}

	CleanupStack::PopAndDestroy(thePrevCFList);
	}

void CSimCallForwarding::RegisterL(RMobilePhone::TMobilePhoneCFCondition* aCF, RMobilePhone::TMobilePhoneCFChangeV1* aCFInfo)
	{
	TInt count = iCFList->Enumerate();
	RMobilePhone::TMobilePhoneCFInfoEntryV1 theNewEntry;

	theNewEntry.iCondition = *aCF;
	theNewEntry.iServiceGroup = aCFInfo->iServiceGroup;
	theNewEntry.iStatus= RMobilePhone::ECallForwardingStatusNotActive;

	theNewEntry.iNumber = aCFInfo->iNumber;
	if (*aCF ==  RMobilePhone::ECallForwardingNoReply)
		{	
		theNewEntry.iTimeout = aCFInfo->iTimeout;
		}
	else
		{
		 theNewEntry.iTimeout = KInvalidTimeout;
		} 

	// retain prev version of the list in local variable and in Cleanup stack
	CMobilePhoneCFList* thePrevCFList = CMobilePhoneCFList::NewL();
	CleanupStack::PushL(thePrevCFList);
	// copy existing data into the temp storage
	for (TInt indx=0; indx<count; indx++)
		{ 
		thePrevCFList->AddEntryL(iCFList->GetEntryL(indx));
		}
	delete iCFList;
	iCFList = NULL;
	// create a new version and hold it in the class member
	iCFList = CMobilePhoneCFList::NewL();
	// get a hold of it in a local variable       
	CMobilePhoneCFList* theNewCFList = iCFList;
		
	if (theNewEntry.iServiceGroup==RMobilePhone::EAllServices)
		{
		//nothing for all conds
		
		//action is to Regiser (and activate)AllServices with a particular condition;
		//Only copy entries with a different condition
		//entries with same condition should not be copied as they will
		//be represented by 1 with allservices
		for (TInt i = 0; i<count; i++)
			{
			RMobilePhone::TMobilePhoneCFInfoEntryV1 entry = thePrevCFList->GetEntryL(i);

			if (entry.iCondition != theNewEntry.iCondition)
				// check if the existing entry is encompassed by the new one
				theNewCFList->AddEntryL(entry);
			}
		theNewCFList->AddEntryL(theNewEntry);			
		}
	else
		{
		//ActivateOne;
		for (TInt i = 0; i<count; i++)
			{
			RMobilePhone::TMobilePhoneCFInfoEntryV1 entry = thePrevCFList->GetEntryL(i);

			if (theNewEntry.iCondition==entry.iCondition
				&&  entry.iServiceGroup==theNewEntry.iServiceGroup)
				{
				theNewCFList->AddEntryL(theNewEntry);
				continue;
				}
			if (entry.iCondition != theNewEntry.iCondition
				|| (entry.iCondition == theNewEntry.iCondition
				&& entry.iServiceGroup != theNewEntry.iServiceGroup
				&& entry.iServiceGroup != RMobilePhone::EAllServices))
				{
				theNewCFList->AddEntryL(entry);	
				continue;
				}
				
			if(theNewEntry.iCondition==entry.iCondition
				&&  entry.iServiceGroup==RMobilePhone::EAllServices)
				{
					
				SplitOneCFEntryIntoSeparateGroupsL( 
											theNewCFList,
											entry,
											theNewEntry.iServiceGroup
											);
					
				theNewCFList->AddEntryL(theNewEntry);
				}//end if
			}//end for
		}//end else
	// now delete the old memory
	CleanupStack::PopAndDestroy(thePrevCFList);
	}

void CSimCallForwarding::DeactivateL(RMobilePhone::TMobilePhoneCFCondition* aCF, 
									 RMobilePhone::TMobilePhoneCFChangeV1* aCFInfo )
	{
	// NOTE: An assumption is made that the calling method has checked the parameters
	// are not NULL.
		
	RMobilePhone::TMobilePhoneCFInfoEntryV1 theNewEntry;

	theNewEntry.iCondition = *aCF;
	theNewEntry.iServiceGroup = aCFInfo->iServiceGroup;
	theNewEntry.iStatus= RMobilePhone::ECallForwardingStatusNotActive;
		
	// retain prev version of the list in local variable and in Cleanup stack
	CMobilePhoneCFList* thePrevCFList = CMobilePhoneCFList::NewL();
	CleanupStack::PushL(thePrevCFList);
		
	const TInt count = iCFList->Enumerate();

	// copy existing data into the temp storage
	for (TInt indx = 0; indx < count; indx++)
		{ 
		thePrevCFList->AddEntryL(iCFList->GetEntryL(indx));
		}
	
	delete iCFList;
	iCFList = NULL;
	
	// create a new version and hold it in the class member
	iCFList = CMobilePhoneCFList::NewL();
	
	// get a hold of it in a local variable       
	CMobilePhoneCFList* theNewCFList = iCFList;
		
	TBool callForwardDeactivated = EFalse;
			
	RMobilePhone::TMobilePhoneCFInfoEntryV1 entry;

	//Deactivate will deactivate active entries 
	for (TInt indx = 0; indx < count; indx++)
		{
		entry = thePrevCFList->GetEntryL(indx);
				
		if(entry.iCondition == theNewEntry.iCondition)
			{
			if(entry.iServiceGroup == theNewEntry.iServiceGroup)
				{
				if(entry.iStatus == RMobilePhone::ECallForwardingStatusActive)
					{
				   	entry.iStatus = RMobilePhone::ECallForwardingStatusNotActive;
					callForwardDeactivated = ETrue;
					}								
				}
			else if(entry.iServiceGroup == RMobilePhone::EAllServices)
				{
				if(entry.iStatus == RMobilePhone::ECallForwardingStatusActive)
					{
					SplitOneCFEntryIntoSeparateGroupsL(theNewCFList,
													   entry,
													   theNewEntry.iServiceGroup);
										
					entry = theNewEntry;
					callForwardDeactivated = ETrue;					
					}			
				}			
			}	
		
		theNewCFList->AddEntryL(entry);
		}//end for
		
	// this method will check if any de-activation has caused another Condition to become 
	// cleared of the quiescent state
	if(callForwardDeactivated)
		{		
		// pass a FALSE to the function to notify of a service de-activation
		UpdateCFQuiescentConditionL(EFalse, theNewEntry.iCondition);
		}

	// now delete the old memory	
	CleanupStack::PopAndDestroy(thePrevCFList);
	}

void CSimCallForwarding::EraseL(RMobilePhone::TMobilePhoneCFCondition* aCF, 
								RMobilePhone::TMobilePhoneCFChangeV1* aCFInfo )
	{
	// NOTE: An assumption is made that the calling method has checked the parameters
	// are not NULL.
	
	RMobilePhone::TMobilePhoneCFInfoEntryV1 theNewEntry;

	theNewEntry.iCondition = *aCF;
	theNewEntry.iServiceGroup = aCFInfo->iServiceGroup;
	theNewEntry.iNumber.iTelNumber.Copy(KEmptyString);
	theNewEntry.iTimeout = KErrNotFound;
	theNewEntry.iStatus = RMobilePhone::ECallForwardingStatusNotRegistered;
	
	// retain prev version of the list in local variable and in Cleanup stack
	CMobilePhoneCFList* thePrevCFList = CMobilePhoneCFList::NewL();
	CleanupStack::PushL(thePrevCFList);
	
	const TInt count = iCFList->Enumerate();

	// copy existing data into the temp storage
	for (TInt indx = 0; indx < count; indx++)
		{ 
		thePrevCFList->AddEntryL(iCFList->GetEntryL(indx));
		}
	
	delete iCFList;
	iCFList = NULL;
	
	// create a new version and hold it in the class member
	iCFList = CMobilePhoneCFList::NewL();
	
	// get a hold of it in a local variable       
	CMobilePhoneCFList* theNewCFList = iCFList;
	
	RMobilePhone::TMobilePhoneCFInfoEntryV1 entry;	
	
	for(TInt i = 0; i < count; i++)
		{
		entry = thePrevCFList->GetEntryL(i);
		
		if(entry.iCondition == theNewEntry.iCondition)
			{
			if(entry.iServiceGroup == theNewEntry.iServiceGroup)
				{
				if(entry.iStatus == RMobilePhone::ECallForwardingStatusNotActive)
					{
				   	entry = theNewEntry;
					}								
				}
			else if(entry.iServiceGroup == RMobilePhone::EAllServices)
				{
				if(entry.iStatus == RMobilePhone::ECallForwardingStatusActive)
					{
					SplitOneCFEntryIntoSeparateGroupsL(theNewCFList,
													   entry,
													   theNewEntry.iServiceGroup);
										
					entry = theNewEntry;
					}			
				}
			}
		theNewCFList->AddEntryL(entry);		
		}

	// now delete the old memory
	CleanupStack::PopAndDestroy(thePrevCFList);	
	}
	
const CTestConfigSection* CSimCallForwarding::CfgFile()
/**
 * Returns a pointer to the current configuration file section.
 *
 * @return CTestConfigSection	A pointer to the current configuration file data section.
 */
	{
	return iPhone->CfgFile();
	}


