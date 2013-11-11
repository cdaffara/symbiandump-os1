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

#ifndef WRITEINQUIRYMODECOMMAND_H
#define WRITEINQUIRYMODECOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the WriteInquiryMode HCI Command
*/

class CWriteInquiryModeCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CWriteInquiryModeCommand* NewL(TUint8 aInquiryMode);
	IMPORT_C static CWriteInquiryModeCommand* NewL();
	// Destructor
	~CWriteInquiryModeCommand();

	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(TUint8 aInquiryMode);

	// Accessor methods for the parameters of the command
	IMPORT_C TUint8 InquiryMode() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CWriteInquiryModeCommand(TUint8 aInquiryMode);
	CWriteInquiryModeCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TUint8 iInquiryMode;
	
	};

#endif // WRITEINQUIRYMODECOMMAND_H
