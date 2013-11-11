// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <kernel/kern_priv.h>
#include <graphics/surfacemanager.h>
#include "surfacemanager_dev.h"




DECLARE_EXTENSION_LDD()
	{
	return new DSurfaceManagerFactory;
	}

static DSurfaceManager Manager;



DSurfaceManagerFactory::DSurfaceManagerFactory()
	{
	// Set version number for this device
	iVersion=RSurfaceManagerDriver::VersionRequired();

	iParseMask=0;
	}

TInt DSurfaceManagerFactory::Install()
	{
	return SetName(&RSurfaceManagerDriver::Name());
	}


/**
  Called by the kernel's device driver framework to create a Logical Channel.
  This is called in the context of the user thread (client) which requested the creation of a Logical Channel
  (E.g. through a call to RBusLogicalChannel::DoCreate)
  The thread is in a critical section.

  @param aChannel  Set to point to the created Logical Channel

  @return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DSurfaceManagerFactory::Create(DLogicalChannelBase*& aChannel)
	{
	aChannel=new DSurfaceManagerChannel();
	if(!aChannel)
		return KErrNoMemory;
	return KErrNone;
	}



/**
  Return the drivers capabilities.
  Called in the response to an RDevice::GetCaps() request.

  @param aDes  User-side descriptor to write capabilities information into
*/
void DSurfaceManagerFactory::GetCaps(TDes8& aDes) const
	{
	// Create a capabilities object
	RSurfaceManagerDriver::TCaps caps;
	caps.iVersion = iVersion;
	// Write it back to user memory
	Kern::InfoCopy(aDes,reinterpret_cast<TUint8*>(&caps),sizeof(caps));
	}



DSurfaceManagerChannel::DSurfaceManagerChannel()
	{
	TRACE(Kern::Printf("SurfaceManagerChannel Creation");)
	}


/**
Channel destructor.
Called when the process owning the channel has died or closed the channel.
Calls the manager object to indicate that the process has closed a session so it 
can cleanup the surfaces which are only owned by that process if it has no further connections.
*/
DSurfaceManagerChannel::~DSurfaceManagerChannel()
	{
	Manager.RemoveConnection(iOwner);
	}


/**
  Second stage constructor called by the kernel's device driver framework.
  This is called in the context of the user thread (client) which requested the creation of a Logical Channel
  (E.g. through a call to RBusLogicalChannel::DoCreate)
  The thread is in a critical section.

  @param aUnit  The unit argument supplied by the client to RBusLogicalChannel::DoCreate
  @param aInfo  The info argument supplied by the client to RBusLogicalChannel::DoCreate
  @param aVer  The version argument supplied by the client to RBusLogicalChannel::DoCreate

  @return KErrNone if successful, otherwise one of the other system wide error codes.
*/
TInt DSurfaceManagerChannel::DoCreate(TInt /*aUnit*/, const TDesC8* /*aInfo*/, const TVersion& aVer)
	{
	// Check version
	if (!Kern::QueryVersionSupported(RSurfaceManagerDriver::VersionRequired(),aVer))
		return KErrNotSupported;
	
	iOwner = &Kern::CurrentProcess();
	TInt ret = Manager.AddConnection(iOwner);
	if (ret != KErrNone)
		iOwner = NULL;
	
	return ret;
	}
	


/**
  Process a request on this logical channel.

  @param aReqNo Request number:
  	            ==KMaxTInt, a 'DoCancel' message
	            >=0, a 'DoControl' message with function number equal to iValue
	            <0, a 'DoRequest' message with function number equal to ~iValue
  @param a1     First argument. For DoRequest requests this is a pointer to the TRequestStatus.
  @param a2     Second argument. For DoRequest this is a pointer to the 2 actual TAny* arguments.

  @return       Result ignored by device driver framework for DoRequest requests.
*/
TInt DSurfaceManagerChannel::Request(TInt aReqNo, TAny* a1, TAny* a2)
	{
	// Decode the message type and dispatch it to the relevent handler function...
	// only using synchronous control messages
	if (static_cast<TUint>(aReqNo) < static_cast<TUint>(KMaxTInt))
		{
		return DoControl(aReqNo, a1, a2);
		}

	return KErrNotSupported;
	}

