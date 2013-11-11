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

#ifndef DIRECTGDIADAPTER_H
#define DIRECTGDIADAPTER_H

/**
@file
@internalComponent
*/

#include <e32cmn.h>

_LIT(KComponentPanicCategory, "DGDIAdapter");
#define SYMBIAN_PANIC_CATEGORY KComponentPanicCategory

#include "directgditypes.h"
#include <graphics/directgdipanics.h>


/**
Enumeration of DirectGDI panic codes. These should be used in conjunction with
the ASSERT_ALWAYS and ASSERT_DEBUG macros defined below.
*/
enum TDirectGdiAdapterPanicCode
	{
	/** The requested functionality has not been implemented - do not use.
	*/ 
	EDirectGdiPanicNotImplemented = 1,

	/** An unexpected error has occured.
	*/ 
	EDirectGdiPanicUnexpectedError = 2,
		
	/**	Invalid reference count to DirectGDI driver.
	*/
	EDirectGdiPanicDriverInvalidRefCount = 6,
	
	/**	NULL bitmap specified.
	*/
	EDirectGdiPanicInvalidBitmap = 7,

	/** Invalid region passed into function.
	*/
	EDirectGdiPanicInvalidRegion = 8,

	/** An attempt has been made to use a patterned brush style when a patterned brush has not been set.
	 */
	EDirectGdiPanicPatternedBrushNotSet = 12,
	
	/** Unable to bind a rendering API.
	 */
	EDirectGdiPanicBindApi = 23,
	
	/** No drawing surface configurations are available.
	 */
	EDirectGdiPanicNoAvailableConfigs = 24,
	
	/** No available drawing surface configurations match specific attributes.
	 */
	EDirectGdiPanicNoMatchingConfig = 25,
	
	/** CVgEngine::DrawPolygon() was passed an invalid fill-rule.
	 */
	EDirectGdiPanicInvalidFillRule = 26,
	
	/** An array defining a polyline/polygon has too few points.
	 */
	EDirectGdiPanicInvalidPointArray = 27,
	
	/** RSgImage::GetPixelFormats() returns pixel count = 0.
	 */
	EDirectGdiPanicNoValidPixelFormats = 29,
	
	/** Open count error in CDirectGdiDrawableRef::Close().
	 */
	EDirectGdiPanicDrawableRefCountError = 30,
	
	/** Attempt to get a handle to a drawable resource using CDirectGdiDriverImpl::GetImageTargetFromHandle(),
	 * CDirectGdiDriverImpl::GetImageSourceFromHandle(), CDirectGdiDriverImpl::GetDrawableSourceFromHandle()
	 * or CSwDirectGdiDriverImpl::FindImageTargetImpl() failed.
	 */
	EDirectGdiPanicResourceHandleNotFound = 32,
	
	/** Driver Process State has not been initialised.
	 */
	EDirectGdiPanicProcessStateNotInitialized = 33,
	
	/** Attempt to activate a target in CVgEngine::Activate() or CSwDirectGdiEngine::Activate() with a NULL handle.
	 */
	EDirectGdiPanicActivateWithNullHandle = 34,
	
	/** Unable to initialise a display.
	 */
	EDirectGdiPanicInitializeDisplay = 35,
	
	/** No display to which graphics are drawn.
	 */
	EDirectGdiPanicNoDisplay = 36,
	
	/** Unable to close a drawable handle.
	 */
	EDirectGdiPanicCloseDrawableHandleFailure = 37,
	
	/** Attempt to destroy a NULL engine in CDirectGdiDriverImpl::DestroyEngine().
	 */
	EDirectGdiPanicDestroyNullEngine = 38,
	
	/** Attempt to activate a NULL target in CDirectGdiDriverImpl::Activate().
	 */
	EDirectGdiPanicNullTargetActivate = 39,
	
	/** Failed to find a config for a particular pixel type.
	 */
	EDirectGdiPanicNoConfigFound = 41,
	
	/** Failed to create a drawing surface.
	 */
	EDirectGdiPanicNoDrawingSurface = 42,
	
	/** Failed to create a drawing context.
	 */
	EDirectGdiPanicNoContext = 43,
	
	/** Attempt to delete a CDirectGdiDrawableRef object when its reference count is non-zero.
	 */
	EDirectGdiPanicDrawableRefDestructorError = 44,
	
	/** A driver was closed while some drawable items were still in the drawable array.
	 */
	EDirectGdiPanicItemsLeftInImageArray = 45,

	/** Inconsistency in internal data of glyph cache structure.
	*/
	EDirectGdiPanicGlyphCacheDataInconsistent = 46,
	
	/** Valid reference to image source not found.
	*/
	EDirectGdiPanicImageSourceNotFound = 47,
	
	/** An invalid image handle was passed to the CImageSourceData constructor.
	 */
	EDirectGdiPanicImageSourceDataConstructorError = 48,
	
	/** The reference count was not zero in the destructor of CImageSourceData.
	 */
	EDirectGdiPanicImageSourceDataRefCountError = 49,
	
	/** The instance count was not zero in the destructor of CDirectGdiDriverProcessState.
	 */
	EDirectGdiPanicProcessStateInstanceCountError = 50,
	
	/** The image count was not zero in the destructor of CDirectGdiDriverProcessState.
	 */
	EDirectGdiPanicProcessStateImageCountError = 51,
	
	/** This panic only occurs when _DEBUG_DIRECTGDI is defined. One of the methods in 
	CDirectGdiDriverProcessState was called without the image's mutex being held.
	 */
	EDirectGdiPanicImageMutexError = 52,
	
	/** Attempt to activate a target in CSwDirectGdiEngine::Activate() with a NULL draw device.
	 */
	EDirectGdiPanicActivateWithNullDrawDevice = 53,
	
	/** Attempt to deactive a NULL target in CDirectGdiDriverImpl.
	 */
	EDirectGdiPanicNullTargetDeactivate = 55,

	/** This panic occurs when DrawGlyph is called on an unsupported glyph bitmap type.
	 */
	EDirectGdiPanicInvalidGlyphBitmapType = 56,
	
	/** Attempt to create a new image source/drawable source using a handle that already has 
 	 an image source/drawable source associated with it.
	 */
	EDirectGdiPanicSourceHandleNotNull = 57,
	
	/** Attempt to create a new image target using a handle that is already has an image target associated with it.
	 */
	EDirectGdiPanicTargetHandleNotNull = 58,
	
	/** An egl Api returned a failure. The actual egl error code is reported in the log.
	 */
	EEglApiFailure = 59,
	
	/** In a call to DestroyEngine(), the engine could not be found in the internal list of engines. 
	 */
	EDirectGdiPanicEngineNotFound = 60,
	
	/** Glyph image storage was not created.
	 */
	EDirectGdiPanicGlyphImageStorageNotCreated = 61,
	
	/** Pen end cap style expected to be rounded, but found not to be.
	 */
	EDirectGdiPanicPenEndCapStyleNotRound = 62,
	
	/** eglWaitClient error.
	 */
	EDirectGdiPanicFinish = 63,
	
	/** An error code from vgGetError which is caused by incorrect usage of OpenVG.
	 */
	EDirectGdiPanicVgError = 64,
	
	/** Whilst performing a Clear() the clear-brush is not a recognised paint brush.  
	 */
	EDirectGdiPanicClearBrushInvalid = 65,
	
	/**  Panic codes corresponding to the old BitGdi codes (+1000).
	Any more panic codes from BitGDI generic DirectGDI should be added in this section.
	*/
	EDirectGdiPanicInvalidParameter = 1006,
	EDirectGdiPanicInvalidDisplayMode = 1009,
	
	/** Attempt to draw outside of clipping region.
	*/
	EDirectGdiPanicOutOfBounds = 1013,
	EDirectGdiPanicPolygonFiller = 1015,
	EDirectGdiPanicZeroLength = 1016,
	EDirectGdiPanicInvalidPointer = 1018,
	EDirectGdiPanicInvalidArg = 1021,
	EDirectGdiPanicNegativeShift = 1022,

	/**
	This panic may occur if MOutlineAndShadowBlend interface is not implemented, or
	the MSgImage_Sw interface is not implemented on RSgImage.	
	*/
	EDirectGdiPanicInvalidInterfaceHandle = 1023,
	// End of panic codes corresponding to the old BitGDI codes.
	};

/** 
Log current eglGetError() to the debug stream, along with line number and source file name.
This must be #defined so that we retain the line and source file information at the
point in which LogEglError() is invoked.
*/
#ifdef _DEBUG
#define LogEglError() {TBuf16<256> message; message.Format(_L16("EGL Error: %x\n"), eglGetError()); GRAPHICS_LOGD_DEBUG(message);}
#else
#define LogEglError() {}
#endif //_DEBUG

#endif /*DIRECTGDIADAPTER_H*/


