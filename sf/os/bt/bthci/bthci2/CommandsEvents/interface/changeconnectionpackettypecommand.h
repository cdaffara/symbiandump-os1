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

#ifndef CHANGECONNECTIONPACKETTYPECOMMAND_H
#define CHANGECONNECTIONPACKETTYPECOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the ChangeConnectionPacketType HCI Command
*/

class CChangeConnectionPacketTypeCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CChangeConnectionPacketTypeCommand* NewL(THCIConnectionHandle aConnectionHandle, TUint16 aPacketType);
	IMPORT_C static CChangeConnectionPacketTypeCommand* NewL();
	// Destructor
	~CChangeConnectionPacketTypeCommand();

	virtual void Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const;
	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(THCIConnectionHandle aConnectionHandle, TUint16 aPacketType);

	// Accessor methods for the parameters of the command
	IMPORT_C THCIConnectionHandle ConnectionHandle() const;
	IMPORT_C TUint16 PacketType() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CChangeConnectionPacketTypeCommand(THCIConnectionHandle aConnectionHandle, TUint16 aPacketType);
	CChangeConnectionPacketTypeCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	THCIConnectionHandle iConnectionHandle;
	TUint16 iPacketType;
	
	};

#endif // CHANGECONNECTIONPACKETTYPECOMMAND_H
