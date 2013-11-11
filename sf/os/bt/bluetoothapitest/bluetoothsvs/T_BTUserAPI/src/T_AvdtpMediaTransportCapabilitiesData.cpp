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

#include <bluetoothav.h>

//	User Includes
#include "T_AvdtpServiceCapabilityData.h"
#include "T_AvdtpMediaTransportCapabilitiesData.h"

/*@{*/
//Commands
_LIT(KCmdnew,								"new");
_LIT(KCmdDestructor,					"~"); 

//Log
_LIT(KLogError,											"Error=%d");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_AvdtpMediaTransportCapabilitiesData* CT_AvdtpMediaTransportCapabilitiesData::NewL()
	{
	CT_AvdtpMediaTransportCapabilitiesData*	ret=new (ELeave) CT_AvdtpMediaTransportCapabilitiesData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_AvdtpMediaTransportCapabilitiesData::CT_AvdtpMediaTransportCapabilitiesData()
:iAvdtpMediaTransportCapabilities(NULL)
	{
	}

void CT_AvdtpMediaTransportCapabilitiesData::ConstructL()
	{
	}

TAvdtpServiceCapability* CT_AvdtpMediaTransportCapabilitiesData::GetAvdtpServiceCapability() const
	{
	return iAvdtpMediaTransportCapabilities;
	}

CT_AvdtpMediaTransportCapabilitiesData::~CT_AvdtpMediaTransportCapabilitiesData()
	{
	DestroyData();
	}

void CT_AvdtpMediaTransportCapabilitiesData::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iAvdtpMediaTransportCapabilities = static_cast<TAvdtpMediaTransportCapabilities*> (aAny);
	}

void CT_AvdtpMediaTransportCapabilitiesData::DisownObjectL()
	{
	iAvdtpMediaTransportCapabilities = NULL;
	}

void CT_AvdtpMediaTransportCapabilitiesData::DestroyData()
	{
	delete iAvdtpMediaTransportCapabilities;
	iAvdtpMediaTransportCapabilities=NULL;	
	}


TBool CT_AvdtpMediaTransportCapabilitiesData::DoCommandL(const TTEFFunction& aCommand,
		const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool	ret = ETrue;
	
	if ( aCommand==KCmdnew )
		{
		DoCmdnewL();
		}
	else if ( aCommand==KCmdDestructor )
		{
		DestroyData();
		}
	else
		{
		ret=CT_AvdtpServiceCapabilityData::DoCommandL(aCommand, aSection, aAsyncErrorIndex);
		}

	return ret;
	}


/**
Test TAvdtpMediaTransportCapabilities()
*/
void CT_AvdtpMediaTransportCapabilitiesData::DoCmdnewL()
	{
	INFO_PRINTF1(_L("TAvdtpMediaTransportCapabilities Constructor Call"));
	DestroyData();
	TRAPD( err, iAvdtpMediaTransportCapabilities = new (ELeave) TAvdtpMediaTransportCapabilities());
	
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(KLogError, err);
		SetError(err);
		}
	
	}
