// writer.cpp
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

#include "writer.h"
#include "panic.h"


CWriter* CWriter::NewL(RSocket& aSocket, MWriterObserver& aObserver)
	{
	return new(ELeave) CWriter(aSocket, aObserver);
	}

CWriter::~CWriter()
	{
	Cancel();
	}

void CWriter::StartWrite(const TDesC8& aDes)
	{
	__ASSERT_DEBUG(!IsActive() && (iPendingDes == NULL), Panic(EPanicWriteAlreadyPending));

	if (Connected())
		{
		iSocket.Send(aDes, 0, iStatus);
		SetActive();
		}
	else
		{
		iPendingDes = &aDes;
		}
	}

void CWriter::Abort()
	{
	iPendingDes = NULL;
	Cancel();
	}

void CWriter::HandleSocketConnected()
	{
	if (iPendingDes)
		{
		const TDesC8* pendingDes = iPendingDes;
		iPendingDes = NULL;
		StartWrite(*pendingDes);
		}
	}

CWriter::CWriter(RSocket& aSocket, MWriterObserver& aObserver)
	: CSocketUser(aSocket), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

void CWriter::RunL()
	{
	iObserver.WriteComplete(iStatus.Int());
	}

void CWriter::DoCancel()
	{
	iSocket.CancelSend();
	}
