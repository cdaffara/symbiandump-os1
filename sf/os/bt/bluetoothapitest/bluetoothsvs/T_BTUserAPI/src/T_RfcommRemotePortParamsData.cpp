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

#include "T_RfcommRemotePortParamsData.h"
#include "T_RfcommRPNTransactionData.h"
#include "T_BTUtil.h"

#include <d32comm.h>

/*@{*/

//Parameters
_LIT(KExpected, 											"expected");
_LIT(KXoffChar,												"xoffchar");
_LIT(KXonChar,												"xonchar");
_LIT(KDataBit,												"databit");
_LIT(KBitRate,												"bitrate");
_LIT(KFlowCtrl,												"flowctrl");
_LIT(KFlowCtrlMask,											"flowctrlmask");
_LIT(KPRNTobject,											"prntransaction");
_LIT(KStopBits,												"stopbits");
_LIT(KParity,												"parity");
_LIT(KParamMask,											"mask");
	
//Commands
_LIT(KCmdTRfcommRemotePortParams,							"TRfcommRemotePortParams");
_LIT(KCmdIsValid,											"IsValid");

_LIT(KCmdGetXOffChar,										"GetXOffChar");
_LIT(KCmdSetXOffChar,										"SetXOffChar");

_LIT(KCmdGetXOnChar,										"GetXOnChar");
_LIT(KCmdSetXOnChar,										"SetXOnChar");

_LIT(KCmdGetFlowCtrl,										"GetFlowCtrl");
_LIT(KCmdSetFlowCtrl,										"SetFlowCtrl");

_LIT(KCmdGetBitRate,										"GetBitRate");
_LIT(KCmdSetBitRate,										"SetBitRate");


_LIT(KCmdGetDataBits,										"GetDataBits");
_LIT(KCmdSetDataBits,										"SetDataBits");

_LIT(KCmdGetStopBit,										"GetStopBit");
_LIT(KCmdSetStopBit,										"SetStopBit");

_LIT(KCmdGetParity,											"GetParity");
_LIT(KCmdSetParity,											"SetParity");

_LIT(KCmdUpdateFlowCtrlBit,									"UpdateFlowCtrlBit");
_LIT(KCmdUpdateWholeFlowCtrl,								"UpdateWholeFlowCtrl");
_LIT(KCmdUpdateFromRPNTransaction,							"UpdateFromRPNTransaction");

