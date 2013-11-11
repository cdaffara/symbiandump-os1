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

#include "T_DataCSdpSearchPattern.h"
#include "T_BTUtil.h"

/*@{*/

//Commands
_LIT(KCmdNewL, 												"NewL");
_LIT(KCmdConstructL, 										"ConstructL");
_LIT(KCmdAddL,												"AddL");
_LIT(KCmdRemove,		 									"Remove");
_LIT(KCmdFind,	 											"Find");
_LIT(KCmdCount,	 											"Count");
_LIT(KCmdAt,	 											"At");
_LIT(KCmdReset,	 											"Reset");
_LIT(KCmdIsEmpty,	 										"IsEmpty");
_LIT(KCmdDestructor, 										"~");


//Parameters
_LIT(KUUID, 												"uuid");
_LIT(KExpected, 											"expected");
_LIT(KPosition, 											"position");
_LIT(KIndex, 												"index");

/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataCSdpSearchPattern* CT_DataCSdpSearchPattern::NewL()
	{
	CT_DataCSdpSearchPattern*	ret=new (ELeave) CT_DataCSdpSearchPattern();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataCSdpSearchPattern::CT_DataCSdpSearchPattern()
:	iSearchPattern(NULL)
,	iElementBuilder(NULL)
	{
	}

void CT_DataCSdpSearchPattern::ConstructL()
	{
	iElementBuilder = new (ELeave) CT_DataSdpElementBuilder(*this);
	}

CT_DataCSdpSearchPattern::~CT_DataCSdpSearchPattern()
	{
	DestroyData();
	delete iElementBuilder;
	iElementBuilder=NULL;
	}

void CT_DataCSdpSearchPattern::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iSearchPattern = static_cast<CSdpSearchPattern*> (aAny);
	}

void CT_DataCSdpSearchPattern::DisownObjectL()
	{
	iSearchPattern = NULL;
	}

void CT_DataCSdpSearchPattern::DestroyData()
	{
	delete iSearchPattern;
	iSearchPattern=NULL;
	}

inline TCleanupOperation CT_DataCSdpSearchPattern::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataCSdpSearchPattern::CleanupOperation(TAny* aAny)
	{
	CSdpSearchPattern* pattern=static_cast<CSdpSearchPattern*>(aAny);
	delete pattern;
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
TBool CT_DataCSdpSearchPattern::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdNewL )
		{
		DoCmdNewL();
		}
	else if ( aCommand==KCmdConstructL )
		{
		DoCmdConstructL();
		}
	else if ( aCommand==KCmdAddL )
		{
		DoCmdAddL(aSection);
		}
	else if ( aCommand==KCmdRemove )
		{
		DoCmdRemove(aSection);
		}
	else if ( aCommand==KCmdFind )
		{
		DoCmdFind(aSection);
		}
	else if ( aCommand==KCmdCount )
		{
		DoCmdCount(aSection);
		}
	else if ( aCommand==KCmdAt )
		{
		DoCmdAt(aSection);
		}
	else if ( aCommand==KCmdReset )
		{
		DoCmdReset();
		}
	else if ( aCommand==KCmdIsEmpty )
		{
		DoCmdIsEmpty(aSection);
		}
	else if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else
		{
		ret = EFalse;
		}

	if( !ret )
		{
		ret=iElementBuilder->DoCommandL(*iSearchPattern, aCommand, aSection, aAsyncErrorIndex);
		}

	return ret;
	}


/**
Test NewL()
*/
void CT_DataCSdpSearchPattern::DoCmdNewL()
	{
	DestroyData();
	INFO_PRINTF1(_L("CSdpSearchPattern NewL call"));
	TRAPD(err, iSearchPattern = CSdpSearchPattern::NewL());
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CSdpSearchPattern NewL failed with error %d"), err);
		SetError(err);
		}
	}

/**
Test ConstructL()
*/
void CT_DataCSdpSearchPattern::DoCmdConstructL()
	{
	INFO_PRINTF1(_L("CSdpSearchPattern ConstructL Call"));

	TRAPD(err, iSearchPattern->ConstructL());
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CSdpSearchPattern NewL failed with error %d"), err);
		SetError(err);
		}
	}

/**
Test AddL()
*/
void CT_DataCSdpSearchPattern::DoCmdAddL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CSdpSearchPattern AddL Call"));

	TInt 								theValue = 0;
	if( GetIntFromConfig(aSection, KUUID(), theValue) )
		{
		const TUUID theUUID(theValue);
		TInt position = 0;
		TRAPD(err, position = iSearchPattern->AddL(theUUID));
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CSdpSearchPattern AddL failed with error %d"), err);
			SetError(err);
			}
		INFO_PRINTF2(_L("CSdpSearchPattern AddL Call result: actual = %d"), position);

		TInt expected = 0;
		if( GetIntFromConfig(aSection, KExpected(), expected) )
			{
			if( position != expected )
				{
				ERR_PRINTF1(_L("AddL not as expected!"));
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("Missing expected value %S"), &KExpected());
			SetBlockResult(EFail);			
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KExpected());
		SetBlockResult(EFail);
		}
	}

