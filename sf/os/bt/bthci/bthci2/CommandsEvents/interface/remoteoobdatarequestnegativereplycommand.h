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
// on Wed, 05 Dec 2007 14:54:29 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef REMOTEOOBDATAREQUESTNEGATIVEREPLYCOMMAND_H
#define REMOTEOOBDATAREQUESTNEGATIVEREPLYCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the RemoteOOBDataRequestNegativeReply HCI Command
*/

class CRemoteOOBDataRequestNegativeReplyCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CRemoteOOBDataRequestNegativeReplyCommand* NewL(const TBTDevAddr& aBDADDR);
	IMPORT_C static CRemoteOOBDataRequestNegativeReplyCommand* NewL();
	// Destructor
	~CRemoteOOBDataRequestNegativeReplyCommand();

	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(const TBTDevAddr& aBDADDR);

	// Accessor methods for the parameters of the command
	IMPORT_C TBTDevAddr BDADDR() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CRemoteOOBDataRequestNegativeReplyCommand(const TBTDevAddr& aBDADDR);
	CRemoteOOBDataRequestNegativeReplyCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TBTDevAddr iBDADDR;
	
	};

#endif // REMOTEOOBDATAREQUESTNEGATIVEREPLYCOMMAND_H
