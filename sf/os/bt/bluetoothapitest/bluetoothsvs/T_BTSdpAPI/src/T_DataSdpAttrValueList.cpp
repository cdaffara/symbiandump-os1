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

#include "T_DataSdpAttrValueList.h"


/*@{*/
//Parameters
_LIT(KExpected,				"expected");
_LIT(KDataSdpAgent,			"datasdpagent");
_LIT(KArrayPartialStart,	"partialstart");
_LIT(KArrayPartialEnd,		"partialend");
_LIT(KArrayIndex,			"arrayindex");

//Commands
_LIT(KCmdAppendValueL,		"AppendValueL");
_LIT(KCmdBuildEncodeL,		"BuildEncodedL");
_LIT(KCmdDestructor,		"~");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataSdpAttrValueList::CT_DataSdpAttrValueList()
:	CT_DataSdpAttrValue()
,	iAttrValueList(NULL)
,	iElementBuilder(NULL)
	{
	}

void CT_DataSdpAttrValueList::ConstructL()
	{
	iElementBuilder = new (ELeave) CT_DataSdpElementBuilder(*this);
	}

CT_DataSdpAttrValueList::~CT_DataSdpAttrValueList()
	{
	DestroyData();
	delete iElementBuilder;
	iElementBuilder=NULL;
	}

CSdpAttrValue* CT_DataSdpAttrValueList::GetSdpAttrValue() const
	{
	return GetSdpAttrValueList();
	}

CSdpAttrValueList*	CT_DataSdpAttrValueList::GetSdpAttrValueList() const
	{
	return iAttrValueList;
	}

void CT_DataSdpAttrValueList::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iAttrValueList = static_cast<CSdpAttrValueList*> (aAny);
	}

void CT_DataSdpAttrValueList::DisownObjectL()
	{
	iAttrValueList = NULL;
	}

void CT_DataSdpAttrValueList::DestroyData()
	{
	delete iAttrValueList;
	iAttrValueList=NULL;
	}

inline TCleanupOperation CT_DataSdpAttrValueList::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataSdpAttrValueList::CleanupOperation(TAny* aAny)
	{
	CSdpAttrValueList* arg=static_cast<CSdpAttrValueList*>(aAny);
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
TBool CT_DataSdpAttrValueList::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdDestructor )
		{
		DoCmdDestructor();
		}
	else if ( aCommand==KCmdAppendValueL )
		{
		DoCmdAppendValueL(aSection);
		}

	else if ( aCommand==KCmdBuildEncodeL )
		{
		DoCmdBuildEncodedL(aSection);
		}
	else
		{
		ret=CT_DataSdpAttrValue::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	if ( !ret )
		{
		ret=iElementBuilder->DoCommandL(*(GetSdpAttrValueList()), aCommand, aSection, aAsyncErrorIndex);
		}
	return ret;
	}


/**
 Test CSdpAttrValueList Destructor() function
*/
void CT_DataSdpAttrValueList::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("Call Destructor function"));
	DestroyData();
	}


/**
 Test CSdpAttrValueList AppendValueL() function
*/
void CT_DataSdpAttrValueList::DoCmdAppendValueL(const TDesC& aSection)
	{
	
	TPtrC	object;
	TPtrC	data_object;
	TBool	valueObjExist = EFalse;
	TBool	dataObjExist = EFalse;

	 if (GetStringFromConfig(aSection, KExpected(), object))
		{
		valueObjExist = ETrue; 
		}
	if (GetStringFromConfig(aSection, KDataSdpAgent(), data_object))
		{
		dataObjExist = ETrue; 
		}

	if ( valueObjExist )
		{
		INFO_PRINTF1(_L("Create a new CSdpAttrValue object"));
		CSdpAttrValue*	sdpAttrValue = static_cast<CSdpAttrValue*>(GetDataObjectL(object));
		INFO_PRINTF2(_L("CSdpAttrValue object type is (%d)"), sdpAttrValue->Type());

		INFO_PRINTF1(_L("Call CSdpAttrValueList::AppendValueL() function"));
		TRAPD(err, GetSdpAttrValueList()->AppendValueL(sdpAttrValue));
		if (err == KErrNone)
			{
			//AppendValueL success. Disown it
			CDataWrapper*	data=GetDataWrapperL(object);
			data->DisownObjectL();
			}
		else
			{
			ERR_PRINTF2(_L("Call CSdpAttrValueList::AppendValueL() failed with error (%d)"), err);
			SetError(err);
			}
		}
		else if ( dataObjExist )
			{
			INFO_PRINTF1(_L("CT_DataSdpAttrValueList: Get CSdpAgent Wrapper from dictionary"));

			RPointerArray<CSdpAttrValue> pointAttrValArray;
			CT_DataSdpAgent* tBTSdpAgentData=static_cast<CT_DataSdpAgent*>(GetDataWrapperL(data_object));
			pointAttrValArray = tBTSdpAgentData->GetAttrValArray();
			TInt actualsize = pointAttrValArray.Count();
			TInt readindex = 0;
			
			if( GetIntFromConfig(aSection, KArrayIndex(), readindex) )
				{
				INFO_PRINTF1(_L("Call CSdpAttrValueList::AppendValueL() function for an entry of an array"));

				if (readindex >= 0 && readindex < actualsize)
					{	
					TRAPD(err, GetSdpAttrValueList()->AppendValueL(pointAttrValArray[readindex]));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Call CSdpAttrValueList::AppendValueL() failed with error (%d)"), err);
						SetError(err);
						}
					}
				else
					{
					WARN_PRINTF1(_L("Invalid array index. Could not read from the array."));
					}
				}
			else
				{
				INFO_PRINTF1(_L("Call CSdpAttrValueList::AppendValueL() function for multiple entries of an array"));     

				TInt arrayReadStart = 0;
				TInt arrayReadEnd = actualsize;

				if (GetIntFromConfig(aSection, KArrayPartialStart(), arrayReadStart))
					{
					if (arrayReadStart < 0)
						{
						arrayReadStart = 0;
						}
					}
				if (GetIntFromConfig(aSection, KArrayPartialEnd(), arrayReadEnd))
					{
					if (arrayReadEnd > actualsize)
						{
						arrayReadEnd = actualsize;
						}
					}
				for(int i=arrayReadStart; i<arrayReadEnd; i++)
					{
					TRAPD(err, GetSdpAttrValueList()->AppendValueL(pointAttrValArray[i]));
					if (err != KErrNone)
						{
						ERR_PRINTF2(_L("Call CSdpAttrValueList::AppendValueL() failed with error (%d)"), err);
						SetError(err);
						}
					}
				}
			}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KArrayPartialEnd());
		SetBlockResult(EFail);
		}
	}


/**
 Test CSdpAttrValueList's BuildEncodedL() function
*/
void CT_DataSdpAttrValueList::DoCmdBuildEncodedL(const TDesC& aSection)
	{

	TPtrC	theString;

	if ( GetStringFromConfig(aSection, KExpected(), theString) )
		{
		HBufC8*	string8 = HBufC8::NewLC(theString.Length());
		string8->Des().Copy(theString);
	    TPtrC8 	tprString8 = string8->Des();

		INFO_PRINTF1(_L("Call CSdpAttrValueList::BuildEncodedL() function"));
		TRAPD(err, GetSdpAttrValueList()->BuildEncodedL(tprString8));
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("Call CSdpAttrValueList::BuildEncodedL() failed with error (%d)"), err);
			SetError(err);
			}
		CleanupStack::PopAndDestroy(string8);
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KExpected());
		SetBlockResult(EFail);
		}
	}
