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

#include "T_DataSdpAttrValueUint.h"


/*@{*/
//Parameters
_LIT(KInputInt,			"inputInt");
_LIT(KSdpIntBufObj,		"sdpintbufobj");

//Commands
_LIT(KCmdNewUintL,		"NewUintL");
_LIT(KCmdDestructor,	"~");
/*@}*/

// const TInt KInterval = 10000;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataSdpAttrValueUint* CT_DataSdpAttrValueUint::NewL()
	{
	CT_DataSdpAttrValueUint*	ret=new (ELeave) CT_DataSdpAttrValueUint();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataSdpAttrValueUint::CT_DataSdpAttrValueUint()
:   iAttrValUint(NULL)
	{
	}

void CT_DataSdpAttrValueUint::ConstructL()
	{
	}

CSdpAttrValue* CT_DataSdpAttrValueUint::GetSdpAttrValue() const
	{
	return iAttrValUint;
	}

CT_DataSdpAttrValueUint::~CT_DataSdpAttrValueUint()
	{
	DestroyData();
	}

void CT_DataSdpAttrValueUint::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iAttrValUint = static_cast<CSdpAttrValueUint*> (aAny);
	}

void CT_DataSdpAttrValueUint::DisownObjectL()
	{
	iAttrValUint = NULL;
	}

void CT_DataSdpAttrValueUint::DestroyData()
	{
	delete iAttrValUint;
	iAttrValUint=NULL;	
	}

inline TCleanupOperation CT_DataSdpAttrValueUint::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataSdpAttrValueUint::CleanupOperation(TAny* aAny)
	{
	CSdpAttrValueUint* arg=static_cast<CSdpAttrValueUint*>(aAny);
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
TBool CT_DataSdpAttrValueUint::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret=ETrue;

	if ( aCommand==KCmdNewUintL )
		{
		DoCmdNewUintL(aSection);         
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

void CT_DataSdpAttrValueUint::DoCmdNewUintL(const TDesC& aSection)
	{
	
	DestroyData();
	TPtrC	theString;
	TPtrC	sdpIntBufName;
	TBool	foundParameter=EFalse;
	if ( GetStringFromConfig(aSection, KInputInt(), theString) )
		{
		foundParameter=ETrue;			
		HBufC8*	string8 = HBufC8::NewLC(theString.Length());
		string8->Des().Copy(theString);
		TPtrC8 stringPtr = string8->Des();
		TRAPD(err, iAttrValUint = CSdpAttrValueUint::NewUintL(stringPtr));
		if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("CSdpAttrValueUint::NewUintL failed with error %d"), err);
			SetError(err);
			}
		CleanupStack::PopAndDestroy(string8);
		}	   
		
	if ( GetStringFromConfig(aSection, KSdpIntBufObj(), sdpIntBufName) )
		{
		foundParameter=ETrue;
		TAny* object=GetDataObjectL(sdpIntBufName);
		TSdpIntBuf<TUint8>*	sdpIntBuf = static_cast<TSdpIntBuf<TUint8>*>(object);
		TSdpIntBuf<TUint8>	sdpIntObj = *sdpIntBuf;
		const TUint8 *tmp=&sdpIntObj[0];			
		TPtrC8 bufferPtr(tmp, sdpIntObj.Length());
		TRAPD(err, iAttrValUint = CSdpAttrValueUint::NewUintL(bufferPtr));
	    if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("CSdpAttrValueInt::NewUintL failed with error %d"), err);
			SetError(err);
			}		
		}
		
	if (!foundParameter)
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KSdpIntBufObj());
		SetBlockResult(EFail);
		}
	}

void CT_DataSdpAttrValueUint::DoCmdDestructor()
	{
	DestroyData();
	}
