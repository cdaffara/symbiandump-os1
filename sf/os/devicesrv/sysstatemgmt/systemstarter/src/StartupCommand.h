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
 
#if !defined (__STARTUPCOMMAND_H__)
#define __STARTUPCOMMAND_H__

class TRequestStatus;

/** Interface to a startup command. All command objects should inherit from this
interface.
@internalComponent
@deprecated
*/
class MStartupCommand
	{
public:
	virtual void Release() = 0;

	/** Execute the command. The system will wait on the supplied TRequestStatus
	argument, so the implementation class can be asynchronous if necessary.
	
	@param aCallerStatus KErrNone if successful, otherwise another of the 
	       system error codes.
	*/
	virtual void Execute(TRequestStatus& aCallerStatus) = 0;
	};

#endif
