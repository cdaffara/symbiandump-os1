// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <e32base.h>

#define __KERNEL_MODE__
#include <kernel.h>
#undef  __KERNEL_MODE__

#include <d32comm.h>
#include <e32ver.h>

#include "d_comm.h"
#include <e32hal.h>
#include "DParams.h"

const TUint KXoffSignal=0x80000000;
//
const TInt KInputHeldFree=(-1);
const TInt KInputHeld=(-2);
//
const TUint KTransmitting=0x01;
const TUint KBreaking=0x02;
const TUint KBreakPending=0x04;
//
enum TPanic
	{
	ESetConfigWhileRequestPending,
	ESetSignalsSetAndClear,
	EResetBuffers,
	ESetReceiveBufferLength,
//	EDoubleTxBufFill,
	};

EXPORT_C DLogicalDevice *CreateLogicalDevice()
//
// Create a new device
//
	{
	return(new DDeviceComm);
	}

DDeviceComm::DDeviceComm()
//
// Constructor
//
	{
	//iParseMask=KDeviceAllowAll;
	iParseMask=KDeviceAllowUnit|KDeviceAllowInfo;
	iUnitsMask=0xffffffff; // Leave units decision to the driver
	iVersion=TVersion(KCommsMajorVersionNumber,KCommsMinorVersionNumber,KCommsBuildVersionNumber);
	}

TInt DDeviceComm::Install()
//
// Install the device driver.
//
	{
    TPtrC name=_L("CommDCE");
	return(SetName(&name));
	}

void DDeviceComm::GetCaps(TDes8 &aDes) const
//
// Return the Comm capabilities.
//
	{

	TCapsDevCommV01 b;
	b.version=iVersion;
	aDes.FillZ(aDes.MaxLength());
	aDes.Copy((TUint8 *)&b,Min(aDes.MaxLength(),sizeof(b)));
	}

DLogicalChannel *DDeviceComm::CreateL()
//
// Create a channel on the device.
//
	{

	return(new(ELeave) DChannelComm(this));
	}


#pragma warning( disable : 4705 )	// statement has no effect
#pragma warning( disable : 4355 )	// this used in intializer list
DChannelComm::DChannelComm(DLogicalDevice *aDevice)
//
// Constructor
//
: DLogicalChannel(aDevice)
	{

	SetBehaviour(RBusDevComm::ERequestReadCancel|
		RBusDevComm::ERequestWriteCancel|
		RBusDevComm::ERequestBreakCancel|
		RBusDevCommDCE::ERequestNotifySignalChangeCancel|
		RBusDevCommDCE::ERequestNotifyFlowControlChangeCancel|
		RBusDevCommDCE::ERequestNotifyConfigChangeCancel);
//
// Setup the default config
//
	iConfig.iRate=DUMMYCONFIGRATE;
	iConfig.iDataBits=DUMMYDATABITS;
	iConfig.iStopBits=DUMMYSTOPBITS;
	iConfig.iHandshake=DUMMYHANDSHAKE;
	}
#pragma warning( default : 4705 )
#pragma warning( disable : 4355 )

DChannelComm::~DChannelComm()
//
// Destructor
//
	{
	CompleteAll(KErrAbort);
	delete iDriver;
	}

void DChannelComm::DoCreateL(TInt /*aUnit*/,CBase *aDriver,const TDesC * /*anInfo*/,const TVersion &aVer)
//
// Create the channel from the passed info.
//
	{
	iDriver=(DComm *)aDriver; // Must do this to ensure the driver is destroyed by the destructor
	if (!User::QueryVersionSupported(TVersion(KCommsMajorVersionNumber,KCommsMinorVersionNumber,KCommsBuildVersionNumber),aVer))
		User::Leave(KErrNotSupported);

//	((DComm *)aDriver)->SetComm(this); // Rog
	}

void DChannelComm::DoCancel(TInt aReqNo)
//
// Cancel an outstanding request.
//
	{

	switch (aReqNo)
		{
	case RBusDevComm::ERequestRead:
		iRxDataAvailablePending=EFalse;
		break;
	case RBusDevCommDCE::ERequestNotifySignalChange:
		iSignalCount++;
		iSignalNotifyPending=EFalse;
		break;
	case RBusDevCommDCE::ERequestNotifyFlowControlChange:
		iFlowControlChangePending=EFalse;
		break;
	case RBusDevCommDCE::ERequestNotifyConfigChange:
		iConfigChangePending=EFalse;
		break;
	default:
		__ASSERT_ALWAYS(0,User::Invariant());
		}
	}

