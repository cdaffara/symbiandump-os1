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
// callBarring status and registration information.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimCallBarringTraces.h"
#endif

#include <testconfigfileparser.h>
#include "CSimCallBarring.h"
#include "CSimPhone.h"

// The Mobile Basic Service Groups used - originally were magic numbers from 1 to 6 incl;
const TInt KMobServiceIndxStart = 1;
const TInt KMobServiceIndxEnd   = 6;


CSimCallBarring* CSimCallBarring::NewL(CSimPhone* aPhone)
/**
 * Standard two-phase constructor.
 * @param aPhone				The parent phone object.
 * @return CSimNetworkStatus	The new network status class.
 */
	{
	CSimCallBarring* self=new(ELeave) CSimCallBarring(aPhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSimCallBarring::CSimCallBarring(CSimPhone* aPhone)
		: iPhone(aPhone)
/**
 * Trivial first phase construction.
 * @param aPhone				The parent phone object.
 */
	{
	}

void CSimCallBarring::ConstructL()
/**
	Second phase of 2-Phase Constructor
  	Retrieves all the Call Barring related tags from the config file
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLBARRING_CONSTRUCTL_1, "Starting to parse Call Barring config parameters...");

	iPassword.Copy(CfgFile()->ItemValue(KCBPassword,KCBDefaultPassword));

	iGetCBStatus = new(ELeave) CArrayPtrFlat<CListReadAllAttempt>(1);
	FindAndCreateCBListL();
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLBARRING_CONSTRUCTL_2, "...Finished parsing Call Barring config parameters...");
	}
	
void CSimCallBarring::FindAndCreateCBListL()
{
/**
  	Retrieves all the Call barring tags that define the 
  	original status of Call barring from the config file
*/
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLBARRING_FINDANDCREATECBLISTL_1, "CSimPhone::FindAndCreateCBListL");
	RMobilePhone::TMobilePhoneCBInfoEntryV1 entry;

	iCBList = CMobilePhoneCBList::NewL();
	TInt count=CfgFile()->ItemCount(KCBList);
	const CTestConfigItem* item=NULL;
	TInt ret=KErrNone;

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLBARRING_FINDANDCREATECBLISTL_2, "Starting to Load and Parse CBList Config parameters");
	TInt i;
	for(i=0;i<count;i++)
		{
		item=CfgFile()->Item(KCBList,i);
		if(!item)
			break;
		
		TInt condition, serviceGroup, status;
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,condition);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLBARRING_FINDANDCREATECBLISTL_3, "WARNING - CONFIGURATION FILE PARSING - Reading element CONDITION returned %d (element no. %d) from tag %s.",ret,0,KCBList);
			continue;
			}
		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,serviceGroup);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLBARRING_FINDANDCREATECBLISTL_4, "WARNING - CONFIGURATION FILE PARSING - Reading element SERVICEGROUP returned %d (element no. %d) from tag %s.",ret,1,KCBList);
			continue;
			}

		ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,status);
		if(ret!=KErrNone)
			{
			OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLBARRING_FINDANDCREATECBLISTL_5, "WARNING - CONFIGURATION FILE PARSING - Reading element STATUS returned %d (element no. %d) from tag %s.",ret,2,KCBList);
			continue;
			}
		
		entry.iCondition=(RMobilePhone::TMobilePhoneCBCondition)condition;
		entry.iServiceGroup=(RMobilePhone::TMobileService)serviceGroup;
		entry.iStatus=(RMobilePhone::TMobilePhoneCBStatus)status;

		iCBList->AddEntryL(entry);
		}
	}

CSimCallBarring::~CSimCallBarring()
	{
	delete iCBList;
	if (iGetCBStatus)
		iGetCBStatus->ResetAndDestroy();
	delete iGetCBStatus;
	}

