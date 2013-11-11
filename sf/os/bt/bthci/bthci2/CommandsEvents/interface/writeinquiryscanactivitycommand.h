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
// on Wed, 25 Jul 2007 17:00:39 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef WRITEINQUIRYSCANACTIVITYCOMMAND_H
#define WRITEINQUIRYSCANACTIVITYCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the WriteInquiryScanActivity HCI Command
*/

class CWriteInquiryScanActivityCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CWriteInquiryScanActivityCommand* NewL(TUint16 aInquiryScanInterval, TUint16 aInquiryScanWindow);
	IMPORT_C static CWriteInquiryScanActivityCommand* NewL();
	// Destructor
	~CWriteInquiryScanActivityCommand();

	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(TUint16 aInquiryScanInterval, TUint16 aInquiryScanWindow);

	// Accessor methods for the parameters of the command
	IMPORT_C TUint16 InquiryScanInterval() const;
	IMPORT_C TUint16 InquiryScanWindow() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CWriteInquiryScanActivityCommand(TUint16 aInquiryScanInterval, TUint16 aInquiryScanWindow);
	CWriteInquiryScanActivityCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TUint16 iInquiryScanInterval;
	TUint16 iInquiryScanWindow;
	
	};

#endif // WRITEINQUIRYSCANACTIVITYCOMMAND_H
