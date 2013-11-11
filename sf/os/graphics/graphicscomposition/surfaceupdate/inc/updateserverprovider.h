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

#ifndef __UPDATESERVERPROVIDER_H__
#define __UPDATESERVERPROVIDER_H__

#include <e32cmn.h>

class TSurfaceId;



/**
@publishedPartner


The class is aimed for use within WSERV process only by the Content update receiver. 
It provides interface for registering Content update receiver instances for each screen. 
*/
class MSurfaceUpdateServerProvider
	{
public:	
	/**
	Assign Content update receiver instance to particular screen. 
	The following calls of this function for the same screen will override the previous.
	
	@param aScreen - screen number, associated with the output, 
		varies from 0 to number of screens present - 1
	@param aUpdateReceiver - pointer to a Content update receiver interface associated 
		with the aScreen. NULL signifies that particular screen doesn't 
		correspond any composition receiver. The ownership of this parameter 
		will not be transferred.
	@param aPriority - priority of the Content update receiver. 
        The higher the number is the higher the priority of the screen. 
		This number will be used to identify the master screen for any particular surface. 
		It is not acceptable to have more than one screen with the same priority.
	@return KErrNone if an operation is successful. 
	@return KErrArgument Arguments don’t lie in expected range.
	@return KErrNotReady The server has not been started.
	@return KErrAlreadyExists Another Content update receiver with the same priority number has already been registered with the SUS. Any other system error codes otherwise.
	@panic EUpdateServPanicRegister If another registration method has already been used.
	*/
	virtual TInt Register(TInt aScreen, CBase* aUpdateReceiver, TInt aPriority) = 0;
private:
	friend class CTSurfaceUpdateStep;	
	/**
	Close the Surface Update Server
	This function is intended for test/debugging only
	The implementation will do nothing in released builds 	
	
	WARNING: Function for internal use ONLY. Compatibility is not guaranteed in future releases.
	*/
	virtual void Terminate() = 0;
	};

/**
Spawn a thread within WSERV process. This will lead to starting the surface update server in it 
@publishedPartner

	
@param aUpdateProvider - [out] reference pointer to surface update server provider, 
		which will be set when the server is started. The variable will be used to registry 
		content update receiver instances. The caller doesn't acquire the ownership of this instance,
		thus mustn't delete it. The pointer will be valid until server is operating, 
		i.e. system is up.
		
@return KErrNone if an operation is successful, any other system error codes otherwise
*/
IMPORT_C TInt StartSurfaceUpdateServer(MSurfaceUpdateServerProvider*& aSurfaceUpdateServerProvider);



#endif
