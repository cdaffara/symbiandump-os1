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

#include "T_DataSdpAttrValueInt.h"
#include "T_BTUtil.h"

/*@{*/

//Parameters
_LIT(KInputInt,				"inputInt");
_LIT(KSdpIntBufObj,			"sdpintbufobj");

//Commands
_LIT(KCmdNewIntL,		"NewIntL");
_LIT(KCmdDestructor,	"~");
/*@}*/

// const TInt KInterval = 10000;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataSdpAttrValueInt* CT_DataSdpAttrValueInt::NewL()
	{
	CT_DataSdpAttrValueInt*	ret=new (ELeave) CT_DataSdpAttrValueInt();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataSdpAttrValueInt::CT_DataSdpAttrValueInt()
:   iAttrValInt(NULL)
	{
	}

void CT_DataSdpAttrValueInt::ConstructL()
	{
	}

CSdpAttrValue* CT_DataSdpAttrValueInt::GetSdpAttrValue() const
	{
	return iAttrValInt;
	}

CT_DataSdpAttrValueInt::~CT_DataSdpAttrValueInt()
	{
	DestroyData();
	}

void CT_DataSdpAttrValueInt::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iAttrValInt = static_cast<CSdpAttrValueInt*> (aAny);
	}

void CT_DataSdpAttrValueInt::DisownObjectL()
	{
	iAttrValInt = NULL;
	}

void CT_DataSdpAttrValueInt::DestroyData()
	{
	delete iAttrValInt;
	iAttrValInt=NULL;	
	}

inline TCleanupOperation CT_DataSdpAttrValueInt::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataSdpAttrValueInt::CleanupOperation(TAny* aAny)
	{
	CSdpAttrValueInt* arg=static_cast<CSdpAttrValueInt*>(aAny);
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
TBool CT_DataSdpAttrValueInt::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
{
	TBool	ret=ETrue;

	if ( aCommand==KCmdNewIntL )
		{
		DoCmdNewIntL(aSection);         
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


void CT_DataSdpAttrValueInt::DoCmdNewIntL(const TDesC& aSection)
	{
	DestroyData();
	TPtrC	inputInt;
	TPtrC	sdpIntBufName;
	TBool	foundParameter=EFalse;
	if ( GetStringFromConfig(aSection, KInputInt(), inputInt) )
		{
		foundParameter=ETrue;	
		HBufC8*	buffer=HBufC8::NewLC(inputInt.Length());
		TPtr8	bufferPtr=buffer->Des();
		bufferPtr.Copy(inputInt);
       	TRAPD(err, iAttrValInt = CSdpAttrValueInt::NewIntL(bufferPtr));	
	    if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("CSdpAttrValueInt::NewIntL failed with error %d"), err);
			SetError(err);
			}
		CleanupStack::PopAndDestroy(buffer);
		}
		
	if ( GetStringFromConfig(aSection, KSdpIntBufObj(), sdpIntBufName) )
		{
		foundParameter=ETrue;
		TAny* object=GetDataObjectL(sdpIntBufName);
		TSdpIntBuf<TUint8>*	sdpIntBuf = static_cast<TSdpIntBuf<TUint8>*>(object);
		TSdpIntBuf<TUint8>	sdpIntObj = *sdpIntBuf;
		const TUint8 *tmp=&sdpIntObj[0];			
		TPtrC8 bufferPtr(tmp, sdpIntObj.Length());
		TRAPD(err, iAttrValInt = CSdpAttrValueInt::NewIntL(bufferPtr));
	    if ( err!=KErrNone )
			{
			ERR_PRINTF2(_L("CSdpAttrValueInt::NewIntL failed with error %d"), err);
			SetError(err);
			}		
		}
		
	if (!foundParameter)
		{
		ERR_PRINTF2(_L("Missing parameter %S"), &KSdpIntBufObj());
		SetBlockResult(EFail);
		}
	}

void CT_DataSdpAttrValueInt::DoCmdDestructor()
	{
    DestroyData();
	}
