// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ccomreadwrite.cpp
// 
//

#include "debug.h"

#include "comreadwrite.h"
#include "GLOBAL.H"

#include <e32base.h>
#include <f32file.h>
#include <c32comm.h>
#include <e32cons.h>


CComReadWrite::CComReadWrite(TInt aPriority, MComReadWriteObserver& aObserver)
				: CActive(aPriority), iObserver(aObserver)
	{
	}

CComReadWrite* CComReadWrite::NewL(MComReadWriteObserver& aObserver)
	{
	DEBUG_OPEN();
	COMRWPRINT(_L("CComReadWrite::NewL()\n"));
	
	CComReadWrite* self = new(ELeave) CComReadWrite(EPriorityHigh, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CComReadWrite::ConstructL()
	{
	CActiveScheduler::Add(this);

	COMRWPRINT(_L("CComReadWrite::ConstructL()\n"));
	}

void CComReadWrite::Open()
	{
	// Connect to Comserver and Open IrComm port
	COMRWPRINT(_L("CComReadWrite::Open()\n"));
	TInt error;
	error = iServer.Connect();
	error = iServer.LoadCommModule(_L("IrCOMM"));
	error = iCommPort.Open(iServer,_L("IRCOMM::0"), ECommExclusive, ECommRoleDCE);
	Signal(error);
	iState = EOpen;
	}

void CComReadWrite::Close()
	{
	COMRWPRINT(_L("CComReadWrite::Close()\n"));
	Cancel();
	Signal(KErrNone);
	iState = EClose;
	}

void CComReadWrite::Signal(TInt aError)
	{
	TRequestStatus *pS=(&iStatus);
	SetActive();
	User::RequestComplete(pS,aError);
	}

CComReadWrite::~CComReadWrite()
	{
	COMRWPRINT(_L("CComReadWrite:: ~CComReadWrite()\n"));
	}

void CComReadWrite::Send(TDesC8& aBuffer)
	{
	COMRWPRINT(_L("CComReadWrite:: ESend request\n"));
	__ASSERT_DEBUG(aBuffer.Length()<= iTxBuffer.MaxLength(), User::Panic(_L("IrTranP"), KErrTooBig));
	if (!(IsActive()))
		{
		iTxBuffer.Zero();
		iTxBuffer.Copy(aBuffer);
		iCommPort.Write(iStatus, iTxBuffer);
		COMRWPRINT(_L("CComReadWrite:: iCommPort.Write cleared\n"));
		iState = ESend;
		SetActive();

		}
	else
		{
		COMRWPRINT(_L("CComReadWrite: Error\n"));
		iObserver.SendError(KErrWrite);
		}

	}

void CComReadWrite::Receive()
	{
	COMRWPRINT(_L("CComReadWrite: EReceive request\n"));
	
	// Clear the buffer and receive the next lot of data;

	iRxBuffer.Zero();
	iCommPort.ReadOneOrMore(iStatus, iRxBuffer);
	iState = EReceive;
	SetActive();
	}

void CComReadWrite::RunL()
	{
	COMRWPRINT(_L("CComReadWrite: RunL()\n"));
	
	switch (iState)
		{
	case EOpen:
		{
		if (iStatus == KErrNone)
			{
			COMRWPRINT(_L("CComReadWrite: EOpen sucessful\n"));
		  	/*TBuf8<1> buf;
			buf.SetLength(0);
			Send(buf);*/

			Receive();
			}
		else
			{
			iObserver.Error(iStatus.Int());
			}
		}
		break;
	case EStop:
		{
		iCommPort.Close();
		iServer.UnloadCommModule(_L("IrCOMM"));
		iServer.Close();
		iObserver.Error(KErrAbort);
		COMRWPRINT(_L("CComReadWrite: EStop sucessfull\n"));
		}
		break;
	case EClose:
		{
		if (iStatus == KErrNone)
			{
			iCommPort.Close();
			iServer.UnloadCommModule(_L("IrCOMM"));
			iServer.Close();
			COMRWPRINT(_L("CComReadWrite: EClose sucessfull\n"));
			}
		else
			{
			iObserver.Error(iStatus.Int());
			}
		}
		break;
	case ESend: // Send Complete
		if (iStatus == KErrNone)
			{
			COMRWPRINT(_L("CComReadWrite: ESend sucessful\n"));
			iObserver.SendComplete(); // posts read request.
			}
		else
			{
			iObserver.SendError(iStatus.Int());
			}
		break;
	case EReceive:
		{
		if (iStatus == KErrNone)
			{
			COMRWPRINT_2(_L("CComReadWrite: ERecieve sucessful %d\n"),iRxBuffer.Length());
			iObserver.ReceiveComplete(iRxBuffer);
			}
		else
			{
			iObserver.ReceiveError(iStatus.Int());
			}
		}
		break;
	default:
		User::Leave(KErrGeneral);
		break;
		}
	
	}

void CComReadWrite::DoCancel()
	{
	COMRWPRINT(_L("CComReadWrite: DoCancel()\n"));

	iCommPort.WriteCancel();
	iCommPort.ReadCancel();
	iState = EStop;
	}