void DChannelComm::DoRequest(TInt aReqNo,TAny *a1,TAny *a2)
//
// Async requests.
//
	{
	aReqNo &= (~KDataAvailableNotifyFlag);
	switch (aReqNo)
		{
	case RBusDevCommDCE::ERequestWrite:
		{
		__ASSERT_ALWAYS(iWritePending==EFalse,User::Invariant());
		iWritePending=ETrue;
		iQueuedWrite.OneShotInMicroSeconds(KDummyTimeOut,DChannelComm::WriteCompleted,this);
		}
		break;
	case RBusDevCommDCE::ERequestRead:	// this is the RxDataAvailable notification. Base have chosen
									// to use the enum for both this and Read().
		{
		__ASSERT_ALWAYS(iRxDataAvailablePending==EFalse,User::Invariant());
		iRxDataAvailablePending=ETrue;
		iQueuedRxDataAvailable.OneShotInMicroSeconds(KDummyTimeOut,DChannelComm::RxDataAvailableCompleted,this);
		}
		break;
	case RBusDevCommDCE::ERequestNotifySignalChange:
		{
		__ASSERT_ALWAYS(iSignalNotifyPending==EFalse,User::Invariant());
		iSignalNotifyPending=ETrue;
		switch (iSignalCount)
			{
		case 0:
		case 2:
			__ASSERT_ALWAYS(*((TUint*)a2) == (DUMMYSIGNALMASK),User::Invariant());	// and wait for cancel
			break;
		case 1:
			__ASSERT_ALWAYS(*((TUint*)a2) == (DUMMYSIGNALMASK|KSignalBreak),User::Invariant());// and wait for cancel
			break;
		case 3:
		case 4:
			{
			iSignals=(TUint*)a1;
			iSignalMask=*((TUint*)a2);
			iQueuedSignalNotify.OneShotInMicroSeconds(KDummyTimeOut,DChannelComm::SignalChangeCompleted,this);
			break;
			}
		default:
			__ASSERT_ALWAYS(0,User::Invariant());
			}
		break;
	case RBusDevCommDCE::ERequestNotifyFlowControlChange:
		{
		__ASSERT_ALWAYS(iFlowControlChangePending==EFalse,User::Invariant());
		iFlowControlChangePending=ETrue;
		iQueuedFlowControl.OneShotInMicroSeconds(KDummyTimeOut,DChannelComm::FlowControlChangeCompleted,this);
		}
		break;
	case RBusDevCommDCE::ERequestNotifyConfigChange:
		{
		__ASSERT_ALWAYS(iConfigChangePending==EFalse,User::Invariant());
		iConfigChangePending=ETrue;
		iConfigPointer = (TCommNotificationPckg*)a1;

		iQueuedConfigChange.OneShotInMicroSeconds(KDummyTimeOut,DChannelComm::ConfigChangeCompleted,this);
		}
		break;
	default:
		Complete(aReqNo,KErrNotSupported);
		}
		}
	}

TInt DChannelComm::DoControl(TInt aFunction,TAny *a1,TAny* /*a2*/)
//
// Sync requests.
//
	{
	TInt r=KErrNone;
	switch (aFunction)
		{
	case RBusDevCommDCE::EControlFlowControlStatus:
		{
		iFlowControl=DUMMYFLOWCONTROLSTATUS;
		*((TFlowControl*)a1) = iFlowControl;
		}
		break;
	case RBusDevCommDCE::EControlSetConfig:
		break;
	default:
		r=KErrNotSupported;
		}
	return(r);
	}


void DChannelComm::SignalChangeCompleted(TAny* aChannel,TInt)
	{
	DChannelComm* dc = (DChannelComm*)aChannel;
	dc->SetSignals();
	dc->CompleteReq(RBusDevCommDCE::ERequestNotifySignalChange);
	}
void DChannelComm::ConfigChangeCompleted(TAny* aChannel,TInt)
	{
	DChannelComm* dc = (DChannelComm*)aChannel;
	dc->ChangeConfig();
	dc->CompleteReq(RBusDevCommDCE::ERequestNotifyConfigChange);
	}
void DChannelComm::RxDataAvailableCompleted(TAny* aChannel,TInt)
	{
	((DChannelComm*)aChannel)->CompleteReq(RBusDevCommDCE::ERequestRead);
	}
void DChannelComm::FlowControlChangeCompleted(TAny* aChannel,TInt)
	{
	((DChannelComm*)aChannel)->CompleteReq(RBusDevCommDCE::ERequestNotifyFlowControlChange);
	}
void DChannelComm::WriteCompleted(TAny* aChannel,TInt)
	{
	((DChannelComm*)aChannel)->CompleteReq(RBusDevCommDCE::ERequestWrite);
	}

void DChannelComm::CompleteReq(TInt aReqNo)
	{

	Complete(aReqNo);
	}

void DChannelComm::SetSignals()
	{
	switch(iSignalCount++)
		{
	case 3:
		(*iSignals)=DUMMYSIGNALS;
		break;
	case 4:
		(*iSignals)=(KSignalBreak|KBreakChanged);
		break;
	default:
		__ASSERT_ALWAYS(0,User::Invariant());
		};
	iSignalNotifyPending=EFalse;
	}

void DChannelComm::ChangeConfig()
	{
	iConfig.iChangedMembers = DUMMYCHANGED;
	iConfigPointer->FillZ(iConfigPointer->MaxLength());
	TInt len=Min(iConfigPointer->MaxLength(),sizeof(iConfig));
	iConfigPointer->Copy((TUint8 *)&iConfig,len);
	}


