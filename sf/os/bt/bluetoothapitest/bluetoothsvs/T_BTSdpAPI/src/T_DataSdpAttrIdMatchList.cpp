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

#include "T_DataSdpAttrIdMatchList.h"

/*@{*/
//Parameters
_LIT(KAddRange1,												"addrangestart");
_LIT(KAddRange2,												"addrangeend");
_LIT(KRemoveRange1,												"removerangestart");
_LIT(KRemoveRange2,												"removerangeend");
_LIT(KInMListBool,												"expectedbool");
_LIT(KInMListInt1,												"inputattrid");
_LIT(KInMListInt2,												"expectedposition");
_LIT(KInMListSwitch,											"switch");
_LIT(KCountInt,													"countint");
_LIT(KIdMatchList,												"idmatchlist");

//Commands
_LIT(KCmdTSDPAIMLNewL, 											"NewL");
_LIT(KCmdTSDPAIMLAddL,											"AddL");
_LIT(KCmdTSDPAIMLRemoveL,										"RemoveL");
_LIT(KCmdTSDPAIMLInMatchList,									"InMatchList");
_LIT(KCmdTSDPAIMLInMatchListRange,								"InMatchListRange");
_LIT(KCmdTSDPAIMLCount,											"Count");
_LIT(KCmdTSDPAIMLDestructor,									"~");

/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataSdpAttrIdMatchList* CT_DataSdpAttrIdMatchList::NewL()
	{
	CT_DataSdpAttrIdMatchList*	ret=new (ELeave) CT_DataSdpAttrIdMatchList();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataSdpAttrIdMatchList::CT_DataSdpAttrIdMatchList()
:	iAttrIdMatchList(NULL)
,	iElementBuilder(NULL)
	{
	}

void CT_DataSdpAttrIdMatchList::ConstructL()
	{
	iElementBuilder = new (ELeave) CT_DataSdpElementBuilder(*this);
	}

MSdpElementBuilder* CT_DataSdpAttrIdMatchList::GetSdpElementBuilder() const
	{
	return iAttrIdMatchList;
	}

CT_DataSdpAttrIdMatchList::~CT_DataSdpAttrIdMatchList()
	{
	DestroyData();
	delete iElementBuilder;
	iElementBuilder=NULL;
	}

void CT_DataSdpAttrIdMatchList::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iAttrIdMatchList = static_cast<CSdpAttrIdMatchList*> (aAny);
	}

void CT_DataSdpAttrIdMatchList::DisownObjectL()
	{
	iAttrIdMatchList = NULL;
	}

void CT_DataSdpAttrIdMatchList::DestroyData()
	{
	delete iAttrIdMatchList;
	iAttrIdMatchList = NULL;
	}

inline TCleanupOperation CT_DataSdpAttrIdMatchList::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataSdpAttrIdMatchList::CleanupOperation(TAny* aAny)
	{
	CSdpAttrIdMatchList* list=static_cast<CSdpAttrIdMatchList*>(aAny);
	delete list;
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
TBool CT_DataSdpAttrIdMatchList::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret = ETrue;

	if ( aCommand==KCmdTSDPAIMLNewL )
		{
		DoCmdNewL(aSection);
		}
	else if ( aCommand==KCmdTSDPAIMLAddL )
		{
		DoCmdAddL(aSection);
		}
	else if ( aCommand==KCmdTSDPAIMLRemoveL )
		{
		DoCmdRemoveL(aSection);
		}
	else if ( aCommand==KCmdTSDPAIMLInMatchList )
		{
		DoCmdInMatchList(aSection);
		}
	else if ( aCommand==KCmdTSDPAIMLInMatchListRange )
		{
		DoCmdInMatchListRange(aSection);
		}		
	else if ( aCommand==KCmdTSDPAIMLCount )
		{
		DoCmdCount(aSection);
		}
	else if ( aCommand==KCmdTSDPAIMLDestructor )
		{
		DoCmdDestructor();
		}
	else
		{
		ret = EFalse;
		}

	if( !ret )
		{
		ret=iElementBuilder->DoCommandL(*iAttrIdMatchList, aCommand, aSection, aAsyncErrorIndex);
		}

	return ret;
	}

