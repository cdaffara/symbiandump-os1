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
#include "T_AvdtpSEPInfoData.h"
#include "T_BTUtil.h"
/*@{*/
//Parameters

//Fields
_LIT(KFldExpected,						"expected");
_LIT(KFldSeid,							"seid");
_LIT(KFldIsInUse,						"isinuse");
_LIT(KFldMediaType,						"mediatype");
_LIT(KFldIsSink,						"issink");

//Commands
_LIT(KCmdnew,							"new");
_LIT(KCmdSEID,							"SEID");
_LIT(KCmdSetSEID,						"SetSEID");
_LIT(KCmdInUse,							"InUse");
_LIT(KCmdSetInUse,						"SetInUse");
_LIT(KCmdMediaType,						"MediaType");
_LIT(KCmdSetMediaType,					"SetMediaType");
_LIT(KCmdIsSink,						"IsSink");
_LIT(KCmdSetIsSink,						"SetIsSink");
_LIT(KCmdDestructor,					"~"); 

//Log
_LIT(KLogError,							"Error=%d");
_LIT(KLogMissingParameter,				"Missing parameter '%S'");
_LIT(KLogMissingParameter2,				"Missing parameter '%S' or '%S'");
_LIT(KLogNotExpectedValue,				"Not expected value");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_AvdtpSEPInfoData* CT_AvdtpSEPInfoData::NewL()
	{
	CT_AvdtpSEPInfoData*	ret=new (ELeave) CT_AvdtpSEPInfoData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_AvdtpSEPInfoData::CT_AvdtpSEPInfoData()
:	iData(NULL)
	{
	}

void CT_AvdtpSEPInfoData::ConstructL()
	{
	}

CT_AvdtpSEPInfoData::~CT_AvdtpSEPInfoData()
	{
	DestroyData();
	}

TAny* CT_AvdtpSEPInfoData::GetObject()
	{
	return iData;
	}


void CT_AvdtpSEPInfoData::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iData = static_cast<TAvdtpSEPInfo*> (aAny);
	}

void CT_AvdtpSEPInfoData::DisownObjectL()
	{
	iData = NULL;
	}

void CT_AvdtpSEPInfoData::DestroyData()
	{
	delete iData;
	iData=NULL;	
	}

inline TCleanupOperation CT_AvdtpSEPInfoData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_AvdtpSEPInfoData::CleanupOperation(TAny* aAny)
	{
	//may be unnecessary
	TAvdtpSEPInfo* arg=static_cast<TAvdtpSEPInfo*>(aAny);
	delete arg;
	}


TBool CT_AvdtpSEPInfoData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;
	
	if ( aCommand==KCmdnew )
		{
		DoCmdnewL(aSection);
		}
	else if ( aCommand==KCmdSEID )
		{
		DoCmdSEIDL(aSection);
		}
	else if ( aCommand==KCmdSetSEID )
		{
		DoCmdSetSEIDL(aSection);
		}
	else if ( aCommand==KCmdInUse )
		{
		DoCmdInUse(aSection);
		}
	else if ( aCommand==KCmdSetInUse )
		{
		DoCmdSetInUse(aSection);
		}
	else if ( aCommand==KCmdMediaType )
		{
		DoCmdMediaType(aSection);
		}
	else if ( aCommand==KCmdSetMediaType )
		{
		DoCmdSetMediaType(aSection);
		}
	else if ( aCommand==KCmdIsSink )
		{
		DoCmdIsSink(aSection);
		}
	else if ( aCommand==KCmdSetIsSink )
		{
		DoCmdSetIsSink(aSection);
		}
	else if ( aCommand==KCmdDestructor )
		{
		DestroyData();
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

/**
Test TAvdtpSEPInfo()
*/
void CT_AvdtpSEPInfoData::DoCmdnewL(const TDesC& /*aSection*/)
	{
	INFO_PRINTF1(_L("TAvdtpSEPInfo Constructor Call."));
	DestroyData();
	
	TInt err = KErrNone;
	TRAP( err, iData = new (ELeave) TAvdtpSEPInfo());

	if ( err!=KErrNone )
		{
		ERR_PRINTF2(KLogError, err);
		SetError(err);
		}
	}

/**
Test SEID()
*/
void CT_AvdtpSEPInfoData::DoCmdSEIDL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpSEPInfo SEID() Call."));
	INFO_PRINTF1(_L("Execute SEID()"));
	TSEID actual = iData->SEID();	
	_LIT(KLogSeid,"actual seid: %d");
	INFO_PRINTF2(KLogSeid, actual.SEID());
	_LIT(KLogIslocalTrue, "IsLocal: true");
	_LIT(KLogIslocalFalse, "IsLocal: false");
	if(actual.IsLocal())
		{
		INFO_PRINTF1(KLogIslocalTrue);
		}
	else
		{
		INFO_PRINTF1(KLogIslocalFalse);
		}
	
	TSEID*	expected=NULL;
	TSEID*	setbackSeid=NULL;
	TPtrC	seidName;
	if ( !GetStringFromConfig(aSection, KFldExpected, seidName) )
		{
		if ( !GetStringFromConfig(aSection, KFldSeid, seidName) )
			{
			ERR_PRINTF3(KLogMissingParameter2, &KFldExpected, &KFldSeid);
			SetBlockResult(EFail);
			}
		else
			{
			_LIT(KLogSetSEIDBack,"Set SEID back. Not compare with expected.");
			INFO_PRINTF1(KLogSetSEIDBack);
			setbackSeid = static_cast<TSEID*>(GetDataObjectL(seidName));
			*setbackSeid = actual;
			}	
		}
	else
		{
		expected = static_cast<TSEID*>(GetDataObjectL(seidName));
		
		if(expected != NULL)
			{
			if(actual != (*expected))
				{
				ERR_PRINTF1(KLogNotExpectedValue);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("TSEID pointer is NULL"));
			SetBlockResult(EFail);			
			}
		}	
	}

