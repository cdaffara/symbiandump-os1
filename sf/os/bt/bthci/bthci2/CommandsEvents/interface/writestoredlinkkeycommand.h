// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// on Wed, 07 Apr 2010 11:43:53 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef WRITESTOREDLINKKEYCOMMAND_H
#define WRITESTOREDLINKKEYCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the WriteStoredLinkKey HCI Command
*/

class CWriteStoredLinkKeyCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CWriteStoredLinkKeyCommand* NewL(TUint8 aNumKeysToWrite, const RArray< TBTDevAddr >& aBDADDR, const RArray< TBTLinkKey >& aLinkKey);
	IMPORT_C static CWriteStoredLinkKeyCommand* NewL();
	// Destructor
	~CWriteStoredLinkKeyCommand();

	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(TUint8 aNumKeysToWrite, const RArray< TBTDevAddr >& aBDADDR, const RArray< TBTLinkKey >& aLinkKey);

	// Accessor methods for the parameters of the command
	IMPORT_C TUint8 NumKeysToWrite() const;
	IMPORT_C TBTDevAddr BDADDR(TInt aIndex) const;
	IMPORT_C TBTLinkKey LinkKey(TInt aIndex) const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CWriteStoredLinkKeyCommand(TUint8 aNumKeysToWrite);
	CWriteStoredLinkKeyCommand();
	void ConstructL(const RArray< TBTDevAddr >& aBDADDR, const RArray< TBTLinkKey >& aLinkKey);
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TUint8 iNumKeysToWrite;
	RArray< TBTDevAddr > iBDADDR;
	RArray< TBTLinkKey > iLinkKey;
	
	};

#endif // WRITESTOREDLINKKEYCOMMAND_H
