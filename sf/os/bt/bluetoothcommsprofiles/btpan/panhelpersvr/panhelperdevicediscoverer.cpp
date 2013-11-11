// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "panhelperdevicediscoverer.h"
using namespace PanHelper;

CPanHelperDeviceDiscoverer* CPanHelperDeviceDiscoverer::NewL()
/**
Return a newly constructed device discoverer
*/
	{
	CPanHelperDeviceDiscoverer* self = new (ELeave) CPanHelperDeviceDiscoverer;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPanHelperDeviceDiscoverer::~CPanHelperDeviceDiscoverer()
/**
Clean up
*/
	{
	iDeviceAddresses.Close();
	iHostResolver.Close();
	iSockServ.Close();
	}
	
void CPanHelperDeviceDiscoverer::RunL()
/**
Found a new device
*/
	{
	if(iStatus==KErrNone)
		{
		TBTDevAddr newDevAddr(static_cast<TInquirySockAddr>(iNameEntry().iAddr).BTAddr());
		TInt index = iDeviceAddresses.Find(newDevAddr);
		if(index != KErrNotFound)
			{
			TPckg<TBTDevAddr> devAddrBuf(iDeviceAddresses[index]);
			iRemDevAddrBuf->Copy(devAddrBuf);
			User::RequestComplete(iClientStatus, KErrNone);	
			Cancel();
			}
		else 
			{
			iHostResolver.Next(iNameEntry, iStatus);
			SetActive();
			}
		}
	else
		{
		User::RequestComplete(iClientStatus, iStatus.Int());
		Cancel();	
		}
	}
	
void CPanHelperDeviceDiscoverer::DoCancel()
/**
Cancel HostResolver
*/
	{
	iHostResolver.Cancel();
	}
	
void CPanHelperDeviceDiscoverer::FindDevice(TDesC& aDeviceAddrList, TDes8& aDeviceAddr, TRequestStatus& aStatus)
/**
Start a generic inquiry
*/
	{
	iRemDevAddrBuf = (TPckgBuf<TBTDevAddr>*) &aDeviceAddr;
	iClientStatus = &aStatus;
	ConvertListToArray(aDeviceAddrList);
	iHostResolver.GetByAddress(iInquiryAddr, iNameEntry, iStatus);
	SetActive();
	}
	
void CPanHelperDeviceDiscoverer::CancelFindDevice()
/**
Cancel
*/
	{
	Cancel();
	User::RequestComplete(iClientStatus, KErrCancel);
	}
	
CPanHelperDeviceDiscoverer::CPanHelperDeviceDiscoverer() :
	CActive(EPriorityStandard)
/**
Constructor
*/
	{
	CActiveScheduler::Add(this);
	iInquiryAddr.SetIAC(KGIAC);
	iInquiryAddr.SetAction(KHostResInquiry); // don't need device names here
	}
	
void CPanHelperDeviceDiscoverer::ConstructL()
/**
Initialise
*/
	{
	User::LeaveIfError(iSockServ.Connect());
	User::LeaveIfError(iHostResolver.Open(iSockServ, KBTAddrFamily, KBTLinkManager));
	}
	
void CPanHelperDeviceDiscoverer::ConvertListToArray(TDesC& aDeviceAddrList)
/**
Put the comma-separated list of readable BT device addresses into an array
*/
	{
	TBTDevAddr devAddr;
	TLex lex(aDeviceAddrList);

	TChar charComma(KCharComma);	// this weird construct is needed to prevent us needing static data
	while(lex.Peek()!=charComma && !lex.Eos())
	{
		lex.Mark();
		while((lex.Get()!=charComma) && !lex.Eos())
			{
			
			}
		devAddr.SetReadable(lex.MarkedToken());

		// This is a best efforst service.  If we can't add the address
		// to our search list we'll just continue with the ones we could.
		(void)iDeviceAddresses.Append(devAddr);
		}
	}
