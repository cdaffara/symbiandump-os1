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
// CallWaiting status and registration information.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimCallWaitingTraces.h"
#endif

#include <testconfigfileparser.h>
#include "CSimCallWaiting.h"
#include "CSimPhone.h"
const TInt KMobServiceIndxStart = 1;
const TInt KMobServiceIndxEnd   = 5;


CSimCallWaiting* CSimCallWaiting::NewL(CSimPhone* aPhone)
/**
 * Standard two-phase constructor.
 * @param aPhone				The parent phone object.
 * @return CSimCallWaiting		The new call waiting class.
 */
	{
	CSimCallWaiting* self=new(ELeave) CSimCallWaiting(aPhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSimCallWaiting::CSimCallWaiting(CSimPhone* aPhone)
		: iPhone(aPhone)
/**
 * Trivial first phase construction.
 * @param aPhone				The parent phone object.
 */
	{
	}

void CSimCallWaiting::ConstructL()
/**
	Second phase of 2-Phase Constructor
  	Retrieves all the Call waiting related tags from the config file
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLWAITING_CONSTRUCTL_1, "Starting to parse Call waiting config parameters...");

	iGetCWStatus = new(ELeave) CArrayPtrFlat<CListReadAllAttempt>(1);
	FindAndCreateCWListL();
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLWAITING_CONSTRUCTL_2, "...Finished parsing Call waiting config parameters...");
	}
	
void CSimCallWaiting::FindAndCreateCWListL()
{
/**
  	Retrieves all the Call waiting tags that define the 
  	original status of Call waiting from the config file
*/
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLWAITING_FINDANDCREATECWLISTL_1, "CSimPhone::FindAndCreateCWListL");
	RMobilePhone::TMobilePhoneCWInfoEntryV1 entry;

	iCWList = CMobilePhoneCWList::NewL();
	TInt count=CfgFile()->ItemCount(KCWList);
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLWAITING_FINDANDCREATECWLISTL_2, "Starting to Load and Parse CWList Config parameters");
	TInt i;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KCWList,i);
		if(!item)
			break;
		
		TInt serviceGroup, status;
		
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,serviceGroup);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLWAITING_FINDANDCREATECWLISTL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element SERVICEGROUP returned %d (element no. %d) from tag %s.",ret,0,KCWList);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,status);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLWAITING_FINDANDCREATECWLISTL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element STATUS returned %d (element no. %d) from tag %s.",ret,1,KCWList);
			continue;
			}
		
		entry.iServiceGroup=(RMobilePhone::TMobileService)serviceGroup;
		entry.iStatus=(RMobilePhone::TMobilePhoneCWStatus)status;

		iCWList->AddEntryL(entry);
		}
	}

CSimCallWaiting::~CSimCallWaiting()
	{
	delete iCWList;
	if (iGetCWStatus)
		iGetCWStatus->ResetAndDestroy();
	delete iGetCWStatus;
	}

TInt CSimCallWaiting::ExtFuncL(const TTsyReqHandle aReqHandle,const TInt aIpc, const TDataPackage& aPckg)
	{
	/**
  	Handles the phone based requests that apply to call waiting
  	@param aReqHandle Handle to the request
  	@param aIpc request identifier
  	@param aPckg contains additional information for the requests
	*/

	TAny* dataPtr = aPckg.Ptr1();
	TAny* dataPtr2 = aPckg.Ptr2();
	// The following requests can be completed even if the completion of another request is pending.
	switch(aIpc)
		{
	case EMobilePhoneSetCallWaitingStatus:
		return SetCallWaitingStatusL(aReqHandle,
			REINTERPRET_CAST(RMobilePhone::TMobileService*, dataPtr),
			REINTERPRET_CAST(RMobilePhone::TMobilePhoneServiceAction*,dataPtr2));
	case EMobilePhoneNotifyCallWaitingStatusChange:
		return NotifyCallWaitingStatusChange(aReqHandle, REINTERPRET_CAST(RMobilePhone::TMobilePhoneCWInfoEntryV1*, dataPtr));
	case EMobilePhoneGetWaitingStatusPhase1:
		return GetCallWaitingStatusPhase1(aReqHandle, 
			REINTERPRET_CAST(CRetrieveMobilePhoneCWList::TGetCallWaitingRequest*, dataPtr), 
			REINTERPRET_CAST(TInt*, dataPtr2));
	case EMobilePhoneGetWaitingStatusPhase2:
		return GetCallWaitingStatusPhase2(aReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), aPckg.Des2n());		
	default:
		break;
		}
	return KErrNotSupported;
	}
	
