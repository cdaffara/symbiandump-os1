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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef MDFPROCESSINGUNIT_H
#define MDFPROCESSINGUNIT_H

#include <e32base.h>
#include <ecom/implementationinformation.h>
#include <mdf/mdfinputport.h>
#include <mdf/mdfoutputport.h>

class MMdfInputPort;
class MMdfOutputPort;
class CMdfProcessingUnit;
class MMdfInputPortObserver;
class MMdfOutputPortObserver;
class TPuConfig;

/** 
 Processing Unit internal state. 
 */	
enum TProcessingUnitState
	{
	/** 
	Processing Unit invalid state 
	 */
	EProcessingUnitInvalid = 0,
	/** 
	Processing Unit loaded state 
	*/
	EProcessingUnitLoaded,
	/** 
	Processing Unit idle state 
	*/
	EProcessingUnitIdle,
	/** 
	Processing Unit executing state 
	*/
	EProcessingUnitExecuting,
	/** 
	Processing Unit paused state 
	*/
	EProcessingUnitPaused,
	/** 
	Processing Unit waiting for resources state 
	*/
	EProcessingUnitWaitingForResources,
	/** 
	Processing Unit loading state 
	*/
	EProcessingUnitLoading,		
	/** 
	Processing Unit initializing state 
	*/
	EProcessingUnitInitializing	
	};

/**
Processing Unit observer class
*/
class MMdfProcessingUnitObserver
	{
public:
	/**
	Called by a Processing Unit when Initialize() has completed.
	@param  aPu
	        The Processing Unit which sent the callback.
	@param aErrorCode
		An error code indicating if the function call was successful. KErrNone on success, otherwise
		another of the system-wide error codes.
	*/
	virtual void InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode) = 0;

	/**
	Called by a Processing Unit  when Execute() has completed.
	@param  aPu
		The Processing Unit which sent the callback.
	@param aErrorCode
		An error code indicating if the function call was successful. KErrNone on success, otherwise
		another of the system-wide error codes.
	*/
	virtual void ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode) = 0;
	};

/**
Processing Unit interface
*/
class CMdfProcessingUnit : public CBase
	{
public:
	/**
	Standard safe constructor that leaves nothing on the cleanup stack
	@param  aImplementationUid
		  The uid of the new created processing unit.
	@return A pointer to the newly constructed object.
	*/
	inline static CMdfProcessingUnit* NewL(TUid aImplementationUid);
		
	/**
	Synchronous method which creates the Processing Unit.
	@param  aProcessingUnitObserver
		  The class to receive asynchronous Processing Unit events.
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt Create(const MMdfProcessingUnitObserver& aProcessingUnitObserver) = 0;

	/**
	Synchronous method which returns the Input Ports that a Processing Unit holds.
	@param  aComponentInputPorts
		  The array to which the Input Ports will be appended.
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt GetInputPorts(RPointerArray<MMdfInputPort>& aComponentInputPorts) = 0;

	/**
	Synchronous method which returns the Output Ports that a Processing Unit holds.
	@param  aComponentOutputPorts
		  The array to which the Output Ports will be appended.
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt GetOutputPorts(RPointerArray<MMdfOutputPort>& aComponentOutputPorts) = 0;

	/**
	Synchronous method which sets the configuration for a Processing Unit.
	@param  aConfigurationSetup
		  The reference to the structure that contains the configuration data.
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.	
	*/
	virtual TInt Configure(const TPuConfig& aConfigurationSetup) = 0;
	
	/**
	Synchronous method which gets a configuration structure.
	@param  aConfigurationSetup
		  The reference to the structure that is to contain the configuration information.
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.	
	*/
	virtual TInt GetConfig(TPuConfig& aConfigurationSetup) = 0;
	
	
	/**
	Asynchronous method which instructs the Processing Unit to start the initialization.
	@see MMdfProcessingUnitObserver::InitializeComplete()
	*/
	virtual void Initialize() = 0;

	/**
	Asynchronous method which starts the execution for a Processing Unit.
	@see MMdfProcessingUnitObserver::ExecuteComplete()
	*/
	virtual void Execute () = 0; 

	/**
	Synchronous method which pauses the current on-going task.
	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt Pause () = 0;

	/**
	Synchronous method which stops the current on-going task. 
	*/
	virtual void Stop () = 0;

	/**
	Synchronous method which returns the current state of the Processing Unit.
	@return	The current state of the Processing Unit.
	*/
	virtual TProcessingUnitState State() = 0;

	/**
	Synchronous method which requests an extension feature.
	This is intended to provide additional features. 
	@param aUid
	       Used to indicate which interface is required. 
	@return Standard error code. KErrNotSupported is used to indicate that the particular
	        plugin is used.
	*/
	virtual TInt CreateCustomInterface(TUid aUid) = 0;

	/**
	Synchronous method which returns a previously created extension.
	This returns a custom interface. This should only be used if CreateCustomInterface() has already
	been called for the same UID value. This means that any construction for that interface
	has already been called, and thus this call cannot fail.
	@param aUid
	       Used to indicate which interface is required. 
	@return The requested interface, or NULL if not known.
	@see CMdfProcessingUnit::CreateCustomInterface()
	*/
	virtual TAny* CustomInterface(TUid aUid) = 0;

	/**
	Destructor.

	The destructor is called by ECom framework allowing derived classes
	to clean up the implementation specific resources.
	*/
	inline virtual ~CMdfProcessingUnit();

private:
	TUid iDtor_ID_Key;
	};

#include <mdf/mdfprocessingunit.inl>

#endif // MDFPROCESSINGUNIT_H
