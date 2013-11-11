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

#include "bluetoothav.h"

//	User Includes
#include "T_AvdtpServiceCapabilityData.h"
#include "T_AvdtpMediaCodecCapabilitiesData.h"
#include "T_NonSBCCodecCapabilitiesChild.h"
#include "T_NonSBCCodecCapabilitiesData.h"
#include "T_BTUtil.h"

/*@{*/
//Parameters
_LIT(KMediaCodecType,				"mediacodectype");
_LIT(KMediaType,					"mediatype");

//Commands
_LIT(KCmdnew,						"new");
_LIT(KCmdDestructor,				"~"); 

//Use of Protected method
_LIT(KCmdSetMediaCodecType, 		"SetMediaCodecType");
_LIT(KCmdSetMediaType, 				"SetMediaType");

//Log
_LIT(KLogError,							"Error=%d");
_LIT(KLogMissingParameter,				"Missing parameter '%S'");

/*@}*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_NonSBCCodecCapabilitiesData* CT_NonSBCCodecCapabilitiesData::NewL()
	{
	CT_NonSBCCodecCapabilitiesData*	ret=new (ELeave) CT_NonSBCCodecCapabilitiesData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_NonSBCCodecCapabilitiesData::CT_NonSBCCodecCapabilitiesData()
:iNonSBCCodecCapabilities(NULL)
	{
	}

void CT_NonSBCCodecCapabilitiesData::ConstructL()
	{
	}

TAvdtpServiceCapability* CT_NonSBCCodecCapabilitiesData::GetAvdtpServiceCapability() const
	{
	return iNonSBCCodecCapabilities;
	}

CT_NonSBCCodecCapabilitiesData::~CT_NonSBCCodecCapabilitiesData()
	{
	DestroyData();
	}

void CT_NonSBCCodecCapabilitiesData::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iNonSBCCodecCapabilities = static_cast<T_NonSBCCodecCapabilitiesChild*> (aAny);
	}

void CT_NonSBCCodecCapabilitiesData::DisownObjectL()
	{
	iNonSBCCodecCapabilities = NULL;
	}

void CT_NonSBCCodecCapabilitiesData::DestroyData()
	{
	delete iNonSBCCodecCapabilities;
	iNonSBCCodecCapabilities=NULL;	
	}


TBool CT_NonSBCCodecCapabilitiesData::DoCommandL(const TTEFFunction& aCommand,
		const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool ret = ETrue;

		if ( aCommand==KCmdnew )
			{
			DoCmdnewL(aSection);
			}
		else if(aCommand == KCmdSetMediaCodecType)
			{
			DoCmdSetMediaCodecType(aSection);
			}
		else if(aCommand == KCmdSetMediaType)
			{
			DoCmdSetMediaType(aSection);
			}
		else if ( aCommand==KCmdDestructor )
			{
			DestroyData();
			}
		else
			{
			ret= CT_AvdtpMediaCodecCapabilitiesData::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
			}

	return ret;
	}

/**
Test TNonSBCCodecCapabilities ()
*/
void CT_NonSBCCodecCapabilitiesData::DoCmdnewL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TNonSBCCodecCapabilities Constructor Call"));
	DestroyData();
	
	SymbianBluetoothAV::TBluetoothMediaType mediaType;
	if( CT_BTUtil::ReadBluetoothMediaType(*this, aSection, KMediaType, mediaType) )
		{
		TInt mediaCodecType;
		if( GetIntFromConfig(aSection, KMediaCodecType, mediaCodecType))
			{
			TRAPD(err, iNonSBCCodecCapabilities = new (ELeave) T_NonSBCCodecCapabilitiesChild(mediaType, (TBluetoothMediaCodecType)mediaCodecType));
			
			if(err != KErrNone)
				{
				ERR_PRINTF2(KLogError, err);
				SetError(err);
				}
			}
		else
			{
			ERR_PRINTF2(KLogMissingParameter, &KMediaCodecType);
			SetBlockResult(EFail);		
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KMediaType);
		SetBlockResult(EFail);		
		}
	}

/**
Test TAvdtpMediaCodecCapabilities::SetMediaCodecType() - protected method
*/
void CT_NonSBCCodecCapabilitiesData::DoCmdSetMediaCodecType(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpMediaCodecCapabilities::SetMediaCodecType() Call"));

	TInt mediaCodecType = 0;
	if ( GetIntFromConfig(aSection, KMediaCodecType, mediaCodecType) )
		{
		iNonSBCCodecCapabilities->SetMediaCodecType((TBluetoothMediaCodecType)mediaCodecType);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KMediaCodecType);
		SetBlockResult(EFail);
		}	
	}

/**
Test TAvdtpMediaCodecCapabilities::SetMediaType() - protected method
*/
void CT_NonSBCCodecCapabilitiesData::DoCmdSetMediaType(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpMediaCodecCapabilities::SetMediaType() Call"));

	SymbianBluetoothAV::TBluetoothMediaType mediaType;
	if ( CT_BTUtil::ReadBluetoothMediaType(*this, aSection, KMediaType, mediaType) )
		{
		iNonSBCCodecCapabilities->SetMediaType(mediaType);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KMediaType);
		SetBlockResult(EFail);
		}		
	}
