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

#include "T_DataSdpAttrValueDES.h"
#include "T_BTUtil.h"

/*@{*/
//Parameters
_LIT(KObjectName,			"objectname");
_LIT(KTypeName,				"typename");
//Commands
_LIT(KCmdNewDESL,			"NewDESL");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataSdpAttrValueDES* CT_DataSdpAttrValueDES::NewL()
	{
	CT_DataSdpAttrValueDES* ret=new (ELeave) CT_DataSdpAttrValueDES();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}


CT_DataSdpAttrValueDES::CT_DataSdpAttrValueDES()
:	CT_DataSdpAttrValueList()
,	iAttrValueDES(NULL)
	{
	}

void CT_DataSdpAttrValueDES::ConstructL()
	{
	CT_DataSdpAttrValueList::ConstructL();
	}

CSdpAttrValueList* CT_DataSdpAttrValueDES::GetSdpAttrValueList() const
	{
	return iAttrValueDES;
	}

void CT_DataSdpAttrValueDES::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iAttrValueDES = static_cast<CSdpAttrValueDES*> (aAny);
	}

void CT_DataSdpAttrValueDES::DisownObjectL()
	{
	iAttrValueDES = NULL;
	}

CT_DataSdpAttrValueDES::~CT_DataSdpAttrValueDES()
	{
	DestroyData();
	}


void CT_DataSdpAttrValueDES::DestroyData()
	{
	delete iAttrValueDES;
	iAttrValueDES=NULL;
	}

inline TCleanupOperation CT_DataSdpAttrValueDES::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataSdpAttrValueDES::CleanupOperation(TAny* aAny)
	{
	CSdpAttrValueDES* arg=static_cast<CSdpAttrValueDES*>(aAny);
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
TBool CT_DataSdpAttrValueDES::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdNewDESL )
		{
		DoCmdNewDESL(aSection);
		}
	else
		{
		ret=CT_DataSdpAttrValueList::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	return ret;
	}


/**
 Test CSdpAttrValueDES's NewDESL() function
*/
void CT_DataSdpAttrValueDES::DoCmdNewDESL(const TDesC& aSection)
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
			INFO_PRINTF1(_L("Create a nested list using CSdpAttrValueDES::NewDESL()"));
			TRAPD( err, iAttrValueDES=CSdpAttrValueDES::NewDESL(elementBuilder) );
			if (err!=KErrNone)
				{
				ERR_PRINTF2(_L("CSdpAttrValueDES::NewDESL function failed with error (%d)"), err);
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
		INFO_PRINTF1(_L("Create a list is not nested using CSdpAttrValueDES::NewDESL(NULL)"));
		TRAPD( errCode, iAttrValueDES=CSdpAttrValueDES::NewDESL(elementBuilder) );
		if (errCode!=KErrNone)
			{
			ERR_PRINTF2(_L("CSdpAttrValueDES::NewDESL function failed with error (%d)"), errCode);
			SetError(errCode);
			}
		}
	}
