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


#include "T_AvdtpServiceCapabilityData.h"
#include "T_BTUtil.h"

/*@{*/
//Parameters
_LIT(KExpected,									"expected");

//Commands
_LIT(KCmdCategory,								"Category");
_LIT(KCmdAsProtocol,							"AsProtocol");
_LIT(KCmdParse,									"Parse");
_LIT(KCmdAllocFromPDUL,							"AllocFromPDUL");
_LIT(KCmdAllocFromPckgL,						"AllocFromPckgL");

///	Logging
_LIT(KLogMissingParameter,						"Missing parameter '%S'");
_LIT(KLogNotExpectedValue,						"Not expected value");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_AvdtpServiceCapabilityData::CT_AvdtpServiceCapabilityData()
	{
	}


TBool CT_AvdtpServiceCapabilityData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;

	if ( aCommand==KCmdCategory )
		{
		DoCmdCategory(aSection);
		}
	else if(aCommand == KCmdAsProtocol)
		{
		DoCmdAsProtocol(aSection);
		}
	else if(aCommand == KCmdParse)
		{
		DoCmdParse(aSection);
		}
	else if(aCommand == KCmdAllocFromPDUL)
		{
		DoCmdAllocFromPDUL(aSection);
		}
	else if(aCommand == KCmdAllocFromPckgL)
		{
		DoCmdAllocFromPDUL(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


/**
Test Category()
*/
void CT_AvdtpServiceCapabilityData::DoCmdCategory(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpServiceCapability Category Call"));
	
	TAvdtpServiceCategory avdtpServiceCategory = GetAvdtpServiceCapability()->Category();
	INFO_PRINTF2(_L("TAvdtpServiceCapability Category result: actual = %d"), avdtpServiceCategory);
	
	TAvdtpServiceCategory	expected;
	if ( CT_BTUtil::ReadAvdtpServiceCategory(*this, aSection, KExpected(), expected) )
		{
		if ( avdtpServiceCategory!=expected )
			{
			ERR_PRINTF1(KLogNotExpectedValue);
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, KExpected);
		SetBlockResult(EFail);		
		}
	}

/**
Test AsProtocol()
*/ 
void CT_AvdtpServiceCapabilityData::DoCmdAsProtocol(const TDesC& /*aSection*/)
	{
	
	}

/**
Test Parse()
*/ 
void CT_AvdtpServiceCapabilityData::DoCmdParse(const TDesC& /*aSection*/)
	{
	
	}

/**
Test AllocFromPDUL()
*/ 
void CT_AvdtpServiceCapabilityData::DoCmdAllocFromPDUL(const TDesC& /*aSection*/)
	{
	
	}

/**
Test AllocFromPckgL()
*/ 
void CT_AvdtpServiceCapabilityData::DoCmdAllocFromPckgL(const TDesC& /*aSection*/)
	{
	
	}