void CT_DataSdpAttrIdMatchList::DoCmdNewL(const TDesC& aSection)
    {
    TInt	err=KErrNone;
    TPtrC	idMatchListName;
    DestroyData();
    if( GetStringFromConfig(aSection, KIdMatchList(), idMatchListName) )
        {
		INFO_PRINTF1(_L("CSdpAttrIdMatchList Copy Constructor 'NewL' Call"));
        CSdpAttrIdMatchList*  idMatchList=static_cast<CSdpAttrIdMatchList*>(GetDataObjectL(idMatchListName));
        TRAP(err, iAttrIdMatchList=CSdpAttrIdMatchList::NewL(*idMatchList));
        }
    else
        {
		INFO_PRINTF1(_L("CSdpAttrIdMatchList empty constructor 'NewL' Call"));
        TRAP(err, iAttrIdMatchList=CSdpAttrIdMatchList::NewL());
        }
    if(err != KErrNone)
        {
        ERR_PRINTF2(_L("CSdpAttrIdMatchList::NewL failed with error %d"), err);
        SetError(err);
        }
    }

void CT_DataSdpAttrIdMatchList::DoCmdAddL(const TDesC& aSection)
	{
	TInt	firstInt = 0;
	TInt	lastInt = 0;
	if( GetIntFromConfig(aSection, KAddRange1(), firstInt))
		{
		TInt	err=KErrNone;
		if( GetIntFromConfig(aSection, KAddRange2(), lastInt))
			{
			INFO_PRINTF3(_L("CSdpAttrIdMatchList AddL Call with 2 parameters (Range: ID1=%d; ID2=%d)"), firstInt, lastInt);
			TAttrRange rangeOfIDs(firstInt,lastInt);
			TRAP(err, iAttrIdMatchList->AddL(rangeOfIDs));
			}
		else
			{
			INFO_PRINTF2(_L("CSdpAttrIdMatchList AddL Call with 1 parameter (ID=%d)"), firstInt);
			TAttrRange rangeOfIDs(firstInt);
			TRAP(err, iAttrIdMatchList->AddL(rangeOfIDs));
			}
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CSdpAttrIdMatchList::AddL failed with error %d"), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KAddRange2());
		SetBlockResult(EFail);
		}
	}

void CT_DataSdpAttrIdMatchList::DoCmdRemoveL(const TDesC& aSection)
	{
	TInt	firstInt = 0;
	TInt	lastInt = 0;
	if( GetIntFromConfig(aSection, KRemoveRange1(), firstInt))
		{
		TInt	err=KErrNone;

		if( GetIntFromConfig(aSection, KRemoveRange2(), lastInt))
			{
			INFO_PRINTF3(_L("CSdpAttrIdMatchList RemoveL Call with 2 parameters (Range: ID1=%d; ID2=%d)"), firstInt, lastInt);
			TAttrRange rangeOfIDs(firstInt,lastInt);
			TRAP(err, iAttrIdMatchList->RemoveL(rangeOfIDs));
			}
		else
			{
			INFO_PRINTF2(_L("CSdpAttrIdMatchList RemoveL Call with 1 parameter (ID=%d)"), firstInt);
			TAttrRange rangeOfIDs(firstInt);
			TRAP(err, iAttrIdMatchList->RemoveL(rangeOfIDs));
			}
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("CSdpAttrIdMatchList::RemoveL failed with error %d"), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KRemoveRange2());
		SetBlockResult(EFail);
		}
	}

