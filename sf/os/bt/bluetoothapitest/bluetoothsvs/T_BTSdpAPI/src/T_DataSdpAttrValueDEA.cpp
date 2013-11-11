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

#include "T_DataSdpAttrValueDEA.h"
#include "T_BTUtil.h"


/*@{*/
//Parameters
_LIT(KObjectName,			"objectname");
_LIT(KTypeName,				"typename");
//Commands
_LIT(KCmdNewDEAL,			"NewDEAL");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataSdpAttrValueDEA* CT_DataSdpAttrValueDEA::NewL()
	{
	CT_DataSdpAttrValueDEA* ret=new (ELeave) CT_DataSdpAttrValueDEA();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


CT_DataSdpAttrValueDEA::CT_DataSdpAttrValueDEA()
:	CT_DataSdpAttrValueList()
,	iAttrValueDEA(NULL)
	{
	}

void CT_DataSdpAttrValueDEA::ConstructL()
	{
	CT_DataSdpAttrValueList::ConstructL();
	}

CSdpAttrValueList* CT_DataSdpAttrValueDEA::GetSdpAttrValueList() const
	{
	return iAttrValueDEA;
	}

void CT_DataSdpAttrValueDEA::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iAttrValueDEA = static_cast<CSdpAttrValueDEA*> (aAny);
	}

void CT_DataSdpAttrValueDEA::DisownObjectL()
	{
	iAttrValueDEA = NULL;
	}

CT_DataSdpAttrValueDEA::~CT_DataSdpAttrValueDEA()
	{
	DestroyData();
	}


void CT_DataSdpAttrValueDEA::DestroyData()
	{
	delete iAttrValueDEA;
	iAttrValueDEA=NULL;
	}

inline TCleanupOperation CT_DataSdpAttrValueDEA::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataSdpAttrValueDEA::CleanupOperation(TAny* aAny)
	{
	CSdpAttrValueDEA* value=static_cast<CSdpAttrValueDEA*>(aAny);
	delete value;
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
TBool CT_DataSdpAttrValueDEA::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdNewDEAL )
		{
		DoCmdNewDEAL(aSection);
		}
	else
		{
		ret=CT_DataSdpAttrValueList::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	return ret;
	}


/**
 Test CSdpAttrValueDEA's NewDEAL() function
*/
void CT_DataSdpAttrValueDEA::DoCmdNewDEAL(const TDesC& aSection)
	{
	DestroyData();
	
	MSdpElementBuilder*					elementBuilder = NULL;
	TPtrC 								objectName;
	TPtrC								objectType;
	if ( GetStringFromConfig(aSection, KObjectName(), objectName) )
		{
		if ( GetStringFromConfig(aSection, KTypeName(), objectType) )
			{
			elementBuilder = CT_BTUtil::ElementBuilderCastL(*this, objectName, objectType);
			INFO_PRINTF1(_L("Create a nested list using CSdpAttrValueDEA::NewDEAL()"));
			TRAPD( err, iAttrValueDEA=CSdpAttrValueDEA::NewDEAL(elementBuilder) );
			if (err!=KErrNone)
				{
				ERR_PRINTF2(_L("CSdpAttrValueDEA::NewDEAL function failed with error (%d)"), err);
				SetError(err);
				}
			}
		else
			{
			ERR_PRINTF2(_L("Missing parameter %S"), &KTypeName());
			SetBlockResult(EFail);	
			}
		}
	else
		{
		INFO_PRINTF1(_L("Create a list is not nested using CSdpAttrValueDEA::NewDEAL(NULL)"));
		TRAPD( errCode, iAttrValueDEA=CSdpAttrValueDEA::NewDEAL(elementBuilder) );
		if (errCode!=KErrNone)
			{
			ERR_PRINTF2(_L("CSdpAttrValueDEA::NewDEAL function failed with error (%d)"), errCode);
			SetError(errCode);
			}
		}

	}
