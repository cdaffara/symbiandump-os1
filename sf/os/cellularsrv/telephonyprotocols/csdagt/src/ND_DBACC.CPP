// Copyright (c) 2000-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// CCommsDbNetDialAccess definitions
// 
//

/**
 @file nd_dbacc.cpp 
*/

#include <nifvar.h>
	#include <commsdattypesv1_1.h>
	#include <commsdatutils.h>
	using namespace CommsDat;
#include "ND_DBACC.H"
#include "ND_STD.H"

/**
@internalComponent
*/
const TUint32 KZeroFieldValue=0uL;

CCommsDbNetDialAccess* CCommsDbNetDialAccess::NewL(CCommsDbAccess* aDbAccess)
/**
2 phased constructor, first phase.

@param aDbAccess a pointer to the database accessor.
@exception Leaves if CCommsDbNetDialAccess() constructor leaves, or not enough memory is available.
@return a new CCommsDbNetDialAccess object.
*/
	{
	return new(ELeave) CCommsDbNetDialAccess(aDbAccess);
	}

CCommsDbNetDialAccess::CCommsDbNetDialAccess(CCommsDbAccess* aDbAccess)
	: iDbAccess(aDbAccess)
/**
Private constructor used in the first phase of construction.

@param aDbAccess a pointer to the database accessor.
*/
	{}

CCommsDbNetDialAccess::~CCommsDbNetDialAccess()
/**
Destructor.
*/
	{}

void CCommsDbNetDialAccess::Close()
/**
Closes database accessor.
*/
	{
	iDbAccess->Close();
	}

void CCommsDbNetDialAccess::GetTsyNameL(TDes& aTsyName)
/**
Gets TSY name from database.

@param aTsyName a reference to the TSY name that is read from database.
@exception Leaves if GetTsyNameL() leaves.
*/
	{
	iDbAccess->GetTsyNameL(aTsyName);
	}	

void CCommsDbNetDialAccess::SetCommPortL(const RCall::TCommPort& aCommPort)
/**
Sets COMM-port into database.

@param aCommPort a reference to the COMM-port written into database.
@exception Leaves if SetCommPortL() leaves.
*/
	{
	iDbAccess->SetCommPortL(aCommPort);
	}

void CCommsDbNetDialAccess::GetCommPortForDirectConnectL(RCall::TCommPort& aCommPort)
/**
Get comm port details for direct connection.

@param aCommPort a reference to the COMM-port read from database.
@exception Leaves if ReadTextL() leaves.
*/
	{
		CCommsDbAccess::CCommsDbAccessModemTable* modemTable = iDbAccess->ModemTable();
		CleanupStack::PushL(modemTable);
		if (modemTable == 0)
			{
			User::Leave(KErrNoMemory); // most likely?
			}
		
		aCommPort.iCsy = static_cast<CCDModemBearerRecord*>(modemTable->iRecord)->iCsyName;
		aCommPort.iPort = static_cast<CCDModemBearerRecord*>(modemTable->iRecord)->iPortName;

		CleanupStack::PopAndDestroy(modemTable);
	}

void CCommsDbNetDialAccess::GetBcaStackL(TDes& aBcaStack)
/**
Gets the Bca Stack

@param aBcaStack the BcaStack
@exception Leaves if ReadTextL() leaves.
*/
	{
		CCommsDbAccess::CCommsDbAccessModemTable* modemTable = iDbAccess->ModemTable();
		CleanupStack::PushL(modemTable);
		if (modemTable == 0)
			{
			User::Leave(KErrNoMemory); // most likely?
			}
		
		aBcaStack = static_cast<CCDModemBearerRecord*>(modemTable->iRecord)->iBcaStack;

		CleanupStack::PopAndDestroy(modemTable);
	}

TInt CCommsDbNetDialAccess::GetIapId()
	{
	_LIT(KIAPId, "IAP\\Id");

	TUint32 iap;
	TInt err = iDbAccess->ReadInt(KIAPId(), iap);
	return (err == KErrNone) ? iap : err;
	}

