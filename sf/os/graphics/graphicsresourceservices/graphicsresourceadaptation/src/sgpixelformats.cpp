// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <hal.h>
#include "sgdriverimpl.h"


TInt SgMinDataStride(TInt aWidth, TUidPixelFormat aPixelFormat)
	{
	switch (aPixelFormat)
		{
	case EUidPixelFormatXRGB_8888:
	case EUidPixelFormatBGRX_8888:
	case EUidPixelFormatXBGR_8888:
	case EUidPixelFormatBGRA_8888:
	case EUidPixelFormatARGB_8888:
	case EUidPixelFormatABGR_8888:
	case EUidPixelFormatARGB_8888_PRE:
	case EUidPixelFormatABGR_8888_PRE:
	case EUidPixelFormatBGRA_8888_PRE:
	case EUidPixelFormatARGB_2101010:
	case EUidPixelFormatABGR_2101010:
		return aWidth * 4;
	case EUidPixelFormatBGR_888:
	case EUidPixelFormatRGB_888:
		return aWidth * 3;
	case EUidPixelFormatRGB_565:
	case EUidPixelFormatBGR_565:
	case EUidPixelFormatARGB_1555:
	case EUidPixelFormatXRGB_1555:
	case EUidPixelFormatARGB_4444:
	case EUidPixelFormatARGB_8332:
	case EUidPixelFormatBGRX_5551:
	case EUidPixelFormatBGRA_5551:
	case EUidPixelFormatBGRA_4444:
	case EUidPixelFormatBGRX_4444:
	case EUidPixelFormatAP_88:
	case EUidPixelFormatXRGB_4444:
	case EUidPixelFormatXBGR_4444:
		return aWidth * 2;
	case EUidPixelFormatRGB_332:
	case EUidPixelFormatA_8:
	case EUidPixelFormatBGR_332:
	case EUidPixelFormatP_8:
		return aWidth;
	case EUidPixelFormatP_4:
		return (aWidth + 1) / 2;
	case EUidPixelFormatP_2:
		return (aWidth + 3) / 4;
	case EUidPixelFormatP_1:
		return (aWidth + 7) / 8;
	default:
		return KErrNotSupported;
		}
	}


const TUint32 KSgSupportedConstantSources = ESgUsageDirectGdiSource
	| ESgUsageOpenGlesTexture2D | ESgUsageOpenVgImage | ESgUsageOpenGles2Texture2D | ESgUsageWindowGcSource;
const TUint32 KSgSupportedCpuSources = ESgUsageDirectGdiSource | ESgUsageCompositionSource
	| ESgUsageOpenGlesTexture2D | ESgUsageOpenVgImage | ESgUsageOpenGles2Texture2D | ESgUsageWindowGcSource;
const TUint32 KSgSupportedOffScreenSources = ESgUsageDirectGdiSource | ESgUsageCompositionSource | ESgUsageScreenSource
	| ESgUsageOpenGlesTexture2D | ESgUsageOpenVgImage | ESgUsageOpenGles2Texture2D | ESgUsageWindowGcSource;

const TUidPixelFormat KSgSupportedOpaqueTargetFormat = EUidPixelFormatXRGB_8888;
const TUidPixelFormat KSgSupportedAlphaTargetFormat = EUidPixelFormatARGB_8888_PRE;

#ifdef SYMBIAN_GRAPHICS_USE_NUMA
const TSgCpuAccess KSgSupportedOffScreenCpuAccess = ESgCpuAccessNone;
#else
const TSgCpuAccess KSgSupportedOffScreenCpuAccess = ESgCpuAccessReadWrite;
#endif

const TSgPixelFormatTableEntry KSgAnyScreenPixelFormatTableEntries[] =
	{
	// Support for constant images
	{EUidPixelFormatRGB_565,       KSgSupportedConstantSources, ESgCpuAccessNone, KSgScreenIdAny},
	{EUidPixelFormatXRGB_8888,     KSgSupportedConstantSources, ESgCpuAccessNone, KSgScreenIdAny},
	{EUidPixelFormatARGB_8888,     KSgSupportedConstantSources, ESgCpuAccessNone, KSgScreenIdAny},
	{EUidPixelFormatARGB_8888_PRE, KSgSupportedConstantSources, ESgCpuAccessNone, KSgScreenIdAny},
	};

