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
#include "T_SEIDData.h"

/*@{*/
//Parameters

//Fields
_LIT(KFldExpected,						"expected");
_LIT(KFldSeid,							"seid");
_LIT(KFldIsLocal,						"islocal");
_LIT(KFldValue,							"value");

//Commands
_LIT(KCmdnew,							"new");
_LIT(KCmdSEID,							"SEID");
_LIT(KCmdPacketValue,					"PacketValue");
_LIT(KCmdIsValid,						"IsValid");
_LIT(KCmdSet,							"Set");
_LIT(KCmdCompare,						"==");
_LIT(KCmdNegativeCompare,				"!=");
_LIT(KCmd_Spare,						"_Spare");
_LIT(KCmdFromPacketL,					"FromPacketL");
_LIT(KCmdReset,							"Reset");
_LIT(KCmdIsLocal,						"IsLocal");
_LIT(KCmdValue,							"Value"); 
_LIT(KCmdDestructor,					"~"); 
//Log
_LIT(KLogError,							"Error=%d");
_LIT(KLogMissingParameter,				"Missing parameter '%S'");
_LIT(KLogNotExpectedValue,				"Not expected value");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_SEIDData* CT_SEIDData::NewL()
	{
	CT_SEIDData*	ret=new (ELeave) CT_SEIDData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_SEIDData::CT_SEIDData()
:	iData(NULL)
	{
	}

void CT_SEIDData::ConstructL()
	{
	}

CT_SEIDData::~CT_SEIDData()
	{
	DestroyData();
	}

TAny* CT_SEIDData::GetObject()
	{
	return iData;
	}


void CT_SEIDData::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iData = static_cast<TSEID*> (aAny);
	}

void CT_SEIDData::DisownObjectL()
	{
	iData = NULL;
	}

void CT_SEIDData::DestroyData()
	{
	delete iData;
	iData=NULL;	
	}

inline TCleanupOperation CT_SEIDData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_SEIDData::CleanupOperation(TAny* aAny)
	{
	//may be unnecessary
	TSEID* arg=static_cast<TSEID*>(aAny);
	delete arg;
	}


TBool CT_SEIDData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;
	
	if ( aCommand==KCmdnew )
		{
		DoCmdnewL(aSection);
		}
	else if ( aCommand==KCmdSEID )
		{
		DoCmdSEID(aSection);
		}
	else if ( aCommand==KCmdPacketValue )
		{
		DoCmdPacketValue(aSection);
		}
	else if ( aCommand==KCmdIsValid )
		{
		DoCmdIsValid(aSection);
		}
	else if ( aCommand==KCmdSet )
		{
		DoCmdSet(aSection);
		}
	else if ( aCommand==KCmdCompare )
		{
		DoCmdCompareL(aSection);
		}
	else if ( aCommand==KCmdNegativeCompare )
		{
		DoCmdNegativeCompareL(aSection);
		}
	else if ( aCommand==KCmd_Spare )
		{
		DoCmd_Spare(aSection);
		}
	else if ( aCommand==KCmdFromPacketL )
		{
		DoCmdFromPacketL(aSection);
		}
	else if ( aCommand==KCmdReset )
		{
		DoCmdReset(aSection);
		}
	else if ( aCommand==KCmdIsLocal )
		{
		DoCmdIsLocal(aSection);
		}
	else if ( aCommand==KCmdValue )
		{
		DoCmdValue(aSection);
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
Test TSEID()
*/
void CT_SEIDData::DoCmdnewL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TSEID Constructor Call"));
	DestroyData();
	
	
	TBool	isTwoParameters=EFalse;
	TBool	isOneParameter=EFalse;
	
	TInt	seid=0;
	TBool	islocal=EFalse;
	
	TInt	value=0;
	TInt 	dataOK = ETrue;
	
	if( GetIntFromConfig(aSection, KFldSeid(), seid) )
		{
		if(GetBoolFromConfig(aSection, KFldIsLocal(), islocal) )
			{
			INFO_PRINTF1(_L("Constructor got 2 parameters.)"));
			isTwoParameters = ETrue;
			}
		else
			{
			ERR_PRINTF2(KLogMissingParameter, &KFldIsLocal);
			dataOK = EFalse;
			SetBlockResult(EFail);
			}
		}
	else
		{
		if( GetIntFromConfig(aSection, KFldValue(), value) )
			{
			INFO_PRINTF1(_L("Constructor got parameter as value"));
			isOneParameter = ETrue;
			}
		}
	
	if(dataOK)
		{
		TInt err = KErrNone;
		if(isTwoParameters)
			{
			INFO_PRINTF1(_L("Constructor called is TSEID(TUint, TBool)"));
			TRAP( err, iData = new (ELeave) TSEID(seid,islocal));
			}
		else if(isOneParameter)
			{
			INFO_PRINTF1(_L("Constructor called is TSEID(TUint)."));
			TRAP( err, iData = new (ELeave) TSEID(value));
			}
		else
			{
			INFO_PRINTF1(_L("Constructor called is TSEID()."));
			TRAP( err, iData = new (ELeave) TSEID());
			}
		
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(KLogError, err);
			SetError(err);
			}
		}
	
	}