void CCommsDbNetDialAccess::GetCommConfigForDirectConnectL(TCommConfig& aCommConfig)
/**
Get settings from modem config settings for data bits, stop bits and parity
and get appropriate handshaking for direct connection.

@param aCommConfig a reference to the configuration read for connection.
@exception Leaves if PopulateModemCommSettingsL() leaves.
*/
	{
	TCommConfigV01& cfg=aCommConfig();
	PopulateModemCommSettingsL(cfg);
	// DSRFail not asserted for battery powered modems that switch on with DTR High
	cfg.iHandshake&=(~(KDCDHandshakeMask | KDSRFailHandshakeMask | KCTSHandshakeMask));
	// Should be 0 for correct script operation
	cfg.iTerminatorCount=0;
	}

void CCommsDbNetDialAccess::GetCommPortRoleL(TCommRole& aRole)
/**
Get comm port role (DCE or DTE)

@param aRole a reference to the role read from the database
@exception Leaves if ReadUintL() leaves.
*/
	{
	TUint32 dummy = KZeroFieldValue;
		CCommsDbAccess::CCommsDbAccessModemTable* modemTable = iDbAccess->ModemTable();
		CleanupStack::PushL(modemTable);
		if (modemTable == 0)
			{
			User::Leave(KErrNoMemory); // most likely?
			}

		dummy = static_cast<CCDModemBearerRecord*>(modemTable->iRecord)->iCommRole;	
		if (dummy & KModemCommRoleDCE)
			{
			aRole = ECommRoleDCE;	
			}			
		else
			{
			aRole = ECommRoleDTE;	
			}				

		CleanupStack::PopAndDestroy(modemTable);
	
	}

void CCommsDbNetDialAccess::GetCallParamsL(RCall::TCallParams& aParams)
/**
Parse info about speaker settings and pause time from modem prefs and location table
into TCallParams struct. Assume we require a location as we are dialling a number.

@param aParams a reference to the Core API call parameters read from database.
@exception Leaves if ReadUintL(), GetIntL() or GetBoolL() leaves.
*/
	{
	TUint32 value=KZeroFieldValue;
		CCommsDbAccess::CCommsDbAccessModemTable* modemTable = iDbAccess->ModemTable();
		CleanupStack::PushL(modemTable);
		if (modemTable == 0)
			{
			User::Leave(KErrNoMemory); // most likely?
			}

		value = static_cast<CCDModemBearerRecord*>(modemTable->iRecord)->iSpeakerPref;
		
	switch (value)
		{
		case EModemSpeakerSettingNever:
			aParams.iSpeakerControl=RCall::EMonitorSpeakerControlAlwaysOff;
			break;
		case EModemSpeakerSettingUntilCallAnswered:
			aParams.iSpeakerControl=RCall::EMonitorSpeakerControlOnUntilCarrier;
			break;
		case EModemSpeakerSettingAlways:
			aParams.iSpeakerControl=RCall::EMonitorSpeakerControlAlwaysOn;
			break;
		case EModemSpeakerSettingAfterDialUntilAnswer:
			aParams.iSpeakerControl=RCall::EMonitorSpeakerControlOnExceptDuringDialling;
			break;
		default:
			aParams.iSpeakerControl=RCall::EMonitorSpeakerControlUnknown;
			break;
		}
	
		value = static_cast<CCDModemBearerRecord*>(modemTable->iRecord)->iSpeakerVolPref;

		CleanupStack::PopAndDestroy(modemTable);
	switch (value)
		{
		case EModemSpeakerVolumeQuiet:
			aParams.iSpeakerVolume=RCall::EMonitorSpeakerVolumeLow;
			break;
		case EModemSpeakerVolumeMedium:
			aParams.iSpeakerVolume=RCall::EMonitorSpeakerVolumeMedium;
			break;
		case EModemSpeakerVolumeLoud:
			aParams.iSpeakerVolume=RCall::EMonitorSpeakerVolumeHigh;
			break;
		default:
			aParams.iSpeakerVolume=RCall::EMonitorSpeakerVolumeUnknown;
			break;
		}
	
	TPtrC loc(LOCATION);
	iDbAccess->GetIntL(loc,TPtrC(LOCATION_PAUSE_AFTER_DIAL_OUT),value);
	aParams.iInterval=(TUint)value;

	TBool bValue;
	iDbAccess->GetBoolL(loc,TPtrC(LOCATION_WAIT_FOR_DIAL_TONE),bValue);
	if (bValue)
		{
		aParams.iWaitForDialTone=RCall::EDialToneWait;
		}
	else
		{
		aParams.iWaitForDialTone=RCall::EDialToneNoWait;
		}
	}

