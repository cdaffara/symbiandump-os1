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

#include "T_DataCSdpAttrValueURL.h"

/*@{*/
//Commands
_LIT(KCmdNewURLL,		"NewURLL");
_LIT(KCmdDestructor,	"~");

//Parameters
_LIT(KNewObject,		"object");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataCSdpAttrValueURL* CT_DataCSdpAttrValueURL::NewL()
	{
	CT_DataCSdpAttrValueURL*	ret=new (ELeave) CT_DataCSdpAttrValueURL();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataCSdpAttrValueURL::CT_DataCSdpAttrValueURL()
:	iAttrValueURL(NULL)
	{
	}

void CT_DataCSdpAttrValueURL::ConstructL()
	{
	}

CSdpAttrValue* CT_DataCSdpAttrValueURL::GetSdpAttrValue() const
	{
	return iAttrValueURL;
	}

CT_DataCSdpAttrValueURL::~CT_DataCSdpAttrValueURL()
	{
	DestroyData();
	}

void CT_DataCSdpAttrValueURL::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iAttrValueURL = static_cast<CSdpAttrValueURL*> (aAny);
	}

void CT_DataCSdpAttrValueURL::DisownObjectL()
	{
	iAttrValueURL = NULL;
	}

void CT_DataCSdpAttrValueURL::DestroyData()
	{
	delete iAttrValueURL;
	iAttrValueURL=NULL;	
	}

inline TCleanupOperation CT_DataCSdpAttrValueURL::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataCSdpAttrValueURL::CleanupOperation(TAny* aAny)
	{
	CSdpAttrValueURL* valueURL=static_cast<CSdpAttrValueURL*>(aAny);
	delete valueURL;
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
TBool CT_DataCSdpAttrValueURL::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdNewURLL )
		{
		DoCmdNewURLL(aSection);
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


/**
Test NewURLL()
*/
void CT_DataCSdpAttrValueURL::DoCmdNewURLL(const TDesC& aSection)
	{

	DestroyData();
	TPtrC	theString;
	if ( GetStringFromConfig(aSection, KNewObject(), theString) )
		{
		HBufC8*	string8 = HBufC8::NewLC(theString.Length());
		string8->Des().Copy(theString);
		TPtrC8 stringPtr = string8->Des();
		TRAPD(err, iAttrValueURL = CSdpAttrValueURL::NewURLL(stringPtr));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("CSdpAttrValueURL NewURLL failed with error %d"), err);
			SetError(err);
			}
		CleanupStack::PopAndDestroy(string8);
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KNewObject());
		SetBlockResult(EFail);
		}
	}

/**
Test ~CSdpAttrValueURL()
*/
void CT_DataCSdpAttrValueURL::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("CSdpAttrValueURL Destructor Call"));

	DestroyData();
	}
