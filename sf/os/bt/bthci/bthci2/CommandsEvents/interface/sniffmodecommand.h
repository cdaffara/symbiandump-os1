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
// on Wed, 25 Jul 2007 17:00:37 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef SNIFFMODECOMMAND_H
#define SNIFFMODECOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the SniffMode HCI Command
*/

class CSniffModeCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CSniffModeCommand* NewL(THCIConnectionHandle aConnectionHandle, TUint16 aSniffMaxInterval, TUint16 aSniffMinInterval, TUint16 aSniffAttempt, TUint16 aSniffTimeout);
	IMPORT_C static CSniffModeCommand* NewL();
	// Destructor
	~CSniffModeCommand();

	virtual void Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const;
	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(THCIConnectionHandle aConnectionHandle, TUint16 aSniffMaxInterval, TUint16 aSniffMinInterval, TUint16 aSniffAttempt, TUint16 aSniffTimeout);

	// Accessor methods for the parameters of the command
	IMPORT_C THCIConnectionHandle ConnectionHandle() const;
	IMPORT_C TUint16 SniffMaxInterval() const;
	IMPORT_C TUint16 SniffMinInterval() const;
	IMPORT_C TUint16 SniffAttempt() const;
	IMPORT_C TUint16 SniffTimeout() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CSniffModeCommand(THCIConnectionHandle aConnectionHandle, TUint16 aSniffMaxInterval, TUint16 aSniffMinInterval, TUint16 aSniffAttempt, TUint16 aSniffTimeout);
	CSniffModeCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	THCIConnectionHandle iConnectionHandle;
	TUint16 iSniffMaxInterval;
	TUint16 iSniffMinInterval;
	TUint16 iSniffAttempt;
	TUint16 iSniffTimeout;
	
	};

#endif // SNIFFMODECOMMAND_H
