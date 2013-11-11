// reader.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "reader.h"
#include "panic.h"


CReader* CReader::NewL(RSocket& aSocket, MReaderObserver& aObserver)
	{
	return new(ELeave) CReader(aSocket, aObserver);
	}

CReader::~CReader()
	{
	Cancel();
	iBuffer.Close();
	}

void CReader::StartRead(TInt aLength, TReadMode aMode)
	{
	__ASSERT_DEBUG(!IsActive() && (iPendingLength == 0), Panic(EPanicReadAlreadyPending));

	if (iBuffer.MaxLength() < aLength)
		{
		TInt err = iBuffer.ReAlloc(aLength);
		if (err)
			{
			iObserver.ReadError(err);
			return;
			}
		}

	if (Connected())
		{
		iBuffer.Zero();

		switch (aMode)
			{
			case EFull:
				{
				iSocket.Recv(iBuffer, 0, iStatus);
				break;
				}
			case EOneOrMore:
				{
				iSocket.RecvOneOrMore(iBuffer, 0, iStatus, iXfrLength);
				break;
				}
			default:
				{
				__ASSERT_DEBUG(EFalse, Panic(EPanicInvalidReadMode));
				}
			};

		SetActive();
		}
	else
		{
		iPendingLength = aLength;
		iPendingMode = aMode;
		}
	}

void CReader::GetBufferLength(TInt& aLength)
	{
	aLength = iBuffer.Length();
	}

void CReader::Abort()
	{
	iPendingLength = 0;
	Cancel();
	}

void CReader::HandleSocketConnected()
	{
	if (iPendingLength > 0)
		{
		TInt pendingLength = iPendingLength;
		iPendingLength = 0;
		StartRead(pendingLength, iPendingMode);
		}
	}

CReader::CReader(RSocket& aSocket, MReaderObserver& aObserver)
	: CSocketUser(aSocket), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

void CReader::RunL()
	{
	if (iStatus.Int())
		{
		iObserver.ReadError(iStatus.Int());
		}
	else
		{
		iObserver.ReadComplete(iBuffer);
		}
	}

void CReader::DoCancel()
	{
	iSocket.CancelRecv();
	}

