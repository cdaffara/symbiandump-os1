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

#include "T_DataSdpAttrValueString.h"

/*@{*/
//Parameters
_LIT(KInputString,		"inputstring");

//Commands
_LIT(KCmdNewStringL,	"NewStringL");
_LIT(KCmdDestructor,	"~");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataSdpAttrValueString* CT_DataSdpAttrValueString::NewL()
	{
	CT_DataSdpAttrValueString*	ret=new (ELeave) CT_DataSdpAttrValueString();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataSdpAttrValueString::CT_DataSdpAttrValueString()
:	iAttrValString(NULL)
	{
	}

void CT_DataSdpAttrValueString::ConstructL()
	{
	}

CSdpAttrValue* CT_DataSdpAttrValueString::GetSdpAttrValue() const
	{
	return iAttrValString;
	}

CT_DataSdpAttrValueString::~CT_DataSdpAttrValueString()
	{
	DestroyData();
	}

void CT_DataSdpAttrValueString::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iAttrValString = static_cast<CSdpAttrValueString*> (aAny);
	}

void CT_DataSdpAttrValueString::DisownObjectL()
	{
	iAttrValString = NULL;
	}

void CT_DataSdpAttrValueString::DestroyData()
	{
	delete iAttrValString;
	iAttrValString=NULL;
	}

inline TCleanupOperation CT_DataSdpAttrValueString::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataSdpAttrValueString::CleanupOperation(TAny* aAny)
	{
	CSdpAttrValueString* arg=static_cast<CSdpAttrValueString*>(aAny);
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
TBool CT_DataSdpAttrValueString::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdNewStringL )
		{
		DoCmdNewStringL(aSection);
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
 Test NewStringL function
 Constructs a new attribute (CSdpAttrValueString)
 param &aSection
*/
void CT_DataSdpAttrValueString::DoCmdNewStringL(const TDesC& aSection)
	{

	DestroyData();
	TPtrC	theString;
	if ( GetStringFromConfig(aSection, KInputString(), theString) )
		{
		HBufC8*	string8 = HBufC8::NewLC(theString.Length());
		string8->Des().Copy(theString);
		TPtrC8 stringPtr = string8->Des();
		TRAPD(err, iAttrValString = CSdpAttrValueString::NewStringL(stringPtr));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("NewStringL function failed with error %d"), err);
			SetError(err);
			}
		CleanupStack::PopAndDestroy(string8);
		}
	else
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KInputString());
		SetBlockResult(EFail);
		}
	}

/**
 Test CSdpAttrValueString Destructor function
*/
void CT_DataSdpAttrValueString::DoCmdDestructor()
	{
	DestroyData();
	}