TInt CCommsDbNetDialAccess::GetRedialAttempts(TUint32& aAttempts)
/**
Reads amount of redial attempts from database.

@param aAttempts a reference to redial attempts read from database.
@return an error code trapped if GetGlobalL() leaves.
*/
	{
	TRAPD(ret,iDbAccess->GetGlobalL(TPtrC(REDIAL_ATTEMPTS),aAttempts));
	return ret;
	}

void CCommsDbNetDialAccess::GetRemoteParty(TDes& aRemoteParty)
/**
Read ISP name from database and return as remote party.

@param aRemoteParty a reference to ISP name read from database.
@exception Leaves if GetDesL() leaves.
*/
	{
	TInt ret(KErrNone);
	TPtrC name(COMMDB_NAME);

	ret = AssertDialOutTable();
	if (ret != KErrNone)
		{
		//if AssertDialOutTable encounters serios errors (OOM)
		//it must set aRemoteParty appropriatelly
		_LIT(unknownParty,"???");
		aRemoteParty=unknownParty;
		return;
		}

	TRAP(ret,iDbAccess->GetDesL(TPtrC(IAP),name,aRemoteParty));

	if(ret!=KErrNone)
		{
		_LIT(unknownParty,"???");
		aRemoteParty=unknownParty;
		}
	}

void CCommsDbNetDialAccess::IsDefaultTelNumL(TBool& aIsTelNum)
/**
Check length of default tel no - if 0 set boolean to false, else true.
Only for dial out

@param aIsTelNum a reference to the default tel num read from database.
@exception Leaves if GetDesL() leaves.

*/
	{
	User::LeaveIfError(AssertDialOutTable());

	TDialString telNum;
	iDbAccess->GetDesL(TPtrC(DIAL_OUT_ISP),TPtrC(ISP_DEFAULT_TEL_NUM),telNum);
	if (telNum.Length()==0) 
		aIsTelNum=EFalse;
	else
		aIsTelNum=ETrue;
	}

void CCommsDbNetDialAccess::GetUsePctL(TBool& aUsePct)
/**
Read the DisplayPct boolean from the ISP database
Only for dial out

@param aUsePct a reference to the DisplayPct boolean read from database.
@exception Leaves if GetBoolL() leaves.
*/
	{
	User::LeaveIfError(AssertDialOutTable());
	iDbAccess->GetBoolL(TPtrC(DIAL_OUT_ISP),TPtrC(ISP_DISPLAY_PCT),aUsePct);
	}

void CCommsDbNetDialAccess::GetScriptDetailsL(TBool& aUseScript, TInt& aLength)
/**
Get the length of the script into aLength.
Dial in or dial out.

@param aUseScript a reference to the script boolean read from database.
@param aLength a reference to the lenght of script read from database.
@exception Leaves if GetBoolL() or GetLengthOfLongDesL() leaves.
*/
	{
	User::LeaveIfError(AssertDialOutTable());
	iDbAccess->GetBoolL(TPtrC(DIAL_OUT_ISP),TPtrC(ISP_USE_LOGIN_SCRIPT),aUseScript);
	aLength=iDbAccess->GetLengthOfLongDesL(TPtrC(DIAL_OUT_ISP),TPtrC(ISP_LOGIN_SCRIPT));
	}	