TInt CSimCallBarring::ExtFunc(const TTsyReqHandle aReqHandle,const TInt aIpc, const TDataPackage& aPckg)
	{
	/**
  	Handles the phone based requests that apply to call barring
  	@param aReqHandle Handle to the request
  	@param aIpc request identifier
  	@param aPckg contains additional information for the requests
	*/

	TAny* dataPtr = aPckg.Ptr1();
	TAny* dataPtr2 = aPckg.Ptr2();
	// The following requests can be completed even if the completion of another request is pending.
	switch(aIpc)
		{
	case EMobilePhoneSetCallBarringStatus:
		return SetCallBarringStatus(aReqHandle,
			reinterpret_cast<RMobilePhone::TMobilePhoneCBCondition*>(dataPtr),
			reinterpret_cast<RMobilePhone::TMobilePhoneCBChangeV1*>(dataPtr2));
	case EMobilePhoneSetCallBarringPassword:
		return SetCallBarringPassword(aReqHandle,reinterpret_cast<RMobilePhone::TMobilePhonePasswordChangeV1*>(dataPtr));
	case EMobilePhoneNotifyCallBarringStatusChange:
		return NotifyCallBarringStatusChange(aReqHandle, reinterpret_cast<RMobilePhone::TMobilePhoneCBCondition*>(dataPtr));
	case EMobilePhoneGetBarringStatusPhase1:
		return GetCallBarringStatusPhase1(aReqHandle, 
			REINTERPRET_CAST(CRetrieveMobilePhoneCBList::TGetCallBarringRequest*, dataPtr), 
			REINTERPRET_CAST(TInt*, dataPtr2));
	case EMobilePhoneGetBarringStatusPhase2:
		return GetCallBarringStatusPhase2(aReqHandle, 
			REINTERPRET_CAST(RMobilePhone::TClientId*, dataPtr), aPckg.Des2n());		
	default:
		break;
		}
	return KErrNotSupported;
	}
	
TInt CSimCallBarring::CancelService(const TInt aIpc, const TTsyReqHandle aReqHandle)
	{
	/**
  	Cancels any phone based request regarding call barring
  	@param aIpc the IPC to cancel
  	@param aReqHandle handle to the request
	*/
	switch(aIpc)
		{
	case EMobilePhoneSetCallBarringStatus:
		return SetCallBarringStatusCancel(aReqHandle);
	case EMobilePhoneSetCallBarringPassword:
		return SetCallBarringPasswordCancel(aReqHandle);
	case EMobilePhoneNotifyCallBarringStatusChange:
		return NotifyCallBarringStatusChangeCancel(aReqHandle);
	default:
		break;
		}
	return KErrNone;
	}

TInt CSimCallBarring::NotifyCallBarringStatusChange(const TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhoneCBCondition* aCB)
	{	
	/**
	Requests to be notified of change in the Call baring status
	@param aReqHandle Handle to the request
	@param aCB condition for which the client is interested in being notified
  	*/
	__ASSERT_ALWAYS(!iCBNotification.iCBChangeInfoNotificationPending,SimPanic(ENotificationReqAlreadyOutstanding));
	iCBNotification.iCBChangeInfoNotificationPending=ETrue;
	iCBNotification.iCBChangeInfoReqHandle=aReqHandle;
	iCBNotification.iCurrentCBCondition=aCB;
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLBARRING_NOTIFYCALLBARRINGSTATUSCHANGE_1, "Finished CSimCallBarring::NotifyCallBarringStatusChange");
	return KErrNone;
	}

TInt CSimCallBarring::NotifyCallBarringStatusChangeCancel(const TTsyReqHandle aReqHandle)
	{
	/**
	Cancels request to be notified of call barring status change
	@param aReqHandle Handle to the request
	*/
	if(iCBNotification.iCBChangeInfoNotificationPending)
		{
		iCBNotification.iCBChangeInfoNotificationPending=EFalse;
		iPhone->ReqCompleted(aReqHandle,KErrCancel);
		}
	return KErrNone;
	}
	
TInt CSimCallBarring::SetCallBarringStatus(const TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhoneCBCondition* aCB,  RMobilePhone::TMobilePhoneCBChangeV1* aInfo)
	{
	/**
	Sets the status for call barring
	@param aReqHandle Handle to the request
	@param aCB condition for which the client whishes to set the call barring status
	@param aInfo additional parameters to set the call barring status
  	*/
	//Password does not correspond, return KErrAccessDenied .. is that right ? Not specified in SDK!
	if (iPassword != aInfo->iPassword)
		{
		iPhone->ReqCompleted(aReqHandle,KErrAccessDenied);
		return KErrNone;
		}
	
	// make sure we know that the operation concluded sucessfully
	TRAPD(ret,UpdateCBListL(aCB,aInfo)); 	
	if ( ret != KErrNone )
		{ 
		iPhone->ReqCompleted(aReqHandle,ret);
		return KErrNone;
		}
		
	// otherwise proceed and notify all went well
	// Notify change
	if (iCBNotification.iCBChangeInfoNotificationPending)
		{
		*(iCBNotification.iCurrentCBCondition)=*aCB;
		iCBNotification.iCBChangeInfoNotificationPending=EFalse;
		iPhone->ReqCompleted(iCBNotification.iCBChangeInfoReqHandle,KErrNone);
		}
		
	iPhone->ReqCompleted(aReqHandle,KErrNone);
		
	return KErrNone;
	}

