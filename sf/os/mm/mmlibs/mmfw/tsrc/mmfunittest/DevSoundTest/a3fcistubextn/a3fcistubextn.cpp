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

#include "a3fcistubextn.h"
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include "a3fcistubextn.hrh"

#include <a3f/maudiocontext.h>
#include <a3f/maudiostream.h>
#include <a3f/maudioprocessingunit.h>
#include <a3f/maudiogaincontrol.h>
#include <a3f/audioprocessingunittypeuids.h>

//      
#include "a3fbackdooraccess.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif


// __________________________________________________________________________
// Client-side extension

MDevSoundCIClientExtension* CA3fClientCiStubExtn::NewL()
	{
	CA3fClientCiStubExtn* self = new (ELeave) CA3fClientCiStubExtn;
	return self;
	}
	
CA3fClientCiStubExtn::~CA3fClientCiStubExtn()
	{
	delete iDummyBuffer;
	REComSession::DestroyedImplementation(iKey);
	}
	
TInt CA3fClientCiStubExtn::Setup(MCustomCommand& aCustomCommand)
	{
	iCommand = &aCustomCommand;
	iDummyBuffer=KNullDesC().Alloc(); // dummy buffer simulates some allocs
	if (!iDummyBuffer)
		{
		return KErrNoMemory;
		}
	return KErrNone;
	}
	
TInt CA3fClientCiStubExtn::CustomInterfaceExtension(TUid aUid, TAny*& aInterface)
	{
	if (aUid==KUidTestSetVolIf)
		{
		MTestSetVolIf* interface = this;
		aInterface = interface;
		return KErrNone;
		}
	return KErrNotSupported;
	}
	
void CA3fClientCiStubExtn::Release()
	{
	delete this;
	}

void CA3fClientCiStubExtn::PassDestructorKey(TUid aDestructorKey)
	{
	iKey = aDestructorKey;
	}
	
TInt CA3fClientCiStubExtn::SetVol(TInt aVol)
	{
	// the key here is the destination package, which represets SetVol. Apart from value
	// the rest of the parameters can be ignored
	TPckgBuf<TInt> volumePkg (aVol);
	TMMFMessageDestinationPckg setVolDest (TMMFMessageDestination(KUidTestSetVolIf, 0));
	TInt result = iCommand->CustomCommandSync(setVolDest, ETestSetVolIfSetVolCommand, volumePkg, KNullDesC8);
	return result;
	}

TInt CA3fClientCiStubExtn::Vol(TInt aMaxVol)
    {
    // again the key is is the destination package
    TPckgBuf<TInt> maxVolPkg (aMaxVol);
    TMMFMessageDestinationPckg setVolDest (TMMFMessageDestination(KUidTestSetVolIf, 0));
    TInt result = iCommand->CustomCommandSync(setVolDest, ETestSetVolIfVolCommand, maxVolPkg, KNullDesC8);
    return result;
    }

// __________________________________________________________________________
// Server-side extension

MDevSoundCIServerExtension* CA3fServerCiStubExtn::NewL()
	{
	CA3fServerCiStubExtn* self = new (ELeave) CA3fServerCiStubExtn;
	return self;
	}
	
CA3fServerCiStubExtn::~CA3fServerCiStubExtn()
	{
	delete iDummyBuffer;
	delete iSetVol;
	REComSession::DestroyedImplementation(iKey);
	}
	
TInt CA3fServerCiStubExtn::Setup(MCustomInterface& aCustomInterface)
	{
	iInterface = &aCustomInterface;
	iDummyBuffer=KNullDesC().Alloc(); // dummy buffer simulates some allocs
	if (!iDummyBuffer)
		{
		return KErrNoMemory;
		}
	return KErrNone;
	}
	
TInt CA3fServerCiStubExtn::HandleMessageL(const RMmfIpcMessage& aMessage)
	{
	TMMFMessageDestinationPckg destinationPckg;
	MmfMessageUtil::ReadL(aMessage, 0, destinationPckg);
	if (destinationPckg().InterfaceId()==KUidTestSetVolIf)
		{
		CSetVol* volHandler = CSetVol::NewL(*iInterface);
		CleanupStack::PushL(volHandler);
		TInt result = volHandler->HandleMessageL(aMessage);
		CleanupStack::Pop(volHandler);
		return result;
		}

	return KErrNotSupported;
	}
	
void CA3fServerCiStubExtn::Release()
	{
	delete this;
	}

void CA3fServerCiStubExtn::PassDestructorKey(TUid aDestructorKey)
	{
	iKey = aDestructorKey;
	}
	
// CSetVol

