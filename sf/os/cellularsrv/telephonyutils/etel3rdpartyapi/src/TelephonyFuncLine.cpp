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
* Code for TelephonyFuncLine class, used by CTelephony class.
*
*/


/**
 @file
*/

#include <e32def.h>
#include "TelephonyFunctions.h"


TInt CTelephonyFunctions::GetLineStatus(const CTelephony::TPhoneLine& aLine, TDes8& aStatus)
/**
Get the current line status.
*/
	{
	TInt ret=KErrAccessDenied;
	
	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}

	CTelephony::TCallStatusV1& CallStatusV1 = reinterpret_cast<CTelephony::TCallStatusV1&> ( const_cast<TUint8&> ( *aStatus.Ptr() ) );
	RMobileCall::TMobileCallStatus callStatus=RMobileCall::EStatusUnknown;

	switch(aLine)
	{
	case CTelephony::EVoiceLine:
		ret= iLineVoice.GetMobileLineStatus(callStatus);
		break;
	case CTelephony::EDataLine:
		if(iLineIsDataOpen)
			{
			ret= iLineData.GetMobileLineStatus(callStatus);
			}
		else
			{
			return KErrNotSupported;
			}
		break;
	case CTelephony::EFaxLine:
		if(iLineIsFaxOpen)
			{
			ret= iLineFax.GetMobileLineStatus(callStatus);
			}
		else
			{
			return KErrNotSupported;
			}
		break;
	default:
		return KErrNotFound;
	}
	GetCallStatus(callStatus, CallStatusV1.iStatus);
	return ret;
	}	

LOCAL_C void CloseArrayOfCallObjects(TAny* aCallArray)
/**
Close and delete the call object array.
*/
	{
	CArrayFixFlat<RMobileCall>* callArray = reinterpret_cast<CArrayFixFlat<RMobileCall>*>(aCallArray);
	TInt count = callArray->Count();
	for (TInt i=count-1; i>=0; --i)
		{
		callArray->At(i).Close();
		}	
	delete callArray;
 	callArray = NULL; 	
	}

