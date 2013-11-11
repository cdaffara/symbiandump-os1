// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined (__STARTUPSTATEINFO_H__)
#define __STARTUPSTATEINFO_H__

#include "StartupCommand.h"

#include <startup.hrh>
typedef  RPointerArray<MStartupCommand> CommandList;
	
/** Definition of the required information for a startup state.
@internalComponent
@deprecated
@see TStartupStateIdentifier
*/
class MStartupStateInfo
	{
public:
    virtual ~MStartupStateInfo() {}

public:
	/** State id for publishing to state-aware applications
	*/
    virtual TUint8 StateId() const = 0;
    
    /** State name - for general purposes (e.g. logging)
    */
    virtual TPtrC Name() const = 0;
     
    /** List of commands to be executed by this state
    */
	virtual const CommandList GetCommandList() const = 0;
	
	/** Get a count of the number of commands in the list
	*/
	virtual TInt Count() const = 0;
	
	/** Number of re-attempts to be made to change state after 
	transition failure.
	*/
 	virtual TNoOfRetries NoOfRetries() const = 0;
 	
 	/** Action to be taken on state transition failure.
 	*/
 	virtual TActionOnStateTransitionFailure ActionOnStateTransitionFailure() const = 0;
	
	/* Get a command from the list. The implementation should mimic the
	action of RPointerArray::operator[] if the index is out of range
	*/
	virtual MStartupCommand* GetCommand(TInt aIndex) const = 0;
	};

#endif // __STARTUPSTATEINFO_H__
