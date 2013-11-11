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
// on Thu, 29 May 2008 15:17:48 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef WRITEDEFAULTERRONEOUSDATAREPORTINGCOMMAND_H
#define WRITEDEFAULTERRONEOUSDATAREPORTINGCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the WriteDefaultErroneousDataReporting HCI Command
*/

class CWriteDefaultErroneousDataReportingCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CWriteDefaultErroneousDataReportingCommand* NewL(TUint8 aErroneousDataReporting);
	IMPORT_C static CWriteDefaultErroneousDataReportingCommand* NewL();
	// Destructor
	~CWriteDefaultErroneousDataReportingCommand();

	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(TUint8 aErroneousDataReporting);

	// Accessor methods for the parameters of the command
	IMPORT_C TUint8 ErroneousDataReporting() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CWriteDefaultErroneousDataReportingCommand(TUint8 aErroneousDataReporting);
	CWriteDefaultErroneousDataReportingCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TUint8 iErroneousDataReporting;
	
	};

#endif // WRITEDEFAULTERRONEOUSDATAREPORTINGCOMMAND_H
