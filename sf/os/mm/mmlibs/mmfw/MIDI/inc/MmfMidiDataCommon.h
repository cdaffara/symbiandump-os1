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

#ifndef __MMFMIDIDATACOMMON_H__
#define __MMFMIDIDATACOMMON_H__


/** 
Class used to hold MIP message values.

@publishedAll
@released
*/
class TMipMessageEntry
	{
public:
	/**
	 * Channel whose MIP value is to be set.
	 */
	TInt iChannel;

	/**
	 * MIP value to use.
	 */
	TInt iMIPValue;
	};

/**
Defines the current playback rate of the currently open MIDI resource.

@publishedAll
@released
*/
enum TPlaybackRate
	{
	/** 
	 * The original playback rate for the currently open MIDI resource prior to any modifications.
	 */
	EOriginalPlaybackRate = 100000
	};

#endif
