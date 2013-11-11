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

#include "T_DataSdpAttrValueBoolean.h"

/*@{*/


//Parameters
_LIT(KInputBool,		"inputbool");

//Commands
_LIT(KCmdNewBoolL,		"NewBoolL");
_LIT(KCmdDestructor,	"~");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataSdpAttrValueBoolean* CT_DataSdpAttrValueBoolean::NewL()
	{
	CT_DataSdpAttrValueBoolean*	ret=new (ELeave) CT_DataSdpAttrValueBoolean();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataSdpAttrValueBoolean::CT_DataSdpAttrValueBoolean()
:   iAttrValBool(NULL)
	{
	}

void CT_DataSdpAttrValueBoolean::ConstructL()
	{
	}

CT_DataSdpAttrValueBoolean::~CT_DataSdpAttrValueBoolean()
	{
	DestroyData();
	}

CSdpAttrValue* CT_DataSdpAttrValueBoolean::GetSdpAttrValue() const
	{
	return iAttrValBool;
	}

void CT_DataSdpAttrValueBoolean::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iAttrValBool = static_cast<CSdpAttrValueBoolean*> (aAny);
	}

void CT_DataSdpAttrValueBoolean::DisownObjectL()
	{
	iAttrValBool = NULL;
	}

void CT_DataSdpAttrValueBoolean::DestroyData()
	{
	delete iAttrValBool;
	iAttrValBool=NULL;	
	}


inline TCleanupOperation CT_DataSdpAttrValueBoolean::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataSdpAttrValueBoolean::CleanupOperation(TAny* aAny)
	{
	CSdpAttrValueBoolean* value=static_cast<CSdpAttrValueBoolean*>(aAny);
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
TBool CT_DataSdpAttrValueBoolean::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret = ETrue;

	if ( aCommand==KCmdNewBoolL )
	   	{
		DoCmdNewBool(aSection);
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

void CT_DataSdpAttrValueBoolean::DoCmdNewBool(const TDesC& aSection)
	{
	DestroyData();

	TBool	inputBool = EFalse;
	if ( GetBoolFromConfig(aSection, KInputBool(), inputBool) )
		{
		TRAPD(err, iAttrValBool = CSdpAttrValueBoolean::NewBoolL(inputBool));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("CSdpAttrValueBoolean::NewBoolL failed with error %d"), err);
			SetError(err);
			}
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KInputBool());
		SetBlockResult(EFail);
		}
	}

void CT_DataSdpAttrValueBoolean::DoCmdDestructor()
	{
	DestroyData();
	}