TInt CSimCallBarring::SetCallBarringStatusCancel(const TTsyReqHandle aReqHandle)
	{
	/**
	Cancels the request to sets the status for call barring
	@param aReqHandle Handle to the request
  	*/
	 
	iPhone->ReqCompleted(aReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CSimCallBarring::SetCallBarringPassword(const TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhonePasswordChangeV1* aInfo)	
	{
	/**
	Sets the password that prevents the call barring status to be modified
	@param aReqHandle Handle to the request
	@param aInfo The old and new password
  	*/
	if (iPassword == aInfo->iOldPassword)
		{
		iPassword = aInfo->iNewPassword;
		iPhone->ReqCompleted(aReqHandle,KErrNone);
		}
	else 
		{
		iPhone->ReqCompleted(aReqHandle,KErrAccessDenied);
		}
	return KErrNone;
	}

TInt CSimCallBarring::SetCallBarringPasswordCancel(const TTsyReqHandle aReqHandle)
	{
	/**
	cancels a request to set the password that prevents the call barring status to be modified
	@param aReqHandle Handle to the request
  	*/
	iPhone->ReqCompleted(aReqHandle,KErrCancel);
	return KErrNone;
	}
	
	
const CTestConfigSection* CSimCallBarring::CfgFile()
/**
 * Returns a pointer to the current configuration file section.
 *
 * @return CTestConfigSection	A pointer to the current configuration file data section.
 */
	{
	return iPhone->CfgFile();
	}

TInt CSimCallBarring::GetCallBarringStatusPhase1(const TTsyReqHandle aTsyReqHandle, 
												 CRetrieveMobilePhoneCBList::TGetCallBarringRequest* aReqData, 
												 TInt* aBufSize)
	{
	/**
	1st phase retrieval of the the call barring status list
	@param aReqHandle Handle to the request
	@param aReqData information about the request
	@param aBufSize Size of the buffer the client has to allocate for the 2nd pahase
  	*/
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLBARRING_GETCALLBARRINGSTATUSPHASE1_1, "CSimPhone::GetCallBarringStatusPhase1");
	
	TInt ret=KErrNone;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, ret=ProcessGetCallBarringStatusPhase1L(aTsyReqHandle, aReqData, aBufSize););
	if (leaveCode != KErrNone)
			iPhone->ReqCompleted(aTsyReqHandle,leaveCode);

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLBARRING_GETCALLBARRINGSTATUSPHASE1_2, "CSimPhone::GetCallBarringStatusPhase1");
	return ret;
	}
	
TInt CSimCallBarring::ProcessGetCallBarringStatusPhase1L(const TTsyReqHandle aTsyReqHandle, 
														 CRetrieveMobilePhoneCBList::TGetCallBarringRequest* aReqData, 
														 TInt* aBufSize)
	{
	/** Retrieve call barring status of each line from phone, 
	store each CB status response as a list entry,
	stream the list and then return size of this buffer to client
	@param aReqHandle Handle to the request
	@param aReqData information about the request
	@param aBufSize Size of the buffer the client has to allocate for the 2nd pahase
	*/
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLBARRING_PROCESSGETCALLBARRINGSTATUSPHASE1L_1, "CSimCallBarring::ProcessGetCallBarringStatusPhase1L");

	CMobilePhoneCBList* list=CMobilePhoneCBList::NewL();
	CleanupStack::PushL(list);
		

	TInt cnt=0;//Only interested by entries with a particular condition
	TInt maxNum=iCBList->Enumerate();
	//if testing all conditions and all CB are non-active
	// then we have to return a single entry EBarAllCases and EAllServices
	if ( maxNum )
		{
		for(TInt i=0;i<maxNum;i++)
			{
			if((iCBList->GetEntryL(i).iCondition == aReqData->iCondition)
			||(aReqData->iCondition==RMobilePhone::EBarUnspecified))
				{
				list->AddEntryL(iCBList->GetEntryL(i));
				cnt++;
				}
			}
		}
	else
		{
		// if the list is empty and the querry general then we have to return general info
		if ( aReqData->iCondition == RMobilePhone::EBarUnspecified )
			{
			RMobilePhone::TMobilePhoneCBInfoEntryV1 entry;
			
			// Check that the data structure is supported by the simulated TSY version
			TInt err = iPhone->CheckSimTsyVersion(entry);
			if(err != KErrNone)
				{
				iPhone->ReqCompleted(aTsyReqHandle, err);
				return KErrNone;
				}

			entry.iCondition 	= RMobilePhone::EBarAllCases;
			entry.iServiceGroup	= RMobilePhone::EAllServices;
			entry.iStatus		= RMobilePhone::ECallBarringStatusNotActive;
			list->AddEntryL( entry );
			}
		}

	// Store the streamed list and the client ID
	CListReadAllAttempt* read=CListReadAllAttempt::NewL(aReqData->iClient,aTsyReqHandle);
	CleanupStack::PushL(read);
	
	read->iListBuf = list->StoreLC();
	CleanupStack::Pop(); // pop the CBufBase allocated by StoreLC
	
	iGetCBStatus->AppendL(read);
	CleanupStack::Pop(read); // pop the CListReadAllAttempt
	
	// return the CBufBase’s size to client
	*aBufSize=(read->iListBuf)->Size();
	
	CleanupStack::PopAndDestroy(list); // pop&destroy list
	
	// Complete first phase of list retrieval
	iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLBARRING_PROCESSGETCALLBARRINGSTATUSPHASE1L_2, "CSimCallBarring::ProcessGetCallBarringStatusPhase1L");
	return KErrNone;
	}

