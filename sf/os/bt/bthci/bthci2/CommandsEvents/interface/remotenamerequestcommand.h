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

#ifndef REMOTENAMEREQUESTCOMMAND_H
#define REMOTENAMEREQUESTCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the RemoteNameRequest HCI Command
*/

class CRemoteNameRequestCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CRemoteNameRequestCommand* NewL(const TBTDevAddr& aBDADDR, THCIPageScanRepetitionMode aPageScanRepetitionMode, THCIReservedOctet aReserved, THCIClockOffset aClockOffset);
	IMPORT_C static CRemoteNameRequestCommand* NewL();
	// Destructor
	~CRemoteNameRequestCommand();

	virtual void Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const;
	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(const TBTDevAddr& aBDADDR, THCIPageScanRepetitionMode aPageScanRepetitionMode, THCIReservedOctet aReserved, THCIClockOffset aClockOffset);

	// Accessor methods for the parameters of the command
	IMPORT_C TBTDevAddr BDADDR() const;
	IMPORT_C THCIPageScanRepetitionMode PageScanRepetitionMode() const;
	IMPORT_C THCIReservedOctet Reserved() const;
	IMPORT_C THCIClockOffset ClockOffset() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CRemoteNameRequestCommand(const TBTDevAddr& aBDADDR, THCIPageScanRepetitionMode aPageScanRepetitionMode, THCIReservedOctet aReserved, THCIClockOffset aClockOffset);
	CRemoteNameRequestCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TBTDevAddr iBDADDR;
	THCIPageScanRepetitionMode iPageScanRepetitionMode;
	THCIReservedOctet iReserved;
	THCIClockOffset iClockOffset;
	
	};

#endif // REMOTENAMEREQUESTCOMMAND_H
