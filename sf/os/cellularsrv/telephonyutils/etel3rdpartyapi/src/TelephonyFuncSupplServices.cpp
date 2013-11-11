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
* Code for CTelephonyFunctions class, used by CTelephony class.
*
*/


/**
 @file
*/

#include <e32def.h>
#include "TelephonyFunctions.h"
#include "TelephonyActSupplServices.h"

TInt CTelephonyFunctions::GetCFSupplementaryServiceStatusL(TRequestStatus& aRequestStatus, const CTelephony::TCallForwardingCondition aCondition, TDes8& aDes,const CTelephony::TServiceGroup aServiceGroup)
/**
Initiate async request to interrogate the call forwarding status for a specified service group and condition.
*/
	{
	if(IsRequestPending(EGetCFSupplServicesStatus)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EGetCFSupplServicesStatus, ETrue);		
		}
			
	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}
		
	iReqStatusTable[EGetCFSupplServicesStatus] = &aRequestStatus;
	
	if(!iGetCFSupplementaryServiceStatus)
		{
		iGetCFSupplementaryServiceStatus = CGetSupplementaryServiceStatusAct::NewL(this);	
		}
	
	return iGetCFSupplementaryServiceStatus->GetCFSupplementaryServiceStatus(aCondition, aDes, aServiceGroup);
	}
	
EXPORT_C void CTelephony::GetCallBarringStatus(TRequestStatus& aRequestStatus, const TCallBarringCondition aCondition, TDes8& aDes, const TServiceGroup aServiceGroup) const
/**
Interrogate the current status of the call barring services. 

Specify the condition for enquiry via the aCondition argument. 
Upon successful completion the status information is returned via the aSSInfo argument. 
Support is available only for the voice line.

This functionality is not available when the phone is in "flight" mode.

@param aRequestStatus Returns the result code after the asynchronous call completes. Pass CTelephony::CancelAsync() CTelephony::EGetCallBarringStatusCancel to cancel.

@param aCondition Call barring condition to be interrogated.
@param aDes  Contains a CTelephony::TCallBarringSupplServicesV1Pckg, a packaged version of the CTelephony::TCallBarringSupplServicesV1 class which is filled on completion.
@param aServiceGroup Service group to be interrogated.

@capability ReadDeviceData
@capability NetworkServices
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->GetCBSupplementaryServiceStatusL(aRequestStatus, aCondition, aDes, aServiceGroup));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetCBSupplServicesStatus, EFalse);	
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetCBSupplServicesStatus, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
TInt CTelephonyFunctions::GetCBSupplementaryServiceStatusL(TRequestStatus& aRequestStatus, const CTelephony::TCallBarringCondition aCondition, TDes8& aDes, const CTelephony::TServiceGroup aServiceGroup)
/**
Initiate async request to interrogate the call barring status for a specified service group and condition.
*/
	{
	if(IsRequestPending(EGetCBSupplServicesStatus)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EGetCBSupplServicesStatus, ETrue);
		}
			
	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}
		
	iReqStatusTable[EGetCBSupplServicesStatus] = &aRequestStatus;
	
	if(!iGetCBSupplementaryServiceStatus)
		{
		iGetCBSupplementaryServiceStatus = CGetSupplementaryServiceStatusAct::NewL(this);	
		}
	
	return iGetCBSupplementaryServiceStatus->GetCBSupplementaryServiceStatus(aCondition, aDes, aServiceGroup);
	}
	
EXPORT_C void CTelephony::GetCallWaitingStatus(TRequestStatus& aRequestStatus, TDes8& aDes, const TServiceGroup aServiceGroup) const
/**
Interrogate the current status of the call waiting services. 

Upon successful completion the status information is returned via the aSSInfo argument. 
Support is available only for the voice line.

This functionality is not available when the phone is in "flight" mode.

@param aRequestStatus Returns the result code after the asynchronous call completes. Pass CCTelephony::ancelAsync() CTelephony::EGetCallWaitingStatusCancel to cancel.
@param aDes  Contains a CTelephony::TCallWaitingSupplServicesV1Pckg, a packaged version of the CTelephony::TCallWaitingSupplServicesV1 class which is filled on completion.
@param aServiceGroup Service group to be interrogated.

@capability ReadDeviceData
@capability NetworkServices
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->GetCWSupplementaryServiceStatusL(aRequestStatus, aDes, aServiceGroup));

	if(leaveCode != KErrNone)
		{	
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetCWSupplServicesStatus, EFalse);
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetCWSupplServicesStatus, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}
	
TInt CTelephonyFunctions::GetCWSupplementaryServiceStatusL(TRequestStatus& aRequestStatus, TDes8& aDes, const CTelephony::TServiceGroup aServiceGroup)
/**
Initiate async request to interrogate the call waiting status for a specified service group.
*/
	{
	if(IsRequestPending(EGetCWSupplServicesStatus)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EGetCWSupplServicesStatus, ETrue);
		}
			
	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}
		
	iReqStatusTable[EGetCWSupplServicesStatus] = &aRequestStatus;
	
	if(!iGetCWSupplementaryServiceStatus)
		{
		iGetCWSupplementaryServiceStatus = CGetSupplementaryServiceStatusAct::NewL(this);	
		}
	
	return iGetCWSupplementaryServiceStatus->GetCWSupplementaryServiceStatus(aDes, aServiceGroup);
	}
	
EXPORT_C CTelephony::TIdentityServiceV1::TIdentityServiceV1()
/**
Default constructor.
*/
	{
	iVersionId=KEtelISVV1;
	}

EXPORT_C void CTelephony::GetIdentityServiceStatus(TRequestStatus& aRequestStatus, const CTelephony::TIdentityService& aService, TDes8& aStatus) const
/**
Interrogate the current status of the identity services. 

Specify the service for enquiry via the aService argument. 
Upon successful completion the service status is returned via the aStatus argument.

This functionality is not available when the phone is in "flight" mode.

Note: This information is only available on GSM/WCDMA networks.

@param aRequestStatus Returns the result code after the asynchronous call completes. Pass CTelephony::CancelAsync() CTelephony::EGetIdentityServiceStatusCancel to cancel.
@param aService  
@param aStatus Contains a CTelephony::TIdentityServiceV1Pckg, a packaged version of the CTelephony::TIdentityServiceV1 class which is filled on completion.

@capability None
*/
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->GetIdentityServiceStatusL(aRequestStatus, aService, aStatus));

	if(leaveCode != KErrNone)
		{
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetIdentityServiceStatus, EFalse);	
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetIdentityServiceStatus, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}

TInt CTelephonyFunctions::GetIdentityServiceStatusL(TRequestStatus& aRequestStatus, const CTelephony::TIdentityService& aService, TDes8& aStatus)
/**
Initiate async request to retrieve the identity service status.
*/
	{
	if(IsRequestPending(EGetIdentityServiceStatus)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EGetIdentityServiceStatus, ETrue);
		}
			
	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}
		
	iReqStatusTable[EGetIdentityServiceStatus] = &aRequestStatus;
	
	if(!iGetIdentityServiceStatus)
		iGetIdentityServiceStatus = CGetIdentityServiceStatusAct::NewL(this);

	iGetIdentityServiceStatus->GetIdentityServiceStatus(aService, aStatus);	

	return KErrNone;
	}

