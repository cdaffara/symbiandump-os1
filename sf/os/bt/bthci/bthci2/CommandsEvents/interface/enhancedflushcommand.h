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
// on Thu, 29 May 2008 16:17:36 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef ENHANCEDFLUSHCOMMAND_H
#define ENHANCEDFLUSHCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the EnhancedFlush HCI Command
*/

class CEnhancedFlushCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CEnhancedFlushCommand* NewL(THCIConnectionHandle aConnectionHandle, TUint8 aPacketType);
	IMPORT_C static CEnhancedFlushCommand* NewL();
	// Destructor
	~CEnhancedFlushCommand();

	virtual void Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const;
	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(THCIConnectionHandle aConnectionHandle, TUint8 aPacketType);

	// Accessor methods for the parameters of the command
	IMPORT_C THCIConnectionHandle ConnectionHandle() const;
	IMPORT_C TUint8 PacketType() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CEnhancedFlushCommand(THCIConnectionHandle aConnectionHandle, TUint8 aPacketType);
	CEnhancedFlushCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	THCIConnectionHandle iConnectionHandle;
	TUint8 iPacketType;
	
	};

#endif // ENHANCEDFLUSHCOMMAND_H