//	Logging
_LIT(KLogMissingParameters,			"Missing parameters '%S' or '%S'");
_LIT(KLogMissingParameter,			"Missing parameter '%S'");
/*@}*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_RfcommRemotePortParamsData* CT_RfcommRemotePortParamsData::NewL()
	{
	CT_RfcommRemotePortParamsData*	ret=new (ELeave) CT_RfcommRemotePortParamsData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


CT_RfcommRemotePortParamsData::CT_RfcommRemotePortParamsData()
:	iData(NULL)
	{
	}


void CT_RfcommRemotePortParamsData::ConstructL()
	{
	}


CT_RfcommRemotePortParamsData::~CT_RfcommRemotePortParamsData()
	{
	DestroyData();
	}

TAny* CT_RfcommRemotePortParamsData::GetObject()
	{
	return iData;
	}

void CT_RfcommRemotePortParamsData::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iData = static_cast<TRfcommRemotePortParams*> (aAny);
	}


void CT_RfcommRemotePortParamsData::DisownObjectL()
	{
	iData = NULL;
	}


void CT_RfcommRemotePortParamsData::DestroyData()
	{
	delete iData;
	iData=NULL;	
	}

inline TCleanupOperation CT_RfcommRemotePortParamsData::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_RfcommRemotePortParamsData::CleanupOperation(TAny* aAny)
	{
	//may be unnecessary
	TRfcommRemotePortParams* arg=static_cast<TRfcommRemotePortParams*>(aAny);
	delete arg;
	}

TBool CT_RfcommRemotePortParamsData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;

	if ( aCommand==KCmdTRfcommRemotePortParams )
		{
		DoCmdTRfcommRemotePortParams();
		}
	else if ( aCommand==KCmdIsValid )
		{
		DoCmdIsValid(aSection);
		}
	else if ( aCommand==KCmdGetDataBits)
		{
		DoCmdGetDataBits(aSection);
		}
	else if ( aCommand==KCmdSetDataBits)
		{
		DoCmdSetDataBits(aSection);
		}
	else if ( aCommand==KCmdSetXOffChar )
		{
		DoCmdSetXOffChar(aSection);
		}
	else if ( aCommand==KCmdSetXOnChar )
		{
		DoCmdSetXOnChar(aSection);
		}
	else if ( aCommand==KCmdGetXOnChar )
		{
		DoCmdGetXOnChar(aSection);
		}
	else if ( aCommand==KCmdGetXOffChar )
		{
		DoCmdGetXOffChar(aSection);
		}
	else if ( aCommand==KCmdGetBitRate )
		{
		DoCmdGetBitRate(aSection);
		}
	else if ( aCommand==KCmdSetBitRate )
		{
		DoCmdSetBitRate(aSection);
		}
	else if ( aCommand==KCmdGetFlowCtrl )
		{
		DoCmdGetFlowCtrl(aSection);
		}			
	else if ( aCommand==KCmdSetFlowCtrl )
		{
		DoCmdSetFlowCtrl(aSection);
		}
	else if ( aCommand==KCmdGetStopBit)
		{
		DoCmdGetStopBit(aSection);
		}
	else if ( aCommand==KCmdSetStopBit)
		{
		DoCmdSetStopBit(aSection);
		}
	else if ( aCommand==KCmdGetParity)
		{
		DoCmdGetParity(aSection);
		}
	else if ( aCommand==KCmdSetParity)
		{
		DoCmdSetParity(aSection);
		}		
	else if ( aCommand==KCmdUpdateFlowCtrlBit )
		{
		DoCmdUpdateFlowCtrlBit(aSection);
		}
	else if ( aCommand==KCmdUpdateWholeFlowCtrl )
		{
		DoCmdUpdateWholeFlowCtrl(aSection);
		}
	else if ( aCommand==KCmdUpdateFromRPNTransaction )
		{
		DoCmdUpdateFromRPNTransaction(aSection);
		}
	else
		{
		ret=EFalse;
		}
	return ret;
	}


/**
Test TRfcommRemotePortParams()
*/
void CT_RfcommRemotePortParamsData::DoCmdTRfcommRemotePortParams()
	{
	DestroyData();
	INFO_PRINTF1(_L("TRfcommRemotePortParams Constructor Call"));
	iData = new (ELeave) TRfcommRemotePortParams();
	
	if ( iData == NULL ) 
		{
		ERR_PRINTF1(_L("TRfcommRemotePortParams is NULL"));
		SetBlockResult(EFail);
		}
	}


/**
Test IsValid()
*/
void CT_RfcommRemotePortParamsData::DoCmdIsValid(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("TRfcommRemotePortParams IsValid Call"));
	TUint8 result = iData->IsValid();
	INFO_PRINTF2(_L("IsValid result (%d)"), result);
	
	TInt expected = 0;
	if(GetIntFromConfig(aSection, KExpected(), expected) )
		{
		if ( result != expected )
			{
			ERR_PRINTF3(_L("Return value (%d) is not as expected (%d)"), result, expected);
			SetBlockResult(EFail);
			}	
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KExpected());
		SetBlockResult(EFail);
		}
	}


/**
Test SetXOffChar()
*/
void CT_RfcommRemotePortParamsData::DoCmdSetXOffChar(const TDesC& aSection)
	{
	TInt xOffCharValue = 0;
	
	if(GetIntFromConfig(aSection, KXoffChar(), xOffCharValue) )
		{
		TUint8	xOffChar = xOffCharValue;
		
		INFO_PRINTF1(_L("TRfcommRemotePortParams SetXOffChar Call"));	
		TInt err = iData->SetXOffChar(xOffChar);
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("SetXOffChar failed with value (%d)"), err );
			SetError(err);		
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KXoffChar());
		SetBlockResult(EFail);
		}
	}


