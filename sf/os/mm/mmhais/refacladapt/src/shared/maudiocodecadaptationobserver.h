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
*
*/



#ifndef MAUDIOCODECADAPTATIONOBSERVER_H
#define MAUDIOCODECADAPTATIONOBSERVER_H

#include <a3f/a3fbase.h>

class CMMFBuffer;

// TO DO Use real Uid
const TUid KUidAudioCodecAdaptationObserver = {0x102834D1};

/**
Interface to get notifications from AudioCodecObserver
MAudioCodecObserver
*/
class MAudioCodecAdaptationObserver
	{
public:
	/**
	*/
	virtual void AllBuffersProcessed() = 0;

	/**
	*/
	virtual void ProcessingUnitError(TInt aError) = 0;

	/**
	*/
	virtual void GetSupportedAModesComplete(TInt aError) = 0;
	virtual void GetSupportedARatesComplete(TInt aError) = 0;
	};

#endif // M_DATAPORT_H
