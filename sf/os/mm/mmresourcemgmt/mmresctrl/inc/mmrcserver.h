// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MMRCSERVER_H__
#define MMRCSERVER_H__

#include <e32std.h>
#include <e32base.h>
#include <a3f/a3fbase.h>
#include <e32msgqueue.h>
#include "mmrcclientserver.h"

/**
* @file
* @internalTechnology
*/

class TServerStart;
class CMMRCServerSession;
class CMMRCServerController;
class MMultimediaResourceControlObserver;
class CFourCCConvertor;

// reasons for server panic
enum TCountServPanic
	{
	EBadRequest = 1,
	EBadDescriptor
	};

/**
@internalComponent

This class implements the MMRC Server. 
The main purpose of this class is to implement the MMRC Server
*/	
NONSHARABLE_CLASS( CMMRCServer ): public CServer2
	{
public:
	/**
	 * Constructs, and returns a pointer to, a new CMMRCServer object.
	 * Leaves on failure.
	 * @return CMMRCServer* A pointer to newly created utlitly object.
	 */		
	static CMMRCServer* NewL(); 
	
	/**
	 * Constructs, leaves object on the cleanup stack, and returns a pointer
	 * to, a new CMMRCServer object.
	 * Leaves on failure.
	 * @return CMMRCServer* A pointer to newly created utlitly object.
	 */		
	static CMMRCServer* NewLC();
	
 	/**
	 * Destructor.
	 */
	~CMMRCServer();
	
public:
	// The thread function executed by the server
	static TInt ThreadFunction(TAny* aStarted);

	// Creates a new session with the server; the function
	// implements the pure virtutal function 
	// defined in class CServer2
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
private :
	// The thread function executed by the server
	static TInt ThreadFunctionL(TAny* aStarted);
	
	// Function to panic the server
	static void PanicServer(TInt aPanic);

	/**
	 * By default Symbian 2nd phase constructor is private.
	 */
	CMMRCServer();
	
	/**
	 * By default Symbian 2nd phase constructor is private.
	 */
	void ConstructL();

private:
	// owned
	CMMRCServerController* iMMRCServerController;	//MMRC Server controller
	
	//Not ownwed.
	CFourCCConvertor* iFourCCConvertor;
	
	};

#endif //__MMRCSERVER_H__