TInt CTelephonyFunctions::GetCallInfoL(TDes8& aCallSelect, TDes8& aCallInfo, TDes8& aRemoteInfo)
/**
Get the current call information.
*/
	{
    CTelephony::TCallSelectionV1& CallSelectionV1 = reinterpret_cast<CTelephony::TCallSelectionV1&> 
                                                        ( const_cast<TUint8&> ( *aCallSelect.Ptr() ) );
    
	if(CallSelectionV1.iLine != CTelephony::EVoiceLine)
		{
		return KErrAccessDenied;
		}

	if(IsFlightModeOn())
		{
		return KErrAccessDenied;
		}

    CTelephony::TCallInfoV1& CallInfoV1 = reinterpret_cast<CTelephony::TCallInfoV1&> 
                                                        ( const_cast<TUint8&> ( *aCallInfo.Ptr() ) );

    CTelephony::TRemotePartyInfoV1& RemotePartyInfoV1 = reinterpret_cast<CTelephony::TRemotePartyInfoV1&> 
                                                        ( const_cast<TUint8&> ( *aRemoteInfo.Ptr() ) );	

	TPtr8* callInfoPkgPtr;
	RMobileCall::TMobileCallInfoV1* callInfoObjPtr;

	if(iTsyVersion == KNoMultimodeTsy)
		{
		return KErrNotSupported;
		}
	else if(iTsyVersion==KETelExtMultimodeV1 || iTsyVersion==KETelExtMultimodeV2)
		{
		RMobileCall::TMobileCallInfoV1*  callInfoV1 = new(ELeave) RMobileCall::TMobileCallInfoV1; 
		CleanupStack::PushL(callInfoV1);
		RMobileCall::TMobileCallInfoV1Pckg* callInfoV1Pckg = new(ELeave) RMobileCall::TMobileCallInfoV1Pckg(*callInfoV1); 
		CleanupStack::PushL(callInfoV1Pckg);
		callInfoPkgPtr = callInfoV1Pckg;
		callInfoObjPtr = callInfoV1;
		}
	else	//TSY v3
		{
		RMobileCall::TMobileCallInfoV3*  callInfoV3 = new(ELeave) RMobileCall::TMobileCallInfoV3; 
		CleanupStack::PushL(callInfoV3);
		RMobileCall::TMobileCallInfoV3Pckg* callInfoV3Pckg = new(ELeave) RMobileCall::TMobileCallInfoV3Pckg(*callInfoV3); 
		CleanupStack::PushL(callInfoV3Pckg);
		callInfoPkgPtr = callInfoV3Pckg;
		callInfoObjPtr = callInfoV3;
		}
 
	TInt numOfCalls=0;
	User::LeaveIfError(iLineVoice.EnumerateCall(numOfCalls));
	if (numOfCalls==0)
		{
		User::Leave(KErrNotFound);
		}
	CArrayFixFlat<RMobileCall>* tempCallArray = new (ELeave) CArrayFixFlat<RMobileCall>(numOfCalls);
	CleanupStack::PushL(TCleanupItem(CloseArrayOfCallObjects, tempCallArray));
	TInt i;
	
	RLine::TCallInfo callinfo;
	RMobileCall::TMobileCallStatus callStatus;
	TBool callFound=EFalse;
	for(i=0; i<numOfCalls; i++)
   		{
   		User::LeaveIfError(iLineVoice.GetCallInfo(i,callinfo));
   		RMobileCall tempCall; 
 		
 		tempCallArray->AppendL(tempCall);
 		User::LeaveIfError(tempCallArray->At(i).OpenExistingCall(iLineVoice,callinfo.iCallName));
		User::LeaveIfError(tempCallArray->At(i).GetMobileCallStatus(callStatus));
		 				
		if((callStatus==RMobileCall::EStatusConnected) &&
			(CallSelectionV1.iSelect==CTelephony::EActiveCall))
			{
			User::LeaveIfError(tempCallArray->At(i).GetMobileCallInfo(*callInfoPkgPtr));
			callFound=ETrue;
			break;
			}
		else if((callStatus==RMobileCall::EStatusHold) &&
			(CallSelectionV1.iSelect==CTelephony::EHeldCall))
			{
			User::LeaveIfError(tempCallArray->At(i).GetMobileCallInfo(*callInfoPkgPtr));
			callFound=ETrue;
			break;
			}
		else if(((callStatus==RMobileCall::EStatusDialling) ||
			  	(callStatus==RMobileCall::EStatusRinging) ||
			 	(callStatus==RMobileCall::EStatusAnswering) ||
				(callStatus==RMobileCall::EStatusConnecting) ||
				(callStatus==RMobileCall::EStatusReconnectPending) ||
				(callStatus==RMobileCall::EStatusDisconnecting) ||
				(callStatus==RMobileCall::EStatusDisconnectingWithInband) ||
				(callStatus==RMobileCall::EStatusTransferring) ||
				(callStatus==RMobileCall:: EStatusTransferAlerting) 
				) &&
			(CallSelectionV1.iSelect==CTelephony::EInProgressCall))
			{
			User::LeaveIfError(tempCallArray->At(i).GetMobileCallInfo(*callInfoPkgPtr));
			callFound=ETrue;
			break;
			}
		
		}	
	
	CleanupStack::PopAndDestroy(tempCallArray);	//tempCallArray
	if(!callFound)
		{
		CleanupStack::PopAndDestroy(2); //(callInfoV1Pckg or callInfoV3Pckg) AND (callInfoV1 or callInfoV3)		
		return KErrNotFound;
		}
	
	GetCallStatus(callInfoObjPtr->iStatus, CallInfoV1.iStatus);	

	if (callInfoObjPtr->iValid & RMobileCall::KCallSecurity)
		{
		if(callInfoObjPtr->ExtensionId() == KETelExtMultimodeV3)
			{
			RMobileCall::TMobileCallInfoV3* callInfoV3Ptr = reinterpret_cast<RMobileCall::TMobileCallInfoV3*>(callInfoObjPtr);
			
			switch(callInfoV3Ptr->iSecurity)
				{
			case RMobilePhone::ECipheringGSM:
				CallInfoV1.iSecurity = CTelephony::ECipheringGSM;
				break;
			case RMobilePhone::ECipheringWCDMA:
				CallInfoV1.iSecurity = CTelephony::ECipheringWCDMA;
				break;
			case RMobilePhone::ECipheringOff:
			default:
				CallInfoV1.iSecurity = CTelephony::ECipheringOff;
				break;
				}		
			}
		else
			CallInfoV1.iSecurity = CTelephony::ECipheringOff;
		}
	else
		{
		CallInfoV1.iSecurity = CTelephony::TPhoneNetworkSecurity();
		}

	
	if (callInfoObjPtr->iValid & RMobileCall::KCallStartTime)
		{
		CallInfoV1.iStartTime = callInfoObjPtr->iStartTime;
		}
	else
		{
		CallInfoV1.iStartTime = TDateTime();
		}
	if (callInfoObjPtr->iValid & RMobileCall::KCallDuration)
		{
		CallInfoV1.iDuration = callInfoObjPtr->iDuration;
		}
	else
		{
		CallInfoV1.iDuration = 0;
		}
	if (callInfoObjPtr->iValid & RMobileCall::KCallExitCode)
		{
		CallInfoV1.iExitCode = callInfoObjPtr->iExitCode;
		}
	else
		{
		CallInfoV1.iExitCode = 0;
		}
	
	if (callInfoObjPtr->iValid & RMobileCall::KCallDialledParty)
		{
		CopyTelAddress(callInfoObjPtr->iDialledParty, CallInfoV1.iDialledParty);
		}
	else
		{
		CallInfoV1.iDialledParty = CTelephony::TTelAddress();
		}

	//see whether we have an existing handle on the call
	CallInfoV1.iCallId = -1;
	RMobileCall* poolCall=NULL;
	RMobileCall::TMobileCallInfoV1  poolCallInfoV1;
	RMobileCall::TMobileCallInfoV1Pckg poolCallInfoV1Pckg(poolCallInfoV1);
	TInt err;
	
	for(int k=0;k<CTelephony::EISVMaxNumOfCalls;k++)
		{
		//get RMobileCall object
		if((poolCall = Call((CTelephony::TCallId)k)) != NULL)
			{
			//get call name
			err = poolCall->GetMobileCallInfo(poolCallInfoV1Pckg);
			if(err ==KErrNone)
				{
				//compare call names
				if(poolCallInfoV1.iCallName.MatchF(callinfo.iCallName)==0)
					{
					CallInfoV1.iCallId = k;
					break;			
					}
				}			
			}		
		}
		
	//fill TRemotePartyInfoV1
	RMobileCall::TMobileCallRemotePartyInfoV1& mobileRemoteParty = callInfoObjPtr->iRemoteParty;
	
	switch(mobileRemoteParty.iRemoteIdStatus)
		{
	case RMobileCall::ERemoteIdentityAvailable:
		RemotePartyInfoV1.iRemoteIdStatus = CTelephony::ERemoteIdentityAvailable;
		break;
	case RMobileCall::ERemoteIdentitySuppressed:
		RemotePartyInfoV1.iRemoteIdStatus = CTelephony::ERemoteIdentitySuppressed;
		break;
	case RMobileCall::ERemoteIdentityUnknown:
	default:
		RemotePartyInfoV1.iRemoteIdStatus = CTelephony::ERemoteIdentityUnknown;		
		break;
		}
		
	RemotePartyInfoV1.iCallingName.Copy(mobileRemoteParty.iCallingName);
	CopyTelAddress(mobileRemoteParty.iRemoteNumber, RemotePartyInfoV1.iRemoteNumber);
		
	switch(mobileRemoteParty.iDirection)
		{
	case RMobileCall::EMobileOriginated:
		RemotePartyInfoV1.iDirection = CTelephony::EMobileOriginated;
		break;
	case RMobileCall::EMobileTerminated:
		RemotePartyInfoV1.iDirection = CTelephony::EMobileTerminated;
		break;
	case RMobileCall::EDirectionUnknown:
	default:
		RemotePartyInfoV1.iDirection = CTelephony::EDirectionUnknown;
		break;
		}	
	
	CleanupStack::PopAndDestroy(2); //(callInfoV1Pckg or callInfoV3Pckg) AND (callInfoV1 or callInfoV3)
	
	return KErrNone;
	}	
	