/**
Test Remove()
*/
void CT_DataCSdpSearchPattern::DoCmdRemove(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CSdpSearchPattern Remove Call"));

	TInt 								theValue = 0;
	if( GetIntFromConfig(aSection, KUUID(), theValue))
		{
		const TUUID theUUID(theValue);
		TInt position = iSearchPattern->Remove(theUUID);
		INFO_PRINTF2(_L("CSdpSearchPattern Remove Call result: actual = %d"), position);

		TInt expected = 0;
		if( GetIntFromConfig(aSection, KExpected(), expected) )
			{
			if( expected != position )
				{
				ERR_PRINTF1(_L("Remove not as expected!"));
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("Missing expected value %S"), &KExpected());
			SetBlockResult(EFail);		
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KExpected());
		SetBlockResult(EFail);
		}
	}

/**
Test Find()
*/
void CT_DataCSdpSearchPattern::DoCmdFind(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CSdpSearchPattern Find Call"));

	TInt 								theValue = 0;
	if( GetIntFromConfig(aSection, KUUID(), theValue))
		{
		const TUUID theUUID(theValue);
		TInt position = 0;
		TInt result = iSearchPattern->Find(theUUID, position);
		INFO_PRINTF2(_L("CSdpSearchPattern Find Call return value: actual = %d"), result);
		INFO_PRINTF2(_L("CSdpSearchPattern Find Call position: actual = %d"), position);

		TInt expectedResult = 0;
		if( GetIntFromConfig(aSection, KExpected(), expectedResult) )
			{
			if( expectedResult != result )
				{
				ERR_PRINTF1(_L("Find not as expected!"));
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("Missing expected value %S"), &KExpected());
			SetBlockResult(EFail);		
			}

		TInt expectedPosition = 0;
		if( GetIntFromConfig(aSection, KPosition(), expectedPosition) )
			{
			if( expectedPosition != position )
				{
				ERR_PRINTF1(_L("Find not as expected!"));
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("Missing expected value %S"), &KPosition());
			SetBlockResult(EFail);		
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KPosition());
		SetBlockResult(EFail);
		}
	}

/**
Test Count()
*/
void CT_DataCSdpSearchPattern::DoCmdCount(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CSdpSearchPattern Count Call"));

	TInt	count = iSearchPattern->Count();
	INFO_PRINTF2(_L("CSdpSearchPattern Count Call result: actual = %d"), count);

	TInt								expected = 0;
	if( GetIntFromConfig(aSection, KExpected(), expected) )
		{
		if( expected != count )
			{
			ERR_PRINTF1(_L("Count not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing expected value %S"), &KExpected());
		SetBlockResult(EFail);		
		}
	}

/**
Test At()
*/
void CT_DataCSdpSearchPattern::DoCmdAt(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CSdpSearchPattern At Call"));

	TInt 								theValue = 0;
	if( GetIntFromConfig(aSection, KIndex(), theValue))
		{
		TUUID	theUUID = iSearchPattern->At(theValue);
		TUint	intUUid= CT_BTUtil::ConvertUUID32toInt(theUUID);
		INFO_PRINTF2(_L("CSdpSearchPattern At Call result: actual = %d"), intUUid);
		
		TInt expected = 0;
		if( GetIntFromConfig(aSection, KExpected(), expected))
			{
			if( TUUID(expected) != theUUID)
				{
				ERR_PRINTF1(_L("At not as expected!"));
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF2(_L("Missing expected value %S"), &KExpected());
			SetBlockResult(EFail);		
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KExpected());
		SetBlockResult(EFail);
		}
	}

/**
Test Reset()
*/
void CT_DataCSdpSearchPattern::DoCmdReset()
	{
	INFO_PRINTF1(_L("CSdpSearchPattern Reset Call"));

	iSearchPattern->Reset();
	}

/**
Test IsEmpty()
*/
void CT_DataCSdpSearchPattern::DoCmdIsEmpty(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CSdpSearchPattern IsEmpty Call"));

	TBool	result = iSearchPattern->IsEmpty();
	INFO_PRINTF2(_L("CSdpSearchPattern IsEmpty Call result: actual = %d"), result);

	TBool expected = EFalse;
	if( GetBoolFromConfig(aSection, KExpected(), expected) )
		{
		if(result != expected)
			{
			ERR_PRINTF1(_L("IsEmpty not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing expected value %S"), &KExpected());
		SetBlockResult(EFail);		
		}
	}

/**
Test ~CSdpSearchPattern()
*/
void CT_DataCSdpSearchPattern::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("CSdpSearchPattern Destructor Call"));

	DestroyData();
	}
