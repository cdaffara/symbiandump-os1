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

#include "T_DataCElementParser.h"
#include "T_BTUtil.h"

/*@{*/

//Commands
_LIT(KCmdNewL, 												"NewL");
_LIT(KCmdReset,		 										"Reset");
_LIT(KCmdBuilder, 											"Builder");
_LIT(KCmdSetBuilder, 										"SetBuilder");
_LIT(KCmdParseElementsL, 									"ParseElementsL");
_LIT(KCmdBufferedParseL, 									"BufferedParseL");
_LIT(KCmdDestructor, 										"~");

//Parameters
_LIT(KObject,												"object");
_LIT(KExpected,												"expected");
_LIT(KType,													"type");
_LIT(KFileName,												"filename");

/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataCElementParser* CT_DataCElementParser::NewL()
	{
	CT_DataCElementParser*	ret=new (ELeave) CT_DataCElementParser();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataCElementParser::CT_DataCElementParser()
:	iCElementParser(NULL)
	{
	}

void CT_DataCElementParser::ConstructL()
	{
	User::LeaveIfError(iRFs.Connect());
	}

CT_DataCElementParser::~CT_DataCElementParser()
	{
	DestroyData();
	iRFs.Close();
	}

void CT_DataCElementParser::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iCElementParser = static_cast<CElementParser*> (aAny);
	}

void CT_DataCElementParser::DisownObjectL()
	{
	iCElementParser = NULL;
	}

void CT_DataCElementParser::DestroyData()
	{
	delete iCElementParser;
	iCElementParser=NULL;
	}

inline TCleanupOperation CT_DataCElementParser::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataCElementParser::CleanupOperation(TAny* aAny)
	{
	CElementParser* parser=static_cast<CElementParser*>(aAny);
	delete parser;
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
TBool CT_DataCElementParser::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdNewL )
		{
		DoCmdNewL(aSection);
		}
	else if ( aCommand==KCmdReset )
		{
		DoCmdReset(aSection);
		}
	else if ( aCommand==KCmdBuilder )
		{
		DoCmdBuilder(aSection);
		}
	else if ( aCommand==KCmdSetBuilder )
		{
		DoCmdSetBuilder(aSection);
		}
	else if ( aCommand==KCmdParseElementsL )
		{
		DoCmdParseElementsL(aSection);
		}
	else if ( aCommand==KCmdBufferedParseL )
		{
		DoCmdBufferedParseL(aSection);
		}
	else if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}


/**
Test NewL()
*/
void CT_DataCElementParser::DoCmdNewL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CElementParser NewL Call"));
	DestroyData();

	MSdpElementBuilder* elementBuilder = NULL;
	TPtrC 								theObject;
	TPtrC								theType;
	if( GetStringFromConfig(aSection, KObject(), theObject) )
		{
		if( GetStringFromConfig(aSection, KType(), theType) )
			{
			elementBuilder = CT_BTUtil::ElementBuilderCastL(*this, theObject, theType);
			}
		else
			{
			ERR_PRINTF1(_L("Error: Type is not specified."));
			SetBlockResult(EFail);
			}
		}

	TRAPD(err, iCElementParser = CElementParser::NewL(elementBuilder));
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CElementParser NewL failed with error %d"), err);
		SetError(err);
		}
	}

/**
Test Reset()
*/
void CT_DataCElementParser::DoCmdReset(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CElementParser Reset Call"));

	TPtrC								theObject;
	TPtrC								theType;
	if( GetStringFromConfig(aSection, KObject(), theObject) )
		{
		if( GetStringFromConfig(aSection, KType(), theType) )
			{
			MSdpElementBuilder* elementBuilder = CT_BTUtil::ElementBuilderCastL(*this, theObject, theType);
			iCElementParser->Reset(elementBuilder);
			}
		else
			{
			ERR_PRINTF1(_L("Error: Type is not specified."));
			SetBlockResult(EFail);
			}
		}
	else
		{
		iCElementParser->Reset();
		}
	}

