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


#ifndef MAUDIOPROCESSINGUNIT_H
#define MAUDIOPROCESSINGUNIT_H

#include <e32base.h>
#include <f32file.h>
#include <mmf/common/mmfcontroller.h>
#include <a3f/a3fbase.h>

class MAudioProcessingUnitObserver;

/**
 * A generic interface for audio processing units.
 */
class MAudioProcessingUnit
	{
public:

	/**
	 * Fetches the identifier of the context in which this processing unit exists.
	 *
	 * @return TUid context instance id.
	 */
	virtual TAudioContextId ContextId() const = 0;


	/**
	 * Fetches the specified interface to this processing unit.
	 * The interface must not be deleted by the client.
	 * The processing unit must be removed once from the context using
	 * any of the retrieved processing unit interfaces.
	 * @param aTypeId the type of the interface
	 */
	virtual TAny* Interface(TUid aType)=0;

	/**
	 * Registers a processing unit observer.
	 *
	 * The observer must be unregistered before the processing unit is removed from the context.
	 * 
	 * @see MAudioProcessingUnitObserver for further inforamtion about the observer usage.
	 * @param aObserver a reference to the observer to register
	 * @return an error code. KErrNone if successful.
	 *         KErrNotSupported the processing unit does not provide support for this type of observation.
	 *         Other system wide error codes are also possible.
	 */
	virtual TInt RegisterProcessingUnitObserver(MAudioProcessingUnitObserver& aObserver)=0;

	/**
	 * Unregisters audio processing unit observer.
	 *
	 * @see RegisterProcessingUnitObserver for further information.
	 * @param aObserver a reference to the processing unit observer to unregister.
	 * @return an error code. KErrNone if successful.
	 */
	virtual void UnregisterProcessingUnitObserver(MAudioProcessingUnitObserver& aObserver)=0; 

	/**
	 * Returns the component instance id.
	 *
	 * @return TUid component instance id.
	 */
	virtual TAudioComponentId InstanceId() const = 0;

	/** 
	 * Returns if the component is the same type of the given feature Uid. 
 	 * @param aTypeId a feature Uid to compare with.
	 */
	virtual TBool IsTypeOf(TUid aTypeId) const =  0;

	};

#endif // MAUDIOPROCESSINGUNIT_H
