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
// on Mon, 02 Jun 2008 11:36:17 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef SNIFFSUBRATINGCOMMAND_H
#define SNIFFSUBRATINGCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the SniffSubrating HCI Command
*/

class CSniffSubratingCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CSniffSubratingCommand* NewL(THCIConnectionHandle aConnectionHandle, TBasebandTime aMaximumLatency, TBasebandTime aMinimumRemoteTimeout, TBasebandTime aMinimumLocalTimeout);
	IMPORT_C static CSniffSubratingCommand* NewL();
	// Destructor
	~CSniffSubratingCommand();

	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(THCIConnectionHandle aConnectionHandle, TBasebandTime aMaximumLatency, TBasebandTime aMinimumRemoteTimeout, TBasebandTime aMinimumLocalTimeout);

	// Accessor methods for the parameters of the command
	IMPORT_C THCIConnectionHandle ConnectionHandle() const;
	IMPORT_C TBasebandTime MaximumLatency() const;
	IMPORT_C TBasebandTime MinimumRemoteTimeout() const;
	IMPORT_C TBasebandTime MinimumLocalTimeout() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CSniffSubratingCommand(THCIConnectionHandle aConnectionHandle, TBasebandTime aMaximumLatency, TBasebandTime aMinimumRemoteTimeout, TBasebandTime aMinimumLocalTimeout);
	CSniffSubratingCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	THCIConnectionHandle iConnectionHandle;
	TBasebandTime iMaximumLatency;
	TBasebandTime iMinimumRemoteTimeout;
	TBasebandTime iMinimumLocalTimeout;
	
	};

#endif // SNIFFSUBRATINGCOMMAND_H
