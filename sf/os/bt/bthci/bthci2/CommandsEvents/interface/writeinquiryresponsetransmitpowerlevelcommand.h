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
// on Tue, 15 Jan 2008 17:08:14 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef WRITEINQUIRYRESPONSETRANSMITPOWERLEVELCOMMAND_H
#define WRITEINQUIRYRESPONSETRANSMITPOWERLEVELCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the WriteInquiryResponseTransmitPowerLevel HCI Command
*/

class CWriteInquiryResponseTransmitPowerLevelCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CWriteInquiryResponseTransmitPowerLevelCommand* NewL(TInt8 aTxPowerLevel);
	IMPORT_C static CWriteInquiryResponseTransmitPowerLevelCommand* NewL();
	// Destructor
	~CWriteInquiryResponseTransmitPowerLevelCommand();

	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(TInt8 aTxPowerLevel);

	// Accessor methods for the parameters of the command
	IMPORT_C TInt8 TxPowerLevel() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CWriteInquiryResponseTransmitPowerLevelCommand(TInt8 aTxPowerLevel);
	CWriteInquiryResponseTransmitPowerLevelCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TInt8 iTxPowerLevel;
	
	};

#endif // WRITEINQUIRYRESPONSETRANSMITPOWERLEVELCOMMAND_H