TInt CSimCallBarring::GetCallBarringStatusPhase2(const TTsyReqHandle aTsyReqHandle, 
												 RMobilePhone::TClientId* aClient, TDes8* aBuf)
	{
	/**
	2nd phase retrieval of the the call barring status list
	@param aReqHandle Handle to the request
	@param aClient Ponter to the client
	@param aBuf Buffer containiong the call barring status list
	*/
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLBARRING_GETCALLBARRINGSTATUSPHASE2_1, "CSimCallBarring::GetCallBarringStatusPhase2");
	CListReadAllAttempt* read=NULL;
	// Find the get detected network attempt from this client
	for (TInt i=0; i<iGetCBStatus->Count(); ++i)
		{
		read = iGetCBStatus->At(i);
		if ((read->iClient.iSessionHandle==aClient->iSessionHandle) &&
		    (read->iClient.iSubSessionHandle==aClient->iSubSessionHandle))
			{
			TPtr8 bufPtr((read->iListBuf)->Ptr(0));
			// Copy the streamed list to the client
			aBuf->Copy(bufPtr);
			delete read;
			iGetCBStatus->Delete(i);
			iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
			return KErrNone;
			}
		}
	// Should handle error case of not finding the matching client from read all phase 1
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLBARRING_GETCALLBARRINGSTATUSPHASE2_2, "CSimCallBarring::GetCallBarringStatusPhase2");
	return KErrNotFound;
	}

TInt CSimCallBarring::GetCallBarringStatusCancel(const TTsyReqHandle aTsyReqHandle)
	{
	/*
	Cancels a Request to retrieve the call barring status list
	@param aReqHandle Handle to the request
	*/
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLBARRING_GETCALLBARRINGSTATUSCANCEL_1, "CSimCallBarring::GetCallBarringStatusCancel");
	iPhone->ReqCompleted(aTsyReqHandle,KErrNone);
	// Remove the read all attempt from iGetCBStatus
	CListReadAllAttempt* read=NULL;
	for (TInt i=0; i<iGetCBStatus->Count(); ++i)
		{
		read = iGetCBStatus->At(i);
		if (read->iReqHandle == aTsyReqHandle)
			{
			delete read;
			iGetCBStatus->Delete(i);
			break;
			}
		}
	iPhone->ReqCompleted(aTsyReqHandle,KErrCancel);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMCALLBARRING_GETCALLBARRINGSTATUSCANCEL_2, "CSimCallBarring::GetCallBarringStatusCancel");
	return KErrNone;
	}

// some useful functions for checking the type of conditions
inline TBool IsOutgoingCondition( RMobilePhone::TMobilePhoneCBCondition aCondition )
	{
	return ( (aCondition)==RMobilePhone::EBarAllOutgoing 		  	|| 
			 (aCondition)==RMobilePhone::EBarOutgoingInternational 	||  	
			 (aCondition)==RMobilePhone::EBarOutgoingInternationalExHC );
	}
	