HBufC* CCommsDbNetDialAccess::GetScriptLC()
/**
Return the script.
Dial in or dial out.

@exception Leaves if GetBoolL() or GetLengthOfLongDesL() leaves.
@return as HBufC* the login script read from database.
*/
	{
	User::LeaveIfError(AssertDialOutTable());
	return iDbAccess->GetLongDesLC(TPtrC(DIAL_OUT_ISP),TPtrC(ISP_LOGIN_SCRIPT));
	}

void CCommsDbNetDialAccess::GetLoginParamsL(TBool& aPromptForLogin,TDes& aLoginName,TDes& aLoginPass,TDes& aIPAddress)
/**
Get boolean PromptForLogin and login name, login password and IP address
Dial out only.

@param aPromptForLogin a reference to the prompt boolean read from database.
@param aLoginName a reference to the login name read from database.
@param aLoginPass a reference to the login password read from database.
@param aIPAddress a reference to the ISP IP address read from database.
@exception Leaves if GetBoolL() or GetDesL() leaves.
*/
	{
	User::LeaveIfError(AssertDialOutTable());

	iDbAccess->GetBoolL(TPtrC(DIAL_OUT_ISP),TPtrC(ISP_PROMPT_FOR_LOGIN),aPromptForLogin);
	iDbAccess->GetDesL(TPtrC(DIAL_OUT_ISP),TPtrC(ISP_LOGIN_NAME),aLoginName);
	iDbAccess->GetDesL(TPtrC(DIAL_OUT_ISP),TPtrC(ISP_LOGIN_PASS),aLoginPass);
	iDbAccess->GetDesL(TPtrC(DIAL_OUT_ISP),TPtrC(ISP_IP_ADDR),aIPAddress);
	}

TInt CCommsDbNetDialAccess::GetCallbackTimeout(TUint32& aTimeout)
/**
Get call back timeout.

@param aTimeout a reference to the dial out time read from database.
@return an error code trapped if GetIntL() leaves.
*/
	{
	TInt ret = AssertDialOutTable();
	if (ret != KErrNone)
		return ret;

	TRAP(ret,iDbAccess->GetIntL(TPtrC(DIAL_OUT_ISP),TPtrC(ISP_CALLBACK_TIMEOUT),aTimeout));
	return ret;
	}

TBool CCommsDbNetDialAccess::HscsdSettingsAvailableL()
/**
Check if HSCSD settings are available in database.

@return ETrue if bearer type in database is EBearerTypeHSCSD. Else return EFalse.
*/
	{
	User::LeaveIfError(AssertDialOutTable());

	TUint32 temp=0uL;
	TRAPD(ret,iDbAccess->GetIntL(TPtrC(DIAL_OUT_ISP),TPtrC(ISP_BEARER_TYPE),temp));
	if (ret==KErrUnknown)
		return EFalse;
	User::LeaveIfError(ret);	// any other error
	if ((TCommsDbBearerType)temp==EBearerTypeHSCSD)
		return ETrue;
	return EFalse;
	}

void CCommsDbNetDialAccess::GetMmHscsdParametersL(RMobileCall::TMobileHscsdCallParamsV1& aMmHscsdParams)
/**
Get Multimode HSCSD call parameters..

@param aMmHscsdParams a reference to the multimode HSCSD call parameters read from database.
@exception Leaves if GetIntL or GetBoolL() leaves.
*/
	{
	User::LeaveIfError(AssertDialOutTable());

	TUint32 temp=0uL;
	TPtrC serviceTableName(DIAL_OUT_ISP);
	iDbAccess->GetIntL(serviceTableName,TPtrC(ISP_AIUR),temp);
	aMmHscsdParams.iWantedAiur=(RMobileCall::TMobileCallAiur)temp;
	iDbAccess->GetIntL(serviceTableName,TPtrC(ISP_REQUESTED_TIME_SLOTS),temp);
	aMmHscsdParams.iWantedRxTimeSlots=(TInt)temp;
	iDbAccess->GetIntL(serviceTableName,TPtrC(ISP_MAXIMUM_TIME_SLOTS),temp);
	aMmHscsdParams.iMaxTimeSlots=(TInt)temp;
	iDbAccess->GetIntL(serviceTableName,TPtrC(ISP_CHANNEL_CODING),temp);
	aMmHscsdParams.iCodings=(TUint32)temp;
	iDbAccess->GetIntL(serviceTableName,TPtrC(ISP_ASYMMETRY),temp);
	aMmHscsdParams.iAsymmetry=(RMobileCall::TMobileCallAsymmetry)temp;
	TBool b;
	iDbAccess->GetBoolL(serviceTableName,TPtrC(ISP_USER_INIT_UPGRADE),b);
	aMmHscsdParams.iUserInitUpgrade=b;
	}