void CT_DataSdpAttrIdMatchList::DoCmdInMatchList(const TDesC& aSection)
	{
	TInt	theInt = 0;
	TInt	expectedPosition = 0;
	TInt	actualPosition = 0;
	TBool	actualResult = EFalse;
	TBool	expectedResult = EFalse;
	TBool	mListSwitch = EFalse;
	
	//First, we check if the mandatory input data "theInt" exists. We can't call InMatchList without it.
	if( GetIntFromConfig(aSection, KInMListInt1(), theInt))
		{
		//InMatchList is an overloaded function which accepts one or two parameters.
		//So, if the input data "KInMListSwitch" exists, we call the first version. If not, the second version is called.
		if( GetBoolFromConfig(aSection, KInMListSwitch(), mListSwitch))
			{
			actualResult = iAttrIdMatchList->InMatchList(theInt, actualPosition);
			INFO_PRINTF3(_L("InMatchList Call Result: is it in List? %d, Position: %d"), actualResult, actualPosition);
			//Check the results
			if(GetIntFromConfig(aSection, KInMListInt2(), expectedPosition))
				{
				if(expectedPosition != actualPosition)
					{
					ERR_PRINTF2(_L("InMatchList() not as expected! Expected position: %d"), expectedPosition);
					SetBlockResult(EFail);
					}
				}
			else
				{
  				ERR_PRINTF2(_L("Missing expected value %S"), &KInMListInt2());
  				SetBlockResult(EFail);		
				}
			}
		else
			{
			INFO_PRINTF2(_L("CSdpAttrIdMatchList InMatchList Call (attrID=%d)"), theInt);
			actualResult = iAttrIdMatchList->InMatchList(theInt);
			INFO_PRINTF2(_L("CSdpAttrIdMatchList InMatchList Call Result: Is it in List? %d"), actualResult);
			}
		if( GetBoolFromConfig(aSection, KInMListBool(), expectedResult))
			{
			if(expectedResult != actualResult)
				{
				ERR_PRINTF2(_L("InMatchList() not as expected! Expected result (is in list?): %d"), expectedResult);
				SetBlockResult(EFail);
				}
			}		
		else
			{
  			ERR_PRINTF2(_L("Missing expected value %S"), &KInMListBool());
  			SetBlockResult(EFail);		
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KInMListBool());
		SetBlockResult(EFail);
		}
	}

void CT_DataSdpAttrIdMatchList::DoCmdCount(const TDesC& aSection)
	{
	TInt  expected = 0;
	TInt	count=iAttrIdMatchList->Count();
	INFO_PRINTF2(_L("CSdpAttrIdMatchList Count Call result: %d"), count);

	if( GetIntFromConfig(aSection, KCountInt(), expected))
		{
		if(expected != count)
			{
			ERR_PRINTF1(_L("Count() not as expected!"));
			SetBlockResult(EFail);
			}
		}
	else
		{
  		ERR_PRINTF2(_L("Missing expected value %S"), &KCountInt());
  		SetBlockResult(EFail);		
		}
	}
void CT_DataSdpAttrIdMatchList::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("CSdpAttrIdMatchList Destructor Call."));
	DestroyData();
	}

void CT_DataSdpAttrIdMatchList::DoCmdInMatchListRange(const TDesC& aSection)
	{
	TInt	theInt = 0; //Used to store the attribute id we are searching for
	TInt	expectedPosition = 0;
	TInt	actualPosition = 0;
	TBool	actualResult = EFalse;
	TBool	expectedResult = EFalse;
	
	//First, we check if the mandatory input data "theInt" exists. We can't call InMatchList without it.
	if( GetIntFromConfig(aSection, KInMListInt1(), theInt))
		{
		actualResult = iAttrIdMatchList->InMatchListRange(theInt, actualPosition);
		INFO_PRINTF3(_L("InMatchListRange Call Result: is it in List? %d, Position: %d"), actualResult, actualPosition);
		//Check the results
		if(GetIntFromConfig(aSection, KInMListInt2(), expectedPosition))
			{
			if(expectedPosition != actualPosition)
				{
				ERR_PRINTF2(_L("InMatchListRange() not as expected! Expected position: %d"), expectedPosition);
				SetBlockResult(EFail);
				}
			}
		else
			{
  			ERR_PRINTF2(_L("Missing expected value %S"), &KInMListInt2());
  			SetBlockResult(EFail);		
			}
		if( GetBoolFromConfig(aSection, KInMListBool(), expectedResult))
			{
			if(expectedResult != actualResult)
				{
				ERR_PRINTF2(_L("InMatchListRange() not as expected! Expected result (is in list?): %d"), expectedResult);
				SetBlockResult(EFail);
				}
			}		
		else
			{
  			ERR_PRINTF2(_L("Missing expected value %S"), &KInMListBool());
  			SetBlockResult(EFail);		
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KInMListBool());
		SetBlockResult(EFail);
		}
	}
