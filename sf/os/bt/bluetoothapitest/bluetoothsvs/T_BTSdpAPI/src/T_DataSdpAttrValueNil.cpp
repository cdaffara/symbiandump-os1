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

#include "T_DataSdpAttrValueNil.h"
/*@{*/

//Commands
_LIT(KCmdNewNilL,		"NewNilL");
_LIT(KCmdDestructor,	"~");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_DataSdpAttrValueNil* CT_DataSdpAttrValueNil::NewL()
	{
	CT_DataSdpAttrValueNil*	ret=new (ELeave) CT_DataSdpAttrValueNil();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataSdpAttrValueNil::CT_DataSdpAttrValueNil()
:   iAttrValNil(NULL)
	{
	}

void CT_DataSdpAttrValueNil::ConstructL()
	{
	}

CSdpAttrValue* CT_DataSdpAttrValueNil::GetSdpAttrValue() const
	{
	return iAttrValNil;
	}

CT_DataSdpAttrValueNil::~CT_DataSdpAttrValueNil()
	{
	DestroyData();
	}

void CT_DataSdpAttrValueNil::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iAttrValNil = static_cast<CSdpAttrValueNil*> (aAny);
	}

void CT_DataSdpAttrValueNil::DisownObjectL()
	{
	iAttrValNil = NULL;
	}

void CT_DataSdpAttrValueNil::DestroyData()
	{
	delete iAttrValNil;
	iAttrValNil=NULL;	
	}

inline TCleanupOperation CT_DataSdpAttrValueNil::CleanupOperation()
	{
	return CleanupOperation;
	}

void CT_DataSdpAttrValueNil::CleanupOperation(TAny* aAny)
	{
	CSdpAttrValueNil* arg=static_cast<CSdpAttrValueNil*>(aAny);
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
TBool CT_DataSdpAttrValueNil::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret = ETrue;

	if ( aCommand==KCmdNewNilL )
		{
		DoCmdNewNil();
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

void CT_DataSdpAttrValueNil::DoCmdNewNil()
	{
	DestroyData();
	INFO_PRINTF1(_L("CSdpAttrValueNil NewNilL Call."));
	TRAPD(err, iAttrValNil = CSdpAttrValueNil::NewNilL());
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("CSdpAttrValueNil::NewNilL failed with error %d"), err);
		SetError(err);
		}
	}

void CT_DataSdpAttrValueNil::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("CSdpAttrValueNil Destructor Call."));
	DestroyData();
	}