TInt CSimCallWaiting::CancelService(const TInt aIpc, const TTsyReqHandle aReqHandle)
	{
	/**
  	Cancels any phone based request regarding call waiting
  	@param aIpc the IPC to cancel
  	@param aReqHandle handle to the request
	*/
	switch(aIpc)
		{
	case EMobilePhoneSetCallWaitingStatus:
		return SetCallWaitingStatusCancel(aReqHandle);
	case EMobilePhoneNotifyCallWaitingStatusChange:
		return NotifyCallWaitingStatusChangeCancel(aReqHandle);
	default:
		break;
		}
	return KErrNone;
	}

TInt CSimCallWaiting::NotifyCallWaitingStatusChange(const TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhoneCWInfoEntryV1* aCW)
	{	
	/**
	Requests to be notified of change in the Call Waiting status
	@param aReqHandle Handle to the request
	@param aCW contains the service group and new status that has changed
  	*/
	__ASSERT_ALWAYS(!iCWNotification.iCWChangeInfoNotificationPending,SimPanic(ENotificationReqAlreadyOutstanding));
	iCWNotification.iCWChangeInfoNotificationPending=ETrue;
	iCWNotification.iCWChangeInfoReqHandle=aReqHandle;
	iCWNotification.iCWInfo=aCW;
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLWAITING_NOTIFYCALLWAITINGSTATUSCHANGE_1, "Finished CSimCallWaiting::NotifyCallWaitingStatusChange");
	return KErrNone;
	}

TInt CSimCallWaiting::NotifyCallWaitingStatusChangeCancel(const TTsyReqHandle aReqHandle)
	{
	/**
	Cancels request to be notified of call Waiting status change
	@param aReqHandle Handle to the request
	*/
	if(iCWNotification.iCWChangeInfoNotificationPending)
		{
		iCWNotification.iCWChangeInfoNotificationPending=EFalse;
		iPhone->ReqCompleted(aReqHandle,KErrCancel);
		}
	return KErrNone;
	}
	
TInt CSimCallWaiting::SetCallWaitingStatusL(const TTsyReqHandle aReqHandle, RMobilePhone::TMobileService* aService,  RMobilePhone::TMobilePhoneServiceAction* aAction)
	{
	/**
	Sets the status for call Waiting
	@param aReqHandle Handle to the request
	@param aCB condition for which the client whishes to set the call Waiting status
	@param aInfo additional parameters to set the call Waiting status
  	*/
	
	// make sure we know that the operation concluded sucessfully
	TRAPD(ret,UpdateCWListL(aService,aAction)); 	
	if ( ret != KErrNone )
		{ 
		iPhone->ReqCompleted(aReqHandle,ret);
		}
		
	// otherwise proceed and notify all went well
	// Notify change
	if (iCWNotification.iCWChangeInfoNotificationPending)
		{
		
		(iCWNotification.iCWInfo->iServiceGroup)=*aService;
				
		switch (*aAction)
			{
			case RMobilePhone::EServiceActionRegister:
			case RMobilePhone::EServiceActionActivate:
				iCWNotification.iCWInfo->iStatus= RMobilePhone::ECallWaitingStatusActive;
				break;
			case RMobilePhone::EServiceActionDeactivate:
			case RMobilePhone::EServiceActionErase:
				iCWNotification.iCWInfo->iStatus= RMobilePhone::ECallWaitingStatusNotActive;
				break;
			case RMobilePhone::EServiceActionUnspecified:
			default:
				iCWNotification.iCWInfo->iStatus= RMobilePhone::ECallWaitingStatusUnknown;
				// no sense doing any further operation - leave
				User::Leave( KErrArgument );
				break;
			}
		
		iCWNotification.iCWChangeInfoNotificationPending=EFalse;
		iPhone->ReqCompleted(iCWNotification.iCWChangeInfoReqHandle,KErrNone);
		}
		
	iPhone->ReqCompleted(aReqHandle,KErrNone);
		
	return KErrNone;
	}