CSetVol* CSetVol::NewL(MCustomInterface& aCustomInterface)
	{
	CSetVol* self = new (ELeave) CSetVol(aCustomInterface);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CSetVol::CSetVol(MCustomInterface& aCustomInterface):
	iInterface(&aCustomInterface)
	{
	// do nothing
	}
		
void CSetVol::ConstructL()
	{
	iWait = new (ELeave) CActiveSchedulerWait();
	}
	
CSetVol::~CSetVol()
	{
	delete iWait;
	}
	
TInt CSetVol::HandleMessageL(const RMmfIpcMessage& aMessage)
	{
    switch (aMessage.Function())
        {
        case ETestSetVolIfSetVolCommand:
            {
            TPckgBuf<TInt> volPckg;
            MmfMessageUtil::ReadL(aMessage, 1, volPckg);
            UpdateA3fPointers(); // grab pointers to context, stream etc
            SetVolumeL(volPckg());

            User::LeaveIfError(iContext->RegisterAudioContextObserver(*this));

            TInt error = iContext->Commit();
            if (!error)
                {
                iError = KErrNone;
                iWait->Start();
                error = iError;
                }
            (void) iContext->UnregisterAudioContextObserver(*this);

            aMessage.Complete(error);
            return KErrNone; // KErrNone says we've handled the message
            }
        case ETestSetVolIfVolCommand:
            {
            TPckgBuf<TInt> maxVolPckg;
            MmfMessageUtil::ReadL(aMessage, 1, maxVolPckg);
            UpdateA3fPointers(); // grab pointers to context, stream etc
            TInt result = VolumeL(maxVolPckg());
            aMessage.Complete(result);
            return KErrNone; // KErrNone says we've handled the message
            }
        default:
            return KErrArgument;
        }
	}
	
void CSetVol::UpdateA3fPointers()
	{
	MA3FBackdoorAccessIf* backdoor = 
		static_cast<MA3FBackdoorAccessIf*>(iInterface->CustomInterface(KA3FBackdoorAccessIfUid));
	if (backdoor)
		{
		iContext = backdoor->AudioContext();
		iStream = backdoor->AudioStream();
		iGain = backdoor->ProcessingUnit(KUidAudioGainControl);
		}
	else
		{
		iContext = NULL;
		iStream = NULL;
		iGain = NULL;
		}
	}
	
void CSetVol::SetVolumeL(TInt aVolume)
	{
	RArray<TAudioChannelGain>	channelGains;
	CleanupClosePushL(channelGains);
	TAudioChannelGain left;
	TAudioChannelGain right;	
	left.iLocation = TAudioChannelGain::ELeft;
	right.iLocation = TAudioChannelGain::ERight;
	left.iGain = right.iGain = aVolume;
	User::LeaveIfError(channelGains.Append(left)); // assumed element 0 in rest of code
	User::LeaveIfError(channelGains.Append(right)); // assumed element 1 in rest of code

	MAudioGainControl* gainControl = static_cast<MAudioGainControl*>(iGain->Interface(KUidAudioGainControl));
	User::LeaveIfError(gainControl->SetGain(channelGains));	
	CleanupStack::PopAndDestroy(&channelGains);
	}

TInt CSetVol::VolumeL(TInt aMaxVolume)
    {
    RArray<TAudioChannelGain>   channelGains;
    CleanupClosePushL(channelGains);
    TInt maxGain;

    MAudioGainControl* gainControl = static_cast<MAudioGainControl*>(iGain->Interface(KUidAudioGainControl));
    User::LeaveIfError(gainControl->GetGain(channelGains));
    User::LeaveIfError(gainControl->GetMaxGain(maxGain));

    TInt basicVolume = (channelGains[0].iGain + channelGains[1].iGain) / 2;
    TInt result = basicVolume * aMaxVolume / maxGain; // scale to 0 to maxVolume

    CleanupStack::PopAndDestroy(&channelGains);
    return result;
    }
	
void CSetVol::ContextEvent(TUid aEvent, TInt aError)
	{
	if (aEvent==KUidA3FContextUpdateComplete || aEvent==KUidA3FContextAbort)
		{
		// we are going to assume the SetGain() works and use the end of
		// the ContextEvent. Really should observe the gain itself
		if (aError)
			{
			iError = aError;
			}
		iWait->AsyncStop();
		}
	}
		
//
// ImplementationTable
//

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KUidA3fClientCiStubExtn, CA3fClientCiStubExtn::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidA3fServerCiStubExtn, CA3fServerCiStubExtn::NewL),
	};


//
// ImplementationGroupProxy
//
//

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}


