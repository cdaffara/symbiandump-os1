/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include <bluetoothav.h>

//	User Includes
#include "T_AvdtpServiceCapabilityData.h"
#include "T_AvdtpRecoveryCapabilitiesData.h"

#include "T_BTUtil.h"

/*@{*/
//Parameters
_LIT(KRecoveryType,										"recoverytype");
_LIT(KMaxWindowSize,										"maxwindowsize");
_LIT(KMinWindowSize,										"minwindowsize");
_LIT(KExpected,												"expected");

//Commands
_LIT(KCmdnew,												"new");
_LIT(KCmdSetRecoveryType,									"SetRecoveryType");
_LIT(KCmdRecoveryType,										"RecoveryType");
_LIT(KCmdSetMaxWindowSize,									"SetMaxWindowSize");
_LIT(KCmdMaxWindowSize,										"MaxWindowSize");
_LIT(KCmdSetMinWindowSize,									"SetMinWindowSize");
_LIT(KCmdMinWindowSize,										"MinWindowSize");
_LIT(KCmdDestructor,										"~"); 

//Log
_LIT(KLogMissingExpected,								"Missing expected '%S'");
_LIT(KLogMissingParameter,								"Missing parameter '%S'");
_LIT(KLogError,											"Error=%d");

/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_AvdtpRecoveryCapabilitiesData* CT_AvdtpRecoveryCapabilitiesData::NewL()
	{
	CT_AvdtpRecoveryCapabilitiesData*	ret=new (ELeave) CT_AvdtpRecoveryCapabilitiesData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_AvdtpRecoveryCapabilitiesData::CT_AvdtpRecoveryCapabilitiesData()
:iAvdtpRecoveryCapabilities(NULL)
	{
	}

void CT_AvdtpRecoveryCapabilitiesData::ConstructL()
	{
	}

TAvdtpServiceCapability* CT_AvdtpRecoveryCapabilitiesData::GetAvdtpServiceCapability() const
	{
	return iAvdtpRecoveryCapabilities;
	}

CT_AvdtpRecoveryCapabilitiesData::~CT_AvdtpRecoveryCapabilitiesData()
	{
	DestroyData();
	}

void CT_AvdtpRecoveryCapabilitiesData::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iAvdtpRecoveryCapabilities = static_cast<TAvdtpRecoveryCapabilities*> (aAny);
	}

void CT_AvdtpRecoveryCapabilitiesData::DisownObjectL()
	{
	iAvdtpRecoveryCapabilities = NULL;
	}

void CT_AvdtpRecoveryCapabilitiesData::DestroyData()
	{
	delete iAvdtpRecoveryCapabilities;
	iAvdtpRecoveryCapabilities=NULL;	
	}

TBool CT_AvdtpRecoveryCapabilitiesData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret = ETrue;
	
	if ( aCommand==KCmdnew )
		{
		DoCmdnewL();
		}
	else if ( aCommand==KCmdSetRecoveryType )
		{
		DoCmdSetRecoveryType(aSection);
		}
	else if ( aCommand==KCmdRecoveryType )
		{
		DoCmdRecoveryType(aSection);
		}
	else if ( aCommand==KCmdSetMaxWindowSize )
		{
		DoCmdSetMaxWindowSize(aSection);
		}
	else if ( aCommand==KCmdMaxWindowSize )
		{
		DoCmdMaxWindowSize(aSection);
		}
	else if ( aCommand==KCmdSetMinWindowSize )
		{
		DoCmdSetMinWindowSize(aSection);
		}
	else if ( aCommand==KCmdMinWindowSize )
		{
		DoCmdMinWindowSize(aSection);
		}
	else if ( aCommand==KCmdDestructor )
		{
		DestroyData();
		}
	else
		{
		ret=CT_AvdtpServiceCapabilityData::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	return ret;
	}


/**
Test TAvdtpRecoveryCapabilities()
*/
void CT_AvdtpRecoveryCapabilitiesData::DoCmdnewL()
	{
	INFO_PRINTF1(_L("TAvdtpRecoveryCapabilities Constructor Call"));
	DestroyData();
	TRAPD( err, iAvdtpRecoveryCapabilities = new (ELeave) TAvdtpRecoveryCapabilities());
	
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(KLogError, err);
		SetError(err);
		}
	}


