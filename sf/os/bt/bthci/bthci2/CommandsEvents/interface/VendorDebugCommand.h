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

#ifndef VENDORDEBUGCOMMAND_H
#define VENDORDEBUGCOMMAND_H

#include <bluetooth/hci/command.h>

class CHctlCommandFrame;
class CVendorDebugCommand;

/**
 This interface provides the ability to define a custom Match method to match an event with
 a vendor debug command.
 A class can derive from this interface, implement MvdcmMatch and then register itself via
 the CVendorDebugCommand::SetMatcher method.
 When the framework calls CVendorDebugCommand::Match it will call MvdcmMatch passing it the 
 control of the matching algorithm. 
*/
class MVendorDebugCommandMatcher
	{
public:
/**
	It defines a custom method to match an event to a CVendorDebugCommand
	@param aVendorDebugCommand is the command to be matched
	@param aEvent is the event to match
	@param aMatchesCmd is set to say if the event matches the command or not
	@param aConcludesCmd is set to say if the command is concluded by this match or not
	@param aContinueMatching is set to say if the framework has to continue the matching process or not 
*/
	virtual void MvdcmMatch(const CVendorDebugCommand& aVendorDebugCommand, const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const = 0;
	};

/**
This class can be used to represent any Vendor Debug command i.e. any
command with an opcode in the Vendor Debug opcode group (i.e. top six
bits of the opcode are all ones).

The specific opcode within that group is supplied as a parameter to the
factory method, and may be changed subsequently using the Reset method.

Since the format of the data associated with this command is undefined/vendor-specific,
a buffer of 255 bytes is provided for the vendor-specific data, and the accessor
method Command gives the user of the class full read-write control of that data.
*/

NONSHARABLE_CLASS(CVendorDebugCommand) : public CHCICommandBase
	{
public:	
	enum
		{
		KVendorSpecificCommandMaxSize = 255,
		};
		
public:
	IMPORT_C static CVendorDebugCommand* NewL(const TUint16 aOpcode);
	IMPORT_C void Reset(const TUint16 aOpcode);
	IMPORT_C TDes8& Command();
	IMPORT_C virtual TInt Extension_(TUint aExtensionId, TAny*& aInterface, TAny* aData);
	void Match(const THCIEventBase& aEvent, TBool& aMatchesCmd, TBool& aConcludesCmd, TBool& aContinueMatching) const;
	IMPORT_C void SetMatcher(MVendorDebugCommandMatcher* aMatcher);
	IMPORT_C void SetCreditsConsumed(TUint aConsumedCredits);
	IMPORT_C void SetExpectsCommandStatusEvent(TBool aExpectsCmdStatus);
	IMPORT_C void SetExpectsCommandCompleteEvent(TBool aExpectsCmdComplete);
	IMPORT_C TInt SetExpectsCompletingEvent(TBool aExpectsCompletingEvent);
	
protected:	
	IMPORT_C CVendorDebugCommand(const TUint16 opcode);
	IMPORT_C void ConstructL();

private:
	void Format(CHctlCommandFrame& aCommandFrame) const;

private:
	TBuf8<KVendorSpecificCommandMaxSize> iVendorSpecificData;
	MVendorDebugCommandMatcher*	iMatchImpl;
	};

#endif // VENDORDEBUGCOMMAND_H
