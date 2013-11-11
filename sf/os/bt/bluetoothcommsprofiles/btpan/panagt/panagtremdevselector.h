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

#ifndef PANAGTREMOTEDEVICESELECTOR_H
#define PANAGTREMOTEDEVICESELECTOR_H

#include <e32std.h>
#include <e32base.h>
#include <cdblen.h>
#include <btextnotifiers.h>
#include "panhelper.h"
#include "panagtutils.h"
#include <btextnotifierspartner.h>

/**
@file
@note Remote device selection classes
*/

class TBTDevAddr;
class CCommsDbAccess;

namespace PanAgent
/**
@internalComponent
@released
@since v8.1
*/
{

/** 
Priority of remote device selection AO
@internalComponent
@see KPanAgtAoPriority
*/
const TInt KPanAgtAoRemoteDevicePromptPriority = CActive::EPriorityStandard; 

class MOutgoingConnectionCreator
/**
An interface to allow the outgoing connection prompter class to hand off new connections to a 
channel manager
@internalComponent
*/
	{
public:
	/**
	Create a new connection to the specified device
	*/
	virtual void CreateNewConnectionForOutgoingL(const TBTDevAddr& aRemDevAddr) = 0;
	
	/**
	An error occured whilst selecting devices
	*/
	virtual TInt DeviceSelectionError(TInt aErr) = 0;
	
protected:
	virtual ~MOutgoingConnectionCreator(){};
	};
	
NONSHARABLE_CLASS(CPanRemoteDeviceSelector) : public CActive
/**
Find out which remote devices to connect to, either by reading addresses out of commdb, or by prompting the user
@note This class passes the remote device addresses back to the role state machine
@note This is one of two ways to start a connection - the other being using the Control() call on the role state machine
@internalComponent
*/
	{
public:
	CPanRemoteDeviceSelector(MOutgoingConnectionCreator& iOutgoingConnectionCreator, CCommsDbAccess& aDatabase);
	~CPanRemoteDeviceSelector();
	void PerformRemoteDeviceSelectionL();
	void CreateNextConnectionL();
	TBool IsPrompting();
	
	virtual void RunL();
	virtual void DoCancel();
	TBool HasMoreDevices();
	void Reset();
private:
	TInt CreateDevicesFromUserSelection();
	TInt DiscoverRemoteDevices(); // for when we've got a list of devices in commdb and we only want to connect to one of them
	void ConvertListToArray(TDesC& aDeviceAddrList);
public:

private:
	enum TRemoteDeviceSelectorState
		{
		ENotWaiting,				//< Not waiting for anything
		EWaitingForNotifier,		//< Waiting for the user to respond to our remote device selection notifier
		EWaitingForDeviceDiscovery	//< Waiting for a device discovery to complete, so we can choose a remote device
		};
		
	MOutgoingConnectionCreator& iOutgoingConnectionCreator; //< The PAN agent
	CCommsDbAccess& iDatabase;								//< Commdb access object
	TRemoteDeviceSelectorState iState;						//< What we're waiting for in our RunL

	TPckgBuf<TBTDevAddr> iRemDevAddrBuf;					//< The remote device address to which we should connect

	RNotifier iNotifier; 				//< Interface to PAN remote device selection dialog
	//Buffer received from the notifier on completion
	TBTDeviceList iDeviceList;			
	//This variable is introduced due to the low usability of TBTDeviceList as a devices storage
	//TBTDeviceList doesn't have the basic operations like Count() or Remove(), but we have to use it
	//for the notifier interface
	RArray<TBTDevAddr> iDevAddresses;
	
	/**
	If no more fixed devices available to try connecting to (we've tried all the
	specified ones but still have no connection), prompt the user.  This is
	different to the other 'prompt' behaviour, in that it takes place after the
	fixed device list has been consulted rather than before.
	*/
	TBool iPromptIfMACFails;
	};

}	// PanAgent
#endif // PANAGTREMOTEDEVICESELECTOR_H
