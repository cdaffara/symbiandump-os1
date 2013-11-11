// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ECAMPLUGIN_H
#define ECAMPLUGIN_H

#include <ecam.h>

_LIT8(KECamPluginMatchString, "*"); // ECom insists on something

class CCameraStructure; // declared here.

/**
Parent class for CCamera plugin.
Abstract class for CCamera plugin, including ECom support.
  
@see CCamera 
*/
class CCameraPlugin : public CCamera
	{
	friend class CCamera; // so can use internal factory functions etc
protected:

	/**
	@internalComponent
	
	@see CCamera::NewL(MCameraObserver& aObserver,TInt aCameraIndex)	
	*/
	IMPORT_C static CCameraPlugin* NewL(MCameraObserver& aObserver,TInt aCameraIndex, TInt aCameraVersion);

	/** 
	@internalComponent
	
	@see CCamera::NewDuplicateL(MCameraObserver& aObserver,TInt aCameraHandle)
	*/
	IMPORT_C static CCameraPlugin* NewDuplicateL(MCameraObserver& aObserver,TInt aCameraHandle, TInt aCameraVersion);	

    /** 
	@internalComponent
	
	@see CCamera::NewL(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority)
	*/
	IMPORT_C static CCameraPlugin* NewL(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority, TInt aCameraVersion);

	/** 
	@internalComponent

	@see CCamera::NewDuplicateL(MCameraObserver2& aObserver,TInt aCameraHandle)
	*/
	IMPORT_C static CCameraPlugin* NewDuplicateL(MCameraObserver2& aObserver,TInt aCameraHandle, TInt aCameraVersion);
	
	/** 
	@internalComponent

	@see CCamera::CamerasAvailable()
	*/
	IMPORT_C static TInt CamerasAvailable();
	
	/**
	Returns the camera API version no.
	
	@return Version of the camera API
	*/
	IMPORT_C TInt CameraVersion();
	
	/**
    Destructor
    */ 
	IMPORT_C ~CCameraPlugin();

protected:

	/** 
	A call to one of the factory functions firstly loads the plugin, followed by a
	call to this function to complete construction.
	
	Creates an object representing a camera.
	
	@param  aObserver
	        Reference to class derived from MCameraObserver2 designed to receive
	        notification of asynchronous event completion.
	@param	aCameraIndex
	        Index from 0 to CamerasAvailable()-1 inclusive specifying the
	        camera device to use.
	@return Pointer to a fully constructed CCamera object. Ownership is passed
	        to the caller.

	@leave  May leave with KErrNoMemory or KErrNotSupported if aCameraIndex is
	        out of range.	
	*/
	virtual void Construct2L(MCameraObserver& aObserver,TInt aCameraIndex)=0;

	/** 
	A call to one of the factory functions firstly loads the plugin, followed by a
	call to this function to complete construction.
	
	Duplicates the original camera object for use by, for example, multimedia systems.

	May leave with KErrNoMemory or KErrNotFound if aCameraHandle is not valid.

	@param  aObserver
	        Reference to an observer.
	@param  aCameraHandle Handle of an existing camera object.

	@return Duplicate of the original camera object. 
	*/
	virtual void Construct2DupL(MCameraObserver& aObserver,TInt aCameraHandle)=0;
	
    /** 
    A call to one of the factory functions firstly loads the plugin, followed by a
	call to this function to complete construction.
    
	Creates an object representing a camera.
	
	@param  aObserver
	        Reference to class derived from MCameraObserver2 designed to receive
	        notification of asynchronous event completion.
	@param	aCameraIndex
	        Index from 0 to CamerasAvailable()-1 inclusive specifying the
	        camera device to use.
	@param	aPriority
	        Value from -100 to 100 indicating relative priority of client to
	        use camera.

	@return Pointer to a fully constructed CCamera object. Ownership is passed
	        to the caller.

	@leave  May leave with KErrNoMemory or KErrNotSupported if aCameraIndex is
	        out of range.
	*/
	virtual void Construct2L(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority)=0;

	/** 
	A call to one of the factory functions firstly loads the plugin, followed by a
	call to this function to complete construction.
	
	Duplicates the original camera object for use by, for example, multimedia systems.

	May leave with KErrNoMemory or KErrNotFound if aCameraHandle is not valid.

	@param  aObserver
	        Reference to an observer.
	@param  aCameraHandle Handle of an existing camera object.

	@leave  May leave with KErrNoMemory or KErrNotFound if aCameraHandle is not valid.	   

	@return Duplicate of the original camera object. 
	*/
	virtual void Construct2DupL(MCameraObserver2& aObserver,TInt aCameraHandle)=0;

protected:

	/**
	Default constructor for this class.
	*/
	IMPORT_C CCameraPlugin();

private:
	
	/**
	Returns the loaded plugin.
	
	@return Pointer to the plugin.
	*/
	static CCameraPlugin* NewL(TInt aCameraVersion);
	
	/**
	Returns the loaded plugin.
	Plugin is left on the cleanup stack.
	
	@param aCameraVersion
		   The version of camera API used by applications	
	
	@return Pointer to the plugin.
	*/
	static CCameraPlugin* NewLC(TInt aCameraVersion);
	
private:
	CCameraStructure* iCameraStructure;
	};

NONSHARABLE_CLASS( CCameraStructure ): public CBase
	{
public:
	TUid iDestructorKey;
	TInt iCameraVersion;
	};

#endif // ECAMPLUGIN_H