TBool CCommsDbNetDialAccess::GetMmCallParams(RMobileCall::TMobileDataCallParamsV1& aMmParams)
/**
Get Multimode data call parameters. If the parameters can be read from database,
return ETrue, else return EFalse.

@param aMmParams a reference to the multimode data call parameters read from database.
@return ETrue if multimode data call parameters are read successfully. Else return EFalse.
*/
	{
	TInt ret = AssertDialOutTable();
	if (ret!=KErrNone)
		return EFalse;

	TUint32 temp=0uL;
	TRAP(ret,iDbAccess->GetIntL(KGeneralServiceTable,TPtrC(ISP_BEARER_SERVICE),temp));
	if (ret!=KErrNone)
		return EFalse;
	aMmParams.iService=(RMobileCall::TMobileCallDataService)temp;
	TRAP(ret,iDbAccess->GetIntL(KGeneralServiceTable,TPtrC(ISP_BEARER_SPEED),temp));
	if (ret!=KErrNone)
		return EFalse;
	aMmParams.iSpeed=(RMobileCall::TMobileCallDataSpeed)temp;
	TRAP(ret,iDbAccess->GetIntL(KGeneralServiceTable,TPtrC(ISP_BEARER_PROTOCOL),temp));
	if (ret!=KErrNone)
		return EFalse;
	aMmParams.iProtocol=(RMobileCall::TMobileCallDataProtocol)temp;
	TRAP(ret,iDbAccess->GetIntL(KGeneralServiceTable,TPtrC(ISP_BEARER_CE),temp));
	if (ret!=KErrNone)
		return EFalse;
	aMmParams.iQoS=(RMobileCall::TMobileCallDataQoS)temp;
	TRAP(ret,iDbAccess->GetIntL(KGeneralServiceTable,TPtrC(ISP_RLP_VERSION),temp));
	if (ret!=KErrNone)
		return EFalse;
	aMmParams.iRLPVersion=(RMobileCall::TMobileCallDataRLPVersion)temp;
	TRAP(ret,iDbAccess->GetIntL(KGeneralServiceTable,TPtrC(ISP_IWF_TO_MS),temp));
	if (ret!=KErrNone)
		return EFalse;
	aMmParams.iModemToMSWindowSize=(TInt)temp;
	TRAP(ret,iDbAccess->GetIntL(KGeneralServiceTable,TPtrC(ISP_MS_TO_IWF),temp));
	if (ret!=KErrNone)
		return EFalse;
	aMmParams.iMSToModemWindowSize=(TInt)temp;
	TRAP(ret,iDbAccess->GetIntL(KGeneralServiceTable,TPtrC(ISP_ACK_TIMER),temp));
	if (ret!=KErrNone)
		return EFalse;
	aMmParams.iAckTimer=(TInt)temp;
	TRAP(ret,iDbAccess->GetIntL(KGeneralServiceTable,TPtrC(ISP_RETRANSMISSION_ATTEMPTS),temp));
	if (ret!=KErrNone)
		return EFalse;
	aMmParams.iRetransmissionAttempts=(TInt)temp;
	TRAP(ret,iDbAccess->GetIntL(KGeneralServiceTable,TPtrC(ISP_RESEQUENCE_PERIOD),temp));
	if (ret!=KErrNone)
		return EFalse;
	aMmParams.iResequencingPeriod=(TInt)temp;
	TRAP(ret,iDbAccess->GetIntL(KGeneralServiceTable,TPtrC(ISP_V42_COMPRESSION),temp));
	if (ret!=KErrNone)
		return EFalse;
	aMmParams.iV42bisReq=(RMobileCall::TMobileCallDataV42bis)temp;
	TRAP(ret,iDbAccess->GetIntL(KGeneralServiceTable,TPtrC(ISP_V42_CODEWORDS),temp));
	if (ret!=KErrNone)
		return EFalse;
	aMmParams.iV42bisCodewordsNum=(TInt)temp;
	TRAP(ret,iDbAccess->GetIntL(KGeneralServiceTable,TPtrC(ISP_V42_MAX_LENGTH),temp));
	if (ret!=KErrNone)
		return EFalse;
	aMmParams.iV42bisMaxStringLength=(TInt)temp;
	TBool useEdge;
	TRAP(ret,iDbAccess->GetBoolL(KGeneralServiceTable,TPtrC(ISP_USE_EDGE),useEdge));
	if (ret!=KErrNone)
		return EFalse;
	aMmParams.iUseEdge=useEdge;
	return ETrue;
	}