/**
Test SetRecoveryType()
*/
void CT_AvdtpRecoveryCapabilitiesData::DoCmdSetRecoveryType(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpRecoveryCapabilities SetRecoveryType() Call"));
	
	SymbianBluetoothAV::TAvdtpRecoveryType type;
	if ( CT_BTUtil::ReadAvdtpRecoveryType(*this, aSection, KRecoveryType(), type) )
		{
		iAvdtpRecoveryCapabilities->SetRecoveryType(type);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KRecoveryType);
		SetBlockResult(EFail);		
		}
	}


/**
Test RecoveryType()
*/
void CT_AvdtpRecoveryCapabilitiesData::DoCmdRecoveryType(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpRecoveryCapabilities RecoveryType() Call"));
	
	SymbianBluetoothAV::TAvdtpRecoveryType type = iAvdtpRecoveryCapabilities->RecoveryType();
	INFO_PRINTF2(_L("TAvdtpServiceCapability RecoveryType result: actual = %d"), type);
	
	SymbianBluetoothAV::TAvdtpRecoveryType	expected;
	if ( CT_BTUtil::ReadAvdtpRecoveryType(*this, aSection, KExpected(), expected) )
		{
		if ( type!=expected )
			{
			ERR_PRINTF3(_L("Result (%d) is not as expected (%d)"), type, expected);
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingExpected, &KExpected);
		SetBlockResult(EFail);		
		}
	}


/**
Test SetMaxWindowSize()
*/
void CT_AvdtpRecoveryCapabilitiesData::DoCmdSetMaxWindowSize(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpRecoveryCapabilities SetMaxWindowSize() Call"));
	
	TInt size = 0;
	if( GetIntFromConfig(aSection, KMaxWindowSize, size) )
		{
		iAvdtpRecoveryCapabilities->SetMaxWindowSize(size);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KMaxWindowSize);
		SetBlockResult(EFail);		
		}
	}


/**
Test MaxWindowSize()
*/
void CT_AvdtpRecoveryCapabilitiesData::DoCmdMaxWindowSize(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpRecoveryCapabilities MaxWindowSize() Call"));
	
	TAvdtpRecoveryWindowSize size = iAvdtpRecoveryCapabilities->MaxWindowSize();
	INFO_PRINTF2(_L("TAvdtpServiceCapability MaxWindowSize result: actual = %d"), size);
	
	TInt expected = 0;
	if( GetIntFromConfig(aSection, KExpected, expected) )
		{
		if ( size!=expected )
			{
			ERR_PRINTF3(_L("Result (%d) is not as expected (%d)"), size, expected);
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingExpected, &KExpected);
		SetBlockResult(EFail);		
		}
	}


/**
Test SetMinWindowSize()
*/
void CT_AvdtpRecoveryCapabilitiesData::DoCmdSetMinWindowSize(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpRecoveryCapabilities SetMinWindowSize() Call"));
	
	TInt size = 0;
	if( GetIntFromConfig(aSection, KMinWindowSize, size) )
		{
		iAvdtpRecoveryCapabilities->SetMinWindowSize(size);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KMinWindowSize);
		SetBlockResult(EFail);		
		}
	}


/**
Test MinWindowSize()
*/
void CT_AvdtpRecoveryCapabilitiesData::DoCmdMinWindowSize(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpRecoveryCapabilities MinWindowSize() Call"));
	
	TAvdtpRecoveryWindowSize size = iAvdtpRecoveryCapabilities->MinWindowSize();
	INFO_PRINTF2(_L("TAvdtpServiceCapability MinWindowSize result: actual = %d"), size);
	
	TInt expected = 0;
	if( GetIntFromConfig(aSection, KExpected, expected) )
		{
		if ( size!=expected )
			{
			ERR_PRINTF3(_L("Result (%d) is not as expected (%d)"), size, expected);
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingExpected, &KExpected);
		SetBlockResult(EFail);		
		}
	}

