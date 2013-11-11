// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Mda\Client\Video.h
// 
//

#ifndef __MDA_CLIENT_VIDEO_H__
#define __MDA_CLIENT_VIDEO_H__

#ifndef __MDA_COMMON_RESOURCE_H__
#include <mda/common/resource.h>
#endif
#ifndef __MDA_COMMON_VIDEO_H__
#include <mda/common/video.h>
#endif

// Media Server Bitmap Device

/**
@internalTechnology
*/
class TMdaFbsBitmapDevice : public TMdaStreamDevice
	{
public:
	inline TMdaFbsBitmapDevice();
	};

/**
@internalTechnology
*/
class TMdaFbsBitmapHandle : public TMdaPackage
	{
public:
	inline TMdaFbsBitmapHandle();
public:
	TInt iBitmapHandle;
	};

/**
@internalTechnology
*/
class TMdaFbsBitmapMaskHandle : public TMdaPackage
	{
public:
	inline TMdaFbsBitmapMaskHandle();
public:
	TInt iBitmapHandle;
	};

#include <mda/client/video.inl>

#endif

