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
#include "T_AvdtpContentProtectionCapabilitiesData.h"


/*@{*/
//Parameters
_LIT(KExpected, 										"expected");
_LIT(KCPType,											"cptype");
_LIT(KData,												"data");

//Commands
_LIT(KCmdnew,											"new");
_LIT(KCmdContentProtectionType,							"ContentProtectionType");
_LIT(KCmdSetContentProtectionType,						"SetContentProtectionType");
_LIT(KCmdContentProtectionData,							"ContentProtectionData");
_LIT(KCmdSetContentProtectionData,						"SetContentProtectionData");
_LIT(KCmdDestructor,									"~"); 

//Log
_LIT(KLogMissingExpected,								"Missing expected '%S'");
_LIT(KLogMissingParameter,								"Missing parameter '%S'");
_LIT(KLogError,											"Error=%d");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_AvdtpContentProtectionCapabilitiesData* CT_AvdtpContentProtectionCapabilitiesData::NewL()
	{
	CT_AvdtpContentProtectionCapabilitiesData*	ret=new (ELeave) CT_AvdtpContentProtectionCapabilitiesData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_AvdtpContentProtectionCapabilitiesData::CT_AvdtpContentProtectionCapabilitiesData()
:iAvdtpContentProtectionCapabilities(NULL)
	{
	}

void CT_AvdtpContentProtectionCapabilitiesData::ConstructL()
	{
	}

TAvdtpServiceCapability* CT_AvdtpContentProtectionCapabilitiesData::GetAvdtpServiceCapability() const
	{
	return iAvdtpContentProtectionCapabilities;
	}

CT_AvdtpContentProtectionCapabilitiesData::~CT_AvdtpContentProtectionCapabilitiesData()
	{
	DestroyData();
	}

void CT_AvdtpContentProtectionCapabilitiesData::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iAvdtpContentProtectionCapabilities = static_cast<TAvdtpContentProtectionCapabilities*> (aAny);
	}

void CT_AvdtpContentProtectionCapabilitiesData::DisownObjectL()
	{
	iAvdtpContentProtectionCapabilities = NULL;
	}

void CT_AvdtpContentProtectionCapabilitiesData::DestroyData()
	{
	delete iAvdtpContentProtectionCapabilities;
	iAvdtpContentProtectionCapabilities=NULL;	
	}


TBool CT_AvdtpContentProtectionCapabilitiesData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret = ETrue;
	
	if ( aCommand==KCmdnew )
		{
		DoCmdnewL();
		}
	else if ( aCommand==KCmdContentProtectionType )
		{
		DoCmdContentProtectionType(aSection);
		}
	else if ( aCommand==KCmdSetContentProtectionType )
		{
		DoCmdSetContentProtectionType(aSection);
		}
	else if ( aCommand==KCmdContentProtectionData )
		{
		DoCmdContentProtectionData(aSection);
		}
	else if ( aCommand==KCmdSetContentProtectionData )
		{
		DoCmdSetContentProtectionData(aSection);
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
Test TAvdtpContentProtectionCapabilities ()
*/
void CT_AvdtpContentProtectionCapabilitiesData::DoCmdnewL()
	{
	INFO_PRINTF1(_L("TAvdtpContentProtectionCapabilities Constructor Call"));
	DestroyData();
	TRAPD( err, iAvdtpContentProtectionCapabilities = new (ELeave) TAvdtpContentProtectionCapabilities ());
	
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(KLogError, err);
		SetError(err);
		}
	}


/**
Test ContentProtectionType ()
*/
void CT_AvdtpContentProtectionCapabilitiesData::DoCmdContentProtectionType(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpContentProtectionCapabilities ContentProtectionType() Call"));	
	TUint16 actualValue = iAvdtpContentProtectionCapabilities->ContentProtectionType();
	INFO_PRINTF2(_L("ContentProtectionType result (%d)"), actualValue);	

	TInt expected;
	if ( GetIntFromConfig(aSection, KExpected, expected) )
		{
		if ( actualValue != (TUint16)expected )
			{
			ERR_PRINTF3(_L("Result (%d) is not as expected (%d)"), actualValue, expected);
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
Test SetContentProtectionType ()
*/
void CT_AvdtpContentProtectionCapabilitiesData::DoCmdSetContentProtectionType(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpContentProtectionCapabilities SetContentProtectionType() Call"));

	TInt cptype;
	if ( GetIntFromConfig(aSection, KCPType, cptype) )
		{
		iAvdtpContentProtectionCapabilities->SetContentProtectionType((TUint16) cptype);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KCPType);
		SetBlockResult(EFail);
		}
	}


/**
Test ContentProtectionData ()
*/
void CT_AvdtpContentProtectionCapabilitiesData::DoCmdContentProtectionData(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpContentProtectionCapabilities ContentProtectionData() Call"));
	TPtrC8 actualValue = iAvdtpContentProtectionCapabilities->ContentProtectionData();
	
	TPtrC	theString;
	if ( GetStringFromConfig(aSection, KExpected, theString) )
		{
		HBufC8*	string8 = HBufC8::NewLC(theString.Length());
		string8->Des().Copy(theString);
		TPtrC8 expected = string8->Des();

		if ( actualValue != expected )
			{
			ERR_PRINTF1( _L("result is not as expected. ") );
			SetBlockResult(EFail);			
			}
		CleanupStack::PopAndDestroy(string8);
		}
	else
		{
		ERR_PRINTF2(KLogMissingExpected, &KExpected);
		SetBlockResult(EFail);		
		}
	}
	

/**
Test SetContentProtectionData ()
*/
void CT_AvdtpContentProtectionCapabilitiesData::DoCmdSetContentProtectionData(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpContentProtectionCapabilities SetContentProtectionData() Call"));
	TPtrC dataValue;
	if ( GetStringFromConfig(aSection, KData, dataValue) )
		{
		HBufC8* data = HBufC8::NewLC(dataValue.Length());
		data->Des().Copy(dataValue);
		
		iAvdtpContentProtectionCapabilities->SetContentProtectionData(*data);
		CleanupStack::PopAndDestroy(data);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KData);
		SetBlockResult(EFail);		
		}	
	}
	
