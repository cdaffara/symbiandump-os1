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
// on Mon, 12 Nov 2007 15:28:59 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef WRITEEXTENDEDINQUIRYRESPONSECOMMAND_H
#define WRITEEXTENDEDINQUIRYRESPONSECOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the WriteExtendedInquiryResponse HCI Command
*/

class CWriteExtendedInquiryResponseCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CWriteExtendedInquiryResponseCommand* NewL(TUint8 aFECRequired, const TDesC8& aExtendedInquiryResponse);
	IMPORT_C static CWriteExtendedInquiryResponseCommand* NewL();
	// Destructor
	~CWriteExtendedInquiryResponseCommand();

	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(TUint8 aFECRequired, const TDesC8& aExtendedInquiryResponse);

	// Accessor methods for the parameters of the command
	IMPORT_C TUint8 FECRequired() const;
	IMPORT_C TDesC8 ExtendedInquiryResponse() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CWriteExtendedInquiryResponseCommand(TUint8 aFECRequired, const TDesC8& aExtendedInquiryResponse);
	CWriteExtendedInquiryResponseCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TUint8 iFECRequired;
	TBuf8<240> iExtendedInquiryResponse;
	
	};

#endif // WRITEEXTENDEDINQUIRYRESPONSECOMMAND_H
