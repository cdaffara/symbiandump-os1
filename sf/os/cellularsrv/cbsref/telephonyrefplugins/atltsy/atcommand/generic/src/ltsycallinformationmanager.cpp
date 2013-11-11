// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This contains CLtsyCallInformationManager which manager call information
// 

//system include

//user include
#include "ltsycallinformationmanager.h"
#include "ltsycommondefine.h"
#include "athangupcommandrecords.h"
#include "mslogger.h"

CLtsyCallInformationManager* CLtsyCallInformationManager::NewL()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CLtsyCallInformationManager::NewL()"));
	
	CLtsyCallInformationManager* self = CLtsyCallInformationManager::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CLtsyCallInformationManager* CLtsyCallInformationManager::NewLC()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CLtsyCallInformationManager::NewLC()"));
	
	CLtsyCallInformationManager* self = new (ELeave) CLtsyCallInformationManager;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CLtsyCallInformationManager::~CLtsyCallInformationManager()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CLtsyCallInformationManager::~CLtsyCallInformationManager()"));
	
	delete iHangupCommandRecords;
	}

CLtsyCallInformationManager::CLtsyCallInformationManager()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CLtsyCallInformationManager::CLtsyCallInformationManager()"));
	//Init call id
	iCallInfo[1].SetCallId(1);
	iCallInfo[2].SetCallId(2);
	iCallInfo[3].SetCallId(3);
	iCallInfo[4].SetCallId(4);
	iCallInfo[5].SetCallId(5);
	iCallInfo[6].SetCallId(6);
	iCallInfo[7].SetCallId(7);
	
	//Init incoming call flag
	iIncomingCallFlag = EFalse;
	}

void CLtsyCallInformationManager::ConstructL()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CLtsyCallInformationManager::ConstructL()"));
	
	//Create hang up call command records
	iHangupCommandRecords = CATHangupCommandRecords::NewL();	
	}

void CLtsyCallInformationManager::ResetCallInformationByCallId(TInt aCallId)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CLtsyCallInformationManager::ResetCallInformationByCallId()"));
	
	ASSERT((aCallId >= KLtsyMinCallId) && (aCallId <= KLtsyMaxCallId));
	
	iCallInfo[aCallId].SetCallIdIsUsedInfo(TLtsyCallInformation::ENotUsed);
	iCallInfo[aCallId].SetCallDirection(TLtsyCallInformation::EUnknownDirection);
	iCallInfo[aCallId].SetCallState(TLtsyCallInformation::EUnknownState);
	iCallInfo[aCallId].SetCallMode(TLtsyCallInformation::EUnKnowCall);
	iCallInfo[aCallId].SetConferenceCall(TLtsyCallInformation::EUnknownIsConference);
	iCallInfo[aCallId].SetMobileTelNum(KNullDesC8);
	iCallInfo[aCallId].SetAddressType(-1);
	iCallInfo[aCallId].SetMobileName(KNullDesC8);
	iCallInfo[aCallId].SetEmergencyCallFlag(EFalse);
	}

void CLtsyCallInformationManager::ResetAllCalInformation()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CLtsyCallInformationManager::ResetAllCalInformation()"));
	
	for (TInt n = KLtsyMinCallId; n <= KLtsyMaxCallId; n++)
		{
		ResetCallInformationByCallId(n);
		}	
	}

TLtsyCallInformation& CLtsyCallInformationManager::GetCallInformationByCallId(TInt aCallId)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CLtsyCallInformationManager::GetCallInformationByCallId()"));
	
	ASSERT((aCallId >= KLtsyMinCallId) && (aCallId <= KLtsyMaxCallId));
	
	return iCallInfo[aCallId];
	}

TInt CLtsyCallInformationManager::FindUnUesedCallId() const
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CLtsyCallInformationManager::FindUnUesedCallId()"));
	
	for (TInt n = KLtsyMinCallId; n <= KLtsyMaxCallId; n++)
		{
		if (iCallInfo[n].GetCallIdIsUsedInfo() == TLtsyCallInformation::ENotUsed)
			{
			return n;
			}
		}
	return KErrNotFound;
	}

TBool CLtsyCallInformationManager::IsHaveUsedCallId() const
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting CLtsyCallInformationManager::FindIsHaveUsedCallId()"));
	
	for (TInt n = KLtsyMinCallId; n <= KLtsyMaxCallId; n++)
		{
		if (iCallInfo[n].GetCallIdIsUsedInfo() == TLtsyCallInformation::EUsed)
			{
			return ETrue;
			}
		}
	return EFalse;	
	}

void CLtsyCallInformationManager::SetIncomingCallFlag(TBool aIncomingCallFlag)
	{
	iIncomingCallFlag = aIncomingCallFlag;
	}

TBool CLtsyCallInformationManager::GetIncomingCallFlag() const
	{
	return iIncomingCallFlag;
	}

TInt CLtsyCallInformationManager::FindIncomingCallId() const
	{
	for (TInt n = KLtsyMinCallId; n <= KLtsyMaxCallId; n++)
		{
		if (iCallInfo[n].GetCallIdIsUsedInfo() == TLtsyCallInformation::EUsed &&
			iCallInfo[n].GetCallState() == TLtsyCallInformation::EInComingCall)
			{
			return n;
			}
		}
	return KErrNotFound;
	}

TInt CLtsyCallInformationManager::FindEmergencyCallId() const
	{
	for (TInt n = KLtsyMinCallId; n <= KLtsyMaxCallId; n++)
		{
		if ((iCallInfo[n].GetCallIdIsUsedInfo() == TLtsyCallInformation::EUsed) &&
			iCallInfo[n].GetEmergencyCallFlag())
			{
			return n;
			}
		}
	return KErrNotFound;	
	}

TInt CLtsyCallInformationManager::FindDialingOrAlertingCallId() const
	{
	for (TInt n = KLtsyMinCallId; n <= KLtsyMaxCallId; n++)
		{
		if (iCallInfo[n].GetCallIdIsUsedInfo() == TLtsyCallInformation::EUsed &&
			((iCallInfo[n].GetCallState() == TLtsyCallInformation::EDialingCall) ||
			 (iCallInfo[n].GetCallState() == TLtsyCallInformation::EAlertingCall)))
			{
			return n;
			}
		}
	return KErrNotFound;	
	}

TInt CLtsyCallInformationManager::FindWaitingCallId() const
	{
	for (TInt n = KLtsyMinCallId; n <= KLtsyMaxCallId; n++)
		{
		if (iCallInfo[n].GetCallIdIsUsedInfo() == TLtsyCallInformation::EUsed &&
			iCallInfo[n].GetCallState() == TLtsyCallInformation::EWaitingCall)
			{
			return n;
			}
		}
	return KErrNotFound;	
	}

// ---------------------------------------------------------------------------
// CLtsyCallInformationManager::GetHangupCommandRecords
// other items were commented in a header
// ---------------------------------------------------------------------------
CATHangupCommandRecords& CLtsyCallInformationManager::GetHangupCommandRecords()
	{
	return (*iHangupCommandRecords);
	}

//End of file
