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
// on Wed, 25 Jul 2007 17:00:40 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef INQUIRYCOMMAND_H
#define INQUIRYCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the Inquiry HCI Command
*/

class CInquiryCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CInquiryCommand* NewL(TUint32 aLAP, TUint8 aInquiryLength, TUint8 aNumResponses);
	IMPORT_C static CInquiryCommand* NewL();
	// Destructor
	~CInquiryCommand();

	virtual void Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const;
	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(TUint32 aLAP, TUint8 aInquiryLength, TUint8 aNumResponses);

	// Accessor methods for the parameters of the command
	IMPORT_C TUint32 LAP() const;
	IMPORT_C TUint8 InquiryLength() const;
	IMPORT_C TUint8 NumResponses() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CInquiryCommand(TUint32 aLAP, TUint8 aInquiryLength, TUint8 aNumResponses);
	CInquiryCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TUint32 iLAP;
	TUint8 iInquiryLength;
	TUint8 iNumResponses;
	
	};

#endif // INQUIRYCOMMAND_H