inline TBool IsIncomingCondition( RMobilePhone::TMobilePhoneCBCondition aCondition )
	{
	return ( (aCondition)==RMobilePhone::EBarAllIncoming ||  	
		 	 (aCondition)==RMobilePhone::EBarIncomingRoaming );
	}	
	
inline TBool IsDeactivationConditionOnly( RMobilePhone::TMobilePhoneCBCondition aCondition )
	{
	return ( (aCondition)==RMobilePhone::EBarAllCases 			 ||  	
		 	 (aCondition)==RMobilePhone::EBarAllOutgoingServices ||
		 	 (aCondition)==RMobilePhone::EBarAllIncomingServices );
	}	
	
inline void SplitOneEntryIntoSeparateGroupsL( 
								CMobilePhoneCBList* aCBList,
								RMobilePhone::TMobilePhoneCBInfoEntryV1 aOrigEntry,
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
	RMobilePhone::TMobilePhoneCBInfoEntryV1 anotherEntry = aOrigEntry;

	// NOTE: A call to CheckSimTsyVersion must be inserted here if there is a 
	// change to the version of the structure declared above

	//anotherEntry.iCondition = aOrigEntry.iCondition;
	//anotherEntry.iStatus = RMobilePhone::ECallBarringStatusActive;
	RMobilePhone::TMobileService service;
	
	for (TInt i = KMobServiceIndxStart;i <= KMobServiceIndxEnd;i++)
		{
		service=static_cast<RMobilePhone::TMobileService>(i);
		if( service != aExceptServiceGroup )
			{
			anotherEntry.iServiceGroup=service;
			aCBList->AddEntryL(anotherEntry);
			}
		}//end for (another)
	}

