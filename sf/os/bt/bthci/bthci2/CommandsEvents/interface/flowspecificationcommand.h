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

#ifndef FLOWSPECIFICATIONCOMMAND_H
#define FLOWSPECIFICATIONCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the FlowSpecification HCI Command
*/

class CFlowSpecificationCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CFlowSpecificationCommand* NewL(THCIConnectionHandle aConnectionHandle, TUint8 aFlags, TUint8 aFlowDirection, TUint8 aServiceType, TUint32 aTokenRate, TUint32 aTokenBucketSize, TUint32 aPeakBandwidth, TUint32 aAccessLatency);
	IMPORT_C static CFlowSpecificationCommand* NewL();
	// Destructor
	~CFlowSpecificationCommand();

	virtual void Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const;
	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(THCIConnectionHandle aConnectionHandle, TUint8 aFlags, TUint8 aFlowDirection, TUint8 aServiceType, TUint32 aTokenRate, TUint32 aTokenBucketSize, TUint32 aPeakBandwidth, TUint32 aAccessLatency);

	// Accessor methods for the parameters of the command
	IMPORT_C THCIConnectionHandle ConnectionHandle() const;
	IMPORT_C TUint8 Flags() const;
	IMPORT_C TUint8 FlowDirection() const;
	IMPORT_C TUint8 ServiceType() const;
	IMPORT_C TUint32 TokenRate() const;
	IMPORT_C TUint32 TokenBucketSize() const;
	IMPORT_C TUint32 PeakBandwidth() const;
	IMPORT_C TUint32 AccessLatency() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CFlowSpecificationCommand(THCIConnectionHandle aConnectionHandle, TUint8 aFlags, TUint8 aFlowDirection, TUint8 aServiceType, TUint32 aTokenRate, TUint32 aTokenBucketSize, TUint32 aPeakBandwidth, TUint32 aAccessLatency);
	CFlowSpecificationCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	THCIConnectionHandle iConnectionHandle;
	TUint8 iFlags;
	TUint8 iFlowDirection;
	TUint8 iServiceType;
	TUint32 iTokenRate;
	TUint32 iTokenBucketSize;
	TUint32 iPeakBandwidth;
	TUint32 iAccessLatency;
	
	};

#endif // FLOWSPECIFICATIONCOMMAND_H
