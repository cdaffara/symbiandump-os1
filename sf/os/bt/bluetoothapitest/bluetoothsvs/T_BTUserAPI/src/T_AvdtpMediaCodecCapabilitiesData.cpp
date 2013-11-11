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
#include "T_AvdtpMediaCodecCapabilitiesData.h"
#include "T_BTUtil.h"

/*@{*/
//Parameters
_LIT(KExpected,									"expected");


//Commands
_LIT(KCmdMediaType,								"MediaType");
_LIT(KCmdMediaCodecType,						"MediaCodecType");
_LIT(KCmdMinWindowSize,							"MinWindowSize");
_LIT(KCmdSetMinWindowSize,						"SetMinWindowSize");
_LIT(KCmdAllocCodecFromPDUL,					"AllocCodecFromPDUL");

//Log
_LIT(KLogMissingExpected,								"Missing expected '%S'");


/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_AvdtpMediaCodecCapabilitiesData::CT_AvdtpMediaCodecCapabilitiesData()
	{
	}


TBool CT_AvdtpMediaCodecCapabilitiesData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret = ETrue;

	if ( aCommand==KCmdMediaType )
		{
		DoCmdMediaType(aSection);
		}
	else if(aCommand == KCmdMediaCodecType)
		{
		DoCmdMediaCodecType(aSection);
		}
	else if(aCommand == KCmdMinWindowSize)
		{
		DoCmdMinWindowSize(aSection);
		}
	else if(aCommand == KCmdSetMinWindowSize)
		{
		DoCmdSetMinWindowSize(aSection);
		}
	else if(aCommand == KCmdAllocCodecFromPDUL)
		{
		AllocCodecFromPDUL(aSection);
		}
	else
		{
		ret = CT_AvdtpServiceCapabilityData::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	return ret;
	}

/**
Test MediaType() 
*/
void CT_AvdtpMediaCodecCapabilitiesData::DoCmdMediaType(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpMediaCodecCapabilities MediaType() Call"));	
	SymbianBluetoothAV::TBluetoothMediaType actualValue = ((TAvdtpMediaCodecCapabilities*)GetAvdtpServiceCapability())->MediaType();
	INFO_PRINTF2(_L("MediaType result (%d)"), actualValue);	

	SymbianBluetoothAV::TBluetoothMediaType expected;
	if ( CT_BTUtil::ReadBluetoothMediaType(*this, aSection, KExpected, expected) )
		{
		if ( actualValue != expected )
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
Test MediaCodecType() 
*/
void CT_AvdtpMediaCodecCapabilitiesData::DoCmdMediaCodecType(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpMediaCodecCapabilities MediaCodecType() Call"));	
	TBluetoothMediaCodecType actualValue = ((TAvdtpMediaCodecCapabilities*)GetAvdtpServiceCapability())->MediaCodecType();
	INFO_PRINTF2(_L("MediaCodecType result (%d)"), actualValue);	

	TInt expected;
	if ( GetIntFromConfig(aSection, KExpected, expected) )
		{
		if ( actualValue != (TBluetoothMediaCodecType)expected )
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
Test MinWindowSize() 
*/
void CT_AvdtpMediaCodecCapabilitiesData::DoCmdMinWindowSize(const TDesC& /*aSection*/)
	{
//	INFO_PRINTF1(_L("TAvdtpMediaCodecCapabilities MinWindowSize() Call"));	
//	TAvdtpRecoveryWindowSize actualValue = ((TAvdtpMediaCodecCapabilities*)GetAvdtpServiceCapability())->MinWindowSize();
//	INFO_PRINTF2(_L("MinWindowSize result (%d)"), actualValue);	
//
//	TInt expected;
//	if ( GetIntFromConfig(aSection, KExpected, expected) )
//		{
//		if ( actualValue != (TAvdtpRecoveryWindowSize)expected )
//			{
//			ERR_PRINTF3(_L("Result (%d) is not as expected (%d)"), actualValue, expected);
//			SetBlockResult(EFail);	
//			}			
//		}
//	else
//		{
//		ERR_PRINTF2(KLogMissingExpected, &KExpected);
//		SetBlockResult(EFail);		
//		}	
	}

/**
Test SetMinWindowSize() 
*/
void CT_AvdtpMediaCodecCapabilitiesData::DoCmdSetMinWindowSize(const TDesC& /*aSection*/)
	{
//	INFO_PRINTF1(_L("TAvdtpMediaCodecCapabilities SetMinWindowSize() Call"));
//
//	TInt size = 0;
//	if ( GetIntFromConfig(aSection, KSize, size) )
//		{
//		((TAvdtpMediaCodecCapabilities*)GetAvdtpServiceCapability())->SetMinWindowSize((TAvdtpRecoveryWindowSize)size);
//		}
//	else
//		{
//		ERR_PRINTF2(KLogMissingParameter, &KSize);
//		SetBlockResult(EFail);
//		}
	}

/**
Test CodecFromPDUL() 
*/
void CT_AvdtpMediaCodecCapabilitiesData::AllocCodecFromPDUL(const TDesC& /*aSection*/)
	{
	
	}
