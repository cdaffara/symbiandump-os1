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
// CATGprsContextDelete

#include "atgprscontextdelete.h" // header file for this source file
#include "mslogger.h"		 // for LOGTEXT macros

_LIT8(KATContextDelete,  "AT+CGDCONT=%d\r");

// ---------------------------------------------------------------------------
// CATGprsContextDelete::NewL
// AT state machine which will send AT+CGDCONT=x to delete a context from the phone.
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsContextDelete* CATGprsContextDelete::NewL(CGlobalPhonemanager& aGloblePhone, 
                                                 CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	CATGprsContextDelete* self = new(ELeave) CATGprsContextDelete(aGloblePhone,
			                                                      aCtsyDispatcherCallback);
	CleanupStack::PushL(self );
	self->ConstructL();
	CleanupStack::Pop(self );
	return self ;
	}

// ---------------------------------------------------------------------------
// CATGprsContextDelete::CATGprsContextDelete
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsContextDelete::CATGprsContextDelete(CGlobalPhonemanager& aGloblePhone, 
                                           CCtsyDispatcherCallback& aCtsyDispatcherCallback)
		:CAtCommandBase(aGloblePhone,aCtsyDispatcherCallback)
	{
	}

// ---------------------------------------------------------------------------
// CATGprsContextDelete::ConstructL()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextDelete::ConstructL()
	{
	CAtCommandBase::ConstructL();
	}

// ---------------------------------------------------------------------------
// CATGprsContextDelete::~CATGprsContextDelete()
// other items were commented in a header
// ---------------------------------------------------------------------------
CATGprsContextDelete::~CATGprsContextDelete()
	{
	}

// ---------------------------------------------------------------------------
// CATGprsContextDelete::StartRequest()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextDelete::StartRequest()
	{
	ExecuteCommand();
	}

// ---------------------------------------------------------------------------
// CATGprsContextDelete::ExecuteCommand()
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextDelete::ExecuteCommand()
	{
	LOGTEXT2(_L8("iCid=%d"),iContextId);
	// Send the AT+CGDCONT=x command to the phone
	iTxBuffer.Format(KATContextDelete,iContextId);
	Write();
	}

// ---------------------------------------------------------------------------
// CATGprsContextDelete::SetContext
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextDelete::SetContext(const TInt aContextId)
	{
	iContextId = aContextId;
	}

// ---------------------------------------------------------------------------
// CATGprsContextDelete::ParseResponseL
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextDelete::ParseResponseL(const TDesC8& /*aResponseBuf*/)
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
// CATGprsContextDelete::EventSignal
// other items were commented in a header
// ---------------------------------------------------------------------------
void CATGprsContextDelete::EventSignal(TAtEventSource aEventSource, TInt aStatus)
	{
	if((KErrNone ==aStatus)&&(aEventSource == EWriteCompletion))
		{
		return;
		}
	TName Name;
	if((KErrNone ==aStatus)&&(aEventSource == EReadCompletion)&&(iError==KErrNone))
		{
		TInt count = iPhoneGlobals.iContextList.Count();
		TInt i;
		//Get the context from context list
		for(i = 0; i < count;i++)
			{
			if(iPhoneGlobals.iContextList[i]->iContextID == iContextId)
				{
				Name = iPhoneGlobals.iContextList[i]->iInfo.iName;
				delete iPhoneGlobals.iContextList[i];
				iPhoneGlobals.iContextList[i] = NULL;
				iPhoneGlobals.iContextList.Remove(i);
				break;
				}
			}
		count = iPhoneGlobals.iContextList.Count();
		}
	iCtsyDispatcherCallback.CallbackPacketServicesDeletePdpContextComp(aStatus,Name);
	CAtCommandBase::Complete();
	iPhoneGlobals.iEventSignalActive = EFalse;
	}

// End of file
