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
// on Mon, 02 Jun 2008 11:36:14 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef IOCAPABILITYREQUESTREPLYCOMMAND_H
#define IOCAPABILITYREQUESTREPLYCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the IOCapabilityRequestReply HCI Command
*/

class CIOCapabilityRequestReplyCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CIOCapabilityRequestReplyCommand* NewL(const TBTDevAddr& aBDADDR, TUint8 aIOCapability, TUint8 aOOBDataPresent, TUint8 aAuthenticationRequirements);
	IMPORT_C static CIOCapabilityRequestReplyCommand* NewL();
	// Destructor
	~CIOCapabilityRequestReplyCommand();

	virtual void Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const;
	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(const TBTDevAddr& aBDADDR, TUint8 aIOCapability, TUint8 aOOBDataPresent, TUint8 aAuthenticationRequirements);

	// Accessor methods for the parameters of the command
	IMPORT_C TBTDevAddr BDADDR() const;
	IMPORT_C TUint8 IOCapability() const;
	IMPORT_C TUint8 OOBDataPresent() const;
	IMPORT_C TUint8 AuthenticationRequirements() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CIOCapabilityRequestReplyCommand(const TBTDevAddr& aBDADDR, TUint8 aIOCapability, TUint8 aOOBDataPresent, TUint8 aAuthenticationRequirements);
	CIOCapabilityRequestReplyCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TBTDevAddr iBDADDR;
	TUint8 iIOCapability;
	TUint8 iOOBDataPresent;
	TUint8 iAuthenticationRequirements;
	
	};

#endif // IOCAPABILITYREQUESTREPLYCOMMAND_H
