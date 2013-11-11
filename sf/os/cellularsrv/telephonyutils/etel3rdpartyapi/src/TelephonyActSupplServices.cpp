/*
* Copyright (c) 2009 Sony Ericsson Mobile Communications AB
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Sony Ericsson Mobile Communications AB - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
* Code for TelephonyActSupplServices class, used by CTelephonyFunctions class.
*
*/


/**
 @file
*/

#include "TelephonyFunctions.h" 
#include "TelephonyActSupplServices.h"


CGetSupplementaryServiceStatusAct* CGetSupplementaryServiceStatusAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CGetSupplementaryServiceStatusAct* self = new(ELeave) CGetSupplementaryServiceStatusAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}

CGetSupplementaryServiceStatusAct::~CGetSupplementaryServiceStatusAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	delete iGetCFList;
	iGetCFList = NULL;
	delete iGetCBList;
	iGetCBList = NULL;
	delete iGetCWList;
	iGetCWList = NULL;	
	}

TInt CGetSupplementaryServiceStatusAct::GetCFSupplementaryServiceStatus(const CTelephony::TCallForwardingCondition aCondition, TDes8& aId, const CTelephony::TServiceGroup aServiceGroup)
/**
Issue Request
*/
	{
	iISVSSType = CTelephonyFunctions::EGetCFSupplServicesStatus;
	iServiceGroup = GetMobileServiceGroup(aServiceGroup);
	iMobileCFCondition = GetMobileCFCondition(aCondition);
	if(iMobileCFCondition == RMobilePhone::ECallForwardingUnspecified)
		{
		return KErrNotSupported;
		}
	iCFCondition = aCondition;
	
	//delete old list
	if(iGetCFList)
		{
		delete iGetCFList;
		iGetCFList = NULL;
		}
		
	iISVCFInfo = reinterpret_cast<CTelephony::TCallForwardingSupplServicesV1*> ( const_cast<TUint8*> (aId.Ptr()) );
	//create instance of CF list			
	TRAPD(leavecode, iGetCFList = CRetrieveMobilePhoneCFList::NewL(*iTelephonyFunctions->Phone()));
	if(leavecode != KErrNone)
		{
		return leavecode;
		}
		
	iGetCFList->Start(iStatus, iMobileCFCondition, iServiceGroup);
	SetActive();
	return KErrNone;			
	}
	
TInt CGetSupplementaryServiceStatusAct::GetCBSupplementaryServiceStatus(const CTelephony::TCallBarringCondition aCondition, TDes8& aId, const CTelephony::TServiceGroup aServiceGroup)
/**
Issue Request
*/
	{
	iISVSSType = CTelephonyFunctions::EGetCBSupplServicesStatus;
	iServiceGroup = GetMobileServiceGroup(aServiceGroup);
	iMobileCBCondition = GetMobileCBCondition(aCondition);
	if(iMobileCBCondition == RMobilePhone::EBarUnspecified)	//not supported
		{
		return KErrNotSupported;
		}
	iCBCondition = aCondition;
	
	//delete old list
	if(iGetCBList)
		{
		delete iGetCBList;
		iGetCBList = NULL;
		}
		
	iISVCBInfo = reinterpret_cast<CTelephony::TCallBarringSupplServicesV1*> (const_cast<TUint8*> (aId.Ptr()) );
	//create instance of CB list			
	TRAPD(leavecode, iGetCBList = CRetrieveMobilePhoneCBList::NewL(*iTelephonyFunctions->Phone()));
	if(leavecode != KErrNone)
		{
		return leavecode;
		}
		
	iGetCBList->Start(iStatus, iMobileCBCondition);
	SetActive();
	return KErrNone;	
	}
	
TInt CGetSupplementaryServiceStatusAct::GetCWSupplementaryServiceStatus(TDes8& aId, const CTelephony::TServiceGroup aServiceGroup)
/**
Issue Request
*/
	{
	iISVSSType = CTelephonyFunctions::EGetCWSupplServicesStatus;
	iServiceGroup = GetMobileServiceGroup(aServiceGroup);
		
	//delete old list
	if(iGetCWList)
		{
		delete iGetCWList;
		iGetCWList = NULL;
		}
		
	iISVCWInfo = reinterpret_cast<CTelephony::TCallWaitingSupplServicesV1*> ( const_cast<TUint8*> (aId.Ptr()) );
	//create instance of CW list			
	TRAPD(leavecode, iGetCWList = CRetrieveMobilePhoneCWList::NewL(*iTelephonyFunctions->Phone()));
	if(leavecode != KErrNone)
		{
		return leavecode;
		}
	iGetCWList->Start(iStatus);
	SetActive();	
	return KErrNone;
	}	
	
