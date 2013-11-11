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

#ifndef WRITELOOPBACKMODECOMMAND_H
#define WRITELOOPBACKMODECOMMAND_H

#include <bluetooth/hci/command.h>
#include <bluetooth/hci/hcitypes.h> // in case the generated class uses Bluetooth types

/**
This class represents the WriteLoopbackMode HCI Command
*/

class CWriteLoopbackModeCommand : public CHCICommandBase
	{
public:
	// Construction
	IMPORT_C static CWriteLoopbackModeCommand* NewL(TUint8 aLoopbackMode);
	IMPORT_C static CWriteLoopbackModeCommand* NewL();
	// Destructor
	~CWriteLoopbackModeCommand();

	

	// Assign new values to the parameters of this command
	IMPORT_C void Reset(TUint8 aLoopbackMode);

	// Accessor methods for the parameters of the command
	IMPORT_C TUint8 LoopbackMode() const;
	
	// Extension function
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

private:
	CWriteLoopbackModeCommand(TUint8 aLoopbackMode);
	CWriteLoopbackModeCommand();
	
	// From CHCICommandBase - knows how to format the parameters of this specific command
	// into the HCTL frame
	virtual void Format(CHctlCommandFrame& aCommandFrame) const;
  
private:
	TUint8 iLoopbackMode;
	
	};

#endif // WRITELOOPBACKMODECOMMAND_H
