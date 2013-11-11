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
// This contains TLtsyCallInformation which store baseband related call information
// 

//system include

//user include
#include "ltsycallinformation.h"
#include "ltsycommondefine.h"
#include "mslogger.h"

TLtsyCallInformation::TLtsyCallInformation()
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting TLtsyCallInformation::TLtsyCallInformation()"));
	
	iCallId = KLtsyErrorCallId;
	iUsedInfo = ENotUsed;
	iDirection = EUnknownDirection;
	iCallState = EUnknownState;
	iCallMode = EUnKnowCall;
	iIsConference = EUnknownIsConference;
	iTelNum = KNullDesC8;
	iAddressType = -1;
	iMobileName = KNullDesC8;
	iIsEmergencyCall = EFalse;
	}

void TLtsyCallInformation::SetCallId(TInt aCallId)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting TLtsyCallInformation::SetCallId()"));
	LOGTEXT2(_L8("[Ltsy CallControl] aCallId = %d"), aCallId);
	
	ASSERT((aCallId >= KLtsyMinCallId) && (aCallId <= KLtsyMaxCallId));
	
	iCallId = aCallId;
	}

TInt TLtsyCallInformation::GetCallId() const
	{
	return iCallId;
	}

void TLtsyCallInformation::SetCallIdIsUsedInfo(TCallIdUsedInfo aUsedInfo)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting TLtsyCallInformation::SetCallId()"));
	LOGTEXT2(_L8("[Ltsy CallControl] aUsedInfo = %d"), aUsedInfo);
	
	iUsedInfo = aUsedInfo;
	}

TLtsyCallInformation::TCallIdUsedInfo TLtsyCallInformation::GetCallIdIsUsedInfo() const
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting TLtsyCallInformation::GetCallIdIsUsedInfo()"));
	
	return iUsedInfo;
	}

void TLtsyCallInformation::SetCallDirection(TDirectionCall aDirection)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting TLtsyCallInformation::SetCallDirection()"));
	LOGTEXT2(_L8("[Ltsy CallControl] aDirection = %d"), aDirection);
	
	iDirection = aDirection;
	}

TLtsyCallInformation::TDirectionCall TLtsyCallInformation::GetCallDirection() const
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting TLtsyCallInformation::GetCallDirection()"));
	
	return iDirection;
	}

void TLtsyCallInformation::SetCallState(TCallState aCallState)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting TLtsyCallInformation::SetCallState()"));
	LOGTEXT2(_L8("[Ltsy CallControl] aCallState = %d"), aCallState);
	
	iCallState = aCallState; 
	}

TLtsyCallInformation::TCallState TLtsyCallInformation::GetCallState() const
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting TLtsyCallInformation::GetCallState()"));
	
	return iCallState;
	}

void TLtsyCallInformation::SetCallMode(TCallMode aCallMode)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting TLtsyCallInformation::SetCallMode()"));
	LOGTEXT2(_L8("[Ltsy CallControl] aCallMode = %d"), aCallMode);
	
	iCallMode = aCallMode;
	}

TLtsyCallInformation::TCallMode TLtsyCallInformation::GetCallMode() const
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting TLtsyCallInformation::GetCallMode()"));
	
	return iCallMode;
	}

void TLtsyCallInformation::SetConferenceCall(TConferenceCall aConferenceCall)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting TLtsyCallInformation::SetConferenceCall()"));
	LOGTEXT2(_L8("[Ltsy CallControl] aConferenceCall = %d"), aConferenceCall);
	
	iIsConference = aConferenceCall;
	}

TLtsyCallInformation::TConferenceCall TLtsyCallInformation::GetConferenceCall() const
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting TLtsyCallInformation::GetConferenceCall()"));
	
	return iIsConference;
	}

void TLtsyCallInformation::SetMobileTelNum(const TDesC8& aTelNum)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting TLtsyCallInformation::SetMobileTelNum()"));
	
	iTelNum.Copy(aTelNum.Left(iTelNum.MaxLength()));
	}

const TDesC8& TLtsyCallInformation::GetMobileTelNum() const
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting TLtsyCallInformation::GetMobileTelNum()"));
	
	return iTelNum;
	}

void TLtsyCallInformation::SetAddressType(TInt aAddressType)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting TLtsyCallInformation::SetAddressType()"));
	
	iAddressType = aAddressType;
	}

TInt TLtsyCallInformation::GetAddressType() const
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting TLtsyCallInformation::GetAddressType()"));
	
	return iAddressType;
	}

void TLtsyCallInformation::SetMobileName(const TDesC8& aMobileName)
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting TLtsyCallInformation::SetMobileName()"));
	
	iMobileName.Copy(aMobileName.Left(iMobileName.MaxLength()));
	}

const TDesC8& TLtsyCallInformation::GetMobileName() const
	{
	LOGTEXT(_L8("[Ltsy CallControl] Starting TLtsyCallInformation::GetMobileName()"));
	
	return iMobileName;
	}

void TLtsyCallInformation::SetEmergencyCallFlag(TBool aIsEmergency)
	{
	iIsEmergencyCall = aIsEmergency;
	}

TBool TLtsyCallInformation::GetEmergencyCallFlag() const
	{
	return iIsEmergencyCall;
	}

//End of file