/**
Test SetSEID()
*/
void CT_AvdtpSEPInfoData::DoCmdSetSEIDL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpSEPInfo SetSEID() Call."));
	TSEID*	seid=NULL;
	TPtrC	seidName;
	if ( !GetStringFromConfig(aSection, KFldSeid, seidName) )
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldSeid);
		SetBlockResult(EFail);
		}
	else
		{
		seid = static_cast<TSEID*>(GetDataObjectL(seidName));
		
		if(!seid)
			{
			ERR_PRINTF1(_L("TSEID pointer is NULL"));
			SetBlockResult(EFail);
			}
		else 
			{
			INFO_PRINTF1(_L("Execute SetSEID(TSEID)"));
			iData->SetSEID(*seid);
			}
		}	
	}

/**
Test InUse()
*/
void CT_AvdtpSEPInfoData::DoCmdInUse(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpSEPInfo InUse() Call."));
	TBool actual = iData->InUse();
	INFO_PRINTF2(_L("Execute InUse()=%d"), actual);
	
	TBool expected = EFalse;
	if( !GetBoolFromConfig(aSection, KFldExpected(), expected) )
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldExpected);
		SetBlockResult(EFail);
		}
	else
		{
		if ( actual!=expected )
			{
			ERR_PRINTF1(KLogNotExpectedValue);
			SetBlockResult(EFail);
			}
		}
	}

/**
Test SetInUse()
*/
void CT_AvdtpSEPInfoData::DoCmdSetInUse(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpSEPInfo SetInUse() Call."));
	TBool isInUse=EFalse;
	if( !GetBoolFromConfig(aSection, KFldIsInUse(), isInUse) )
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldIsInUse);
		SetBlockResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("Execute SetInUse(TBool), isinuse = %d"), isInUse);
		iData->SetInUse(isInUse);
		}
	}

/**
Test MediaType()
*/
void CT_AvdtpSEPInfoData::DoCmdMediaType(const TDesC& aSection) 
	{	
	INFO_PRINTF1(_L("TAvdtpSEPInfo MediaType() Call."));
	SymbianBluetoothAV::TBluetoothMediaType actual = iData->MediaType();
	INFO_PRINTF2(_L("Execute MediaType()=%d"), actual);
	
	SymbianBluetoothAV::TBluetoothMediaType expected;
	if( CT_BTUtil::ReadBluetoothMediaType(*this, aSection, KFldExpected(), expected) )
		{
		if ( actual!=expected )
			{
			ERR_PRINTF1(KLogNotExpectedValue);
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, KFldExpected);
		SetBlockResult(EFail);		
		}
	}

/**
Test SetMediaType()
*/
void CT_AvdtpSEPInfoData::DoCmdSetMediaType(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpSEPInfo SetMediaType() Call."));
	SymbianBluetoothAV::TBluetoothMediaType mediaType;
	if( CT_BTUtil::ReadBluetoothMediaType(*this, aSection, KFldMediaType(), mediaType) )
		{
		INFO_PRINTF2(_L("Execute SetMediaType(SymbianBluetoothAV::TBluetoothMediaType), MediaType = %d"), mediaType);
		iData->SetMediaType(mediaType);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, KFldMediaType);
		SetBlockResult(EFail);		
		}
	}

/**
Test IsSink()
*/
void CT_AvdtpSEPInfoData::DoCmdIsSink(const TDesC& aSection) 
	{
	INFO_PRINTF1(_L("TAvdtpSEPInfo IsSink() Call."));
	TBool actual = iData->IsSink();
	INFO_PRINTF2(_L("Execute IsSink()()=%d"), actual);
	
	TBool expected = EFalse;
	if( !GetBoolFromConfig(aSection, KFldExpected(), expected) )
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldExpected);
		SetBlockResult(EFail);
		}
	else
		{
		if ( actual!=expected )
			{
			ERR_PRINTF1(KLogNotExpectedValue);
			SetBlockResult(EFail);
			}
		}
	}

/**
Test SetIsSink()
*/
void CT_AvdtpSEPInfoData::DoCmdSetIsSink(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TAvdtpSEPInfo SetIsSink() Call."));
	TBool isSink=EFalse;
	if( !GetBoolFromConfig(aSection, KFldIsSink(), isSink) )
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldIsInUse);
		SetBlockResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("Execute SetIsSink(TBool), isSink = %d"), isSink);
		iData->SetIsSink(isSink);
		}
	}