RMobilePhone::TMobilePhoneCBCondition CGetSupplementaryServiceStatusAct::GetMobileCBCondition(const CTelephony::TCallBarringCondition aCBCondition) const
/**
Maps CTelephony::TCallBarringCondition to  RMobilePhone::TMobilePhoneCBCondition
*/
	{
	RMobilePhone::TMobilePhoneCBCondition condition = RMobilePhone::EBarUnspecified;
	switch(aCBCondition)
		{
	case CTelephony::EBarAllIncoming:
		condition = RMobilePhone::EBarAllIncoming;
		break;
	case CTelephony::EBarIncomingRoaming:
		condition = RMobilePhone::EBarIncomingRoaming;
		break;
	case CTelephony::EBarAllOutgoing:
		condition = RMobilePhone::EBarAllOutgoing;
		break;
	case CTelephony::EBarOutgoingInternational:
		condition = RMobilePhone::EBarOutgoingInternational;
		break;
	case CTelephony::EBarOutgoingInternationalExHC:
		condition = RMobilePhone::EBarOutgoingInternationalExHC;
		break;
	default:
		condition = RMobilePhone::EBarUnspecified;
		break;
		};		
	return condition;
	}
	
RMobilePhone::TMobilePhoneCFCondition CGetSupplementaryServiceStatusAct::GetMobileCFCondition(const CTelephony::TCallForwardingCondition aCFCondition) const
/**
Maps CTelephony::TCallForwardingCondition to  RMobilePhone::TMobilePhoneCFCondition
*/
	{
	RMobilePhone::TMobilePhoneCFCondition condition = RMobilePhone::ECallForwardingUnspecified;
	
	switch(aCFCondition)
		{
	case CTelephony::ECallForwardingUnconditional:
		condition = RMobilePhone::ECallForwardingUnconditional;
		break;
	case CTelephony::ECallForwardingBusy:
		condition = RMobilePhone::ECallForwardingBusy;
		break;
	case CTelephony::ECallForwardingNoReply:
		condition = RMobilePhone::ECallForwardingNoReply;
		break;
	case CTelephony::ECallForwardingNotReachable:
		condition = RMobilePhone::ECallForwardingNotReachable;
		break;
	default:
		condition = RMobilePhone::ECallForwardingUnspecified;
		break;
		};		
	return condition;
	}
	
RMobilePhone::TMobileService CGetSupplementaryServiceStatusAct::GetMobileServiceGroup(const CTelephony::TServiceGroup aServiceGroup)
/**
Map CTelephony::TServiceGroup to RMobilePhone::TMobileService
*/
	{
	RMobilePhone::TMobileService serviceGroup;
	
	switch(aServiceGroup)
		{
	case CTelephony::EFaxService:	
		serviceGroup = RMobilePhone::EFaxService;
		break;
	case CTelephony::EDataService:	
		serviceGroup = RMobilePhone::ECircuitDataService;
		break;
	case CTelephony::EVoiceService:		
	default:	
		serviceGroup = RMobilePhone::EVoiceService;
		}	
	return serviceGroup;
	}


void CGetSupplementaryServiceStatusAct::Complete()
/**
Service Completed request.
*/
	{
	TInt leaveCode=iStatus.Int();	
		
	if(iISVSSType != CTelephonyFunctions::EMaxNumberOperations && iTelephonyFunctions->IsRequestPending(iISVSSType))
		{
		if(leaveCode==KErrNone)
			{		
			switch(iISVSSType)
				{
			case CTelephonyFunctions::EGetCFSupplServicesStatus:
				TRAP(leaveCode, ResolveCFListL());
				break;
			case CTelephonyFunctions::EGetCBSupplServicesStatus:
				TRAP(leaveCode, ResolveCBListL());
				break;
			case CTelephonyFunctions::EGetCWSupplServicesStatus:
				TRAP(leaveCode, ResolveCWListL());
				break;
			default:
				leaveCode = KErrNotSupported;
				}
			}		
		iTelephonyFunctions->CompleteRequest(iISVSSType, iStatus.Int());		
		}
	}
	