/**
Test SEID()
*/
void CT_SEIDData::DoCmdSEID(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TSEID SEID() Call."));
	TInt actual = iData->SEID();
	INFO_PRINTF2(_L("SEID()=%d"), actual);
	
	TInt expected=0;
	if( !GetIntFromConfig(aSection, KFldExpected(), expected) )
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
Test PacketValue()
*/
void CT_SEIDData::DoCmdPacketValue(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TSEID PacketValue() Call."));
	TInt actual = iData->PacketValue();
	INFO_PRINTF2(_L("PacketValue()=%d"), actual);
	
	TInt expected=0;
	if( !GetIntFromConfig(aSection, KFldExpected(), expected) )
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
Test IsValid()
*/
void CT_SEIDData::DoCmdIsValid(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TSEID IsValid() Call."));
	TBool actual = iData->IsValid();
	INFO_PRINTF2(_L("IsValid()=%d"), actual);
	
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
Test Set()
*/
void CT_SEIDData::DoCmdSet(const TDesC& aSection)
	{	
	INFO_PRINTF1(_L("TSEID Set() Call."));
	TInt value=0;
	if( !GetIntFromConfig(aSection, KFldValue(), value) )
		{
		ERR_PRINTF2(KLogMissingParameter, &KFldValue);
		SetBlockResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("Execute Set(TUint), value = %d"), value);
		iData->Set(value);
		}
	}

/**
Test operator==()
*/
void CT_SEIDData::DoCmdCompareL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TSEID == Call."));
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
		}	
	
	if(seid)
		{
		TBool actual = iData->operator ==(*seid);
		INFO_PRINTF2(_L("execute operator==(TSEID) = %d"), actual);
		
		TBool expected=EFalse;
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
	else
		{
		ERR_PRINTF1(_L("seid is NULL"));
		SetBlockResult(EFail);
		}
	}

/**
Test operator!=()
*/
void CT_SEIDData::DoCmdNegativeCompareL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TSEID != Call."));
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
		}	
	
	if(seid)
		{
		TBool actual = iData->operator !=(*seid);
		INFO_PRINTF2(_L("execuete operator !=(TSEID) = %d"), actual);
		
		TBool expected=EFalse;
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
	else
		{
		ERR_PRINTF1(_L("seid is NULL"));
		SetBlockResult(EFail);
		}
	}

/**
Test _Spare()
*/
void CT_SEIDData::DoCmd_Spare(const TDesC& /*aSection*/)
	{
	//Not implemented!
	}

/**
Test FromPacketL()
*/
void CT_SEIDData::DoCmdFromPacketL(const TDesC& /*aSection*/)
	{
	//Not implemented!
	}

/**
Test Reset()
*/
void CT_SEIDData::DoCmdReset(const TDesC& /*aSection*/)
	{
	INFO_PRINTF1(_L("TSEID Reset() Call."));
	iData->Reset();
	}

/**
Test IsLocal()
*/
void CT_SEIDData::DoCmdIsLocal(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TSEID IsLocal() Call."));
	TBool actual = iData->IsLocal();
	INFO_PRINTF2(_L("IsLocal()=%d"), actual);
	
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
Test Value()
*/
void CT_SEIDData::DoCmdValue(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TSEID Value() Call."));
	TInt actual = iData->Value();
	INFO_PRINTF2(_L("Value()=%d"), actual);
	
	TInt expected=0;
	if( !GetIntFromConfig(aSection, KFldExpected(), expected) )
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
