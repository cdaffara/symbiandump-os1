#include <e32debug.h>
// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
 @deprecated
*/
#include "socketreader.h"

#include "supldevloggermacros.h"

#include "suplasn1decoder.h"
#include "suplmessagebase.h"


CSocketReader* CSocketReader::NewL(RSocket& aSocket, MSuplSocketObserver& aObserver, TUint aCallbackId)
	{
	SUPLLOG(ELogP1, "CSocketReader::NewL() Begin\n");
	CSocketReader* self = new (ELeave) CSocketReader(aSocket, aObserver, aCallbackId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	SUPLLOG(ELogP1, "CSocketReader::NewL() End\n");
	return self;
	}

CSocketReader::~CSocketReader()
	{
	SUPLLOG(ELogP1, "CSocketReader::~CSocketReader() Begin\n");
	Cancel();
	//iSocket.Close(); => Passed in as reference
	iBuffer.Close();
	delete iDecoder;
	SUPLLOG(ELogP1, "CSocketReader::~CSocketReader() End\n");
	}

CSocketReader::CSocketReader(RSocket& aSocket, MSuplSocketObserver& aObserver, TUint aCallbackId) :
	CActive(EPriorityStandard), iSocket(aSocket), iObserver(aObserver), iIsFirstFragment(ETrue), 
	iCurrentMessageSize(0), iReadSoFar(0), iPtr(NULL, 0), iCallbackId(aCallbackId)
	{
	SUPLLOG(ELogP1, "CSocketReader::CSocketReader() Begin\n");
	CActiveScheduler::Add(this);
	SUPLLOG(ELogP1, "CSocketReader::CSocketReader() End\n");
	}

void CSocketReader::ConstructL()
	{
	SUPLLOG(ELogP1, "CSocketReader::ConstructL() Begin\n");
	iBuffer.CreateL(KSuplMaxMessageLength);
	iBuffer.SetLength(iBuffer.MaxLength());
	iDecoder = CSuplAsn1Decoder::NewL();
	
	IssueRead();
	SUPLLOG(ELogP1, "CSocketReader::ConstructL() End\n");
	}

void CSocketReader::RunL()
	{
	SUPLLOG(ELogP1, "CSocketReader::RunL() Begin\n");
	TBool cleanup = EFalse;
	if (iStatus == KErrNone)
		{
		iReadSoFar += iBufferRead();
		if (iIsFirstFragment && (iReadSoFar > 1)) // If first fragment we need to get the size of the data
			{
			SuplMessageLength();
			iIsFirstFragment = EFalse;
			} // if
		
		if (iReadSoFar == iCurrentMessageSize)
			{
			// Fire observation
			iBuffer.SetLength(iCurrentMessageSize);
			FireObservation();
			
			iIsFirstFragment = ETrue;
			iCurrentMessageSize = 0;
			iReadSoFar = 0;
			iBuffer.SetLength(iBuffer.MaxLength());
			} // if
		else if (iReadSoFar > iCurrentMessageSize)
			{
			SUPLLOG(ELogP1, "CSocketReader::RunL(), ReadSoFar > CurrentMessageSize\n");
			SUPLLOG(ELogP1, "CSocketReader::FireObservation() Firing ConnectionError\n");

			iObserver.ConnectionError(MSuplSocketObserver::ETooMuchDataRead, iCallbackId, 0, cleanup);
			if (cleanup)
				{
				delete this;
				}
			} // else if
		else
			{
			// Do nothing.
			}
		
		// Kick off the read again
		if (!cleanup)	//cleanup status from iObserver.ConnectionError
			{
			IssueRead();
			}
		} // if
	else
		{
		SUPLLOG(ELogP1, "CSecureSocketReader::FireObservation() Firing ConnectionError\n");

		iObserver.ConnectionError(MSuplSocketObserver::EFailedToRead, iCallbackId, 0, cleanup);
		if (cleanup)
			{
			delete this;
			}
		}
	SUPLLOG(ELogP1, "CSocketReader::RunL() End\n");
	}

void CSocketReader::DoCancel()
	{
	SUPLLOG(ELogP1, "CSocketReader::DoCancel() Begin\n");
	iSocket.CancelRecv();
	SUPLLOG(ELogP1, "CSocketReader::DoCancel() End\n");
	}

void CSocketReader::RunError()
	{
	SUPLLOG(ELogP1, "CSocketReader::RunError() Begin\n");
	ASSERT(EFalse);
	SUPLLOG(ELogP1, "CSocketReader::RunError() End\n");
	}

void CSocketReader::IssueRead()
	{
	SUPLLOG(ELogP1, "CSocketReader::IssueRead() Begin\n");
	if (!IsActive())
		{
		TPtr8 ptr = iBuffer.MidTPtr(iReadSoFar);
		iPtr.Set(const_cast<TUint8*>(ptr.Ptr()), 0, ptr.MaxLength());
		iSocket.RecvOneOrMore(iPtr, 0, iStatus, iBufferRead);
		SetActive();
		}
	SUPLLOG(ELogP1, "CSocketReader::IssueRead() End\n");
	}

void CSocketReader::SuplMessageLength()
{
	SUPLLOG(ELogP1, "CSocketReader::SuplMessageLength() Begin\n");
	// Get the first two bytes of iBuffer as a TInt and put it in iSuplMessageLength
	iCurrentMessageSize = STATIC_CAST(TUint8,iBuffer[0]);
	iCurrentMessageSize = iCurrentMessageSize << 8;
	iCurrentMessageSize |= STATIC_CAST(TUint8,iBuffer[1]);
	SUPLLOG(ELogP1, "CSocketReader::SuplMessageLength() End\n");
}

void CSocketReader::FireObservation()
	{
	SUPLLOG(ELogP1, "CSocketReader::FireObservation() Begin\n");
	CSuplMessageBase* message = NULL; 
	TInt err;
	TInt traperr = err = KErrNone;
	TPtrC8 ptr(iBuffer);
	TRAP(traperr, message = iDecoder->DecodeL(&ptr, err));
	if ((err != KErrNone) || (traperr != KErrNone))
		{
		// Fire error message => Session Id is not known
		TBool cleanup = EFalse;
		iObserver.ConnectionError(MSuplSocketObserver::EFailedToDecode, iCallbackId, 0, cleanup);
		if (cleanup)
			{
			delete this;
			}
		}
	else
		{
		iObserver.MessageReceived(message); // Passes ownership
		}
	SUPLLOG(ELogP1, "CSocketReader::FireObservation() End\n");
	}

/*****************************************************************************
 * CSecureSocketReader 
 */
CSecureSocketReader* CSecureSocketReader::NewL(CSecureSocket& aSocket, MSuplSocketObserver& aObserver, TUint aCallbackId)
	{
	SUPLLOG(ELogP1, "CSecureSocketReader::NewL() Begin\n");
	CSecureSocketReader* self = new (ELeave) CSecureSocketReader(aSocket, aObserver, aCallbackId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	SUPLLOG(ELogP1, "CSecureSocketReader::NewL() End\n");

	return self;
	}

CSecureSocketReader::~CSecureSocketReader()
	{
	SUPLLOG(ELogP1, "CSecureSocketReader::~CSecureSocketReader() Begin\n");
	Cancel();
	//iSocket.Close(); => Passed in as reference
	iBuffer.Close();
	delete iDecoder;
	SUPLLOG(ELogP1, "CSecureSocketReader::~CSecureSocketReader() End\n");
	}

CSecureSocketReader::CSecureSocketReader(CSecureSocket& aSocket, MSuplSocketObserver& aObserver, TUint aCallbackId) :
	CActive(EPriorityStandard), iSocket(aSocket), iObserver(aObserver), iIsFirstFragment(ETrue), 
	iCurrentMessageSize(0), iReadSoFar(0),  iPtr(NULL, 0), iCallbackId(aCallbackId)
	{
	SUPLLOG(ELogP1, "CSecureSocketReader::CSecureSocketReader() Begin\n");
	CActiveScheduler::Add(this);
	SUPLLOG(ELogP1, "CSecureSocketReader::CSecureSocketReader() End\n");
	}

void CSecureSocketReader::ConstructL()
	{
	SUPLLOG(ELogP1, "CSecureSocketReader::ConstructL() Begin\n");
	iBuffer.CreateL(KSuplMaxMessageLength);
	iBuffer.SetLength(KSuplMaxMessageLength);
	iDecoder = CSuplAsn1Decoder::NewL();
	
	IssueRead();
	SUPLLOG(ELogP1, "CSecureSocketReader::ConstructL() End\n");
	}

void CSecureSocketReader::RunL()
	{
	SUPLLOG2(ELogP1, "CSecureSocketReader::RunL() Begin, iStatus: %d\n", iStatus.Int());
	TBool cleanup = EFalse;
	if (iStatus == KErrNone)
		{
		iReadSoFar += iBufferRead();
		if (iIsFirstFragment && (iReadSoFar > 1)) // If first fragment we need to get the size of the data
			{
			SuplMessageLength();
			iIsFirstFragment = EFalse;
			} // if
		
		if (iReadSoFar == iCurrentMessageSize)
			{
			// Fire observation
			iBuffer.SetLength(iCurrentMessageSize);
			FireObservation();
			
			iIsFirstFragment = ETrue;
			iCurrentMessageSize = 0;
			iReadSoFar = 0;
			iBuffer.SetLength(iBuffer.MaxLength());
			} // if
		else if (iReadSoFar > iCurrentMessageSize)
			{
			SUPLLOG(ELogP1, "CSecureSocketReader::RunL(), ReadSoFar > CurrentMessageSize\n");
			SUPLLOG(ELogP1, "CSecureSocketReader::FireObservation() Firing ConnectionError\n");

			iObserver.ConnectionError(MSuplSocketObserver::ETooMuchDataRead, iCallbackId, 0, cleanup);
			if (cleanup)
				{
				delete this;
				}
			} // else if
		else
			{
			// Nothing to do just fall through, as we always want to re-issue the read.
			}
		
		// Kick off the read again
		if (!cleanup)	//cleanup status from iObserver.ConnectionError
			{
			IssueRead();
			}
		} // if
	else
		{
		SUPLLOG(ELogP1, "CSecureSocketReader::FireObservation() Firing ConnectionError\n");

		iObserver.ConnectionError(MSuplSocketObserver::EFailedToRead, iCallbackId, 0, cleanup);
		if (cleanup)
			{
			delete this;
			}
		}
	SUPLLOG(ELogP1, "CSecureSocketReader::RunL() End\n");
	}

void CSecureSocketReader::DoCancel()
	{
	SUPLLOG(ELogP1, "CSecureSocketReader::DoCancel() Begin\n");
	iSocket.CancelAll();
	SUPLLOG(ELogP1, "CSecureSocketReader::DoCancel() End\n");
	}

void CSecureSocketReader::RunError()
	{
	SUPLLOG(ELogP1, "CSecureSocketReader::RunError() Begin\n");
	SUPLLOG(ELogP1, "CSecureSocketReader::RunError() End\n");
	}

void CSecureSocketReader::IssueRead()
	{
	SUPLLOG(ELogP1, "CSecureSocketReader::IssueRead() Begin\n");
	if (!IsActive())
		{
		TPtr8 ptr = iBuffer.MidTPtr(iReadSoFar);
		iPtr.Set(const_cast<TUint8*>(ptr.Ptr()), 0, ptr.MaxLength());
		iSocket.RecvOneOrMore(iPtr, iStatus, iBufferRead);
		SetActive();
		}
	SUPLLOG(ELogP1, "CSecureSocketReader::IssueRead() End\n");
	}

void CSecureSocketReader::SuplMessageLength()
{
	SUPLLOG(ELogP1, "CSecureSocketReader::SuplMessageLength() Begin\n");
	// Get the first two bytes of iBuffer as a TInt and put it in iSuplMessageLength
	iCurrentMessageSize = STATIC_CAST(TUint8,iBuffer[0]);
	iCurrentMessageSize = iCurrentMessageSize << 8;
	iCurrentMessageSize |= STATIC_CAST(TUint8,iBuffer[1]);
	SUPLLOG(ELogP1, "CSecureSocketReader::SuplMessageLength() End\n");
}

void CSecureSocketReader::FireObservation()
	{
	SUPLLOG(ELogP1, "CSecureSocketReader::FireObservation() Begin\n");
	CSuplMessageBase* message = NULL; 
	TInt err;
	TInt traperr = err = KErrNone;
	TPtrC8 ptr(iBuffer);
	TRAP(traperr, message = iDecoder->DecodeL(&ptr, err));
	if ((err != KErrNone) || (traperr != KErrNone))
		{
		// Fire error message
		SUPLLOG(ELogP1, "CSecureSocketReader::FireObservation() Firing ConnectionError\n");
		TBool cleanup = EFalse;
		iObserver.ConnectionError(MSuplSocketObserver::EFailedToDecode, iCallbackId, 0, cleanup);
		if (cleanup)
			{
			delete this;
			}
		}
	else
		{
		iObserver.MessageReceived(message); // Passes ownership
		}
	SUPLLOG(ELogP1, "CSecureSocketReader::FireObservation() End\n");
	}
