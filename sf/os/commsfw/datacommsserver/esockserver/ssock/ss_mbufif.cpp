// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ss_std.h>
#include <es_mbufif.h>
#include <comms-infras/ss_flowbinders.h>

using namespace ESock;

// the CWaitForMBufs active object class is used to handle an out of mbufs 
// condition during CServProviderBaseMBuf::Write. If a normal allocation fails 
// then one of these AOs is created to wait on mbufs becomign available.
//
// when they are, this AO calls CanSend on the socket, which results
// in Write being called, which should now be able to complete successfully.
// 


CWaitForMBufs* CWaitForMBufs::NewL(MSessionDataNotify* aSocket)
	{
	CWaitForMBufs* self = new(ELeave) CWaitForMBufs(aSocket);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CWaitForMBufs::CWaitForMBufs(MSessionDataNotify* aSocket) 
: CActive(0),
  iSocket(aSocket)
	{
	}

void CWaitForMBufs::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

CWaitForMBufs::~CWaitForMBufs()  
	{
	Cancel();
	iMBufChain.Free();
	}

// from CActive
void CWaitForMBufs::RunL()     
	{
	iMBufChain.Free();
	if(iSignalCanSend)
		{
		iSignalCanSend = EFalse;  //Has to be before because CanSend can re-request
		iSocket->CanSend();
		}
	if(iSignalNewData)
		{
		iSignalNewData = EFalse;  //Has to be before because CanSend can re-request
		iSocket->NewData(0);
		}
	}

void CWaitForMBufs::DoCancel() 
	{
		iSignalNewData = EFalse;  
		iSignalCanSend = EFalse;
		iRequest.Cancel();
	}

// requests

void CWaitForMBufs::CancelNewData() 
	{
	iSignalNewData = EFalse;  
	if (!iSignalNewData && !iSignalCanSend)
		{
		iRequest.Cancel();
		}
	}

void CWaitForMBufs::CancelCanSend() 
	{
	iSignalCanSend = EFalse;
	if (!iSignalNewData && !iSignalCanSend)
		{
		iRequest.Cancel();
		}
	}
	
void CWaitForMBufs::AllocWaitAndSignalNewData(TUint aSize)

	{
	if(!IsActive())
		{
		iRequest.Alloc(iMBufChain, aSize, iStatus);
		SetActive();
		}
	iSignalNewData = ETrue;
	}

void CWaitForMBufs::AllocWaitAndSignalCanSend(TUint aSize)
	{
	if(!IsActive())
		{
		iRequest.Alloc(iMBufChain, aSize, iStatus);
		SetActive();
		}
	iSignalCanSend = ETrue;
	}