/**
  Process synchronous 'control' requests
*/
TInt DSurfaceManagerChannel::DoControl(TInt aFunction, TAny* a1, TAny* a2)
	{
	TRACE(Kern::Printf(">DSurfaceManagerChannel::DoControl fn=%d process = %u\n",aFunction, iOwner);)

	TInt r;
	switch(aFunction)
		{
		case RSurfaceManagerDriver::EControlCreateSurface:
			r = Manager.CreateSurface(reinterpret_cast<const TDesC8*>(a1), reinterpret_cast<TSurfaceId*>(a2));
			break;
			
		case RSurfaceManagerDriver::EControlOpenSurface:
			r = Manager.OpenSurface(reinterpret_cast<TSurfaceId*>(a1));
			break;
			
		case RSurfaceManagerDriver::EControlCloseSurface:
			r = Manager.CloseSurface(reinterpret_cast<TSurfaceId*>(a1));
			break;
			
		case RSurfaceManagerDriver::EControlAccessSurfaceData:
			r = Manager.MapSurface(reinterpret_cast<TSurfaceId*>(a1));
			break;
			
		case RSurfaceManagerDriver::EControlSurfaceInfo:
			r = Manager.SurfaceInfo(reinterpret_cast<TSurfaceId*>(a1), reinterpret_cast<TDes8*>(a2));
			break;

		case RSurfaceManagerDriver::EControlCreateSurfaceEx:
			r = Manager.CreateSurface(reinterpret_cast<RSurfaceManagerDriver::TDeviceParam*>(a1), (TInt)a2);
			break;
			
		case RSurfaceManagerDriver::EControlSynchronizeCache:
			r = Manager.SynchronizeCache(reinterpret_cast<RSurfaceManagerDriver::TDeviceParam*>(a1), (RSurfaceManager::TSyncOperation&)a2);
			break;

		case RSurfaceManagerDriver::EControlGetSurfaceHint:
			r = Manager.GetSurfaceHint(reinterpret_cast<TSurfaceId*>(a1), reinterpret_cast<RSurfaceManager::THintPair*>(a2));
			break;

		case RSurfaceManagerDriver::EControlSetSurfaceHint:
			r = Manager.SetSurfaceHint(reinterpret_cast<TSurfaceId*>(a1), reinterpret_cast<const RSurfaceManager::THintPair*>(a2));
			break;

		case RSurfaceManagerDriver::EControlAddSurfaceHint:
			r = Manager.AddSurfaceHint(reinterpret_cast<TSurfaceId*>(a1), reinterpret_cast<const RSurfaceManager::THintPair*>(a2));
			break;
		case RSurfaceManagerDriver::EControlGetBufferOffset:
			r = Manager.GetBufferOffset(reinterpret_cast<RSurfaceManagerDriver::TDeviceParam*>(a1),reinterpret_cast<TUint*>(a2));
			break;
		case RSurfaceManagerDriver::EControlGetSurfaceManagerAttrib:
			r = Manager.GetSurfaceManagerAttrib(reinterpret_cast<RSurfaceManager::TSurfaceManagerAttrib*>(a1),reinterpret_cast<TInt*>(a2));
			break;			
		default:
			r = KErrNotSupported;
			break;
		}
	TRACE(Kern::Printf("<SurfaceManagerChannel::DoControl result=%d\n",r);)
	return r;
	}



DECLARE_STANDARD_EXTENSION()
	{
	//called when kernel extension is loaded
	//initialise the kernel extension
	TRACE(Kern::Printf("<SurfaceManager Extension entry point\n");)
	return KErrNone;
	}

