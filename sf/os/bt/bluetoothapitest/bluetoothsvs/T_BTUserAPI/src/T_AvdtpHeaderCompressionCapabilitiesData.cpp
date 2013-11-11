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
#include "T_AvdtpHeaderCompressionCapabilitiesData.h"


/*@{*/
//Parameters
_LIT(KExpected,												"expected");
_LIT(KMedia,												"media");
_LIT(KRecovery,												"recovery");
_LIT(KBackChannel,											"backchannel");

//Commands
_LIT(KCmdnew,												"new");
_LIT(KCmdMedia,												"Media");
_LIT(KCmdRecovery,											"Recovery");
_LIT(KCmdBackChannel,										"BackChannel");
_LIT(KCmdSetCapabilities,									"SetCapabilities");
_LIT(KCmdDestructor,										"~"); 

//Log
_LIT(KLogMissingExpected,								"Missing expected '%S'");
_LIT(KLogError,											"Error=%d");

/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_AvdtpHeaderCompressionCapabilitiesData* CT_AvdtpHeaderCompressionCapabilitiesData::NewL()
	{
	CT_AvdtpHeaderCompressionCapabilitiesData*	ret=new (ELeave) CT_AvdtpHeaderCompressionCapabilitiesData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_AvdtpHeaderCompressionCapabilitiesData::CT_AvdtpHeaderCompressionCapabilitiesData()
: iAvdtpHeaderCompressionCapabilities(NULL)
	{
	}

void CT_AvdtpHeaderCompressionCapabilitiesData::ConstructL()
	{
	}

TAvdtpServiceCapability* CT_AvdtpHeaderCompressionCapabilitiesData::GetAvdtpServiceCapability() const
	{
	return iAvdtpHeaderCompressionCapabilities;
	}

CT_AvdtpHeaderCompressionCapabilitiesData::~CT_AvdtpHeaderCompressionCapabilitiesData()
	{
	DestroyData();
	}

void CT_AvdtpHeaderCompressionCapabilitiesData::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iAvdtpHeaderCompressionCapabilities = static_cast<TAvdtpHeaderCompressionCapabilities*> (aAny);
	}

void CT_AvdtpHeaderCompressionCapabilitiesData::DisownObjectL()
	{
	iAvdtpHeaderCompressionCapabilities = NULL;
	}

void CT_AvdtpHeaderCompressionCapabilitiesData::DestroyData()
	{
	delete iAvdtpHeaderCompressionCapabilities;
	iAvdtpHeaderCompressionCapabilities=NULL;	
	}


TBool CT_AvdtpHeaderCompressionCapabilitiesData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret = ETrue;
	
	if ( aCommand==KCmdnew )
		{
		DoCmdnewL();
		}
	else if ( aCommand==KCmdMedia )
		{
		DoCmdMedia(aSection);
		}
	else if ( aCommand==KCmdRecovery )
		{
		DoCmdRecovery(aSection);
		}
	else if ( aCommand==KCmdBackChannel )
		{
		DoCmdBackChannel(aSection);
		}
	else if ( aCommand==KCmdSetCapabilities )
		{
		DoCmdSetCapabilities(aSection);
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
Test TAvdtpHeaderCompressionCapabilities()
*/
void CT_AvdtpHeaderCompressionCapabilitiesData::DoCmdnewL()
	{
	INFO_PRINTF1(_L("TAvdtpHeaderCompressionCapabilities Constructor Call"));
	DestroyData();
	
	TRAPD( err, iAvdtpHeaderCompressionCapabilities = new (ELeave) TAvdtpHeaderCompressionCapabilities());
	
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(KLogError, err);
		SetError(err);
		}
	}


/**
Test Media()
*/
void CT_AvdtpHeaderCompressionCapabilitiesData::DoCmdMedia(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpHeaderCompressionCapabilities Media() Call"));

	TBool media = iAvdtpHeaderCompressionCapabilities->Media();
	INFO_PRINTF2(_L("TAvdtpHeaderCompressionCapabilities Media result: actual = %d"), media);
	
	TBool expected = EFalse;
	if( GetBoolFromConfig(aSection, KExpected, expected) )
		{
		if( expected != media )
			{
			ERR_PRINTF1(_L("Media not as expected!"));
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
Test Recovery()
*/
void CT_AvdtpHeaderCompressionCapabilitiesData::DoCmdRecovery(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpHeaderCompressionCapabilities Recovery() Call"));
	
	TBool recovery = iAvdtpHeaderCompressionCapabilities->Recovery();
	INFO_PRINTF2(_L("TAvdtpHeaderCompressionCapabilities Recovery result: actual = %d"), recovery);
	
	TBool expected = EFalse;
	if( GetBoolFromConfig(aSection, KExpected, expected) )
		{
		if( expected != recovery )
			{
			ERR_PRINTF1(_L("Recovery not as expected!"));
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
Test BackChannel()
*/
void CT_AvdtpHeaderCompressionCapabilitiesData::DoCmdBackChannel(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpHeaderCompressionCapabilities BackChannel() Call"));
	
	TBool backChannel = iAvdtpHeaderCompressionCapabilities->BackChannel();
	INFO_PRINTF2(_L("TAvdtpHeaderCompressionCapabilities BackChannel result: actual = %d"), backChannel);
	
	TBool expected = EFalse;
	if( GetBoolFromConfig(aSection, KExpected, expected) )
		{
		if( expected != backChannel )
			{
			ERR_PRINTF1(_L("BackChannel not as expected!"));
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
Test SetCapabilities()
*/
void CT_AvdtpHeaderCompressionCapabilitiesData::DoCmdSetCapabilities(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpHeaderCompressionCapabilities SetCapabilities() Call"));
	
	TBool media = EFalse;
	if( GetBoolFromConfig(aSection, KMedia, media) )
		{
		TBool recovery = EFalse;
		if( GetBoolFromConfig(aSection, KRecovery, recovery) )
			{
			TBool backChannel = EFalse;
			if( GetBoolFromConfig(aSection, KBackChannel, backChannel) )
				{
				iAvdtpHeaderCompressionCapabilities->SetCapabilities(media, recovery, backChannel);
				}
			else
				{
				ERR_PRINTF2(_L("Missing parameter %S"), &KBackChannel);
				SetBlockResult(EFail);		
				}
			}
		else
			{
			ERR_PRINTF2(_L("Missing parameter %S"), &KRecovery);
			SetBlockResult(EFail);		
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KMedia);
		SetBlockResult(EFail);		
		}
	}

