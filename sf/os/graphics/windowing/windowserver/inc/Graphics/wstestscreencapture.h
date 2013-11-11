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
// Interface for Render Stage Screen Capture Render Stage
// 
//

#ifndef WSTESTSCREENCAPTURE_H_
#define WSTESTSCREENCAPTURE_H_

/**
@file
@publishedpartner 
@released
*/

#include <graphics/wsgraphicdrawerinterface.h>
#include <graphics/wsscreendeviceutils.h>

class CScreen;
class TWsSdComposeScreenCommand;
class MWsTestScreenCapture : public MWsObjectProvider
	{
public:
	enum	
		{
		KMWsScreenCapture =  0x10286503,	// registered GUID
		ETypeId=				KMWsScreenCapture,
		EWsObjectInterfaceId=	KMWsScreenCapture
		};
	virtual TInt TestScreenCaptureL(const TAny* aMessage, MWsScreenDeviceUtils& aUtils) =0;
	};

#endif /*WSTESTSCREENCAPTURE_H_*/


