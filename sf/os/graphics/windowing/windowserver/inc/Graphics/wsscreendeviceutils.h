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
// wsscreencapture.h
// Interface for Render Stage Screen Capture Utils to be used by the render stage
// 
//

#ifndef WSTESTSCREENDEVICEUTILS_H_
#define WSTESTSCREENDEVICEUTILS_H_

/**
@file
@publishedpartner 
@released
*/

#include <graphics/wsgraphicdrawerinterface.h>

class MWsScreenDeviceUtils : public MWsObjectProvider
	{
public:
	enum	
		{
		KMWsScreenCaptureUtils =  0x10286505,	// registered GUID
		ETypeId=				KMWsScreenCaptureUtils,
		EWsObjectInterfaceId=	KMWsScreenCaptureUtils
		};
	virtual void PanicCurrentClient(TInt aPanic) = 0;
	virtual void SetReply(TInt aReply) = 0;
	virtual void RedrawNowIfPending() = 0;
	virtual void ReplyBuf(const TAny* aSource, TInt aLength) = 0;
	};

#endif /*WSTESTSCREENDEVICEUTILS_H_*/


