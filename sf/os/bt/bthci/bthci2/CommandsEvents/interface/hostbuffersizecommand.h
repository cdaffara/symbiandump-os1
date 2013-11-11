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
// on Wed, 25 Jul 2007 17:00:41 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef HOSTBUFFERSIZECOMMAND_H
#define HOSTBUFFERSIZECOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the HostBufferSize HCI Command
*/

class CHostBufferSizeCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CHostBufferSizeCommand* NewL(TUint16 aHostACLDataPacketLength, TUint8 aHostSynchronousDataPacketLength, TUint16 aHostTotalNumACLDataPackets, TUint16 aHostTotalNumSynchronousDataPackets);
	IMPORT_C static CHostBufferSizeCommand* NewL();
	// Destructor
	~CHostBufferSizeCommand();

	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(TUint16 aHostACLDataPacketLength, TUint8 aHostSynchronousDataPacketLength, TUint16 aHostTotalNumACLDataPackets, TUint16 aHostTotalNumSynchronousDataPackets);

	// Accessor methods for the parameters of the command
	IMPORT_C TUint16 HostACLDataPacketLength() const;
	IMPORT_C TUint8 HostSynchronousDataPacketLength() const;
	IMPORT_C TUint16 HostTotalNumACLDataPackets() const;
	IMPORT_C TUint16 HostTotalNumSynchronousDataPackets() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CHostBufferSizeCommand(TUint16 aHostACLDataPacketLength, TUint8 aHostSynchronousDataPacketLength, TUint16 aHostTotalNumACLDataPackets, TUint16 aHostTotalNumSynchronousDataPackets);
	CHostBufferSizeCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TUint16 iHostACLDataPacketLength;
	TUint8 iHostSynchronousDataPacketLength;
	TUint16 iHostTotalNumACLDataPackets;
	TUint16 iHostTotalNumSynchronousDataPackets;
	
	};

#endif // HOSTBUFFERSIZECOMMAND_H
