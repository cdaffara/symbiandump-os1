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

/**
 @file
 @publishedPartner 
 @released
*/


#ifndef MSYNCCISUPPORT_H
#define MSYNCCISUPPORT_H

#include <e32base.h>
#include <a3f/a3fbase.h>

const TUid KUidExtensionInferface = { 0x1028348B };

/**
 * An interface to get notifications from Custom Interface state change.
 */
class MCustomInterfaceSupportObserver
	{
public:
	/* *
	 * Called to state that CI is being removed use depends on which type of CI is involved.
	 * @param aUid the Uid of the CI.
	 *
	 * @param aPtr on returns contains a pointer to the CI.
	 */
	virtual void CustomInterfaceRemoval(TUid aInterfaceUid, TAny* aPtr) = 0;
	};


/**
 * An interface to get Custom Interface.
 */
class MCustomInterfaceSupport
	{
public:
	/**
	 * Request custom interface associated with aUid. 
	 * Whether multiple calls returns the same pointer and/or
	 * ownership is transferred depends on the specific uid. 
	 *
	 * @param aUid the Uid of the CI .
	 * @param aPtr on returns contains a pointer to the CI.
	 */
	virtual TInt RequestCustomInterface(TUid aUid, TAny*& aPtr) = 0;

	/**
	 * Registers an interface support observer.
	 *
	 * The observer will be notified when the custom interface has been removed .
	 *
	 * @param aObserver the observer reference to register.
	 * @return An error code. KErrNone on success, otherwise one of the system wide error codes.
	 */
	virtual TInt RegisterObserver(MCustomInterfaceSupportObserver& aObserver) = 0;

	/**
	 * Unregisters an interface support observer.
	 * 
	 * @param aObserver the observer reference to unregister.
	 * @return An error code. KErrNone on success, otherwise one of the system wide error codes.
 	 */
	virtual void UnRegisterObserver(MCustomInterfaceSupportObserver& aObserver) = 0; 
	};

#endif  // MSYNCCISUPPORT_H
