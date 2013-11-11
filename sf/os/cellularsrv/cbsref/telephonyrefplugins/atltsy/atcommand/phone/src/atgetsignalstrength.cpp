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
// CATGetModemStatus
// Description:
// CATGetSignal

#include "atgetsignalstrength.h"
#include "mslogger.h"


_LIT8(KGetSignalQualityCommand,"AT+CSQ\r");
_LIT8(KGetSingalCommandRepStr,"+CSQ:");
// Class CATGetSignal
// ---------------------------------------------------------------------------
// CATGetSignal::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetSignal* CATGetSignal::NewL(CGlobalPhonemanager& aGloblePhone, 
	                             CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGetSignal* self = new(ELeave) CATGetSignal(aGloblePhone,
			                                      aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATGetSignal::CATGetSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetSignal::CATGetSignal(CGlobalPhonemanager& aGloblePhone, 
	                       CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATGetSignal::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetSignal::ConstructL()
	{
	CAtCommandBase::ConstructL();
	iAtType = ELtsyAT_Phone_GetSignal;
	}
// ---------------------------------------------------------------------------
// CATGetSignal::~CATGetSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGetSignal::~CATGetSignal()
	{
	}
// ---------------------------------------------------------------------------
// CATGetSignal::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetSignal::StartRequest()
	{
	ExecuteCommand();
	}
// ---------------------------------------------------------------------------
// CATGetSignal::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetSignal::ExecuteCommand( )
	{
	iTxBuffer.Copy(KGetSignalQualityCommand);
	Write();
	}
// ---------------------------------------------------------------------------
// CATGetSignal::getSignalInfo
// other items were commented in a header
// ---------------------------------------------------------------------------
TTsySignalInfo CATGetSignal::getSignalInfo()
	{
	return iSignalInfo;
	}
// ---------------------------------------------------------------------------
// CATGetSignal::GetErrorValue
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATGetSignal::GetErrorValue()
	{
	return iError;
	}
// ---------------------------------------------------------------------------
// CATGetSignal::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetSignal::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	if (CurrentLine().Match(KLtsyOkString) != 0)
		{
		iError = KErrGeneral;
		return ;
		}
	RArray<TPtrC8> array;
	CleanupClosePushL(array);
	iParser->ParseRespondedBuffer(array,PrecedingLine());
	TInt Count = array.Count();
	if (Count <= 1)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrGeneral;
		return ;
		}
	if( array[0].MatchF(KGetSingalCommandRepStr)==KErrNotFound)
		{
		CleanupStack::PopAndDestroy();
		iError = KErrGeneral;
		return ;
		}
	TInt val = 0;
	TLex8 lex(array[1]);
	lex.Val(val);
	if (val < 0) 
		{
		iSignalInfo.iSignalStrength = 0;
		}
	else	
		{
		iSignalInfo.iSignalStrength = val;
		}
	TLex8 lex1(array[2]);
	lex1.Val(val);
	if (val < 0) 
		{
		iSignalInfo.iBar = 0;
		}
	else	
		{
		iSignalInfo.iBar = val;
		}

	CleanupStack::PopAndDestroy();
	iError = KErrNone;
	}
// ---------------------------------------------------------------------------
// CATGetSignal::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGetSignal::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	if(KErrNone ==aStatus)
		{
		if(aEventSource == EReadCompletion)
			{
			aStatus = iError;
			}
		else
			return;
		}
	CAtCommandBase::Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	iCtsyDispatcherCallback.CallbackPhoneGetSignalStrengthComp(aStatus,
							                                   iSignalInfo.iSignalStrength,
							                                   iSignalInfo.iBar);
	}
//
// End of file

