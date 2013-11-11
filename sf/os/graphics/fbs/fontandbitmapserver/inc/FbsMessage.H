// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __FBSMESSAGE_H__
#define __FBSMESSAGE_H__

/**
@internalComponent
*/
enum TFbsMessage
	{
	EFbsMessInit,
	EFbsMessShutdown,
	EFbsMessClose,
	EFbsMessResourceCount,
	EFbsMessNumTypefaces,
	EFbsMessTypefaceSupport,
	EFbsMessFontHeightInTwips,
	EFbsMessFontHeightInPixels,
	EFbsMessAddFontStoreFile,
	EFbsMessInstallFontStoreFile,
	EFbsMessRemoveFontStoreFile,
	EFbsMessSetPixelHeight,
	EFbsMessGetFontById,
	EFbsMessFontDuplicate,
	EFbsMessBitmapCreate,
	EFbsMessBitmapResize,
	EFbsMessBitmapDuplicate,
	EFbsMessBitmapLoad,
	EFbsMessDefaultAllocFail,
	EFbsMessDefaultMark,
	EFbsMessDefaultMarkEnd,
	EFbsMessUserAllocFail,
	EFbsMessUserMark,
	EFbsMessUserMarkEnd,
	EFbsMessHeapCheck,
	EFbsMessRasterize,
	EFbsMessFaceAttrib,
	EFbsMessHasCharacter,
	EFbsMessSetDefaultGlyphBitmapType,
	EFbsMessGetDefaultGlyphBitmapType,
	EFbsMessFontNameAlias,
	EFbsMessBitmapCompress,
	EFbsMessGetHeapSizes,
	EFbsMessGetNearestFontToDesignHeightInTwips,
	EFbsMessGetNearestFontToMaxHeightInTwips,
	EFbsMessGetNearestFontToDesignHeightInPixels,
	EFbsMessGetNearestFontToMaxHeightInPixels,
 	EFbsMessShapeText,
 	EFbsMessShapeDelete,
	EFbsMessDefaultLanguageForMetrics,
	EFbsMessSetTwipsHeight,
	EFbsMessGetTwipsHeight,
	EFbsCompress,
	EFbsMessBitmapBgCompress,
	EFbsUnused1,
	EFbsSetSystemDefaultTypefaceName,
	EFbsGetAllBitmapHandles,
	EFbsMessUnused1, 	//Implementation removed
	EFbsMessSetHeapFail,	//for memory testing only
	EFbsMessHeapCount, //for memory testing only
	EFbsMessSetHeapReset, //for memory testing only
	EFbsMessSetHeapCheck, //for memory testing only
	EFbsMessHeap, //for memory testing only
	EFbsMessUnused2, //Implementation removed
	EFbsMessBitmapClean, // replace a dirty bitmap with the clean one
	EFbsMessBitmapLoadFast, // for loading bitmap from mbm or rsc file not opened by the client
	EFbsMessBitmapNotifyDirty, // notify when any bitmap becomes dirty
	EFbsMessBitmapCancelNotifyDirty, // cancel request for notification of any bitmap becoming dirty
	EFbsMessRegisterLinkedTypeface, //Register linked typeface specification with rasterizer (PREQ2146)
	EFbsMessFetchLinkedTypeface, //Retrieve linked typeface specification from rasterizer (PREQ2146)
	EFbsMessSetDuplicateFail, //Test Only - cause font duplicate to fail, or reset this
	EFbsMessUpdateLinkedTypeface, //Update an existing linked typeface specification with rasterizer; file valid after reboot
	EFbsMessGetFontTable,
	EFbsMessReleaseFontTable,
	EFbsMessGetGlyphOutline,
	EFbsMessReleaseGlyphOutline,
	EFbsMessGetGlyphs, // Retrieve rasterised glyphs from glyph atlas and closes last glyph retrieved
	EFbsMessNoOp, // No-op call; used to ensure that the RSgImage of the last glyph retrieved from glyph atlas has been closed
	EFbsMessGetGlyphMetrics, // Retrieve metrics of multiple glyph codes in one message
	EFbsMessAtlasFontCount, // (Debug-only) Retrieve the number of fonts with glyphs in the H/W Glyph cache
	EFbsMessAtlasGlyphCount, // (Debug-only) Retrieve the number of glyphs (optionally, given a specific font) stored in the H/W Glyph cache
	EFbsMessOogmNotification, // An action requested by the GOoM framework. Reduce or re-instate graphics memory usage.
	EFbsMessGetGlyphCacheMetrics, // Retrieve the glyph-cache size, its maximum limit and whether the maximum is the reduced value used in OoGm situations.
	// If you are adding new messages don't forget to check that the 
	// security permissions are set correctly (KRanges & KElementsIndex in server.cpp)
	};

#endif // __FBSMESSAGE_H__