void CGetSupplementaryServiceStatusAct::ResolveCBListL()
/**
Resolve the CB list. The CB list only contains conditions that are active for a particular service group. 
We want to know the status for a specific service group AND CB condition.  

The following scenarios are covered.

1.We do not successfully retrieve the list.  
  If this is true then return the status as EUnknown.
2.An entry has the same service group OR is EAllServices AND the same CB condition as requested. 
  If this is true then return the status of the entry.
3.An entry has the same service group OR is EAllServices AND the entry condition is EAllOutgoingServices.  
  If the requested CB condition was either EBarAllOutgoing, EBarOutgoingInternational or EBarOutgoingInternationalExHC then return the status of the entry. 
4.An entry has the same service group OR is EAllServices AND the entry condition is EAllIncomingServices.  
  If the requested CB condition was either EBarAllIncoming or EBarIncomingRoaming then return the status of the entry. 
5.Default case
  return the status as ENotActive.
*/
	{	
	iISVCBInfo->iCallBarringCondition = iCBCondition;
	iISVCBInfo->iCallBarring = CTelephony::EUnknown;	//(Scenario 1) 
	
	CMobilePhoneCBList* callBarringList=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, callBarringList=iGetCBList->RetrieveListL(););
	if (callBarringList)
		{
		CleanupStack::PushL(callBarringList);
		iISVCBInfo->iCallBarring = CTelephony::ENotActive;	//(Scenario 5) 
		
		TInt count = callBarringList->Enumerate();
		RMobilePhone::TMobilePhoneCBInfoEntryV1 entry;	
		for (TInt i=0; i<count; ++i)
			{
			leaveCode = KErrNone;
			TRAP(leaveCode,entry=callBarringList->GetEntryL(i));
			if (leaveCode == KErrNone && (iServiceGroup == entry.iServiceGroup || entry.iServiceGroup == RMobilePhone::EAllServices))  
				{				
				if( iMobileCBCondition == entry.iCondition ||	//(Scenario 2)
				    ( entry.iCondition == RMobilePhone::EBarAllOutgoingServices && //(Scenario 3)
				      (iMobileCBCondition == RMobilePhone::EBarAllOutgoing || 
				       iMobileCBCondition == RMobilePhone::EBarOutgoingInternational || 
				       iMobileCBCondition == RMobilePhone::EBarOutgoingInternationalExHC) ) || 
				    ( entry.iCondition == RMobilePhone::EBarAllIncomingServices && //(Scenario 4)
				      (iMobileCBCondition == RMobilePhone::EBarAllIncoming || 
				       iMobileCBCondition == RMobilePhone::EBarIncomingRoaming) ) )
					{
					iISVCBInfo->iCallBarring = GetCBSupplServiceStatus(entry.iStatus);	
					break;
					}	
				}
			}
		CleanupStack::PopAndDestroy(callBarringList);	//callBarringList
		}
	}
	
CTelephony::TSupplServiceStatus CGetSupplementaryServiceStatusAct::GetCBSupplServiceStatus(RMobilePhone::TMobilePhoneCBStatus aStatus)
/**
Map RMobilePhone::TMobilePhoneCBStatus to CTelephony::TSupplServiceStatus
*/
	{
	CTelephony::TSupplServiceStatus supplServiceStatus;
		
	switch(aStatus)
		{
	case RMobilePhone::ECallBarringStatusActive:
		supplServiceStatus = CTelephony::EStatusActive;
		break;
	case RMobilePhone::ECallBarringStatusNotActive:
		supplServiceStatus = CTelephony::ENotActive;
		break;
	case RMobilePhone::ECallBarringStatusNotProvisioned:
		supplServiceStatus = CTelephony::ENotProvisioned;
		break;
	case RMobilePhone::ECallBarringStatusNotAvailable:
		supplServiceStatus = CTelephony::ENotAvailable;
		break;
	case RMobilePhone::ECallBarringStatusUnknown:
	default:
		supplServiceStatus = CTelephony::EUnknown;
		break;		
		}
	return supplServiceStatus;		
	}
	
