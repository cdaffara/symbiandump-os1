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

#ifndef WRITEAUTHENTICATIONENABLECOMMAND_H
#define WRITEAUTHENTICATIONENABLECOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the WriteAuthenticationEnable HCI Command
*/

class CWriteAuthenticationEnableCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CWriteAuthenticationEnableCommand* NewL(TUint8 aAuthenticationEnable);
	IMPORT_C static CWriteAuthenticationEnableCommand* NewL();
	// Destructor
	~CWriteAuthenticationEnableCommand();

	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(TUint8 aAuthenticationEnable);

	// Accessor methods for the parameters of the command
	IMPORT_C TUint8 AuthenticationEnable() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CWriteAuthenticationEnableCommand(TUint8 aAuthenticationEnable);
	CWriteAuthenticationEnableCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TUint8 iAuthenticationEnable;
	
	};

#endif // WRITEAUTHENTICATIONENABLECOMMAND_H
