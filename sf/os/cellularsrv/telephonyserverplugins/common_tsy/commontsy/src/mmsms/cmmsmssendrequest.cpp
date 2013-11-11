// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//



//  INCLUDE FILES
#include "cmmsmssendrequest.h"

// ======== MEMBER FUNCTIONS ========

CSmsSendRequest::CSmsSendRequest() : CBase()
    {
    iSendCounter = 0;
    }

CSmsSendRequest::~CSmsSendRequest()
    {
    }

//---------------------------------------------------------------------------- 
// CSmsSendRequest::GetSmsDataAndAttributes
// This method returns SMS data and attributes
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TSendSmsDataAndAttributes CSmsSendRequest::GetSmsDataAndAttributes() const
	{
	return iSmsDataAndAttributes;
	}
	
//---------------------------------------------------------------------------- 
// CSmsSendRequest::SetSmsDataAndAttributes
// This method sets SMS data and attributes
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CSmsSendRequest::SetSmsDataAndAttributes(
	TSendSmsDataAndAttributes const& aSmsDataAndAttributes )
	{
	iSmsDataAndAttributes = aSmsDataAndAttributes;
	}
	
//---------------------------------------------------------------------------- 
// CSmsSendRequest::IncreaseSendCounter
// This method increases iSmsSendCounter
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CSmsSendRequest::IncreaseSendCounter()
	{
	iSendCounter++;
	}

//---------------------------------------------------------------------------- 
// CSmsSendRequest::SetSmsDataAndAttributes
// This method gets iSmsSendCounter
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TUint8 CSmsSendRequest::GetSendCounter() const
	{
	return iSendCounter;
	}

//  End of File
