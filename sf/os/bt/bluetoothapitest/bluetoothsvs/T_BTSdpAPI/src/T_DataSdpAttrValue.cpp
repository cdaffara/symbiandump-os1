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

#include "T_DataSdpAttrValue.h"
#include "T_BTUtil.h"

/*@{*/
//Parameters
_LIT(KExpected,						"expected");
_LIT(KExpectedRes,					"expectedVisitorResult");
_LIT(KExpectedLL,					"expectedLL");
_LIT(KExpectedLH,					"expectedLH");
_LIT(KExpectedHL,					"expectedHL");
_LIT(KExpectedHH,					"expectedHH");

//Commands
_LIT(KCmdAcceptVisitorL,			"AcceptVisitorL");
_LIT(KCmdBool,						"Bool");
_LIT(KCmdDataSize,					"DataSize");
_LIT(KCmdDes,						"Des");
_LIT(KCmdDoesIntFit,				"DoesIntFit");
_LIT(KCmdInt,						"Int");
_LIT(KCmdType,						"Type");
_LIT(KCmdUint,						"Uint");
_LIT(KCmdUUID,						"UUID");
_LIT(KCmdMSAVV_ExtensionInterfaceL,	"MSAVV_ExtensionInterfaceL");
/*@}*/

const TInt KAttrValueStringBufLen = 256;
const TInt KVisitorResLen = 512;
const TInt KMaxDataSize = 4;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataSdpAttrValue::CT_DataSdpAttrValue()
:	iVisitorResult(new TUint16[KVisitorResLen],KVisitorResLen)
	{
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
TBool CT_DataSdpAttrValue::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret = ETrue;

	if ( aCommand==KCmdAcceptVisitorL )
		{
		DoCmdAcceptVisitor(aSection);
		}
	else if ( aCommand==KCmdBool )
		{
		DoCmdBool(aSection);
		}
	else if ( aCommand==KCmdDataSize )
		{
		DoCmdDataSize(aSection);
		}
	else if ( aCommand==KCmdDes )
		{
		DoCmdDes(aSection);
		}
	else if ( aCommand==KCmdDoesIntFit )
		{
		DoCmdDoesIntFit(aSection);
		}
	else if ( aCommand==KCmdInt )
		{
		DoCmdInt(aSection);
		}
	else if ( aCommand==KCmdType )
		{
		DoCmdType(aSection);
		}
	else if ( aCommand==KCmdUint )
		{
		DoCmdUint(aSection);
		}
	else if ( aCommand==KCmdUUID )
		{
		DoCmdUUID(aSection);
		}
	else if ( aCommand==KCmdMSAVV_ExtensionInterfaceL )
		{
		DoCmdMSAVV_ExtensionInterfaceL();
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

void CT_DataSdpAttrValue::DoCmdAcceptVisitor(const TDesC& aSection)
	{
	TRAPD(err, GetSdpAttrValue()->AcceptVisitorL(*this));
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("CSdpAttrValue::AcceptVisitorL failed with error %d"), err);
		SetError(err);
		return;
		}

	TPtrC expected;
	if(GetStringFromConfig(aSection, KExpectedRes(), expected))
		{
		TPtr actual = GetVisitorResult();	
		INFO_PRINTF1(_L("Check result. Actual value:"));
		INFO_PRINTF1(actual);				
		if(actual != expected)
			{
        	INFO_PRINTF1(_L("CSdpAttrValue::AcceptVisitorL, Result is not as expected !"));
			SetBlockResult(EFail);
			}
		else
			{
        	INFO_PRINTF1(_L("CSdpAttrValue::AcceptVisitorL, Result is correct !"));
			}	
		}	
	else
		{
  		ERR_PRINTF2(_L("Missing expected value %S"), &KExpectedRes());
  		SetBlockResult(EFail);		
		}
	}

