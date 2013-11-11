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
//

#ifndef HCIPROXY_H__
#define HCIPROXY_H__

#include <es_sock.h>
#include <bt_sock.h>
#include <e32test.h>
#include <c32comm.h>
#include <f32file.h>
#include <e32std.h>

//
// Forward declarations
//
struct THCIWriteVendorSpecificFrameIoctl;

//
// Package for sending a THCIWriteVendorSpecificFrameIoctl via IPC
//
typedef TPckgBuf<THCIWriteVendorSpecificFrameIoctl> THCIWriteVendorSpecificFrameBuf;

// Category name for panics
_LIT(KHciExtConduitPanicCat, "CHciExtensionConduit");

enum THciProxyPanics
	{
	EAlreadyActive		= 1,
	EInvalidParamSize,
	EInvalidIoctl,
	EUnexpectedCce,
	};

/**
This class provides a mixin interface for reception of command completions for vendor
specific commands and vendor specific events.
@publishedPartner
@released
**/
class MVendorSpecificHciConduit
	{
public:	
/**
This is the callback for both CHciExtensionConduit::IssueCommand and 
CHciExtensionConduit::IssueCommandExpectingCompleteEventL
If CHciExtensionConduit::IssueCommandL was called CommandComplete will be 
called when the Bluetooth stack receives the associated Ioctl.
If CHciExtensionConduit::IssueCommandExpectingCompleteEventL was called 
CommandComplete will be called when a command complete event with a 
vendor specific opcode is received from the hardware.

@see CHciExtensionConduit::IssueCommandL
@see CHciExtensionConduit::IssueCommandExpectingCompleteEventL
@param aError	:	The error code with which the command completed. See 'WARNING' above.
**/	
	virtual void CommandCompleted(TInt aError)=0;
/**
CommandCompleted is called when a vendor specific command issued through the conduit
receives a completion from the hardware.

@param aEvent	:	Returns raw vendor specific debug event data generated from the hardware 
(with no HCTL framing)
@param aError	:	An error indicating that the conduit did not successfully receive the event.
@return If ETrue, the conduit will continue to listen for vendor specific debug events, 
If EFalse, the conduit stops listening for these events.
**/	
	virtual TBool ReceiveEvent(TDesC8& aEvent, TInt aError)=0;
	};

/**
This class acts as a wrapper for a group of Ioctls which used together provide a means 
for licensees to pass proprietary commands direct to the hardware at the bottom of the 
stack, and to receive events elicited by these commands back from the hardware.

On construction, the CHciExtensionConduit registers itself with the Bluetooth stack,
which  ensures that there can be only one HCI Extension Conduit in existance on a machine 
at one time. This means that the first such conduit constructed after bootup has sole
access to issue vendor specific commands and more importantly receive back vendor specific
events.

It works in conjunction with the MVendorSpecificHciConduit class, which provides a mixin
interface for reception of command completions for vendor specific commands and vendor 
specific events.
@publishedPartner
@released
**/
NONSHARABLE_CLASS(CHciExtensionConduit) : public CActive
	{
public:
  	// Construct/destruct
	IMPORT_C static CHciExtensionConduit* NewL(MVendorSpecificHciConduit& aConduit);
	IMPORT_C ~CHciExtensionConduit();

	// Commands
	IMPORT_C TInt IssueCommandL(const TUint16 aOpcode, const TDesC8& aParameters);
	IMPORT_C TInt IssueCommandExpectingCompleteEventL(const TUint16 aOpcode, const TDesC8& aParameters);
	IMPORT_C TInt WaitForEvent();
	IMPORT_C void StopWaitingForEvent();
	IMPORT_C void CommandCompleteEventData(TPtrC8& aOutPtrToData);

protected:
	IMPORT_C void ConstructL();

private:
	// Construct/destruct
	CHciExtensionConduit(MVendorSpecificHciConduit& aConduit);

	// Command engine
	TInt IssueCommandL(const TUint16 aOpcode, const TDesC8& aParameters, TUint aName);
	

	// From CActive
	void RunL();
	void DoCancel();

private:
	TUint								iCurrentIoctl;
	TBool								iWaitForEvent;
	RSocketServ							iSocketServ;
	RSocket								iSocket;
	MVendorSpecificHciConduit&			iConduit;
	TProtocolDesc						iL2CapProtoDesc;
	TBuf8<255>							iBuf;
	// On the downcall of the Ioctl iVSIoctl is a pointer to a package buffer of the command to send.
	// Upon return the pointer is (of course) to a descriptor which is now raw command complete event data.
	// There's a 'gotcha' related to this (ab)use of the buffer: the maximum size of the data returned
	// is 2 bytes more than that of the data written (more headers), which means we can really handle
	// only 253 bytes of user data in the Command Completion Event.
	THCIWriteVendorSpecificFrameBuf*	iVSIoctl;
	};

#endif
