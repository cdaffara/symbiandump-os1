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
//

#ifndef DISPCTRL_CLIENT_H_
#define DISPCTRL_CLIENT_H_

#include <graphics/displaycontrolbase.h>

/**
 * Window Server client interface for display control.
 * 
 * All functions in MDisplayControlBase will automatically flush the client-server
 * session buffer as part of their behavior.
 * 
 * Setting the display configuration successfully shall cause the screen mode
 * index to change to one of the last two entries (dependent on rotation) and that
 * mode's size information shall be updated as necessary.
 * 
 * @publishedPartner
 * @prototype
 */
class MDisplayControl : public MDisplayControlBase
	{
public:
	enum	
		{
		KUidDisplayControl =  0x10286499, 	  //To    0x1028649A
		ETypeId=	 KUidDisplayControl 
		};
public:
	/** 
	 * Returns whether changes in the display will trigger an EEventDisplayChanged 
	 * event to be issued.
	 *
	 * Events will be triggered configuration changes that have taken place on 
	 * the display, when a display has been connected / disconnected (if it can be 
	 * detected), and when the available resolutions change.
	 *
	 * This function always causes a flush of the window server buffer.
	 *	
	 * @return ETrue if display change events are enabled, EFalse otherwise
	 */
	virtual TBool DisplayChangeEventsEnabled() const =0;
	/** 
	 * Determines whether changes in the display will trigger an EEventDisplayChanged 
	 * event to be issued.
	 * 
	 * @see DisplayChangeEventsEnabled
	 * 
	 * @param aEnable ETrue to enable display change events, EFalse to disable them
	 */	
	virtual void EnableDisplayChangeEvents(TBool aEnable) =0;
	};


#endif /*DISPCTRL_CLIENT_H_*/
