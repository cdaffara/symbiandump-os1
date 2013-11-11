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
//

/**
 @file
 @publishedPartner
 @released
*/


#ifndef CHCICOMMAND_H
#define CHCICOMMAND_H

#include <e32base.h>
#include <bluetooth/hci/hciopcodes.h>
#include <bluetooth/hci/commandbaseextension.h>

class CHctlCommandFrame;
class THCIEventBase;
class CHCICommandBaseExtension;

/**
This class defines the basis for specific HCI Commands that are to be
sent over the HCI.

Any class that derives from this class must call the BaseConstructL() function
during its NewL(), to ensure that the base class is fully initialised.
*/
NONSHARABLE_CLASS(CHCICommandBase) : public CBase
	{
public:
	IMPORT_C THCIOpcode Opcode() const;
	~CHCICommandBase();

	IMPORT_C TUint CreditsConsumed() const;
	
	IMPORT_C TBool ExpectsCommandStatusEvent() const;
	IMPORT_C TBool ExpectsCommandCompleteEvent() const;
	virtual void Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const;

	IMPORT_C void FormatCommand(CHctlCommandFrame& aCommandFrame);
	
	virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);

protected:
	CHCICommandBase(THCIOpcode aOpcode);
	void BaseConstructL();
	
	void SetCreditsConsumed(TUint aConsumedCredits);
	void SetExpectsCommandStatusEvent(TBool aExpectsCmdStatus);
	void SetExpectsCommandCompleteEvent(TBool aExpectsCmdComplete);

private:
	virtual void Format(CHctlCommandFrame& aCommandFrame) const = 0;

protected:
	THCIOpcode iOpcode;
	// This pointer declaration was previously reserved. 
	CHCICommandBaseExtension* iCommandBaseExtension;

private:
	TUint iCreditsConsumed;
	TBool iExpectsCommandStatusEvent;
	TBool iExpectsCommandCompleteEvent;
	};

#endif // CHCICOMMAND_H 
