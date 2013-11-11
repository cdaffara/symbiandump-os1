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

#ifndef REJECTCONNECTIONREQUESTCOMMAND_H
#define REJECTCONNECTIONREQUESTCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the RejectConnectionRequest HCI Command
*/

class CRejectConnectionRequestCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CRejectConnectionRequestCommand* NewL(const TBTDevAddr& aBDADDR, TUint8 aReason);
	IMPORT_C static CRejectConnectionRequestCommand* NewL();
	// Destructor
	~CRejectConnectionRequestCommand();

	virtual void Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const;
	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(const TBTDevAddr& aBDADDR, TUint8 aReason);

	// Accessor methods for the parameters of the command
	IMPORT_C TBTDevAddr BDADDR() const;
	IMPORT_C TUint8 Reason() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CRejectConnectionRequestCommand(const TBTDevAddr& aBDADDR, TUint8 aReason);
	CRejectConnectionRequestCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TBTDevAddr iBDADDR;
	TUint8 iReason;
	
	};

#endif // REJECTCONNECTIONREQUESTCOMMAND_H
