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
// This contains CATHelperCommand which is composite class of a number classes
// 

#include "athelpercommand.h"
#include "mslogger.h"

// Class CATHelperCommand
// ---------------------------------------------------------------------------
// CATHelperCommand::NewL
// other items were commented in a header
// ---------------------------------------------------------------------------
CATHelperCommand* CATHelperCommand::NewL(CGlobalPhonemanager& aGloblePhone, 
	                                       CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATHelperCommand* self = new(ELeave) CATHelperCommand(aGloblePhone,
			                                                aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}
// ---------------------------------------------------------------------------
// CATHelperCommand::CATHelperCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
CATHelperCommand::CATHelperCommand(CGlobalPhonemanager& aGloblePhone, 
	                                 CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}
// ---------------------------------------------------------------------------
// CATHelperCommand::ConstructL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATHelperCommand::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}
// ---------------------------------------------------------------------------
// CATHelperCommand::~CATHelperCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
CATHelperCommand::~CATHelperCommand()
	{
	}
// ---------------------------------------------------------------------------
// CATHelperCommand::StartRequestL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATHelperCommand::StartRequest()
	{
	ExecuteCommand();
	}
// ---------------------------------------------------------------------------
// CATHelperCommand::ExecuteCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATHelperCommand::ExecuteCommand()
	{
	iTxBuffer.Copy(iATBuffer);
	Write();
	}
// ---------------------------------------------------------------------------
// CATHelperCommand::CancelCommand
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATHelperCommand::CancelCommand()
	{	
	}
// ---------------------------------------------------------------------------
// CATHelperCommand::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATHelperCommand::ParseResponseL(const TDesC8& /*aResponseBuf*/)
	{
	if (CurrentLine().Match(KLtsyOkString) == 0)
		{
		iError = KErrNone;
		}
	else
		{
		iError = KErrGeneral;
		}
	}
// ---------------------------------------------------------------------------
// CATHelperCommand::GetErrorValue
// other items were commented in a header
// ---------------------------------------------------------------------------
TInt CATHelperCommand::GetErrorValue()
	{
	return iError;
	}
// ---------------------------------------------------------------------------
// CATHelperCommand::SetAtCommandString
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATHelperCommand::SetAtCommandString(const TDesC8& aATString)
	{
	iATBuffer.Copy(aATString);
	}
// ---------------------------------------------------------------------------
// CATHelperCommand::StartATType
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATHelperCommand::SetATType(TLtsyATCommandType aATType)
	{
	iAtType = aATType;
	}
//
// End of file
