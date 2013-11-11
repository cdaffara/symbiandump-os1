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
// on Wed, 25 Jul 2007 17:00:39 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef ACCEPTSYNCHRONOUSCONNECTIONREQUESTCOMMAND_H
#define ACCEPTSYNCHRONOUSCONNECTIONREQUESTCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the AcceptSynchronousConnectionRequest HCI Command
*/

class CAcceptSynchronousConnectionRequestCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CAcceptSynchronousConnectionRequestCommand* NewL(const TBTDevAddr& aBDADDR, TUint32 aTransmitBandwidth, TUint32 aReceiveBandwidth, TUint16 aMaxLatency, TUint16 aContentFormat, TUint8 aRetransmissionEffort, TUint16 aPacketType);
	IMPORT_C static CAcceptSynchronousConnectionRequestCommand* NewL();
	// Destructor
	~CAcceptSynchronousConnectionRequestCommand();

	virtual void Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const;
	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(const TBTDevAddr& aBDADDR, TUint32 aTransmitBandwidth, TUint32 aReceiveBandwidth, TUint16 aMaxLatency, TUint16 aContentFormat, TUint8 aRetransmissionEffort, TUint16 aPacketType);

	// Accessor methods for the parameters of the command
	IMPORT_C TBTDevAddr BDADDR() const;
	IMPORT_C TUint32 TransmitBandwidth() const;
	IMPORT_C TUint32 ReceiveBandwidth() const;
	IMPORT_C TUint16 MaxLatency() const;
	IMPORT_C TUint16 ContentFormat() const;
	IMPORT_C TUint8 RetransmissionEffort() const;
	IMPORT_C TUint16 PacketType() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CAcceptSynchronousConnectionRequestCommand(const TBTDevAddr& aBDADDR, TUint32 aTransmitBandwidth, TUint32 aReceiveBandwidth, TUint16 aMaxLatency, TUint16 aContentFormat, TUint8 aRetransmissionEffort, TUint16 aPacketType);
	CAcceptSynchronousConnectionRequestCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TBTDevAddr iBDADDR;
	TUint32 iTransmitBandwidth;
	TUint32 iReceiveBandwidth;
	TUint16 iMaxLatency;
	TUint16 iContentFormat;
	TUint8 iRetransmissionEffort;
	TUint16 iPacketType;
	
	};

#endif // ACCEPTSYNCHRONOUSCONNECTIONREQUESTCOMMAND_H