void CTelephonyFunctions::CopyTelAddress(const RMobilePhone::TMobileAddress& aFromAddress, CTelephony::TTelAddress& aToAddress)
/**
Maps a RMobilePhone::TMobileAddress object to a CTelephony::TTelAddress object.
*/
	{
	aToAddress.iTelNumber.Copy(aFromAddress.iTelNumber);
	
	switch(aFromAddress.iTypeOfNumber)
		{
	case RMobilePhone::EInternationalNumber:
		aToAddress.iTypeOfNumber=CTelephony::EInternationalNumber;
		break;
	case RMobilePhone::ENationalNumber:
		aToAddress.iTypeOfNumber=CTelephony::ENationalNumber;
		break;
	case RMobilePhone::ENetworkSpecificNumber:
		aToAddress.iTypeOfNumber=CTelephony::ENetworkSpecificNumber;
		break;
	case RMobilePhone::ESubscriberNumber:
		aToAddress.iTypeOfNumber=CTelephony::ESubscriberNumber;
		break;
	case RMobilePhone::EAlphanumericNumber:
		aToAddress.iTypeOfNumber=CTelephony::EAlphanumericNumber;
		break;
	case RMobilePhone::EAbbreviatedNumber:
		aToAddress.iTypeOfNumber=CTelephony::EAbbreviatedNumber;
		break;
	case RMobilePhone::EUnknownNumber:
	default:
		aToAddress.iTypeOfNumber=CTelephony::EUnknownNumber;
		break;
		}

	switch(aFromAddress.iNumberPlan)
		{
	case RMobilePhone::EIsdnNumberPlan:
		aToAddress.iNumberPlan=CTelephony::EIsdnNumberPlan;
		break;
	case RMobilePhone::EDataNumberPlan:
		aToAddress.iNumberPlan=CTelephony::EDataNumberPlan;
		break;
	case RMobilePhone::ETelexNumberPlan:
		aToAddress.iNumberPlan=CTelephony::ETelexNumberPlan;
		break;
	case RMobilePhone::EServiceCentreSpecificPlan1:
		aToAddress.iNumberPlan=CTelephony::EServiceCentreSpecificPlan1;
		break;
	case RMobilePhone::EServiceCentreSpecificPlan2:
		aToAddress.iNumberPlan=CTelephony::EServiceCentreSpecificPlan2;
		break;
	case RMobilePhone::ENationalNumberPlan:
		aToAddress.iNumberPlan=CTelephony::ENationalNumberPlan;
		break;
	case RMobilePhone::EPrivateNumberPlan:
		aToAddress.iNumberPlan=CTelephony::EPrivateNumberPlan;
		break;
	case RMobilePhone::EERMESNumberPlan:
		aToAddress.iNumberPlan=CTelephony::EERMESNumberPlan;
		break;
	case RMobilePhone::EUnknownNumberingPlan:
	default:
		aToAddress.iNumberPlan=CTelephony::EUnknownNumberingPlan;
		break;
		}
	}

