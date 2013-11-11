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
// on Tue, 20 Feb 2007 10:27:14 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef QOSSETUPCOMMAND_H
#define QOSSETUPCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the QOSSetup HCI Command
*/

class CQOSSetupCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CQOSSetupCommand* NewL(THCIConnectionHandle aConnectionHandle, TUint8 aFlags, TUint8 aServiceType, TUint32 aTokenRate, TUint32 aPeakBandwidth, TUint32 aLatency, TUint32 aDelayVariation);
	IMPORT_C static CQOSSetupCommand* NewL();
	// Destructor
	~CQOSSetupCommand();

	virtual void Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const;
	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(THCIConnectionHandle aConnectionHandle, TUint8 aFlags, TUint8 aServiceType, TUint32 aTokenRate, TUint32 aPeakBandwidth, TUint32 aLatency, TUint32 aDelayVariation);

	// Accessor methods for the parameters of the command
	IMPORT_C THCIConnectionHandle ConnectionHandle() const;
	IMPORT_C TUint8 Flags() const;
	IMPORT_C TUint8 ServiceType() const;
	IMPORT_C TUint32 TokenRate() const;
	IMPORT_C TUint32 PeakBandwidth() const;
	IMPORT_C TUint32 Latency() const;
	IMPORT_C TUint32 DelayVariation() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CQOSSetupCommand(THCIConnectionHandle aConnectionHandle, TUint8 aFlags, TUint8 aServiceType, TUint32 aTokenRate, TUint32 aPeakBandwidth, TUint32 aLatency, TUint32 aDelayVariation);
	CQOSSetupCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	THCIConnectionHandle iConnectionHandle;
	TUint8 iFlags;
	TUint8 iServiceType;
	TUint32 iTokenRate;
	TUint32 iPeakBandwidth;
	TUint32 iLatency;
	TUint32 iDelayVariation;
	
	};

#endif // QOSSETUPCOMMAND_H
