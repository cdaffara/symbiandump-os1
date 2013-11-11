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
// on Wed, 07 Apr 2010 11:43:52 (time stamp)
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef WRITECURRENTIACLAPCOMMAND_H
#define WRITECURRENTIACLAPCOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the WriteCurrentIACLAP HCI Command
*/

class CWriteCurrentIACLAPCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CWriteCurrentIACLAPCommand* NewL(TUint8 aNumCurrentIAC, const RArray< TUint32 >& aIACLAP);
	IMPORT_C static CWriteCurrentIACLAPCommand* NewL();
	// Destructor
	~CWriteCurrentIACLAPCommand();

	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(TUint8 aNumCurrentIAC, const RArray< TUint32 >& aIACLAP);

	// Accessor methods for the parameters of the command
	IMPORT_C TUint8 NumCurrentIAC() const;
	IMPORT_C TUint32 IACLAP(TInt aIndex) const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CWriteCurrentIACLAPCommand(TUint8 aNumCurrentIAC);
	CWriteCurrentIACLAPCommand();
	void ConstructL(const RArray< TUint32 >& aIACLAP);
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TUint8 iNumCurrentIAC;
	RArray< TUint32 > iIACLAP;
	
	};

#endif // WRITECURRENTIACLAPCOMMAND_H