/**
Test GetXOffChar()
*/
void CT_RfcommRemotePortParamsData::DoCmdGetXOffChar(const TDesC& aSection)
	{
	TBool expected = EFalse;
	TBool param = EFalse;
	TInt xOffCharValue = 0;
	TUint8 exp = 0;
	TUint8	xOffChar = 0;
    
	INFO_PRINTF1(_L("TRfcommRemotePortParams GetXOffChar Call"));	
	TBool	result = iData->GetXOffChar(xOffChar);
	INFO_PRINTF3(_L("Return result (%d) and XOff char on return (%d)"), result, xOffChar);
    
	if(GetBoolFromConfig(aSection, KExpected(), expected) )
		{
		param = ETrue;
		if ( result != expected )
			{
			ERR_PRINTF3(_L("Result (%d) is not as expected (%d)"), result, expected);
			SetBlockResult(EFail);			
			}
		else 
			{
			if ( result )
				{
				INFO_PRINTF1(_L("Verify that aXOffChar has been set with the expected value"));
				if(GetIntFromConfig(aSection, KXoffChar(), xOffCharValue) )
					{
					param = ETrue;
					exp = xOffCharValue;
				
					if ( xOffChar != exp )
						{
						ERR_PRINTF3(_L("aXOffChar (%d) is not as expected (%d)"), xOffChar, exp);
						SetBlockResult(EFail);
						}
					}
				}
			}
		}
	if ( !param )
		{
		ERR_PRINTF3(KLogMissingParameters, &KExpected(), &KXoffChar());
        SetBlockResult(EFail);
		}
	}


/**
Test SetXOnChar()
*/
void CT_RfcommRemotePortParamsData::DoCmdSetXOnChar(const TDesC& aSection)
	{
	TInt xOnCharValue = 0;
	
	if(GetIntFromConfig(aSection, KXonChar(), xOnCharValue) )
		{
		TUint8	xOnChar = xOnCharValue;
		INFO_PRINTF1(_L("TRfcommRemotePortParams SetXOnChar Call"));
		TInt err = iData->SetXOnChar(xOnChar);
		if ( err != KErrNone )
            {
			ERR_PRINTF2(_L("SetXOnChar failed with value (%d)"), err);
			SetError(err);		
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KXonChar());
		SetBlockResult(EFail);
		}
	}


/**
Test GetXOnChar()
*/
void CT_RfcommRemotePortParamsData::DoCmdGetXOnChar(const TDesC& aSection)
	{
	TBool expected = EFalse;
	TBool param = EFalse;
    TInt xOnCharValue = 0;
	TUint8 exp = 0;
	TUint8	xOnChar = 0;
    
    INFO_PRINTF1(_L("TRfcommRemotePortParams GetXOnChar Call"));
	TBool result = iData->GetXOnChar(xOnChar);
    INFO_PRINTF3(_L("Return result (%d) and XON char on return (%d)"), result, xOnChar);
    
	if(GetBoolFromConfig(aSection, KExpected(), expected) )
		{
		param = ETrue;
		if ( result != expected )
			{
			ERR_PRINTF3(_L("Result (%d) is not as expected (%d)"), result, expected);
			SetBlockResult(EFail);
			}
		else 
			{
			if ( result )
				{
				INFO_PRINTF1(_L("Verify that aXON has been set with the expected value"));
				if(GetIntFromConfig(aSection, KXonChar(), xOnCharValue) )
					{
					param = ETrue;
					exp = xOnCharValue;
				
					if ( xOnChar != exp )
						{
						ERR_PRINTF3(_L("aXON (%d) is not as expected (%d)"), xOnChar, exp);
						SetBlockResult(EFail);
						}
					}
				}
			}
		}
	if ( !param )
		{
		ERR_PRINTF3(KLogMissingParameters, &KExpected(), &KXonChar());
        SetBlockResult(EFail);
		}	
	}


