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

#ifndef __SSMCUSTOMCOMMAND_H__
#define __SSMCUSTOMCOMMAND_H__

#include <e32base.h>
class RFs;

/**
Provides an environment to pass to the custom command dll during initialisation.

@publishedPartner
@released
*/

NONSHARABLE_CLASS (CSsmCustomCommandEnv) : public CBase
	{
public:
	/**
	 * Creates a custom command environment.
	 * 
	 * This should never be called by implementations of MSsmCustomCommand as
	 * the lifetime of the environment is managed by the command framework which 
	 * uses the MSsmCustomCommand implementation. 
	 * 
	 * @internalTechnology
	 */
	static CSsmCustomCommandEnv* NewL(RFs& aRfs);
	
	
	/**
	 * Reference to an RFs handle for use by the custom command
	 * 
	 * @publishedPartner
	 * @released


	 */
	IMPORT_C virtual const RFs& Rfs() const;
	
	/**
	 * Destructor for custom command environment.
	 * 
	 * This should never be called by implementations of MSsmCustomCommand as
	 * the lifetime of the environment is managed by the command framework which 
	 * uses the MSsmCustomCommand implementation. 
	 * 
	 * @internalTechnology
	 */
	~CSsmCustomCommandEnv();

private:
	// don't allow objects to be copied or assigned
	CSsmCustomCommandEnv(RFs& aRfs);
	CSsmCustomCommandEnv(const CSsmCustomCommandEnv& aEnv);
	CSsmCustomCommandEnv& operator= (const CSsmCustomCommandEnv& aEnv);

private:
	RFs& iFs;
	};


/** 
Abstract interface to a custom command.
All custom command objects should derive from this interface.

@publishedPartner
@released
*/
class MSsmCustomCommand
	{
public:
	/**
	 * Initializes this custom command.
	 * 
	 * @param aCmdEnv A environment for use by the custom command. This pointer will be valid until
	 *                Release is called on this custom command.
	 * 
	 * @return KErrNone on success, otherwise one of the system wide error codes.
	 * 
	 * @publishedPartner
	 * @released
	 */
	virtual TInt Initialize(CSsmCustomCommandEnv* aCmdEnv) = 0;
	
	/**
	 * Executes this custom command.
	 * 
	 * @param aParams The extra information data for this instance of the custom command. If this
	 *                custom command was created from a resource file struct, this will be the 
	 *                resource pointed to by dll_info.
	 * @param aRequest Request object completed with KErrNone when the command has successfully
	 * 					finished execution, otherwise one of the system wide error codes.
	 * 
	 * @publishedPartner
	 * @released
	 */
	virtual void Execute(const TDesC8& aParams, TRequestStatus& aRequest) = 0;
	
	/**
	 * Closes resources initialized in a previous call to Initialize.
	 * 
	 * @publishedPartner
	 * @released
	 */
	virtual void Close() = 0;

	/**
	 * Releases memory associated with the Custom Command (deletes the MSsmCustomCommand object)
	 * 
	 * @publishedPartner
	 * @released
	 */
	virtual void Release() = 0;

	/**
	 * Cancels current execution of this custom command.
	 * 
	 * @publishedPartner
	 * @released
	 */
	virtual void ExecuteCancel() = 0;
	};

#endif // __SSMCUSTOMCOMMAND_H__