void CCommsDbNetDialAccess::CopyIspInitStringToModemL()
/**
Copy over ISP init string from ISP to modem table.

@exception Leaves if GetDesL(), UpdateRecord() or PutRecordChanges() leaves.
*/
	{
	TBuf8<KCommsDbSvrMaxFieldLength> initStringIsp;
	iDbAccess->GetDesL (KGeneralServiceTable, TPtrC (ISP_INIT_STRING), initStringIsp);
	TBuf8<KCommsDbSvrMaxFieldLength> initStringModem;	

	CCommsDbAccess::CCommsDbAccessModemTable* modemTable = iDbAccess->ModemTable();
	CleanupStack::PushL(modemTable);
	if (modemTable == 0)
		{
		User::Leave(KErrNoMemory); // most likely?
		}

	initStringModem = static_cast<CCDModemBearerRecord*>(modemTable->iRecord)->iModemInitString;

	if (initStringIsp.CompareF (initStringModem) != 0)
		{
		modemTable->iRecord->RefreshL(*(modemTable->iSession));
		static_cast<CCDModemBearerRecord*>(modemTable->iRecord)->iModemInitString = initStringIsp;
		modemTable->iRecord->ModifyL(*(modemTable->iSession));
		} 

	CleanupStack::PopAndDestroy(modemTable);
	}

void CCommsDbNetDialAccess::DoDiallingResolutionL(TDialString& aTelNum, TParseMode aMode)
/**
Decide which phone number to use by checking alternative locations and do correct dialling 
resolution. Write correct dial string back to aTelNum.
Only for dial out.

@param aTelNum a reference to the telephone number resolved in database.
@param aMode a parse mode passed for parsing.
@exception Leaves if GetDesL(), GetBoolL(), SetUnresolvedDialStringL(), GetIntL() or ResolvePhoneNumberL() leaves.
*/
	{
	User::LeaveIfError(AssertDialOutTable());
	TDialString fullNum;
	iDbAccess->GetDesL(TPtrC(DIAL_OUT_ISP),TPtrC(ISP_DEFAULT_TEL_NUM),fullNum);
	TBool doDiallingResolution;
	iDbAccess->GetBoolL(TPtrC(DIAL_OUT_ISP),TPtrC(ISP_DIAL_RESOLUTION),doDiallingResolution);

	if(!doDiallingResolution || fullNum.Length()==0)
		{
		SetUnresolvedDialStringL(aTelNum,fullNum,aMode);
		}
	else
		{
		TUint32 chargecardId=KZeroFieldValue;
		TInt ret=KErrNone;

		TRAP(ret,iDbAccess->GetIntL(TPtrC(DIAL_OUT_ISP),TPtrC(ISP_CHARGECARD),chargecardId));

		if (ret==KErrUnknown)		// don't care if there isn't one
			{
			ret=KErrNone;
			}
		
		User::LeaveIfError(ret);
		CommsDatUtils::CCommsDatUtils::ResolvePhoneNumberL(fullNum,aTelNum,aMode,iDbAccess->LocationId(),chargecardId);
		}
	}

