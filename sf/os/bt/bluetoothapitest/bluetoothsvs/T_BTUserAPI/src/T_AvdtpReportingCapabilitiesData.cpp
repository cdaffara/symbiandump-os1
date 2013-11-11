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
#include "T_AvdtpReportingCapabilitiesData.h"


/*@{*/
//Commands
_LIT(KCmdnew,											"new");
_LIT(KCmdDestructor,									"~"); 

//Log
_LIT(KLogError,											"Error=%d");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CT_AvdtpReportingCapabilitiesData* CT_AvdtpReportingCapabilitiesData::NewL()
	{
	CT_AvdtpReportingCapabilitiesData*	ret=new (ELeave) CT_AvdtpReportingCapabilitiesData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_AvdtpReportingCapabilitiesData::CT_AvdtpReportingCapabilitiesData()
:iAvdtpReportingCapabilities(NULL)
	{
	}

void CT_AvdtpReportingCapabilitiesData::ConstructL()
	{
	}

TAvdtpServiceCapability* CT_AvdtpReportingCapabilitiesData::GetAvdtpServiceCapability() const
	{
	return iAvdtpReportingCapabilities;
	}

CT_AvdtpReportingCapabilitiesData::~CT_AvdtpReportingCapabilitiesData()
	{
	DestroyData();
	}

void CT_AvdtpReportingCapabilitiesData::SetObjectL(TAny* aAny)
	{
	DestroyData();
	iAvdtpReportingCapabilities = static_cast<TAvdtpReportingCapabilities*> (aAny);
	}

void CT_AvdtpReportingCapabilitiesData::DisownObjectL()
	{
	iAvdtpReportingCapabilities = NULL;
	}

void CT_AvdtpReportingCapabilitiesData::DestroyData()
	{
	delete iAvdtpReportingCapabilities;
	iAvdtpReportingCapabilities=NULL;	
	}

TBool CT_AvdtpReportingCapabilitiesData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
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
Test TAvdtpReportingCapabilities()
*/
void CT_AvdtpReportingCapabilitiesData::DoCmdnewL()
	{
	INFO_PRINTF1(_L("TAvdtpReportingCapabilities Constructor Call"));
	DestroyData();
	TRAPD( err, iAvdtpReportingCapabilities = new (ELeave) TAvdtpReportingCapabilities());
	
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(KLogError, err);
		SetError(err);
		}
	}