const TSgPixelFormatTableEntry KSgPerScreenPixelFormatTableEntries[] =
	{
	// Support for CPU rendering
	{EUidPixelFormatRGB_565,       KSgSupportedCpuSources, ESgCpuAccessReadWrite, KSgScreenIdMain},
	{EUidPixelFormatXRGB_8888,     KSgSupportedCpuSources, ESgCpuAccessReadWrite, KSgScreenIdMain},
	{EUidPixelFormatARGB_8888,     KSgSupportedCpuSources, ESgCpuAccessReadWrite, KSgScreenIdMain},
	{EUidPixelFormatARGB_8888_PRE, KSgSupportedCpuSources, ESgCpuAccessReadWrite, KSgScreenIdMain},

	// Support for off-screen rendering
	{KSgSupportedOpaqueTargetFormat, KSgSupportedOffScreenSources|ESgUsageDirectGdiTarget|ESgUsageOpenGlesTarget|ESgUsageOpenGles2Target|ESgUsageOpenVgTarget, KSgSupportedOffScreenCpuAccess, KSgScreenIdMain},
	{KSgSupportedAlphaTargetFormat,  KSgSupportedOffScreenSources|ESgUsageDirectGdiTarget|ESgUsageOpenGlesTarget|ESgUsageOpenGles2Target|ESgUsageOpenVgTarget, KSgSupportedOffScreenCpuAccess, KSgScreenIdMain},
	{EUidPixelFormatRGB_565,         KSgSupportedOffScreenSources|ESgUsageDirectGdiTarget|ESgUsageOpenGlesTarget|ESgUsageOpenGles2Target|ESgUsageOpenVgTarget, KSgSupportedOffScreenCpuAccess, KSgScreenIdMain},

	{KSgSupportedOpaqueTargetFormat, KSgSupportedOffScreenSources|ESgUsageCompositionTarget, KSgSupportedOffScreenCpuAccess, KSgScreenIdMain},
	{KSgSupportedAlphaTargetFormat,  KSgSupportedOffScreenSources|ESgUsageCompositionTarget, KSgSupportedOffScreenCpuAccess, KSgScreenIdMain},
	{EUidPixelFormatRGB_565,         KSgSupportedOffScreenSources|ESgUsageCompositionTarget, KSgSupportedOffScreenCpuAccess, KSgScreenIdMain},

	{KSgSupportedOpaqueTargetFormat, KSgSupportedOffScreenSources|ESgUsageEglCopyBuffersTarget, KSgSupportedOffScreenCpuAccess, KSgScreenIdMain},
	{KSgSupportedAlphaTargetFormat,  KSgSupportedOffScreenSources|ESgUsageEglCopyBuffersTarget, KSgSupportedOffScreenCpuAccess, KSgScreenIdMain},
	{EUidPixelFormatRGB_565,         KSgSupportedOffScreenSources|ESgUsageEglCopyBuffersTarget, KSgSupportedOffScreenCpuAccess, KSgScreenIdMain},
	};


TInt XSgDriverImpl::ConstructPixelFormatTable()
	{
	TInt numberOfScreens;
	TInt err = HAL::Get(HAL::EDisplayNumberOfScreens, numberOfScreens);
	if (err != KErrNone)
		{
		return err;
		}
	for (TInt i = 0; i < sizeof(KSgAnyScreenPixelFormatTableEntries) / sizeof(TSgPixelFormatTableEntry); ++i)
		{
		RHeap* prevHeap = User::SwitchHeap(iHeap);
		err = iPixelFormatTable.Append(KSgAnyScreenPixelFormatTableEntries[i]);
		User::SwitchHeap(prevHeap);
		if (err != KErrNone)
			{
			return err;
			}
		}
	for (TInt screenId = 0; screenId < numberOfScreens; ++screenId)
		{
		for (TInt i = 0; i < sizeof(KSgPerScreenPixelFormatTableEntries) / sizeof(TSgPixelFormatTableEntry); ++i)
			{
			TSgPixelFormatTableEntry entry = KSgPerScreenPixelFormatTableEntries[i];
			entry.iScreenId = screenId;
			RHeap* prevHeap = User::SwitchHeap(iHeap);
			err = iPixelFormatTable.Append(entry);
			User::SwitchHeap(prevHeap);
			if (err != KErrNone)
				{
				return err;
				}
			}
		}
	return KErrNone;
	}
