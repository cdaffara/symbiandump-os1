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
// on Wed, 25 Jul 2007 17:00:40 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef SETCONNECTIONENCRYPTIONCOMMAND_H
#define SETCONNECTIONENCRYPTIONCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the SetConnectionEncryption HCI Command
*/

class CSetConnectionEncryptionCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CSetConnectionEncryptionCommand* NewL(THCIConnectionHandle aConnectionHandle, TUint8 aEncryptionEnable);
	IMPORT_C static CSetConnectionEncryptionCommand* NewL();
	// Destructor
	~CSetConnectionEncryptionCommand();

	virtual void Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const;
	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(THCIConnectionHandle aConnectionHandle, TUint8 aEncryptionEnable);

	// Accessor methods for the parameters of the command
	IMPORT_C THCIConnectionHandle ConnectionHandle() const;
	IMPORT_C TUint8 EncryptionEnable() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CSetConnectionEncryptionCommand(THCIConnectionHandle aConnectionHandle, TUint8 aEncryptionEnable);
	CSetConnectionEncryptionCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	THCIConnectionHandle iConnectionHandle;
	TUint8 iEncryptionEnable;
	
	};

#endif // SETCONNECTIONENCRYPTIONCOMMAND_H