void CSimCallBarring::UpdateCBListL(RMobilePhone::TMobilePhoneCBCondition* aCB, RMobilePhone::TMobilePhoneCBChangeV1* aCBInfo )
	{
	/*
	Updates the status of call barring.
	@param aCB condition to set
	@param aCBInfo details of the call barring status to set
	*/
	// no sense processing if either condition or change info are NULLS 
	User::LeaveIfNull(aCBInfo);
	User::LeaveIfNull(aCB);
    
    // check for more conditions as some of the combinations really do not make sense
    // and no need to do processing in this case
	if (*aCB == RMobilePhone::EBarUnspecified)
		User::Leave(KErrArgument);
	if ( IsDeactivationConditionOnly(*aCB) 
	     && ( !( (aCBInfo->iAction == RMobilePhone::EServiceActionDeactivate )
	           || (aCBInfo->iAction == RMobilePhone::EServiceActionErase ))))
		User::Leave(KErrArgument);

	//Lets do some work then
	RMobilePhone::TMobilePhoneCBInfoEntryV1 theNewEntry;

	theNewEntry.iCondition = *aCB;
	theNewEntry.iServiceGroup = aCBInfo->iServiceGroup;
	switch (aCBInfo->iAction)
		{
		case RMobilePhone::EServiceActionRegister:
		case RMobilePhone::EServiceActionActivate:
			theNewEntry.iStatus= RMobilePhone::ECallBarringStatusActive;
			break;
		case RMobilePhone::EServiceActionDeactivate:
		case RMobilePhone::EServiceActionErase:
			theNewEntry.iStatus= RMobilePhone::ECallBarringStatusNotActive;
			break;
		case RMobilePhone::EServiceActionUnspecified:
		default:
			// no sense doing any further operation - leave
			User::Leave(KErrArgument);
			break;
		}
	 
	// retain prev version of the list in local variable and in Cleanup stack
	CMobilePhoneCBList* thePrevCBList = CMobilePhoneCBList::NewL();
	CleanupStack::PushL(thePrevCBList);
	// copy existing data into the temp storage
	for (TInt indx=0; indx<iCBList->Enumerate(); indx++)
		{ 
		thePrevCBList->AddEntryL(iCBList->GetEntryL(indx));
		}
	delete iCBList;
	iCBList = NULL;
	// get a hold of it in a local variable       
	iCBList = CMobilePhoneCBList::NewL();
	// get a hold of it in a local variable       
	CMobilePhoneCBList* theNewCBList = iCBList;	
	RMobilePhone::TMobilePhoneCBInfoEntryV1 entry;

	TInt count = thePrevCBList->Enumerate();

	if (theNewEntry.iStatus == RMobilePhone::ECallBarringStatusActive)
		{
		if (theNewEntry.iServiceGroup==RMobilePhone::EAllServices)
			{
			//Activate all conds
			//action is to ActivateAllServices with a particular condition;
			//Only copy entries with a different condition	
			for (TInt i = 0; i<count; i++)
				{
				entry = thePrevCBList->GetEntryL(i);
				//check if the new entry will encompass conditions already defined
				// if it does the existing entry is not needed
				if (!(
  					theNewEntry.iCondition==entry.iCondition 
  					|| 
  					( IsOutgoingCondition(entry.iCondition) && IsOutgoingCondition(theNewEntry.iCondition))
  					||
  					( IsIncomingCondition(entry.iCondition) && IsIncomingCondition(theNewEntry.iCondition))
  					))
					theNewCBList->AddEntryL(entry);
				}//end for	
				theNewCBList->AddEntryL(theNewEntry);			
			}
		else
			{
			//ActivateOne;
			// working under the assumption that if EBarOutgoingInternational is enabled 
			// for EAllServices then setting the fax group, for example to EBarOutgoingInternational 
			// specifically, will be ignored due to inclusion. Also setting the same condition 
			// for the same service group(s) again will be ignored. 
            TBool addNewElem = TRUE;
            
			for (TInt i = 0; i<count; i++)
				{
				entry = thePrevCBList->GetEntryL(i);
				// are in the same direction?
				if ( (IsOutgoingCondition(theNewEntry.iCondition) && IsOutgoingCondition(entry.iCondition))
					|| (IsIncomingCondition(theNewEntry.iCondition) && IsIncomingCondition(entry.iCondition)))
					{ 
					// they are in the same direction
					// is the current entry specific to a group or not ?
					if (entry.iServiceGroup != RMobilePhone::EAllServices)
						{	
						if (theNewEntry.iServiceGroup != entry.iServiceGroup)
							theNewCBList->AddEntryL(entry);	
						}
					else
						{
						// the entry is for all groups
						if (theNewEntry.iCondition == entry.iCondition)
							{
							// the group already precludes this condition; 
							// just add the existing
							theNewCBList->AddEntryL(entry);	
							addNewElem = FALSE;
							}
						else
							{
							// we have to split the entry into entries 
							// per group with the same condition 
							SplitOneEntryIntoSeparateGroupsL( 
								        theNewCBList, entry, theNewEntry.iServiceGroup
										);
							}// end split entry
						}
					}
				else 
					{
					// they are in different directions
					// no need to check further; just add it
					theNewCBList->AddEntryL(entry);	
					}//end are in the same direction?
				}// for loop
				if (addNewElem) 
					{
					theNewCBList->AddEntryL(theNewEntry);
					}	
			}//end else
		}	
	else //deactivate
		{
		if (
			(theNewEntry.iServiceGroup==RMobilePhone::EAllServices)
			&&(theNewEntry.iCondition==RMobilePhone::EBarAllCases)
			)
			;//Deactivate all conditions: do nothing, list is empty.
		else
			{ 
			for (TInt i = 0; i<count; i++)
				{
				entry = thePrevCBList->GetEntryL(i); 
				
				if (theNewEntry.iCondition == RMobilePhone::EBarAllCases
				||
				(( theNewEntry.iCondition == RMobilePhone::EBarAllOutgoingServices || IsOutgoingCondition(theNewEntry.iCondition))
				   && IsOutgoingCondition(entry.iCondition))
				|| 
				((theNewEntry.iCondition == RMobilePhone::EBarAllIncomingServices || IsIncomingCondition(theNewEntry.iCondition))
				   && IsIncomingCondition(entry.iCondition)))
					{
					if(theNewEntry.iServiceGroup!=RMobilePhone::EAllServices
						&& entry.iServiceGroup==RMobilePhone::EAllServices)
						{
						SplitOneEntryIntoSeparateGroupsL( 
						        theNewCBList, entry, theNewEntry.iServiceGroup
								);
						}//end if(create others)
					else
						{//action applies to different service group
						if((theNewEntry.iServiceGroup!=RMobilePhone::EAllServices )
						   && (theNewEntry.iServiceGroup!=entry.iServiceGroup))
							theNewCBList->AddEntryL(entry);
						}
					
					}//end if
				else 
					{//action applies to different condition
					theNewCBList->AddEntryL(entry);
					}//end else
				}//end for (entries)
			}//end else(all)
		}//end else(deactivate)
	
	//destroy the previous version, the local theNewCBList will be destroyed
	CleanupStack::PopAndDestroy(thePrevCBList);
	}