void CGetSupplementaryServiceStatusAct::ResolveCFListL()
/**
Resolve the CF list. The CF list should contain an entry for each condition of a particular service group regardless of whether the 
status is active or inactive.. 
We want to know the status for a specific service group AND CF condition.  

The following scenarios are covered.

1.We do not successfully retrieve the list.  
  If this is true then return the status as EUnknown.
2.An entry has the the same service group OR is EAllServices AND the same CF condition as requested. 
  If this is true then return the status of the entry.
3.An entry has the the same service group OR is EAllServices AND the entry condition is ECallForwardingAllCases.  
  If this is true then return the status of the entry.
4.An entry has the the same service group OR is EAllServices AND the entry condition is ECallForwardingAllConditionalCases.  
  If the requested CF condition was either ECallForwardingBusy, ECallForwardingNoReply or ECallForwardingNotReachable then return the status of the entry.
*/
	{
	iISVCFInfo->iCallForwardingCondition = iCFCondition;
	iISVCFInfo->iCallForwarding = CTelephony::EUnknown;	//(Scenario 1) Initially set to unknown until we are sure we successfully get the list.
	
	CMobilePhoneCFList* callForwardingList=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, callForwardingList=iGetCFList->RetrieveListL(););
	if (callForwardingList)
		{
		CleanupStack::PushL(callForwardingList);
		
		TInt count = callForwardingList->Enumerate();
		
		RMobilePhone::TMobilePhoneCFInfoEntryV1 entry;
		
		for (TInt i=0; i<count; ++i)
			{
			leaveCode = KErrNone;
			TRAP(leaveCode,entry=callForwardingList->GetEntryL(i));
			if (leaveCode == KErrNone && (iServiceGroup == entry.iServiceGroup || entry.iServiceGroup == RMobilePhone::EAllServices)) 
				{
				if( iMobileCFCondition == entry.iCondition || //(Scenario 2)	
				    entry.iCondition == RMobilePhone::ECallForwardingAllCases || //(Scenario 3)
				    ( entry.iCondition == RMobilePhone::ECallForwardingAllConditionalCases && //(Scenario 4)
				      (iMobileCFCondition == RMobilePhone::ECallForwardingBusy ||
				       iMobileCFCondition == RMobilePhone::ECallForwardingNoReply ||
				       iMobileCFCondition == RMobilePhone::ECallForwardingNotReachable ) ) )				
					{
					iISVCFInfo->iCallForwarding = GetCFSupplServiceStatus(entry.iStatus);
					break;			
					}							
				}
			}
		CleanupStack::PopAndDestroy(callForwardingList);	//callForwardingList
		}
	}
	
CTelephony::TSupplServiceStatus CGetSupplementaryServiceStatusAct::GetCFSupplServiceStatus(RMobilePhone::TMobilePhoneCFStatus aStatus)
/**
Map RMobilePhone::TMobilePhoneCFStatus to CTelephony::TSupplServiceStatus
*/
	{
	CTelephony::TSupplServiceStatus supplServiceStatus;
		
	switch(aStatus)
		{
	case RMobilePhone::ECallForwardingStatusActive:
		supplServiceStatus = CTelephony::EStatusActive;
		break;
	case RMobilePhone::ECallForwardingStatusNotActive:
	case RMobilePhone::ECallForwardingStatusNotRegistered:
		supplServiceStatus = CTelephony::ENotActive;
		break;
	case RMobilePhone::ECallForwardingStatusNotProvisioned:
		supplServiceStatus = CTelephony::ENotProvisioned;
		break;
	case RMobilePhone::ECallForwardingStatusNotAvailable:
		supplServiceStatus = CTelephony::ENotAvailable;
		break;
	case RMobilePhone::ECallForwardingStatusUnknown:
	case RMobilePhone::ECallForwardingStatusActivePIAllowed:
	case RMobilePhone::ECallForwardingStatusActivePINotAvailable:
	case RMobilePhone::ECallForwardingStatusActivePIClir:
	case RMobilePhone::ECallForwardingStatusActivePIClirSIVerified:
	case RMobilePhone::ECallForwardingStatusActivePIClirSINotScreened:
	case RMobilePhone::ECallForwardingStatusActivePIClirSINetwork:
	default:
		supplServiceStatus = CTelephony::EUnknown;
		break;		
		}
	return supplServiceStatus;		
	}
	
