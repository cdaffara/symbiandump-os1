/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name        : EPos_MPosModulesObserver.h
* Part of     : Location Framework
* Interface   : Location Settings API
* MPosModulesObserver class declaration
* Version     : %version %
*
*/




/**
 @file
 @publishedPartner
 @released
*/
#ifndef MPOSMODULESOBSERVER_H
#define MPOSMODULESOBSERVER_H

#include <e32base.h>
#include <lbscommon.h>

/** Type for bitmap values of event types, defined by  _TPosModulesEventType */
typedef TInt TPosModulesEventType;

/** Event types for socation settings changes.
 *  Used by TPosModulesEventType */
enum _TPosModulesEventType
	{
	/** Several changes happenned, no module ID is set. */
	EPosModulesEventUnspecified  = 0x00,
	/** New module installed, module ID is set. */
	EPosModulesEventModuleInstalled = 0x01,
	/** A module is removed from system, module ID is set. */
	EPosModulesEventModuleRemoved = 0x02,
	/** Priority of modules is changed. No module ID is set as this changes
	    applies to several modules */
	EPosModulesEventPrioritiesChanged = 0x04,
	/** Cost attribute is changed for a module, ID is set */
	EPosModulesEventCostChanged = 0x08,
	/** Availability attribute is changed for a module, ID is set */
	EPosModulesEventAvailabilityChanged = 0x10,
	/**Visibility attribute is changed for a module, ID is set */
	EPosModulesEventVisibilityChanged = 0x20
	};

/**
 * Struct for location settings change event details. These include the type of
 * change event and the ID of the module affected by the change (if relevant).
 */
struct TPosModulesEvent
	{
	TPosModulesEventType iType; /**< Type of the event. */
	TPositionModuleId iModuleId;  /**< ID of affected module, if applicable. */
	};

/**
*  Abstract class for location settings observers.
*
*  This class is implemented by clients who wants to listen for changes in the
*  location settings. CPosModules::SetObserver() is used to register
*  location settings observers.
*
*/
class MPosModulesObserver
    {
    public:

        /**
        * Called when a change has been detected in location settings.
        *
        * To be able to receive this notification, the observer object must
        * first be registered by calling CPosModules::SetObserverL() .
        *
        * @param aEvent Information about the location settings event.
        * @see TPosModulesEvent
        */
        virtual void HandleSettingsChangeL( TPosModulesEvent aEvent ) = 0;

    };

#endif      // MPOSMODULESOBSERVER_H

