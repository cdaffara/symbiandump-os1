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
// Interface for Render Stage Display Resolution
// 
//

#ifndef WSDISPLAYCONTROL_H_
#define WSDISPLAYCONTROL_H_

/**
@file
@publishedpartner 
@released
*/

#include <graphics/displaycontrolbase.h>
#include <graphics/wsgraphicdrawerinterface.h>

class MWsDisplayControl: public MWsObjectProvider,public MDisplayControlBase
	{
public:
	enum	
		{
		KUidWsDisplayControl =  0x10286497, 	  //To    0x1028649A
		ETypeId=				KUidWsDisplayControl,
		EWsObjectInterfaceId=	KUidWsDisplayControl
		};
public:
	/**
	 * Asynchronous request to complete aStatus when a change of display is detected.
	 * 
	 * The request is completed when the connectedness of the display changes, or when
	 * the set of available resolutions changes.
	 * 
	 * @param aStatus Asynchronous request status.
	 */
	virtual void NotifyOnDisplayChange(TRequestStatus& aStatus)=0;
	/**
	 * Cancels a pending request to notify on display change.
	 * 
	 * If there is a pending notification request, the status value will be set to 
	 * KErrCancelled and it will be completed. If there is no pending request for 
	 * notification, the call will simply return.
	 */
	virtual void NotifyOnDisplayChangeCancel()=0;
	/**
	 * Asynchronous request to complete aStatus when a configuration change occurs.
	 * 
	 * The request is completed when a new configuration has been set, either 
	 * explicitly or by a render stage. Earlier render stages and the Window Server 
	 * can use this to update their state.
	 * 
	 * @param aStatus Asynchronous request status.
	 */
	virtual void NotifyOnConfigChange(TRequestStatus& aStatus)=0;
	/**
	 * Cancels a pending request to notify on configuration change.
	 * 
	 * If there is a pending notification request, the status value will be set to 
	 * KErrCancelled and it will be completed. If there is no pending request for 
	 * notification, the call will simply return.
	 */
	virtual void NotifyOnConfigChangeCancel()=0;
	};

#endif /*WSDISPLAYCONTROL_H_*/