void CGetSupplementaryServiceStatusAct::ResolveCWListL()
/**
Resolve the CB list. The CB list only entries for active status's for a particular service group. 
We want to know the status for a specific service group.  

There are four scenarios to cover.

1.We do not successfully retrieve the list.  If this is true we shall set the status as EUnknown.
2.An entry in the list matches the requested service group.  If this is true we return the status as defined in the entry.
3.An entry in the list has the EAllServices service group.  If this is true we return the status as defined in the entry.
4.Default case: return the status as ENotActive.
*/
	{
	iISVCWInfo->iCallWaiting = CTelephony::EUnknown;	//(Scenario 1)
	
	CMobilePhoneCWList* callWaitingList=NULL;
	TInt leaveCode=KErrNone;
	TRAP(leaveCode, callWaitingList=iGetCWList->RetrieveListL(););
	if (callWaitingList)
		{
		CleanupStack::PushL(callWaitingList);
		
		iISVCWInfo->iCallWaiting = CTelephony::ENotActive; 	//(Scenario 4)
		
		TInt count = callWaitingList->Enumerate();
		RMobilePhone::TMobilePhoneCWInfoEntryV1 entry;
		
		for (TInt i=0; i<count; ++i)
			{
			leaveCode = KErrNone;
			TRAP(leaveCode,entry=callWaitingList->GetEntryL(i));
			if (leaveCode == KErrNone && (iServiceGroup == entry.iServiceGroup || entry.iServiceGroup == RMobilePhone::EAllServices))
				{
				iISVCWInfo->iCallWaiting = GetCWSupplServiceStatus(entry.iStatus);	//(Scenario 2 & 3)	
				break;		
				}
			}
		CleanupStack::PopAndDestroy(callWaitingList);	//callWaitingList
		}
	
	}
			
CTelephony::TSupplServiceStatus CGetSupplementaryServiceStatusAct::GetCWSupplServiceStatus(RMobilePhone::TMobilePhoneCWStatus aStatus)
/**
Map RMobilePhone::TMobilePhoneCWStatus to CTelephony::TSupplServiceStatus
*/
	{
	CTelephony::TSupplServiceStatus supplServiceStatus;
		
	switch(aStatus)
		{
	case RMobilePhone::ECallWaitingStatusActive:
		supplServiceStatus = CTelephony::EStatusActive;
		break;
	case RMobilePhone::ECallWaitingStatusNotActive:
		supplServiceStatus = CTelephony::ENotActive;
		break;
	case RMobilePhone::ECallWaitingStatusNotProvisioned:
		supplServiceStatus = CTelephony::ENotProvisioned;
		break;
	case RMobilePhone::ECallWaitingStatusNotAvailable:
		supplServiceStatus = CTelephony::ENotAvailable;
		break;
	case RMobilePhone::ECallWaitingStatusUnknown:
	default:
		supplServiceStatus = CTelephony::EUnknown;
		break;		
		}
	return supplServiceStatus;		
	}

TInt CGetSupplementaryServiceStatusAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	if(iISVSSType != CTelephonyFunctions::EMaxNumberOperations)
		{
		iTelephonyFunctions->CompleteRequest(iISVSSType, aLeaveCode);
		}
	return KErrNone;	// to ActiveScheduler.
	}

void CGetSupplementaryServiceStatusAct::DoCancel()
/**
Cancel request.

Async request to be cancelled.
*/
	{
	if(iGetCFList)
		{
		iGetCFList->Cancel();
		}		
	if(iGetCBList)
		{
		iGetCBList->Cancel();
		}		
	if(iGetCWList)
		{
		iGetCWList->Cancel();
		}
	}


CGetSupplementaryServiceStatusAct::CGetSupplementaryServiceStatusAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions),
  iISVSSType(CTelephonyFunctions::EMaxNumberOperations)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}


