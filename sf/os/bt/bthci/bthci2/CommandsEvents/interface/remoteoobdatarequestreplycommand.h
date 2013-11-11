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
// This file was generated automatically from the template commandheader.tmpl
// on Sat, 31 May 2008 18:58:48 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef REMOTEOOBDATAREQUESTREPLYCOMMAND_H
#define REMOTEOOBDATAREQUESTREPLYCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types
#include <bttypespartner.h>

/**
This class represents the RemoteOOBDataRequestReply HCI Command
*/

class CRemoteOOBDataRequestReplyCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CRemoteOOBDataRequestReplyCommand* NewL(const TBTDevAddr& aBDADDR, const TBluetoothSimplePairingHash& aOOBDataC, const TBluetoothSimplePairingRandomizer& aOOBDataR);
	IMPORT_C static CRemoteOOBDataRequestReplyCommand* NewL();
	// Destructor
	~CRemoteOOBDataRequestReplyCommand();

	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(const TBTDevAddr& aBDADDR, const TBluetoothSimplePairingHash& aOOBDataC, const TBluetoothSimplePairingRandomizer& aOOBDataR);

	// Accessor methods for the parameters of the command
	IMPORT_C TBTDevAddr BDADDR() const;
	IMPORT_C TBluetoothSimplePairingHash OOBDataC() const;
	IMPORT_C TBluetoothSimplePairingRandomizer OOBDataR() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CRemoteOOBDataRequestReplyCommand(const TBTDevAddr& aBDADDR, const TBluetoothSimplePairingHash& aOOBDataC, const TBluetoothSimplePairingRandomizer& aOOBDataR);
	CRemoteOOBDataRequestReplyCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TBTDevAddr iBDADDR;
	TBluetoothSimplePairingHash iOOBDataC;
	TBluetoothSimplePairingRandomizer iOOBDataR;
	
	};

#endif // REMOTEOOBDATAREQUESTREPLYCOMMAND_H