/**
Test Builder()
*/
void CT_DataCElementParser::DoCmdBuilder(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CElementParser Builder Call"));
	MSdpElementBuilder* actualBuilder = iCElementParser->Builder();

	TPtrC								theObject;
	TPtrC								theType;
	if( GetStringFromConfig(aSection, KObject(), theObject) )
		{
		if( GetStringFromConfig(aSection, KType(), theType) )
			{
			MSdpElementBuilder*	expectedBuilder = CT_BTUtil::ElementBuilderCastL(*this, theObject, theType);
			if( actualBuilder != expectedBuilder )
				{
				ERR_PRINTF1(_L("Builder not as expected!"));
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("Error: Type is not specified."));
			SetBlockResult(EFail);
			}
		}
	}

/**
Test SetBuilder()
*/
void CT_DataCElementParser::DoCmdSetBuilder(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CElementParser SetBuilder Call"));

	MSdpElementBuilder* elementBuilder = NULL;
	TPtrC								theObject;
	TPtrC								theType;
	if( GetStringFromConfig(aSection, KObject(), theObject) )
		{
		if( GetStringFromConfig(aSection, KType(), theType) )
			{
			elementBuilder = CT_BTUtil::ElementBuilderCastL(*this, theObject, theType);
			}
		else
			{
			ERR_PRINTF1(_L("Error: Type is not specified."));
			SetBlockResult(EFail);
			}
		}

	iCElementParser->SetBuilder(elementBuilder);
	}

/**
Test ParseElementsL()
*/
void CT_DataCElementParser::DoCmdParseElementsL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CElementParser ParseElementsL Call"));

	TPtrC								fileName;
	if( GetStringFromConfig(aSection, KFileName(), fileName) )
		{
		RFile file;
		User::LeaveIfError(file.Open(iRFs, fileName, EFileRead));
		CleanupClosePushL(file);
		
		TInt fileSize = 0;
		file.Size(fileSize);
		HBufC8* buff = HBufC8::NewLC(fileSize);
		TPtr8 tprBuff = buff->Des();
		file.Read(tprBuff);
		
		TInt numberOfBytes = 0;
		TRAPD(err, numberOfBytes = iCElementParser->ParseElementsL(tprBuff));
		INFO_PRINTF2(_L("CElementParser::ParseElementsL() result: actual = %d"), numberOfBytes);
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CElementParser ParseElementsL failed with error %d"), err);
			SetError(err);
			}
		else
			{
			TInt	expected;
			if( GetIntFromConfig(aSection, KExpected(), expected) )
				{
				if( numberOfBytes != expected )
					{
					ERR_PRINTF1(_L("ParseElementsL not as expected!"));
					SetBlockResult(EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("Missing expected value %S"), &KExpected());
				SetBlockResult(EFail);
				}
			}
		CleanupStack::PopAndDestroy(2, &file);
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KExpected());
		SetBlockResult(EFail);
		}
	}

/**
Test BufferedParseL()
*/
void CT_DataCElementParser::DoCmdBufferedParseL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CElementParser BufferedParseL Call"));

	TPtrC								fileName;
	if( GetStringFromConfig(aSection, KFileName(), fileName) )
		{
		RFile file;
		User::LeaveIfError(file.Open(iRFs, fileName, EFileRead));
		CleanupClosePushL(file);
		
		TInt fileSize = 0;
		file.Size(fileSize);
		HBufC8* buff = HBufC8::NewLC(fileSize);
		TPtr8 tprBuff = buff->Des();
		file.Read(tprBuff);

		TBool	boolResult = EFalse;
		TRAPD(err, boolResult = iCElementParser->BufferedParseL(tprBuff));
		INFO_PRINTF2(_L("CElementParser::BufferedParseL() result: actual = %d"), boolResult);
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CElementParser BufferedParseL failed with error %d"), err);
			SetError(err);
			}
		else
			{
			TBool expected = EFalse;
			if( GetBoolFromConfig(aSection, KExpected(), expected) )
				{
				if( boolResult != expected )
					{
					ERR_PRINTF1(_L("BufferedParseL not as expected!"));
					SetBlockResult(EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("Missing expected value %S"), &KExpected());
				SetBlockResult(EFail);
				}
			}
		CleanupStack::PopAndDestroy(2, &file);
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KExpected());
		SetBlockResult(EFail);
		}
	}

/**
Test ~CElementParser()
*/
void CT_DataCElementParser::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("CElementParser Destructor Call"));

	DestroyData();
	}
