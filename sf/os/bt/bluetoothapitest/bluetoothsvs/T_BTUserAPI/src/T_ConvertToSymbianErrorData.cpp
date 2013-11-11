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
#include "T_ConvertToSymbianErrorData.h"
#include "T_BTUtil.h"

/*@{*/
//Parameters
_LIT(KFldAvdtpError,								"avdtperror%d");
_LIT(KFldGavdpError,								"gavdperror%d");
_LIT(KFldA2dpError,									"a2dperror%d");
_LIT(KFldVdpError,									"vdperror%d");
_LIT(KFldExpected,									"expected%d");

//Commands
_LIT(KCmdAvdtpError,							"AvdtpError");
_LIT(KCmdGavdpError,							"GavdpError");
_LIT(KCmdA2dpError,								"A2dpError");
_LIT(KCmdVdpError,								"VdpError");

///	Logging
_LIT(KLogMissingParameter,						"Missing parameter '%S'");
_LIT(KLogNotExpectedValue,						"Not expected value");

/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_ConvertToSymbianErrorData* CT_ConvertToSymbianErrorData::NewL()
	{
	CT_ConvertToSymbianErrorData*	ret=new (ELeave) CT_ConvertToSymbianErrorData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_ConvertToSymbianErrorData::CT_ConvertToSymbianErrorData()
	{
	}

void CT_ConvertToSymbianErrorData::ConstructL()
	{
	}

CT_ConvertToSymbianErrorData::~CT_ConvertToSymbianErrorData()
	{
	}

TAny* CT_ConvertToSymbianErrorData::GetObject()
	{
	return NULL;
	}


TBool CT_ConvertToSymbianErrorData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;
	
	if ( aCommand==KCmdAvdtpError )
		{
		DoCmdAvdtpError(aSection);
		}
	else if ( aCommand==KCmdGavdpError )
		{
		DoCmdGavdpError(aSection);
		}
	else if ( aCommand==KCmdA2dpError )
		{
		DoCmdA2dpError(aSection);
		}
	else if ( aCommand==KCmdVdpError )
		{
		DoCmdVdpError(aSection);
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

/**
Test AvdtpError()
*/
void CT_ConvertToSymbianErrorData::DoCmdAvdtpError(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("ConvertToSymbianError::AvdtpError(TAvdtpSignallingErrorCode) Call"));
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	
	SymbianBluetoothAV::TAvdtpSignallingErrorCode errorCode;
	TBool moreData = ETrue;
	TUint index = 0;
	while(moreData)
		{
		tempStore.Format(KFldAvdtpError(), ++index);
		moreData = CT_BTUtil::ReadAvdtpSignallingErrorCode(*this,aSection, tempStore, errorCode);
		
		if(moreData)
			{
			TInt actualCode = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(errorCode);
			INFO_PRINTF2(_L("ConvertToSymbianError::AvdtpError(TAvdtpSignallingErrorCode)=%d"), actualCode);
			
			TInt expected = 0;
			tempStore.Format(KFldExpected(), index);
			if(!GetIntFromConfig(aSection, tempStore, expected))
				{
				ERR_PRINTF2(KLogMissingParameter, &tempStore);
				SetBlockResult(EFail);
				}
			else
				{
				if(actualCode != expected)
					{
					ERR_PRINTF1(KLogNotExpectedValue);
					SetBlockResult(EFail);
					}
				}
			}
		}
	}

/**
Test GavdpError()
*/
void CT_ConvertToSymbianErrorData::DoCmdGavdpError(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("ConvertToSymbianError::GavdpError(TGavdpSignallingErrorCode) Call"));
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	
	SymbianBluetoothAV::TGavdpSignallingErrorCode errorCode;
	TBool moreData = ETrue;
	TUint index = 0;
	while(moreData)
		{
		tempStore.Format(KFldGavdpError(), ++index);
		moreData = CT_BTUtil::ReadGavdpSignallingErrorCode(*this,aSection, tempStore, errorCode);
		
		if(moreData)
			{
			TInt actualCode = SymbianBluetoothAV::ConvertToSymbianError::GavdpError(errorCode);
			INFO_PRINTF2(_L("ConvertToSymbianError::GavdpError(TGavdpSignallingErrorCode)=%d"), actualCode);
			
			TInt expected = 0;
			tempStore.Format(KFldExpected(), index);
			if(!GetIntFromConfig(aSection, tempStore, expected))
				{
				ERR_PRINTF2(KLogMissingParameter, &tempStore);
				SetBlockResult(EFail);
				}
			else
				{
				if(actualCode != expected)
					{
					ERR_PRINTF1(KLogNotExpectedValue);
					SetBlockResult(EFail);
					}
				}
			}
		}	
	}

/**
Test A2dpError()
*/
void CT_ConvertToSymbianErrorData::DoCmdA2dpError(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("ConvertToSymbianError::A2dpError(TA2dpSignallingErrorCode) Call"));
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	
	SymbianBluetoothAV::TA2dpSignallingErrorCode errorCode;
	TBool moreData = ETrue;
	TUint index = 0;
	while(moreData)
		{
		tempStore.Format(KFldA2dpError(), ++index);
		moreData = CT_BTUtil::ReadA2dpSignallingErrorCode(*this,aSection, tempStore, errorCode);
		
		if(moreData)
			{
			TInt actualCode = SymbianBluetoothAV::ConvertToSymbianError::A2dpError(errorCode);
			INFO_PRINTF2(_L("ConvertToSymbianError::A2dpError(TA2dpSignallingErrorCode)=%d"), actualCode);
			
			TInt expected = 0;
			tempStore.Format(KFldExpected(), index);
			if(!GetIntFromConfig(aSection, tempStore, expected))
				{
				ERR_PRINTF2(KLogMissingParameter, &tempStore);
				SetBlockResult(EFail);
				}
			else
				{
				if(actualCode != expected)
					{
					ERR_PRINTF1(KLogNotExpectedValue);
					SetBlockResult(EFail);
					}
				}
			}
		}	
	}

/**
Test VdpError()
*/
void CT_ConvertToSymbianErrorData::DoCmdVdpError(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("ConvertToSymbianError::VdpError(TVdpSignallingErrorCode) Call"));
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	
	SymbianBluetoothAV::TVdpSignallingErrorCode errorCode;
	TBool moreData = ETrue;
	TUint index = 0;
	while(moreData)
		{
		tempStore.Format(KFldVdpError(), ++index);
		moreData = CT_BTUtil::ReadVdpSignallingErrorCode(*this,aSection, tempStore, errorCode);
		
		if(moreData)
			{
			TInt actualCode = SymbianBluetoothAV::ConvertToSymbianError::VdpError(errorCode);
			INFO_PRINTF2(_L("ConvertToSymbianError::VdpError(TVdpSignallingErrorCode)=%d"), actualCode);
			
			TInt expected = 0;
			tempStore.Format(KFldExpected(), index);
			if(!GetIntFromConfig(aSection, tempStore, expected))
				{
				ERR_PRINTF2(KLogMissingParameter, &tempStore);
				SetBlockResult(EFail);
				}
			else
				{
				if(actualCode != expected)
					{
					ERR_PRINTF1(KLogNotExpectedValue);
					SetBlockResult(EFail);
					}
				}
			}
		}	
	}

