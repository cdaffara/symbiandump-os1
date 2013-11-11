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

#include "T_DataSdpAttrValueUUID.h"
#include "T_BTUtil.h"
#include <e32math.h>


/*@{*/

//Parameters
_LIT(KInputInt,         "inputInt");
_LIT(KInputIntLL,       "inputIntLL");
_LIT(KInputIntLH,       "inputIntLH");
_LIT(KInputIntHL,       "inputIntHL");
_LIT(KInputIntHH,       "inputIntHH");
_LIT(KExpected,			"expected");
_LIT(KExpectedLL,		"expectedLL");
_LIT(KExpectedLH,		"expectedLH");
_LIT(KExpectedHL,		"expectedHL");
_LIT(KExpectedHH,		"expectedHH");
//Commands
_LIT(KCmdNewUUIDL,		"NewUUIDL");
_LIT(KCmdDes,			"Des");
_LIT(KCmdDestructor,	"~");
//buffer index
const TInt  KOneByteValue =256;
const TInt  KMinRangeHH = 0;
const TInt  KMaxRangeHH = 3;
const TInt	KMinRangeHL = 4;
const TInt	KMaxRangeHL = 7;
const TInt	KMinRangeLH = 8;
const TInt	KMaxRangeLH = 11;
const TInt	KMinRangeLL = 12;
const TInt	KMaxRangeLL = 15;

/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataSdpAttrValueUUID* CT_DataSdpAttrValueUUID::NewL()
	{
	CT_DataSdpAttrValueUUID*	ret=new (ELeave) CT_DataSdpAttrValueUUID();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataSdpAttrValueUUID::CT_DataSdpAttrValueUUID()
:   iAttrValUUID(NULL)
	{
	}

void CT_DataSdpAttrValueUUID::ConstructL()
	{
	}

CSdpAttrValue* CT_DataSdpAttrValueUUID::GetSdpAttrValue() const
	{
	return iAttrValUUID;
	}

CT_DataSdpAttrValueUUID::~CT_DataSdpAttrValueUUID()
	{
	DestroyData();
	}

void CT_DataSdpAttrValueUUID::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iAttrValUUID = static_cast<CSdpAttrValueUUID*> (aAny);
	}

void CT_DataSdpAttrValueUUID::DisownObjectL()
	{
	iAttrValUUID = NULL;
	}

void CT_DataSdpAttrValueUUID::DestroyData()
	{
	delete iAttrValUUID;
	iAttrValUUID=NULL;
	}

inline TCleanupOperation CT_DataSdpAttrValueUUID::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataSdpAttrValueUUID::CleanupOperation(TAny* aAny)
	{
	CSdpAttrValueUUID* arg=static_cast<CSdpAttrValueUUID*>(aAny);
	delete arg;
	}

/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 * @leave					System wide error
 */
TBool CT_DataSdpAttrValueUUID::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
{
	TBool	ret=ETrue;

	if ( aCommand==KCmdNewUUIDL )
    	{
		DoCmdNewUUIDL(aSection);
    	}
	else if ( aCommand==KCmdDes )
    	{
		DoCmdDes(aSection);
	    }
	else if ( aCommand==KCmdDestructor )
	    {
		DoCmdDestructor();
	    }
	else
	    {
		ret=CT_DataSdpAttrValue::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
	    }
   
	return ret;
}