CGetIdentityServiceStatusAct* CGetIdentityServiceStatusAct::NewL(CTelephonyFunctions* aTelephonyFunctions)
/**
Public constructor which can Leave().

@param	aTelephonyFunctions Object that constructs us.
@leave	Leaves if no memory.
*/
	{
	CGetIdentityServiceStatusAct* self = new(ELeave) CGetIdentityServiceStatusAct(aTelephonyFunctions);
	CActiveScheduler::Add(self);
	return self;
	}
	
CGetIdentityServiceStatusAct::~CGetIdentityServiceStatusAct()
/**
Destructor
*/
	{
	Cancel();
	Complete();
	}

void CGetIdentityServiceStatusAct::GetIdentityServiceStatus(const CTelephony::TIdentityService aService, TDes8& aId)
/**
Issue Request
*/
	{
	iISVServiceStatus = reinterpret_cast<CTelephony::TIdentityServiceV1*> ( const_cast<TUint8*> ( aId.Ptr() ) );
	
	RMobilePhone::TMobilePhoneIdService thisService;
	
	switch(aService)
	{
	case CTelephony::EIdServiceUnspecified:
		thisService=RMobilePhone::EIdServiceUnspecified;
		break;
	case CTelephony::EIdServiceCallerPresentation:
		thisService=RMobilePhone::EIdServiceCallerPresentation;
		break;
	case CTelephony::EIdServiceCallerRestriction:
		thisService=RMobilePhone::EIdServiceCallerRestriction;
		break;
	default:
		thisService=RMobilePhone::EIdServiceUnspecified;
		break;
	}

	iTelephonyFunctions->Phone()->GetIdentityServiceStatus(iStatus, thisService, iMMServiceStatus);
	SetActive();
	}

void CGetIdentityServiceStatusAct::Complete()
/**
Service Completed request.
*/
	{
	if(iTelephonyFunctions->IsRequestPending(CTelephonyFunctions::EGetIdentityServiceStatus))
		{
		if(iStatus==KErrNone)
			{
				switch(iMMServiceStatus)
					{
				case RMobilePhone::EIdServiceActivePermanent:
					iISVServiceStatus->iIdentityStatus=CTelephony::EIdServiceActivePermanent;
					break;
				case RMobilePhone::EIdServiceActiveDefaultRestricted:
					iISVServiceStatus->iIdentityStatus=CTelephony::EIdServiceActiveDefaultRestricted;
					break;
				case RMobilePhone::EIdServiceActiveDefaultAllowed:
					iISVServiceStatus->iIdentityStatus=CTelephony::EIdServiceActiveDefaultAllowed;
					break;
				case RMobilePhone::EIdServiceNotProvisioned:
					iISVServiceStatus->iIdentityStatus=CTelephony::EIdServiceNotProvisioned;
					break;
				case RMobilePhone::EIdServiceUnknown:
					iISVServiceStatus->iIdentityStatus=CTelephony::EIdServiceUnknown;
					break;
				default:
					iISVServiceStatus->iIdentityStatus=CTelephony::EIdServiceUnknown;
					}		
			}
		iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetIdentityServiceStatus, iStatus.Int());
		}
	}

TInt CGetIdentityServiceStatusAct::RunError(TInt aLeaveCode)
/**
Handle any Leave() from inside RunL().

@param	aLeaveCode	passed in if RunL Leaves.
@return	KErrNone.
*/
	{
	iTelephonyFunctions->CompleteRequest(CTelephonyFunctions::EGetIdentityServiceStatus, aLeaveCode);
	return KErrNone;	// to ActiveScheduler.
	}

void CGetIdentityServiceStatusAct::DoCancel()
/**
Cancel request.

Async request to dial is cancelled.
*/
	{
	iTelephonyFunctions->Phone()->CancelAsyncRequest(EMobilePhoneGetIdentityServiceStatus);
	}


CGetIdentityServiceStatusAct::CGetIdentityServiceStatusAct(CTelephonyFunctions* aTelephonyFunctions)
: CAsyncRequestBaseAct(),
  iTelephonyFunctions(aTelephonyFunctions)
/**
First-phase constructor which cannot Leave().

@param	aTelephonyFunctions Object that constructs us.
*/
	{
	}
	