TInt CSimCallWaiting::SetCallWaitingStatusCancel(const TTsyReqHandle aReqHandle)
	{
	/**
	Cancels the request to sets the status for call waiting
	@param aReqHandle Handle to the request
  	*/
	 
	iPhone->ReqCompleted(aReqHandle,KErrCancel);
	return KErrNone;
	}
	
	
	
const CTestConfigSection* CSimCallWaiting::CfgFile()
/**
 * Returns a pointer to the current configuration file section.
 *
 * @return CTestConfigSection	A pointer to the current configuration file data section.
 */
	{
	return iPhone->CfgFile();
	}

TInt CSimCallWaiting::GetCallWaitingStatusPhase1(const TTsyReqHandle aTsyReqHandle, 
												 CRetrieveMobilePhoneCWList::TGetCallWaitingRequest* aReqData, 
												 TInt* aBufSize)
	{
	/**
	1st phase retrieval of the the call waiting status list
	@param aReqHandle Handle to the request
	@param aReqData information about the request
	@param aBufSize Size of the buffer the client has to allocate for the 2nd pahase
  	*/
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLWAITING_GETCALLWAITINGSTATUSPHASE1_1, "CSimCallWaiting::GetCalWaitingStatusPhase1");
	
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessGetCallWaitingStatusPhase1L(aTsyReqHandle, aReqData, aBufSize););
	if (leaveCode != KErrNone)
			iPhone->ReqCompleted(aTsyReqHandle,leaveCode);

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLWAITING_GETCALLWAITINGSTATUSPHASE1_2, "CSimCallWaiting::GetCalWaitingStatusPhase1");
	return ret;
	}
	
TInt CSimCallWaiting::ProcessGetCallWaitingStatusPhase1L(const TTsyReqHandle aTsyReqHandle, 
														 CRetrieveMobilePhoneCWList::TGetCallWaitingRequest* aReqData, 
														 TInt* aBufSize)
	{
	/** Retrieve call waiting status of each line from phone, 
	store each CB status response as a list entry,
	stream the list and then return size of this buffer to client
	@param aReqHandle Handle to the request
	@param aReqData information about the request
	@param aBufSize Size of the buffer the client has to allocate for the 2nd pahase
	*/
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLWAITING_PROCESSGETCALLWAITINGSTATUSPHASE1L_1, "CSimCallWaiting::ProcessGetCallWaitingStatusPhase1L");

	CMobilePhoneCWList* list=CMobilePhoneCWList::NewL();
	CleanupStack::PushL(list);
		
	TInt maxNum=iCWList->Enumerate();
	//if testing all CB are non-active
	// then we have to return a single entry EAllServices
	if ( maxNum )
		{
		for(TInt i=0;i<maxNum;i++)
			{
			list->AddEntryL(iCWList->GetEntryL(i));
			}
		}
	else
		{
		// if the list is empty and the querry general then we have to return general info
		RMobilePhone::TMobilePhoneCWInfoEntryV1 entry;
		
		// Check that the data structure is supported by the simulated TSY version
		TInt err = iPhone->CheckSimTsyVersion(entry);
		if(err != KErrNone)
			{
			iPhone->ReqCompleted(aTsyReqHandle, err);
			return KErrNone;
			}

		entry.iServiceGroup	= RMobilePhone::EAllServices;
		entry.iStatus		= RMobilePhone::ECallWaitingStatusNotActive;
		list->AddEntryL( entry );
		}

	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aReqData->iClient,aTsyReqHandle);
	CleanupStack::PushL(read);
	
	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufBase allocated by StoreLC
	
	iGetCWStatus->AppendL(read);
	CleanupStack::Pop(read); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	CleanupStack::PopAndDestroy(list); // pop&destroy list
	
	// Complete first phase of list retrieval
	iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLWAITING_PROCESSGETCALLWAITINGSTATUSPHASE1L_2, "CSimCallWaiting::ProcessGetCallWaitingStatusPhase1L");
	return KErrNone;
	}

