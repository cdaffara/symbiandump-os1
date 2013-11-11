// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// on Wed, 07 Apr 2010 11:43:50 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef HOSTNUMBEROFCOMPLETEDPACKETSCOMMAND_H
#define HOSTNUMBEROFCOMPLETEDPACKETSCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the HostNumberOfCompletedPackets HCI Command
*/

class CHostNumberOfCompletedPacketsCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CHostNumberOfCompletedPacketsCommand* NewL(TUint8 aNumberOfHandles, const RArray< THCIConnectionHandle >& aConnectionHandle, const RArray< THCINumOfCompletedPackets >& aHostNumOfCompletedPackets);
	IMPORT_C static CHostNumberOfCompletedPacketsCommand* NewL();
	// Destructor
	~CHostNumberOfCompletedPacketsCommand();

	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(TUint8 aNumberOfHandles, const RArray< THCIConnectionHandle >& aConnectionHandle, const RArray< THCINumOfCompletedPackets >& aHostNumOfCompletedPackets);

	// Accessor methods for the parameters of the command
	IMPORT_C TUint8 NumberOfHandles() const;
	IMPORT_C THCIConnectionHandle ConnectionHandle(TInt aIndex) const;
	IMPORT_C THCINumOfCompletedPackets HostNumOfCompletedPackets(TInt aIndex) const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CHostNumberOfCompletedPacketsCommand(TUint8 aNumberOfHandles);
	CHostNumberOfCompletedPacketsCommand();
	void ConstructL(const RArray< THCIConnectionHandle >& aConnectionHandle, const RArray< THCINumOfCompletedPackets >& aHostNumOfCompletedPackets);
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TUint8 iNumberOfHandles;
	RArray< THCIConnectionHandle > iConnectionHandle;
	RArray< THCINumOfCompletedPackets > iHostNumOfCompletedPackets;
	
	};

#endif // HOSTNUMBEROFCOMPLETEDPACKETSCOMMAND_H