void CT_DataSdpAttrValueUUID::DoCmdNewUUIDL(const TDesC& aSection)
	{
	TInt  inputInt;
	TInt  inputIntLL;
	TInt  inputIntLH;
	TInt  inputIntHL;
	TInt  inputIntHH;
	TBool ifInputExit = EFalse;
	TBool ifInputHHExit = EFalse;
	TBool ifInputHLExit = EFalse;
	TBool ifInputLHExit = EFalse;
	TBool ifInputLLExit = EFalse;
	
	
	DestroyData();
	
	if ( GetIntFromConfig(aSection, KInputInt(), inputInt) )
		{
		ifInputExit = ETrue;
		}
	else //to get four input values for 128-bit TUUID object
		{

		if ( GetIntFromConfig(aSection, KInputIntHH(), inputIntHH))
			{
			ifInputHHExit = ETrue;
			}
		else
			{
			ERR_PRINTF2(_L("Missing parameter %S"), &KInputIntHH());
			SetBlockResult(EFail);
			}
			
		if ( GetIntFromConfig(aSection, KInputIntHL(), inputIntHL))
			{
			ifInputHLExit = ETrue;
			}
		else
			{
			ERR_PRINTF2(_L("Missing parameter %S"), &KInputIntHL());
			SetBlockResult(EFail);
			}

		if ( GetIntFromConfig(aSection, KInputIntLH(), inputIntLH))
			{
			ifInputLHExit = ETrue;
			}
		else
			{
			ERR_PRINTF2(_L("Missing parameter %S"), &KInputIntLH());
			SetBlockResult(EFail);
			}
			
		if ( GetIntFromConfig(aSection, KInputIntLL(), inputIntLL))
			{
			ifInputLLExit = ETrue;
			}
		else
			{
			ERR_PRINTF2(_L("Missing parameter %S"), &KInputIntLL());
			SetBlockResult(EFail);
			}
		}	


	if ( ifInputExit )
		{
		
		TRAPD(err, iAttrValUUID = CSdpAttrValueUUID::NewUUIDL(inputInt));
		
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("CSdpAttrValueUUID::NewUUIDL failed with error %d"), err);
			SetError(err);
			}	
		}
	else if( ifInputHHExit && ifInputHLExit && ifInputLHExit && ifInputLLExit)
		{
		INFO_PRINTF2(_L("NewUUIDL Highest order word (HH bits 96 - 127): %d"), inputIntHH);
		INFO_PRINTF2(_L("NewUUIDL Second highest order word  (HL bits 64 - 95): %d"), inputIntHL);
		INFO_PRINTF2(_L("NewUUIDL Second lowest order word  (LH bits 32 - 63): %d"), inputIntLH);
		INFO_PRINTF2(_L("NewUUIDL Low order word (LL bits 0 - 31): %d"), inputIntLL);
		
		TUUID objUUID(inputIntHH,inputIntHL,inputIntLH,inputIntLL);
		TRAPD(err, iAttrValUUID = CSdpAttrValueUUID::NewUUIDL(objUUID));
		
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("CSdpAttrValueUUID::NewUUIDL failed with error %d"), err);
			SetError(err);
			}	
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KInputIntLL());
		SetBlockResult(EFail);
		}
	}
	
