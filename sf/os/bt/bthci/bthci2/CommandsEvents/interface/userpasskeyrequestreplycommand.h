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
// on Wed, 05 Dec 2007 14:43:53 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef USERPASSKEYREQUESTREPLYCOMMAND_H
#define USERPASSKEYREQUESTREPLYCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the UserPasskeyRequestReply HCI Command
*/

class CUserPasskeyRequestReplyCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CUserPasskeyRequestReplyCommand* NewL(const TBTDevAddr& aBDADDR, TUint32 aNumericValue);
	IMPORT_C static CUserPasskeyRequestReplyCommand* NewL();
	// Destructor
	~CUserPasskeyRequestReplyCommand();

	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(const TBTDevAddr& aBDADDR, TUint32 aNumericValue);

	// Accessor methods for the parameters of the command
	IMPORT_C TBTDevAddr BDADDR() const;
	IMPORT_C TUint32 NumericValue() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CUserPasskeyRequestReplyCommand(const TBTDevAddr& aBDADDR, TUint32 aNumericValue);
	CUserPasskeyRequestReplyCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TBTDevAddr iBDADDR;
	TUint32 iNumericValue;
	
	};

#endif // USERPASSKEYREQUESTREPLYCOMMAND_H
