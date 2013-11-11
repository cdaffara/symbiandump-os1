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
// on Wed, 25 Jul 2007 17:00:38 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef WRITELINKSUPERVISIONTIMEOUTCOMMAND_H
#define WRITELINKSUPERVISIONTIMEOUTCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the WriteLinkSupervisionTimeout HCI Command
*/

class CWriteLinkSupervisionTimeoutCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CWriteLinkSupervisionTimeoutCommand* NewL(THCIConnectionHandle aConnectionHandle, TUint16 aLinkSupervisionTimeout);
	IMPORT_C static CWriteLinkSupervisionTimeoutCommand* NewL();
	// Destructor
	~CWriteLinkSupervisionTimeoutCommand();

	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(THCIConnectionHandle aConnectionHandle, TUint16 aLinkSupervisionTimeout);

	// Accessor methods for the parameters of the command
	IMPORT_C THCIConnectionHandle ConnectionHandle() const;
	IMPORT_C TUint16 LinkSupervisionTimeout() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CWriteLinkSupervisionTimeoutCommand(THCIConnectionHandle aConnectionHandle, TUint16 aLinkSupervisionTimeout);
	CWriteLinkSupervisionTimeoutCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	THCIConnectionHandle iConnectionHandle;
	TUint16 iLinkSupervisionTimeout;
	
	};

#endif // WRITELINKSUPERVISIONTIMEOUTCOMMAND_H
