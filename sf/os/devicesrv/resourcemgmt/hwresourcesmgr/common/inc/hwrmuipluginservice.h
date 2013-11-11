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
 
#ifndef HWRMUIPLUGINSERVICE_H
#define HWRMUIPLUGINSERVICE_H

// INCLUDES
#include <e32base.h>


// CONSTANTS
/** ECOM Plugin interface UID */
const TUid KHwrmUiPluginServiceInterfaceUid = { 0x102857B5 };                                                       

/**
* A callback interface for foreground observer.
*
* @publishedPartner
* @released
*/
class MHwrmForegroundObserver
    {
public:       
    /** 
    * Called when the client window gets the focus.
    */
   	virtual void HandleGainingForeground() = 0;
	
    /** 
    * Called when the client window looses the focus.
    */
	  virtual void HandleLosingForeground() = 0;	
    };
    

/**
*  UI Plugin interface class to be implemented by the foreground observer.
*
* @publishedPartner
* @released
*/
class CHwrmUiPluginService : public CBase
	{
public:
    /**
    * Factory method to create implementation. Implementation is found via ECom.
    */
    static CHwrmUiPluginService* NewL();

    /**
    * Factory method to create implementation. Implementation is found via ECom.
    */
    inline virtual ~CHwrmUiPluginService();

    /**
    * Update UI foreground observer
    *
    */
    void  virtual AddForegroundObserverL(MHwrmForegroundObserver* aObserver) = 0;
	
    /**
    * Returns the foreground state of the window
    *
    * @return ETrue if the window is on focus otherwise EFalse
    *
    */
    TBool virtual InForeground() const = 0;
    
protected:	
    TUid iDestructorIDKey;
	};

#include <hwrm/hwrmuipluginservice.inl>

#endif  // HWRMUIPLUGINSERVICE_H
