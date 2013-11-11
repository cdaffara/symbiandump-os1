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

#ifndef DIRECTGDIPANICCODES_H
#define DIRECTGDIPANICCODES_H

#include <e32std.h>

_LIT(KComponentPanicCategory, "DGDI");
#define SYMBIAN_PANIC_CATEGORY KComponentPanicCategory
#include <graphics/directgdipanics.h>

/**
Enumeration of DirectGDI panic codes. These should be used in conjunction with
the GRAPHICS_ASSERT_ALWAYS and GRAPHICS_PANIC_ALWAYS macros defined in directgdipanics.inl.

@internalComponent
*/
enum TDirectGdiPanicCode
	{
	/** The requested functionality has not been implemented - do not use.
	*/ 
	EDirectGdiPanicNotImplemented = 1,

	/** An assertion has evaluated to false.
	*/ 
	EDirectGdiPanicAssertionFailure = 2,
		
	/**	Valid reference to rendering engine	not found. 
	*/
	EDirectGdiPanicRenderingEngineNotSet = 3,
	
	/**	Valid reference to rendering target not found. 
	*/
	EDirectGdiPanicRenderingTargetNotSet = 4,
	
	/**	Valid reference to DirectGDI driver not initialised. 
	*/
	EDirectGdiPanicDirectGdiDriverNotInitialised = 5,
	
	/**	Invalid reference count to DirectGDI driver.
	*/
	EDirectGdiPanicDriverInvalidRefCount = 6,
	
	/** Context has not been activated. 
	*/
	EDirectGdiPanicContextNotActivated = 7,
	
	/** Invalid brush pattern passed to SetBrushPattern().
	*/
	EDirectGdiPanicInvalidBrushPattern = 8,
	
	/** Attempt to use a brush with brush style style is EPatternedBrush but a pattern has not 
	    been successfully set using SetBrushPattern().
	*/
	EDirectGdiPanicBrushPatternNotSet = 9,
	
	/** Invalid brush style.
	*/
	EDirectGdiPanicInvalidBrushStyle = 10,
	
	/** No valid font has been selected.
	*/
	EDirectGdiPanicNoFontSelected = 11,
	
	/** Invalid font.
	*/
	EDirectGdiPanicInvalidFont = 12,
	
	/** UpdateJustification called wrongly.
	*/
	EDirectGdiPanicAutoUpdateJustificationUsed = 13,
	
	/** A check on the open count in CDirectGdiDriver::Open() failed.
	*/
	EDirectGdiPanicDriverOpenCountError = 14,
	
	/** A check on the open count in CDirectGdiDriver::~CDirectGdiDriver() failed.
	*/
	EDirectGdiPanicDriverDestructorOpenCountError = 15,
	
	/** The RDirectGdiImageTarget object has no pointer to the DirectGDI driver
	*/
	EDirectGdiPanicImageTargetWithoutDriver = 16,
	
	/** The RDirectGdiImageTarget object already has an image target in use in CDirectGdiDriver::CreateImageTarget()
	*/
	EDirectGdiPanicImageTargetAlreadyExists = 17,
	
	/** The RDirectGdiDrawableSource object has no pointer to the DirectGDI driver
	*/
	EDirectGdiPanicDrawableSourceWithoutDriver = 18,
	
	/** The RDirectGdiDrawableSource object already has a drawable source in use in CDirectGdiDriver::CreateDrawableSource()
	*/
	EDirectGdiPanicDrawableSourceAlreadyExists = 19,
	
	/** GetInfo failed for an image in CDirectGdiDriver::CreateDrawableSource()
	 */
	EDirectGdiPanicImageSourceInfoError = 20,
	
	/** GetInfo failed for an image in CDirectGdiDriver::CreateImageTarget()
	 */
	EDirectGdiPanicImageTargetInfoError = 21,
	
	/** Bad parameter is passed to the function
	 */
	EDirectGdiPanicBadParameter = 22,
	};

#endif /*DIRECTGDIPANICCODES_H*/