// Private functions 

TInt CCommsDbNetDialAccess::AssertDialOutTable() const
/**
Reads service type from database.

@exception Panics if GetServiceTypeL() leaves or service type read from database is not DIAL_OUT_ISP.
*/
	{
	TBuf<KCommsDbSvrMaxFieldLength> serviceType;
	TRAPD(ret, iDbAccess->GetServiceTypeL(serviceType));
	if (ret == KErrNone && serviceType!=TPtrC(DIAL_OUT_ISP))
		{
		NetDialPanic(EIllegalDbRequestForService);
		}

	return ret;
	}

void CCommsDbNetDialAccess::SetUnresolvedDialStringL(TDialString& aTelNum,const TDialString& aFullNum, TParseMode aMode)
/**
Resolves which letters 'T' or 'P' should prefix number when NOT doing dial resolution.
Returns number with prefix in aFullNum.

@param aTelNum a reference to the telephone number to be resolved.
@param aFullNum a reference to the full resolved telephone number, filled here.
@param aMode a parse mode passed for parsing.
@exception Leaves if GetBoolL() leaves.
*/
	{
	aTelNum.Zero();			// restart the dial string 
	TBool mobile;
	TPtrC loc(LOCATION);
	iDbAccess->GetBoolL(loc,TPtrC(LOCATION_MOBILE),mobile);
	if (!mobile && aMode==EForDialing)
		{
		TBool pulseDial;
		iDbAccess->GetBoolL(loc,TPtrC(LOCATION_USE_PULSE_DIAL),pulseDial);
		if (pulseDial)
			aTelNum.Append(KCharP);
		else
			aTelNum.Append(KCharT);
		}
	aTelNum.Append(aFullNum);
	}

void CCommsDbNetDialAccess::PopulateModemCommSettingsL(TCommConfigV01& aCfg)
/**
Read configuration for COMM-port from database and sets it into aCfg 

@param aCfg a reference to COMM-port confiquration, read values are passed back in the parameter.
@exception Leaves if ReadUintL() leaves.
*/
	{
	TUint32 dummy;
		CCommsDbAccess::CCommsDbAccessModemTable* modemTable = iDbAccess->ModemTable();
		CleanupStack::PushL(modemTable);
		if (modemTable == 0)
			{
			User::Leave(KErrNoMemory); // most likely?
			}
		CCDModemBearerRecord* table = static_cast<CCDModemBearerRecord*>(modemTable->iRecord);
		CleanupStack::PopAndDestroy(modemTable);

		dummy = table->iRate;
		aCfg.iRate=TBps(dummy);	
		dummy = table->iDataBits;
		aCfg.iDataBits=TDataBits(dummy);
		dummy = table->iStopBits;
		aCfg.iStopBits=TStopBits(dummy);
		dummy = table->iParity;
		aCfg.iParity=TParity(dummy);
		aCfg.iHandshake=TUint(table->iHandshaking);
		
		aCfg.iParityError=KConfigParityErrorIgnore;
		aCfg.iFifo=EFifoEnable;
		
		aCfg.iSpecialRate=TInt(table->iSpecialRate);
		
		aCfg.iTerminatorCount=2;
		aCfg.iTerminator[0]=0x0a;
		aCfg.iTerminator[1]=0x0d;
		
		aCfg.iXonChar=TText8(table->iXonChar);
		aCfg.iXoffChar=TText8(table->iXoffChar);
		
		aCfg.iParityErrorChar=0;
		aCfg.iSIREnable=ESIRDisable;
		aCfg.iSIRSettings=0;
		
		aCfg.iSIRSettings=TUint(table->iSirSettings);				
	}

