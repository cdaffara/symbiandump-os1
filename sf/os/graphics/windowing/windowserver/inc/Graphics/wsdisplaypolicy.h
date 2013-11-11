// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Interface for Render Stage Display Policy
// 
//

#ifndef WSDISPLAYPOLICY_H_
#define WSDISPLAYPOLICY_H_

/**
@file
@publishedpartner 
@released
*/

#include <graphics/displayconfiguration.h>
#include <graphics/wsgraphicdrawerinterface.h>

class MWsDisplayPolicy: public MWsObjectProvider
	{
public:
	enum	
		{
		KUidWsDisplayPolicy	 =  0x102869FA, 	 
		ETypeId=				KUidWsDisplayPolicy,
		EWsObjectInterfaceId=	KUidWsDisplayPolicy
		};
	
	enum TDisplayStatus
		{
		EDetach = 0,
		EAttach,
		EConnecting,
		};
public:
	/**
	 * Notification from the Window Server that the application size mode list has been updated.
	 */
	virtual void NewAppModesAvailable()=0;
	/**
	 * Determine the display configuration and application rendering extent to use for a given screen size mode.
	 * 
	 * The Window Server uses this function when a request to change to a new screen size mode or new display 
	 * configuration is made by a client. The display configuration passed in may be partially filled, and the 
	 * combination of the defined entries in it and the requested screen size mode values shall be used to define 
	 * the rest of the configuration and new application extent to be used.
	 * 
	 * Only undefined entries in the configuration may be set by this function. Entries already defined must retain 
	 * the same values. If the combination of defined entries and size mode values is not acceptable to the implementer, 
	 * the function shall return KErrArgument.
	 * 
	 * If the function returns KErrNone, the Window Server shall set the returned display configuration, notify render 
	 * stages of the new application extent and change to use the screen size mode. If either this function returns 
	 * EFalse or the call to set the display configuration fails for any reason (such as KErrNoMemory, for example), 
	 * then no further changes are made.
	 * 
	 * @see MWsScreenConfigList
	 * @see MWsDisplayMapping
	 * 
	 * @param aScreenSizeMode The requested screen size mode. If static, the size in pixels, size in twips, and 
	 * the rotation must be respected. If dynamic, only the rotation must be respected.
	 * @param aConfig The requested display configuration on input, the required configuration on return. Only 
	 * undefined entries may be set.
	 * @param aExtent The screen size mode offset and size on input, the required application offset and size on return. 
	 * If the screen driver buffer is used as the UI surface, this must be left unchanged.
	 * @return KErrNone if no error, otherwise a system wide error code
	 */
	virtual TInt GetSizeModeConfiguration(TInt aScreenSizeMode, TDisplayConfiguration& aConfig, TRect& aSizeModePosition) const =0;
	virtual TInt SuitableAppMode(TDisplayStatus aSituation)=0;
	/**
	 * Sets the application mode resumed to after a display is connected again after a disconnect.
	 * 
	 * @param aMode The application mode.
	 */
	virtual void SetLastAppMode(TInt aMode)=0;
	};

#endif /*WSDISPLAYPOLICY_H_*/