void CT_DataSdpAttrValue::DoCmdBool(const TDesC& aSection)
	{
	TBool	actual = GetSdpAttrValue()->Bool();
	INFO_PRINTF2(_L("CSdpAttrValue::Bool = %d"), actual);

	TBool	expected;
	if ( GetBoolFromConfig(aSection, KExpected(), expected) )
		{
		if ( actual!=expected )
			{
			ERR_PRINTF1(_L("Bool is not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
  		ERR_PRINTF2(_L("Missing expected value %S"), &KExpected());
  		SetBlockResult(EFail);		
		}
	}

void CT_DataSdpAttrValue::DoCmdDataSize(const TDesC& aSection)
	{
	TUint	actual = GetSdpAttrValue()->DataSize();
	INFO_PRINTF2(_L("CSdpAttrValue::Size = %d"), actual);

	TInt	expected;
	if (GetIntFromConfig(aSection, KExpected(), expected))
		{
		if(actual!=expected )
			{
			ERR_PRINTF1(_L("DataSize is not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
  		ERR_PRINTF2(_L("Missing expected value %S"), &KExpected());
  		SetBlockResult(EFail);		
		}
	}

void CT_DataSdpAttrValue::DoCmdDes(const TDesC& aSection)
	{
	TPtrC8	actual=GetSdpAttrValue()->Des();
	HBufC*	buffer=HBufC::NewLC(actual.Length());
	TPtr	bufferPtr=buffer->Des();
	bufferPtr.Copy(actual);
	INFO_PRINTF1(_L("CSdpAttrValue::Des result:"));
	INFO_PRINTF1(bufferPtr);

	TPtrC	expected;
	if ( GetStringFromConfig(aSection, KExpected(), expected) )
		{
		if ( bufferPtr!=expected )
			{
			ERR_PRINTF1(_L("Des is not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
  		ERR_PRINTF2(_L("Missing expected value %S"), &KExpected());
  		SetBlockResult(EFail);		
		}
	CleanupStack::PopAndDestroy(buffer);
	}

void CT_DataSdpAttrValue::DoCmdDoesIntFit(const TDesC& aSection)
	{
	TBool	actual=GetSdpAttrValue()->DoesIntFit();

	TBool	expected;
	if ( GetBoolFromConfig(aSection, KExpected(), expected) )
		{
		if ( actual!=expected )
			{
			ERR_PRINTF1(_L("DoesIntFit is not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
  		ERR_PRINTF2(_L("Missing expected value %S"), &KExpected());
  		SetBlockResult(EFail);		
		}
	}

void CT_DataSdpAttrValue::DoCmdInt(const TDesC& aSection)
	{
	TInt	actual = GetSdpAttrValue()->Int();
	INFO_PRINTF2(_L("CSdpAttrValue::Int = %d"), actual);

	TInt	expected;
	if ( GetIntFromConfig(aSection, KExpected(), expected) )
		{
		if ( actual!=expected )
			{
			ERR_PRINTF1(_L("Int is not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
  		ERR_PRINTF2(_L("Missing expected value %S"), &KExpected());
  		SetBlockResult(EFail);		
		}
	}

void CT_DataSdpAttrValue::DoCmdType(const TDesC& aSection)
	{
	TSdpElementType	actual=GetSdpAttrValue()->Type();
	INFO_PRINTF2(_L("CSdpAttrValue::Type = %d"), actual);

	TSdpElementType	expected;
	if ( CT_BTUtil::ReadSdpElementType(*this, expected, aSection, KExpected()) )
		{
		if ( actual!=expected )
			{
			ERR_PRINTF1(_L("Type is not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
  		ERR_PRINTF1(_L("Missing expected value Type"));
  		SetBlockResult(EFail);		
		}
	}

void CT_DataSdpAttrValue::DoCmdUint(const TDesC& aSection)
	{
	TInt	actual = static_cast<TInt>(GetSdpAttrValue()->Uint());
	INFO_PRINTF2(_L("CSdpAttrValue::Uint = %d"), actual);

	TInt	expected;
	if ( GetIntFromConfig(aSection, KExpected(), expected) )
		{
		if ( actual!=expected )
			{
			ERR_PRINTF1(_L("Uint is not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
  		ERR_PRINTF2(_L("Missing expected value %S"), &KExpected());
  		SetBlockResult(EFail);		
		}
	}

void CT_DataSdpAttrValue::DoCmdUUID(const TDesC& aSection)
	{
	TBool ifexpectedExit = EFalse;
	TBool ifexpectedHHExit = EFalse;
	TBool ifexpectedHLExit = EFalse;
	TBool ifexpectedLHExit = EFalse;
	TBool ifexpectedLLExit = EFalse;
	TInt	expected   = 0;
	TInt	expectedHH = 0;
	TInt	expectedHL = 0;
	TInt	expectedLH = 0;
	TInt	expectedLL = 0;
	TUUID actualUUid = NULL;
	TInt actualIntUUid = NULL;
		
	
	
	actualUUid = GetSdpAttrValue()->UUID();
	actualIntUUid = CT_BTUtil::ConvertUUID32toInt(actualUUid);
	
	
	if ( GetIntFromConfig(aSection, KExpected(), expected) )
		{
		INFO_PRINTF2(_L("CSdpAttrValue::UUID call actual result: %d"), actualIntUUid);
		ifexpectedExit=ETrue;
		}
	else //to get four input values for 128-bit TUUID object
		{
		if ( GetIntFromConfig(aSection, KExpectedHH(), expectedHH))
			{
			ifexpectedHHExit=ETrue;
			}
		else
			{
			ERR_PRINTF2(_L("Missing expected value %S"), &KExpectedHH());
			SetBlockResult(EFail);
		}	
		
		if ( GetIntFromConfig(aSection, KExpectedHL(), expectedHL))
			{
			ifexpectedHLExit=ETrue;
			}
		else
			{
			ERR_PRINTF2(_L("Missing expected value %S"), &KExpectedHL());
			SetBlockResult(EFail);
			}

		if ( GetIntFromConfig(aSection, KExpectedLH(), expectedLH))
			{
			ifexpectedLHExit=ETrue;
			}
		else
			{
			ERR_PRINTF2(_L("Missing expected value %S"), &KExpectedLH());
			SetBlockResult(EFail);
			}

		if ( GetIntFromConfig(aSection, KExpectedLL(), expectedLL))
			{
			ifexpectedLLExit=ETrue;
			}
		else
			{
			ERR_PRINTF2(_L("Missing expected value %S"), &KExpectedLL());
			SetBlockResult(EFail);
			}
		}
	
	
	if (ifexpectedExit)
		{
		if ( actualIntUUid!=expected )
			{
			ERR_PRINTF1(_L("UUID is not as expected!"));
			SetBlockResult(EFail);
			}		
		}
	else if(ifexpectedHHExit && ifexpectedHLExit && ifexpectedLHExit && ifexpectedLLExit)//to compare four values with input
		{	
		INFO_PRINTF2(_L("expected Highest order word (HH bits 96 - 127): %d"), expectedHH);
		INFO_PRINTF2(_L("expected Second highest order word  (HL bits 64 - 95): %d"), expectedHL);
		INFO_PRINTF2(_L("expected Second lowest order word  (LH bits 32 - 63): %d"), expectedLH);
		INFO_PRINTF2(_L("expected Low order word (LL bits 0 - 31): %d"), expectedLL);
		
		TUUID expectedUUID(expectedHH,expectedHL,expectedLH,expectedLL);
		
		if ( actualUUid!=expectedUUID )
			{
			ERR_PRINTF1(_L("128 bits of UUID is not as expected!"));
			SetBlockResult(EFail);
			}
		}
	}

void CT_DataSdpAttrValue::VisitAttributeValueL(CSdpAttrValue& aValue, TSdpElementType aType)
	{
	INFO_PRINTF1(_L("MSdpAttributeValueVisitor VisitAttributeValueL Call"));
	HBufC16*	string16 = HBufC16::NewLC(KAttrValueStringBufLen);
	
	switch (aType)
		{
	case ETypeNil:
		INFO_PRINTF1(_L("TSdpElementType is ETypeNil"));
		iVisitorResult.Append(_L("&type=Nil"));
		break;
	case ETypeUint:
		if( aValue.DataSize() <= (static_cast<TUint>(KMaxDataSize)) )
			{
			INFO_PRINTF2(_L("TSdpElementType is ETypeUint. Value = %d"), aValue.Uint());
			iVisitorResult.Append(_L("&type=Uint&value="));
			iVisitorResult.AppendNum(aValue.Uint());
			}
		else
			{
			string16->Des().Copy(aValue.Des());
			TPtrC16 	tprStringUint = string16->Des();
			INFO_PRINTF1(_L("TSdpElementType is ETypeUint. Value = "));
			INFO_PRINTF1(tprStringUint);
			iVisitorResult.Append(_L("&type=Uint&value="));
			iVisitorResult.Append(tprStringUint);
			}	
		break;
	case ETypeInt:
		INFO_PRINTF2(_L("TSdpElementType is ETypeInt. Value = %d"), aValue.Int());
		iVisitorResult.Append(_L("&type=Int&value="));
		iVisitorResult.AppendNum(aValue.Int());
		break;
	case ETypeUUID:
		{
		TUUID	uUid = aValue.UUID();
		TUint	intUUid= CT_BTUtil::ConvertUUID32toInt(uUid);
		INFO_PRINTF2(_L("TSdpElementType is ETypeUUID: Value is %d"),intUUid);
		iVisitorResult.Append(_L("&type=UUID&value="));
		iVisitorResult.AppendNum(intUUid);
		}
		break;
	case ETypeString:
		{
		string16->Des().Copy(aValue.Des());
		TPtrC16 	tprString = string16->Des();
		INFO_PRINTF1(_L("TSdpElementType is ETypeString. Value = "));
		INFO_PRINTF1(tprString);
		iVisitorResult.Append(_L("&type=String&value="));
		iVisitorResult.Append(tprString);
		}
		break;
	case ETypeBoolean:
		INFO_PRINTF2(_L("TSdpElementType is ETypeBoolean. Value = %d"), aValue.Bool());
		iVisitorResult.Append(_L("&type=Boolean&value="));
		iVisitorResult.AppendNum(aValue.Bool());
		break;
	case ETypeDES:
		INFO_PRINTF1(_L("TSdpElementType is ETypeDES"));
		iVisitorResult.Append(_L("&type=DES"));
		break;
	case ETypeDEA:
		INFO_PRINTF1(_L("TSdpElementType is ETypeDEA"));
		iVisitorResult.Append(_L("&type=DEA"));
		break;
	case ETypeURL:
		{
		string16->Des().Copy(aValue.Des());
		TPtrC16 	tprStringURL = string16->Des();
		INFO_PRINTF1(_L("TSdpElementType is ETypeURL. Value = "));
		INFO_PRINTF1(tprStringURL);
		iVisitorResult.Append(_L("&type=URL&value="));
		iVisitorResult.Append(tprStringURL);
		}
		break;
	case ETypeEncoded:
		INFO_PRINTF1(_L("TSdpElementType is ETypeEncoded"));
		iVisitorResult.Append(_L("&type=Encoded"));
		break;
	default:
		INFO_PRINTF2(_L("TSdpElementType is Unknown %d"), aType);
		iVisitorResult.Append(_L("&type=Unknown"));
		break;
		}
	CleanupStack::PopAndDestroy(string16);
	}

TPtr CT_DataSdpAttrValue::GetVisitorResult()
	{
	return iVisitorResult;
	}
	
void CT_DataSdpAttrValue::ResetVisitorResult()
	{
	iVisitorResult.Zero();
	}
	
void CT_DataSdpAttrValue::StartListL(CSdpAttrValueList& /*aList*/)
	{
	}

void CT_DataSdpAttrValue::EndListL()
	{
	}

void CT_DataSdpAttrValue::DoCmdMSAVV_ExtensionInterfaceL()
	{
	INFO_PRINTF1(_L("MSdpAttributeValueVisitor MSAVV_ExtensionInterfaceL call"));
	void* tmpVoid;
	TRAPD(err, this->MSAVV_ExtensionInterfaceL(KNullUid, tmpVoid));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("MSdpAttributeValueVisitor MSAVV_ExtensionInterfaceL failed with error %d"), err);
		SetError(err);
		}
	}