/**
Test SetFlowCtrl()
*/
void CT_RfcommRemotePortParamsData::DoCmdSetFlowCtrl(const TDesC& aSection)
	{
	TInt flowCtrlValue = 0;
	
	if(GetIntFromConfig(aSection, KFlowCtrl(), flowCtrlValue) )
		{
		TUint8	flowCtrl = flowCtrlValue;
		INFO_PRINTF1(_L("TRfcommRemotePortParams SetFlowCtrl Call"));
		TInt err = iData->SetFlowCtrl(flowCtrl);
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("SetFlowCtrl failed with value  (%d)"), err);
			SetBlockResult(EFail);	
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KFlowCtrl());
		SetBlockResult(EFail);
		}
	}


/**
Test GetFlowCtrl()
*/
void CT_RfcommRemotePortParamsData::DoCmdGetFlowCtrl(const TDesC& aSection)
	{
	TBool expected = EFalse;
	TBool param = EFalse;
	TInt	flowCtrlValue = 0;
	TUint8	exp = 0;
    TUint8	flowCtrl = 0;

	INFO_PRINTF1(_L("TRfcommRemotePortParams GetFlowCtrl Call"));
	TBool result = iData->GetFlowCtrl(flowCtrl);
	INFO_PRINTF3(_L("Return result (%d) and flow control on return (%d)"), result, flowCtrl);

	if(GetBoolFromConfig(aSection, KExpected(), expected) )
		{
		param = ETrue;
		if ( result != expected )
			{
			ERR_PRINTF3(_L("Result (%d) is not as expected (%d)"), result, expected);
			SetBlockResult(EFail);
			}
		else 
			{
			if ( result )
				{
				INFO_PRINTF1(_L("Verify that aFlowCtrl has been set with the expected value"));
				if(GetIntFromConfig(aSection, KFlowCtrl(), flowCtrlValue) )
					{
					param = ETrue;
					exp = flowCtrlValue;
				
					if ( flowCtrl != exp )
						{
						ERR_PRINTF3(_L("aFlowCtrl (%d) is not as expected (%d)"), flowCtrl, exp);
						SetBlockResult(EFail);
						}
						
					}
				}
			}
		}
	if ( !param )
		{
		ERR_PRINTF3(KLogMissingParameters, &KExpected(), &KFlowCtrl());
        SetBlockResult(EFail);
		}
	}


/**
Test SetBitRate()
*/
void CT_RfcommRemotePortParamsData::DoCmdSetBitRate(const TDesC& aSection)
	{
	TBps bps;
	CT_BTUtil::GetBitValue(*this, bps, aSection, KBitRate());
	INFO_PRINTF1(_L("TRfcommRemotePortParams SetBitRate Call"));
	TInt err = iData->SetBitRate( bps );
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("SetBitRate failed with value (%d)"), err);
		SetError(err);
		}
	}


/**
Test GetBitRate()
*/
void CT_RfcommRemotePortParamsData::DoCmdGetBitRate(const TDesC& aSection)
	{
	TBps bps = EBpsSpecial;
	TBps exp = EBpsSpecial;
	TBool expected = EFalse;

	INFO_PRINTF1(_L("TRfcommRemotePortParams GetBitRate Call"));
	TBool result = iData->GetBitRate( bps );
	INFO_PRINTF3(_L("GetBitRate result (%d) and bit rate is (%d)"), result, bps);
	
	if(GetBoolFromConfig(aSection, KExpected(), expected) )
		{
		if ( result != expected )
			{
			ERR_PRINTF3(_L("Result (%d) is not as expected (%d)"), result, expected);
			SetBlockResult(EFail);	
			}
		else
			{
			if ( result )
				{
				INFO_PRINTF1(_L("Verify that aBitRate has been set with the expected value"));
				CT_BTUtil::GetBitValue(*this, exp, aSection, KBitRate());
				if ( bps != exp )
					{	
					ERR_PRINTF3(_L("aBitRate (%d) is not as expected (%d)"), bps, exp);
					SetBlockResult(EFail);
					}
				}
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KExpected());
		SetBlockResult(EFail);
		}	
	}

	
