// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// BT HCI Vendor Specific Command API
// 
//

#include "hciproxy.h"
#include "vendorspecific.h"

#include <bluetooth/hci/hciconsts.h>
#include <bluetooth/hci/hciframe.h>

const TUint KNullIoctl=0xFFFFFFFF;

/**
two phase constructor to create an HCI Conduit. Note that access to an HCI Conduit from a thread
other than that which creates the conduit is not supported
@param aConduit The instance of MVendorSpecificHciConduit to receive notifications of conduit events
@return pointer to allocated object
*/
EXPORT_C CHciExtensionConduit* CHciExtensionConduit::NewL(MVendorSpecificHciConduit& aConduit)
	{
	CHciExtensionConduit* self= new (ELeave) CHciExtensionConduit(aConduit);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CHciExtensionConduit::CHciExtensionConduit(MVendorSpecificHciConduit& aConduit) 
:	CActive(0),
	iConduit(aConduit)
	{
	}

/**
2nd phase construction
*/
EXPORT_C void CHciExtensionConduit::ConstructL()
	{
	// Open a connection to the socket server, then the socket
	User::LeaveIfError(iSocketServ.Connect((TUint)-1));
	User::LeaveIfError(iSocket.Open(iSocketServ,
									KBTAddrFamily, KSockBluetoothTypeVendorSpecific, KBTLinkManager));

	// Register with the scheduler
	CActiveScheduler::Add(this);
	}	

/**
destructor
*/
EXPORT_C CHciExtensionConduit::~CHciExtensionConduit()
	{
	// Stop the active object 
	Cancel();

	// close the socket server and socket
	iSocket.Close();
	iSocketServ.Close();

	// delete VS ioctl buff
	delete iVSIoctl;
	}

EXPORT_C TInt CHciExtensionConduit::IssueCommandL(const TUint16 aOpcode, const TDesC8& aParameters)
/**
Issues a vendor specific command to the hardware.

The client need not be concerned with the exact formatting of the HCI frame
as the HCI only requires the opcode and the correctly fomatted parameters in
order to construct the appropriate the vendor debug frame.  Note that the
caller need NOT to be concerned with the HCTL framing at all.
The HCI will only then copy this frame to the Host Controller after putting
it in an HCTL frame.
This command will be completed when the Bluetooth stack receives it. This API 
should not be used if the caller wants to receive any data from an associated 
command complete event. 

Note: a command complete event is NOT a vendor debug event.

@param aOpcodec The opcode for the frame, i.e. (KVendorDebugOGF & vendorOCFRequired)
@param aParameters A descriptor containing the raw paramters for the command to be passed to hardware (maximum KHCIMaxCommandLength bytes)
@return Error code indicating whether command could be passed to the hardware
*/
	{
	return IssueCommandL(aOpcode, aParameters, KHCIWriteVendorSpecificFrameNoEventExpectedIoctl);
	}

EXPORT_C TInt CHciExtensionConduit::IssueCommandExpectingCompleteEventL(const TUint16 aOpcode, const TDesC8& aParameters)
/**
Issues a vendor specific command to the hardware. This command should have an associated command complete event.

The client need not be concerned with the exact formatting of the HCI frame
as the HCI only requires the opcode and the correctly fomatted parameters in
order to construct the appropriate the vendor debug frame.  Note that the
caller need NOT to be concerned with the HCTL framing at all.
The HCI will only then copy this frame to the Host Controller after putting
it in an HCTL frame.
This command will not be completed until the hardware responds with the command complete event.
Note: a command complete event is NOT a vendor debug event.

Warning: If this command is used after the command IssueRequestL has been called, the associated callback,
MVendorSpecificHciConduit::CommandCompleted, could receive the wrong CommandCompleteEvent data.

@see MVendorSpecificHciConduit::CommandCompleted
@param aOpcodec The opcode for the frame, i.e. (KVendorDebugOGF & vendorOCFRequired)
@param aParameters A descriptor containing the raw paramters for the command to be passed to hardware (maximum KHCIMaxCommandLength bytes)
@return Error code indicating whether command could be passed to the hardware
*/
	{
	return IssueCommandL(aOpcode, aParameters, KHCIWriteVendorSpecificFrameIoctl);
	}

TInt CHciExtensionConduit::IssueCommandL(const TUint16 aOpcode, const TDesC8& aParameters, TUint aName)
	{
	__ASSERT_DEBUG((aParameters.Length() > 0) && (aParameters.Length() <= CHctlCommandFrame::KHCIMaxCommandLength), User::Panic(KHciExtConduitPanicCat, EInvalidParamSize));

	if(IsActive())
		{
		if(iCurrentIoctl==KHCIWaitForVendorSpecificDebugEventIoctl)
			{
			//	Need to cancel the outstanding listening Ioctl before proceeding
			Cancel();
			}
		else
			{
			//	Already have an outstanding Ioctl which we can't really cancel
			return KErrInUse;
			}
		}

	__ASSERT_ALWAYS(!IsActive(), User::Panic(KHciExtConduitPanicCat, EAlreadyActive));

	iCurrentIoctl = aName;

	if (iVSIoctl)
		{
		delete iVSIoctl;
		iVSIoctl = NULL;
		}

	iVSIoctl = new(ELeave) THCIWriteVendorSpecificFrameBuf();

	(*iVSIoctl)().iOpcode = aOpcode;
	(*iVSIoctl)().iParams.Copy(aParameters);

	iSocket.Ioctl(iCurrentIoctl, iStatus, iVSIoctl, KSolBtHCI);

	SetActive();

	return KErrNone;
	}
	
/**
This method allows a client to obtain the command complete event data when being notified of a CommandComplete.
That is it can be called as part of his implementation of "CommandCompleted" in his MVendorSpecificHciConduit derived class.

*** NOTE: If no event data is returned from the stack, as is the case if  IssueCommandL was called,	***
*** this will return the original command data! 													***

@see MVendorSpecificHciConduit
*/
EXPORT_C void CHciExtensionConduit::CommandCompleteEventData(TPtrC8& aOutPtrToData)
	{
	__ASSERT_DEBUG((!IsActive() && iVSIoctl), User::Panic(KHciExtConduitPanicCat, EUnexpectedCce));

	// on output from the Ioctl the descriptor the iVSIoctl points to contains the command complete event data
	aOutPtrToData.Set(*iVSIoctl);
	}

/**
Instructs the conduit that vendor debug events are expected back from the hardware.
The conduit will listen for events, and on reception of one will issue an upcall
on the MVendorSpecificHciConduit derived object passed into the CHciExtensionConduit
factory function.

@return Error code indicating whether the conduit has switched into listening mode
@publishedPartner
@released
*/
EXPORT_C TInt CHciExtensionConduit::WaitForEvent()
	{
	if(IsActive())
		{
		//	We're already doing something
		return KErrInUse;
		}

	iBuf.Zero();	//	Prepare the buffer for the returning values

	iWaitForEvent=ETrue;
	iCurrentIoctl=KHCIWaitForVendorSpecificDebugEventIoctl;
	iSocket.Ioctl(iCurrentIoctl, iStatus, &iBuf, KSolBtHCI);

	SetActive();

	return KErrNone;
	}

/**
Instructs the conduit that no more vendor debug events are expected back from the hardware.
The conduit will issue no more upcalls on the MVendorSpecificHciConduit derived object passed 
into the CHciExtensionConduit factory function.
@publishedPartner
@released
*/
EXPORT_C void CHciExtensionConduit::StopWaitingForEvent()
	{
	if(iCurrentIoctl==KHCIWaitForVendorSpecificDebugEventIoctl)
		{
		Cancel();
		}
	iWaitForEvent = EFalse;
	}

void CHciExtensionConduit::RunL()
	{
	switch(iCurrentIoctl)
		{
	case KHCIWriteVendorSpecificFrameIoctl:
	case KHCIWriteVendorSpecificFrameNoEventExpectedIoctl:		
		iCurrentIoctl=KNullIoctl;
		iConduit.CommandCompleted(iStatus.Int());

		// delete the VS IOCTL buf as the IOCTL has now completed
		delete iVSIoctl;
		iVSIoctl = NULL;

		if(iWaitForEvent)
			{
			WaitForEvent();
			}

		break;

	case KHCIWaitForVendorSpecificDebugEventIoctl:
		//	Pass up the data, requeue waiting Ioctl if return value is ETrue
		iCurrentIoctl=KNullIoctl;
		iWaitForEvent=iConduit.ReceiveEvent(iBuf, iStatus.Int());
		if(iWaitForEvent)
			{
			WaitForEvent();
			}
			//	No longer going to have a queued Ioctl.
		break;

	default:
		User::Panic(KHciExtConduitPanicCat, EInvalidIoctl);
		}
	}

void CHciExtensionConduit::DoCancel()
	{
	iSocket.CancelIoctl();
	iCurrentIoctl=KNullIoctl;
	}