void CT_DataSdpAttrValueUUID::DoCmdDes(const TDesC& aSection)
	{
	TInt  inputIntLL = 0;
	TInt  inputIntLH = 0;
	TInt  inputIntHL = 0;
	TInt  inputIntHH = 0;
	TInt  expected;
	TInt  expectedLL;
	TInt  expectedLH;
	TInt  expectedHL;
	TInt  expectedHH;
	TInt  sumValue = 0;
	TInt  curPowervalue;
	TInt  curPowervalueHH = 0;
	TInt  curPowervalueHL = 0;
	TInt  curPowervalueLH = 0;
	TInt  curPowervalueLL = 0;
	TUUID actualUUid = NULL;

	TBool ifExpectedExit = EFalse;
	TBool ifExpectedHHExit = EFalse;
	TBool ifExpectedHLExit = EFalse;
	TBool ifExpectedLHExit = EFalse;
	TBool ifExpectedLLExit = EFalse;
	TReal64	powValue;
	
	TPtrC8	actualDesUUid=GetSdpAttrValue()->Des();
	HBufC*	buffer=HBufC::NewLC(actualDesUUid.Length());
	TPtr	bufferPtr=buffer->Des();
	bufferPtr.Copy(actualDesUUid);
	
		
	
	if ( GetIntFromConfig(aSection, KExpected(), expected))
		{
		ifExpectedExit = ETrue;
		}
	else //to get four input values for 128-bit TUUID object
		{
		
		if ( GetIntFromConfig(aSection, KExpectedHH(), expectedHH))
			{
			ifExpectedHHExit = ETrue;
			}
		else
			{
			ERR_PRINTF2(_L("Missing expected value %S"), &KExpectedHH());
			SetBlockResult(EFail);
			}

		if ( GetIntFromConfig(aSection, KExpectedHL(), expectedHL))
			{
			ifExpectedHLExit = ETrue;
			}
		else
			{
			ERR_PRINTF2(_L("Missing expected value %S"), &KExpectedHL());
			SetBlockResult(EFail);
			}

		if ( GetIntFromConfig(aSection, KExpectedLH(), expectedLH))
			{
			ifExpectedLHExit = ETrue;
			}
		else
			{
			ERR_PRINTF2(_L("Missing expected value %S"), &KExpectedLH());
			SetBlockResult(EFail);
			}
			
		if ( GetIntFromConfig(aSection, KExpectedLL(), expectedLL))
			{
			ifExpectedLLExit = ETrue;
			}
		else
			{
			ERR_PRINTF2(_L("Missing expected value %S"), &KExpectedLL());
			SetBlockResult(EFail);
			}	
		}
	
	
	if( ifExpectedExit )
		{
		for(TInt numRange = actualDesUUid.Length() -1; numRange >= 0; numRange--)
			{
			curPowervalue=Abs(numRange-(actualDesUUid.Length()-1));
			Math::Pow(powValue,TReal(KOneByteValue),TReal(curPowervalue));
			sumValue +=buffer[0][numRange]*powValue;
			}
		
		if ( sumValue!=expected )
			{
			ERR_PRINTF1(_L("Des is not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else if(ifExpectedHHExit || ifExpectedHLExit || ifExpectedLHExit || ifExpectedLLExit)
		{
		for(TInt numRange = actualDesUUid.Length() -1; numRange >= 0; numRange--)
			{
			if( numRange >=KMinRangeLL && numRange <=KMaxRangeLL )
				{
				Math::Pow(powValue,TReal(KOneByteValue),TReal(curPowervalueLL));
				curPowervalueLL++;
				inputIntLL +=buffer[0][numRange]*powValue;
				}
			else if( numRange >=KMinRangeLH && numRange <=KMaxRangeLH )
				{
				Math::Pow(powValue,TReal(KOneByteValue),TReal(curPowervalueLH));
				curPowervalueLH++;
				inputIntLH +=buffer[0][numRange]*powValue;
				}
			else if( numRange >=KMinRangeHL && numRange <=KMaxRangeHL )
				{
				Math::Pow(powValue,TReal(KOneByteValue),TReal(curPowervalueHL));
				curPowervalueHL++;
				inputIntHL +=buffer[0][numRange]*powValue;
				}
			else if( numRange <=KMaxRangeHH && numRange >=KMinRangeHH )
				{
				Math::Pow(powValue,TReal(KOneByteValue),TReal(curPowervalueHH));
				curPowervalueHH++;
				inputIntHH +=buffer[0][numRange]*powValue;
				}
			}
			INFO_PRINTF2(_L("expected Highest order word (HH bits 96 - 127): %d"), expectedHH);
			INFO_PRINTF2(_L("expected Second highest order word  (HL bits 64 - 95): %d"), expectedHL);
			INFO_PRINTF2(_L("expected Second lowest order word  (LH bits 32 - 63): %d"), expectedLH);
			INFO_PRINTF2(_L("expected Low order word (LL bits 0 - 31): %d"), expectedLL);
		//to compare four values with input	
		if ( inputIntHH!=expectedHH || inputIntHL!=expectedHL || inputIntLH!=expectedLH || inputIntLL!=expectedLL )
			{
			ERR_PRINTF1(_L("Des is not as expected!"));
			SetBlockResult(EFail);
			}	
		}
	
	CleanupStack::PopAndDestroy(buffer);
	}

void CT_DataSdpAttrValueUUID::DoCmdDestructor()
	{
	DestroyData();
	}