/**
Test SetDataBits()
*/
void CT_RfcommRemotePortParamsData::DoCmdSetDataBits(const TDesC& aSection)
	{
	
	TDataBits dataBits;
    CT_BTUtil::GetDataBits(*this, dataBits, aSection, KDataBit());
	INFO_PRINTF1(_L("TRfcommRemotePortParams SetDataBits Call"));
	TInt err = iData->SetDataBits( dataBits );
    INFO_PRINTF2(_L("SetDataBits result (%d)"), err);
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("SetDataBits return result (%d)"), err);
		SetError(err);
		}
	}
	

/**
Test GetDataBits()
*/
void CT_RfcommRemotePortParamsData::DoCmdGetDataBits(const TDesC& aSection)
	{
	TBool expected = EFalse;
    TDataBits dataBits = EData5;
	TDataBits exp = EData5;
	
	INFO_PRINTF1(_L("TRfcommRemotePortParams GetDataBits Call"));
	TBool result = iData->GetDataBits( dataBits );
    INFO_PRINTF3(_L("GetDataBits return (%d) and data bits (%d)"), result, dataBits);
    
	if(GetBoolFromConfig(aSection, KExpected(), expected) )
		{
		if ( result != expected )
			{
			ERR_PRINTF3(_L("Result (%d) is not as expected (%d)"), result, expected);
			SetBlockResult(EFail);
			}
		else
			{
			if ( result )
				{
				INFO_PRINTF1(_L("Verify that aDataBits has been set with the expected value"));
				CT_BTUtil::GetDataBits(*this, exp, aSection, KDataBit());
				if ( dataBits != exp )
					{
					ERR_PRINTF3(_L("aDataBits (%d) is not as expected (%d)"), dataBits, exp);
					SetBlockResult(EFail);
					}
				}
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KExpected());
		SetBlockResult(EFail);
		}
	}
	

/**
Test SetParity()
*/
void CT_RfcommRemotePortParamsData::DoCmdSetParity(const TDesC& aSection)
	{
	TParity parity;
	CT_BTUtil::GetParity(*this, parity, aSection, KParity());
	INFO_PRINTF1(_L("TRfcommRemotePortParams SetParity Call"));
	TInt err = iData->SetParity( parity );
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("SetParity failed with value (%d)"), err);
		SetError(err);
		}
	}


/**
Test GetParity()
*/
void CT_RfcommRemotePortParamsData::DoCmdGetParity(const TDesC& aSection)
	{
	TBool expected = EFalse;
	TParity parity = EParityNone;
	TParity exp = EParityNone;
	
	INFO_PRINTF1(_L("TRfcommRemotePortParams GetParity Call"));
	TBool result = iData->GetParity( parity );
	INFO_PRINTF3(_L("GetParity result (%d) and parity on return (%d)"), result, parity);
	
	if(GetBoolFromConfig(aSection, KExpected(), expected) )
		{
		if ( result != expected )
			{
			ERR_PRINTF3(_L("Result (%d) is not as expected (%d)"), result, expected);
			SetBlockResult(EFail);
			}
		else
			{
			if ( result )
				{
				INFO_PRINTF1(_L("Verify that aParity has been set with the expected value"));
    			CT_BTUtil::GetParity(*this, exp, aSection, KParity());
				if ( parity != exp )
					{
					ERR_PRINTF3(_L("aParity (%d) is not as expected (%d)"), parity, exp);
					SetBlockResult(EFail);
					}
				}
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KExpected());
		SetBlockResult(EFail);
		}
	}
	

/**
Test SetStopBit()
*/
void CT_RfcommRemotePortParamsData::DoCmdSetStopBit(const TDesC& aSection)
	{
	TStopBits stopBits;
	CT_BTUtil::GetStopBits(*this, stopBits, aSection, KStopBits());
	INFO_PRINTF1(_L("TRfcommRemotePortParams SetStopBit Call"));
	TInt err = iData->SetStopBit( stopBits );
	if ( err != KErrNone )
		{
		ERR_PRINTF2(_L("SetStopBit failed with value (%d)"), err);
		SetError(err);
		}
	}