TInt CSimCallWaiting::GetCallWaitingStatusPhase2(const TTsyReqHandle aTsyReqHandle, 
												 RMobilePhone::TClientId* aClient, TDes8* aBuf)
	{
	/**
	2nd phase retrieval of the the call waiting status list
	@param aReqHandle Handle to the request
	@param aClient Ponter to the client
	@param aBuf Buffer containiong the call waiting status list
	*/
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLWAITING_GETCALLWAITINGSTATUSPHASE2_1, "CSimCallWaiting::GetCallWaitingStatusPhase2");
	CListReadAllAttempt* read=NULL;
	// Find the get detected network attempt from this client
	for (TInt i=0; i<iGetCWStatus->Count(); ++i)
		{
		read = iGetCWStatus->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuf->Copy(bufPtr);
			delete read;
			iGetCWStatus->Delete(i);
			iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLWAITING_GETCALLWAITINGSTATUSPHASE2_2, "CSimCallWaiting::GetCallWaitingStatusPhase2");
	return KErrNotFound;
	}

TInt CSimCallWaiting::GetCallWaitingStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	/*
	Cancels a Request to retrieve the call waiting status list
	@param aReqHandle Handle to the request
	*/
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLWAITING_GETCALLWAITINGSTATUSCANCEL_1, "CSimCallWaiting::GetCallWaitingStatusCancel");
	iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
	// Remove the read all attempt from iGetCBStatus
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iGetCWStatus->Count(); ++i)
		{
		read = iGetCWStatus->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iGetCWStatus->Delete(i);
			break;
			}
		}
	iPhone->ReqCompleted(aTsyReqHandle,KErrCancel);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLWAITING_GETCALLWAITINGSTATUSCANCEL_2, "CSimCallWaiting::GetCallWaitingStatusCancel");
	return KErrNone;
	}
	
inline void SplitOneEntryIntoSeparateGroupsL( 
								CMobilePhoneCWList* aCWList,
								RMobilePhone::TMobilePhoneCWInfoEntryV1 aOrigEntry,
								RMobilePhone::TMobileService aExceptServiceGroup
									)
	{
	/*
	Splits one entry valid for all basic service groups into individual entries 
	and places them into a CMobilePhoneCBList. This excludes one service group.	 
	@param aCBList The list to which to store the entries
	@param aOrigEntry The original entry; details of which should be passed to individual group entries
	@param aExceptServiceGroup The group which has to be avoided
	*/
	// we have to split the entry into entries 
	// per group with the same condition; excluding the specified service group 
	RMobilePhone::TMobilePhoneCWInfoEntryV1 anotherEntry = aOrigEntry;

	//anotherEntry.iCondition = aOrigEntry.iCondition;
	//anotherEntry.iStatus = RMobilePhone::ECallBarringStatusActive;
	RMobilePhone::TMobileService service;
	
	for (TInt i=KMobServiceIndxStart; i<=KMobServiceIndxEnd;i++)
		{
		service=static_cast<RMobilePhone::TMobileService>(i);
		if( service != aExceptServiceGroup )
			{
			anotherEntry.iServiceGroup=service;
			aCWList->AddEntryL(anotherEntry);
			}
		}//end for (another)
	}
	
