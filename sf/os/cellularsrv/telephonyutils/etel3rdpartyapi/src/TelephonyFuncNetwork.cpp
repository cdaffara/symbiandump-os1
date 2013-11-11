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
#include "TelephonyActNetwork.h"

TInt CTelephonyFunctions::GetNetworkRegistrationStatusL(TRequestStatus& aRequestStatus, TDes8& aDes)
/**
Initiate async request to retrieve the network registration status.
*/
	{
	if(IsRequestPending(EGetNetworkRegistrationStatus)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EGetNetworkRegistrationStatus, ETrue);
		}
			
	iReqStatusTable[EGetNetworkRegistrationStatus] = &aRequestStatus;
	
	if(!iGetNetworkRegistrationStatus)
		{
		iGetNetworkRegistrationStatus = CGetNetworkRegistrationStatusAct::NewL(this);
		}
		
	iGetNetworkRegistrationStatus->GetNetworkRegistrationStatus(aDes);	
	 
	return KErrNone;
	}
	
TInt CTelephonyFunctions::GetCurrentNetworkInfoL(TRequestStatus& aRequestStatus, TDes8& aDes)
/**
Initiate async request to retrieve current network information.
*/
	{
	if(IsRequestPending(EGetCurrentNetworkInfo)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EGetCurrentNetworkInfo, ETrue);
		}
			
	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}
		
	iReqStatusTable[EGetCurrentNetworkInfo] = &aRequestStatus;
	
	if(!iGetCurrentNetworkInfo)
		{
		iGetCurrentNetworkInfo = CGetCurrentNetworkInfoAct::NewL(this);
		}
		
	iGetCurrentNetworkInfo->GetCurrentNetworkInfo(aDes);	
	 
	return KErrNone;
	}

/**
Initiate async request to retrieve the ICC-stored current network name.

This functionality is not available when the phone is in "flight" mode.

@param aRequestStatus Returns the result code after the asynchronous call completes. 
                      KErrAccessDenied will be returned if the phone is in flight mode.

@param aDes Contains a CTelephony::TNetworkNameV1Pckg, a packaged version of the
            CTelephony::TNetworkNameV1 class which is filled on completion.

@capability None
*/
TInt CTelephonyFunctions::GetCurrentNetworkNameL(TRequestStatus& aRequestStatus, TDes8& aDes)
	{
	if(IsRequestPending(EGetCurrentNetworkName)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EGetCurrentNetworkName, ETrue);
		}

	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}

	if(iTsyVersion == KNoMultimodeTsy || iTsyVersion == KETelExtMultimodeV1 || iTsyVersion == KETelExtMultimodeV2)	//GetCurrentNetworkName not suported in V1/V2 TSY
		{
		return KErrNotSupported;
		}
			
	iReqStatusTable[EGetCurrentNetworkName] = &aRequestStatus;
	
	if(!iGetCurrentNetworkName)
		{
		iGetCurrentNetworkName = CGetCurrentNetworkNameAct::NewL(this);
		}
		
	iGetCurrentNetworkName->GetCurrentNetworkName(aDes);	
	 
	return KErrNone;
	}
	
EXPORT_C CTelephony::TOperatorNameV1::TOperatorNameV1()
/**
Default constructor.
*/
	{
	iVersionId=KEtelISVV1;
	}

/**
Retrieve ICC stored information about the currently registered mobile network. 
Upon successful completion the retrieved network name(s) is returned via the aOperator argument. 

This functionality is not available when the phone is in "flight" mode.

Cancellation of the request can be achieved by passing
CTelephony::EGetOperatorNameCancel to the function CTelephony::CancelAsync().

Note: This information is only available on GSM/WCDMA networks.

@param aRequestStatus Returns the result code after the asynchronous call completes. 
                      KErrAccessDenied will be returned if the phone is in flight mode.

@param aDes Contains a CTelephony::TOperatorNameV1Pckg, a packaged version
            of the CTelephony::TOperatorNameV1 class which is filled on completion.

@capability None
*/
EXPORT_C void CTelephony::GetOperatorName(TRequestStatus& aRequestStatus, TDes8& aDes) const
	{
	TRequestStatus* reqStatusPtr = &aRequestStatus;
	
	__ASSERT_ALWAYS(aRequestStatus != KRequestPending,
		User::Panic(KTelephonyCategory, KTelephonyPanicIllegalReuse) );

	aRequestStatus = KRequestPending;
	TInt rc=KErrInUse;	
		
	TRAPD(leaveCode, rc=iTelephonyFunctions->GetOperatorNameL(aRequestStatus, aDes));

	if(leaveCode != KErrNone)
		{
		iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetOperatorName, EFalse);	
		User::RequestComplete(reqStatusPtr, leaveCode);
		}
	else if (rc != KErrNone)
		{
		if(rc != KErrInUse)	//this means request already pending, so do not reset
			{
			iTelephonyFunctions->SetRequestPending(CTelephonyFunctions::EGetOperatorName, EFalse);
			}
		User::RequestComplete(reqStatusPtr, rc);
		}
	}

/**
Initiate async request to retrieve the operator name.

This functionality is not available when the phone is in "flight" mode.

Note: This information is only available on GSM/WCDMA networks.

@param aRequestStatus Returns the result code after the asynchronous call completes. 
                      KErrAccessDenied will be returned if the phone is in flight mode.

@param aDes Contains a CTelephony::TOperatorNameV1Pckg, a packaged version
            of the CTelephony::TOperatorNameV1 class which is filled on completion.

@capability None
*/
TInt CTelephonyFunctions::GetOperatorNameL(TRequestStatus& aRequestStatus, TDes8& aDes)
	{
	if(IsRequestPending(EGetOperatorName)!=EFalse)
		{
		return KErrInUse;
		}
	else
		{
		SetRequestPending(CTelephonyFunctions::EGetOperatorName, ETrue);
		}

	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}

	if(iTsyVersion == KNoMultimodeTsy || iTsyVersion == KETelExtMultimodeV1)	//GetOperatorName not suported in V1 TSY
		{
		return KErrNotSupported;
		}
			
	iReqStatusTable[EGetOperatorName] = &aRequestStatus;
	
	if(!iGetOperatorName)
		{
		iGetOperatorName = CGetOperatorNameAct::NewL(this);
		}
		
	iGetOperatorName->GetOperatorName(aDes);	
	 
	return KErrNone;
	}