/**
Test GetStopBit()
*/
void CT_RfcommRemotePortParamsData::DoCmdGetStopBit(const TDesC& aSection)
	{
	TBool expected = EFalse;
	TStopBits stopBits = EStop1;
	TStopBits exp = EStop1;
	
	INFO_PRINTF1(_L("TRfcommRemotePortParams GetStopBit Call"));
	TBool result = iData->GetStopBit( stopBits );
	INFO_PRINTF3(_L("GetStopBit result (%d) and stop bit (%d)"), result, stopBits);

	if(GetBoolFromConfig(aSection, KExpected(), expected))
		{
		if ( result != expected )
			{
			ERR_PRINTF3(_L("Result (%d) is not as expected (%d)"), result, expected);
			SetBlockResult(EFail);
			}
		else
			{
			if ( result )
				{
				INFO_PRINTF1(_L("Verify that aStopBit has been set with the expected value"));
				CT_BTUtil::GetStopBits(*this, exp, aSection, KStopBits());
				if ( stopBits != exp )
					{
					ERR_PRINTF3(_L("aStopBit (%d) is not as expected (%d)"), stopBits, exp);
					SetBlockResult(EFail);
					}
				}
			}
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KExpected());
		SetBlockResult(EFail);
		}
	}


/**
Test UpdateWholeFlowCtrl()
*/
void CT_RfcommRemotePortParamsData::DoCmdUpdateWholeFlowCtrl(const TDesC& aSection)
	{
	TInt dataInput = 0;
	TBool parameters = EFalse;
	if(GetIntFromConfig(aSection, KParamMask(), dataInput) )
		{
		parameters = ETrue;
		TUint16	paramMask=dataInput;
		if(GetIntFromConfig(aSection, KFlowCtrl(), dataInput) )
			{
			parameters = ETrue;
			TUint8	flowCtrl=dataInput;
			INFO_PRINTF1(_L("TRfcommRemotePortParams UpdateWholeFlowCtrl Call"));
			iData->UpdateWholeFlowCtrl( paramMask, flowCtrl);
			}
		}
	if ( parameters == EFalse )
		{
		ERR_PRINTF3(KLogMissingParameters, &KParamMask(), &KFlowCtrl());
		SetBlockResult(EFail);
		}
	}


/**
Test UpdateFlowCtrlBit()
*/
void CT_RfcommRemotePortParamsData::DoCmdUpdateFlowCtrlBit(const TDesC& aSection)
	{
	TInt	param = 0;
	
	if(GetIntFromConfig(aSection, KFlowCtrl(), param) )
		{
		TUint8	paramMask=param;
		
		TRPNFlowCtrlMask flowMask;
		CT_BTUtil::GetTRPNFlowCtrlMask(*this, flowMask, aSection, KFlowCtrlMask());
		INFO_PRINTF1(_L("TRfcommRemotePortParams UpdateFlowCtrlBit Call"));
		iData->UpdateFlowCtrlBit( paramMask, flowMask );
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KFlowCtrl());
		SetBlockResult(EFail);
		}
	}


/**
Test UpdateFromRPNTransaction()
*/
void CT_RfcommRemotePortParamsData::DoCmdUpdateFromRPNTransaction(const TDesC& aSection)
	{
	TPtrC	object;
	
	if(GetStringFromConfig(aSection, KPRNTobject(), object) )
		{
		TRfcommRPNTransaction* thePRNTrascationData=static_cast<TRfcommRPNTransaction*>(GetDataObjectL(object));
		
		INFO_PRINTF2(_L("TRfcommRPNTransaction.iParamMask (%d)"), thePRNTrascationData->iParamMask );
		
		INFO_PRINTF1(_L("TRfcommRemotePortParams UpdateFromRPNTransaction Call"));
		iData->UpdateFromRPNTransaction(*thePRNTrascationData);
		}
	else
		{
		ERR_PRINTF2(KLogMissingParameter, &KPRNTobject());
		SetBlockResult(EFail);
		}
	}