void CSimCallWaiting::UpdateCWListL(RMobilePhone::TMobileService* aServiceGroup, RMobilePhone::TMobilePhoneServiceAction* aAction )
	{
	/*
	Updates the status of call waiting.
	@param aServiceGroup service group to set
	@param aAction ation to be performed 
	*/
	
	// no sense processing if either condition or change info are NULLS 
	User::LeaveIfNull(aServiceGroup);
	User::LeaveIfNull(aAction);
    
	
	//Lets do some work then
	RMobilePhone::TMobilePhoneCWInfoEntryV1 theNewEntry;

	theNewEntry.iServiceGroup = *aServiceGroup;
	
	switch (*aAction)
		{
		case RMobilePhone::EServiceActionRegister:
		case RMobilePhone::EServiceActionActivate:
			theNewEntry.iStatus= RMobilePhone::ECallWaitingStatusActive;
			break;
		case RMobilePhone::EServiceActionDeactivate:
		case RMobilePhone::EServiceActionErase:
			theNewEntry.iStatus= RMobilePhone::ECallWaitingStatusNotActive;
			break;
		case RMobilePhone::EServiceActionUnspecified:
		default:
			theNewEntry.iStatus= RMobilePhone::ECallWaitingStatusUnknown;
			// no sense doing any further operation - leave
			User::Leave( KErrArgument );
			break;
		}
		
	// retain prev version of the list in local variable and in Cleanup stack
	CMobilePhoneCWList* thePrevCWList = CMobilePhoneCWList::NewL();
	CleanupStack::PushL(thePrevCWList);
	// copy existing data into the temp storage
	for (TInt indx=0; indx<iCWList->Enumerate(); indx++)
		{ 
		thePrevCWList->AddEntryL(iCWList->GetEntryL(indx));
		}
	delete iCWList;
	iCWList = NULL;
	
	// create a new version and hold it in the class member
	iCWList = CMobilePhoneCWList::NewL();
	// get a hold of it in a local variable       
	CMobilePhoneCWList* theNewCWList = iCWList;
	RMobilePhone::TMobilePhoneCWInfoEntryV1 entry;

	TInt count = thePrevCWList->Enumerate();
	
	if (theNewEntry.iStatus == RMobilePhone::ECallWaitingStatusActive)
		{
		if (theNewEntry.iServiceGroup==RMobilePhone::EAllServices)
			{
			theNewCWList->AddEntryL(theNewEntry);
			}
		else
			{
			for (TInt i = 0; i<count; i++)
				{
				entry = thePrevCWList->GetEntryL(i);
				if(entry.iServiceGroup!=theNewEntry.iServiceGroup
					&& entry.iServiceGroup!=RMobilePhone::EAllServices)
					theNewCWList->AddEntryL(entry);
				}
			theNewCWList->AddEntryL(theNewEntry);
			}
		}
	else //deactivate
		{
		if (theNewEntry.iServiceGroup==RMobilePhone::EAllServices)
			{
			theNewCWList->AddEntryL(theNewEntry);
			}
		else
			{
			for (TInt i = 0; i<count; i++)
				{
				entry = thePrevCWList->GetEntryL(i);
				if(entry.iServiceGroup==RMobilePhone::EAllServices)
					{
					//add to the list active entries for all servive groups but the one to be deactivated
					SplitOneEntryIntoSeparateGroupsL(theNewCWList,entry, theNewEntry.iServiceGroup);
					continue;
					}
				if(entry.iServiceGroup!=theNewEntry.iServiceGroup)
					{
					theNewCWList->AddEntryL(entry);
					}
				}				
			}
			
		}
	// now delete the old memory	
	CleanupStack::PopAndDestroy(thePrevCWList);	
	}

