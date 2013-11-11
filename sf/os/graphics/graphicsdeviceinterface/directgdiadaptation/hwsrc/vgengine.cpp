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
// Unless stated otherwise, all co-ordinates are recorded and calculated using 
// Symbian graphics format: origin at top left of image, 'y' increasing towards bottom
// of image. Note that OpenVG places the origin at an image's bottom left corner, with
// 'y' increasing towards the top of the image. 
// 
//

#include "vgengine.h"
#include "directgdiadapter.h"
#include "directgdidriverimpl.h"
#include "directgdiimagetargetimpl.h"
#include "directgdiimagesourceimpl.h"
#include "confighelper.h"
#include <graphics/directgdiimagetarget.h>
#include <graphics/directgdidrawablesource.h>
#include <fbs.h>
#include <e32cmn.h>
#include <e32math.h>
#include <s32strm.h>
#include <pixelformats.h>
#include <bitdraworigin.h>
#include <bitdrawinterfaceid.h>

#include "blendingalgorithms.inl"

/**
Pre-calculation for normalising a 0-255 colour channel to 0..1.
*/
const VGfloat KColorConversion = 1.0f/255.0f;


inline TInt Scale(TInt aValue,TInt aNum,TInt aDen)
	{
	if (aNum==aDen)
		return aValue;
	TInt64 result(aValue);
	result=(result*aNum+(aDen/2))/aDen;
	return I64INT(result);
	}

/**
Maps TDisplayMode onto a supported VGImageFormat. Only compatible formats are returned,
anything else results in VG_IMAGE_FORMAT_INVALID being returned.

@param 	aDisplayMode Mode to convert from.

@return The VGImageFormat which matches the specified TDisplayMode pixel format exactly.
		If no exact match exists, then VG_IMAGE_FORMAT_INVALID is returned.
 */
static VGImageFormat MapToVgDisplayMode(TDisplayMode aDisplayMode)
	{
	switch(aDisplayMode)
		{
	case ENone:
		return VG_IMAGE_FORMAT_INVALID;
	case EGray2:
		return VG_BW_1;
	case EGray4: 
	case EGray16:
		return VG_IMAGE_FORMAT_INVALID;
	case EGray256:
		return VG_sL_8;
	case EColor16:
	case EColor256:
	case EColor4K:
		return VG_IMAGE_FORMAT_INVALID;
	case EColor64K:
		return VG_sRGB_565;
	case EColor16M:
	case ERgb:
		return VG_IMAGE_FORMAT_INVALID;
	case EColor16MU:
		return VG_sXRGB_8888;
	case EColor16MA:
		return VG_sARGB_8888;
	case EColor16MAP:
		return VG_sARGB_8888_PRE;
	case EColorLast:
		return VG_IMAGE_FORMAT_INVALID;
		}
	
	return VG_IMAGE_FORMAT_INVALID;
	}

/**
Destructor
 */
CVgEngine::~CVgEngine()
	{
	iRegionManager.Close();
	
	if (iBrushPatternUser != VG_INVALID_HANDLE)	
		vgDestroyImage(iBrushPatternUser);		
		
	if (iBrushPatternStandard != VG_INVALID_HANDLE)
		vgDestroyImage(iBrushPatternStandard);		
	
	if (iBrushPatternStandardRegion != VG_INVALID_HANDLE)
		vgDestroyImage(iBrushPatternStandardRegion);		
	
	if (iBrushPatternNonZeroOrigin != VG_INVALID_HANDLE)
		vgDestroyImage(iBrushPatternNonZeroOrigin);	
	
	User::Free(iPathCommands);	
	User::Free(iPathCoords);
	
	Deactivate();
	}

/**
Constructor
 */
CVgEngine::CVgEngine(CDirectGdiDriverImpl& aDriver)
	:iDriver(aDriver),
	iPen(VG_INVALID_HANDLE),
	iBrush(VG_INVALID_HANDLE),
	iClearBrush(VG_INVALID_HANDLE),
	iVgPath(VG_INVALID_HANDLE),
	iTextBrush(VG_INVALID_HANDLE),
	iBrushPatternUser(VG_INVALID_HANDLE),
	iBrushPatternStandard(VG_INVALID_HANDLE),
	iBrushPatternStandardRegion(VG_INVALID_HANDLE),
	iBrushPatternNonZeroOrigin(VG_INVALID_HANDLE)
	{		
	// Set the default paint mode for drawing and filling shapes to
	// just draw the pen and not the brush as default (to match BitGdi)
	iPaintMode = VG_STROKE_PATH;
	
	//cache interface to use every time glyph gets drawn
	GetInterface(TUid::Uid(KDirectGdiGetGlyphStorageUid), (TAny*&) iFontGlyphImageStorage);
	}

/**
Applies the engine's state to OpenVG, re-applying all the VgEngine settings. It is called by 
Activate() as well as when the engine is being made current. 
*/
void CVgEngine::SetVgState()
	{
	ResetVgMatrix();
	vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_NONANTIALIASED);
	vgSeti(VG_IMAGE_QUALITY, VG_IMAGE_QUALITY_NONANTIALIASED);
	vgSeti(VG_STROKE_CAP_STYLE, VG_CAP_ROUND);
	vgSeti(VG_STROKE_JOIN_STYLE, VG_JOIN_ROUND);
	vgSetPaint(iPen, VG_STROKE_PATH);
	vgSetPaint(iBrush, VG_FILL_PATH);
	SetDrawMode(iDrawMode); 
	SetPenColor(iPenColor);
	SetPenStyle(iPenStyle);
	SetPenSize(iPenSize);
	SetBrushColor(iBrushColor);
	SetBrushStyle(iBrushStyle);
	SetBrushOrigin(iBrushOrigin);
	vgSetParameteri(iClearBrush, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
	vgSetParameteri(iTextBrush, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
	}

/**
Binds a rendering target to this OpenVG rendering engine.
Activates the target, which increments the reference count of target as it can be shared across many 
DirectGDI contexts.

The rendering target, for this implementation, specifies an Embedded OpenVG rendering surface.

@see MDirectGdiEngine::Activate()
@see Deactivate()

@panic DGDIAdapter 34, if the passed target has a NULL handle.
@panic DGDIAdapter 39, if target associated with the handle is NULL.
 */
TInt CVgEngine::Activate(RDirectGdiImageTarget& aTarget)
	{
	TInt result = KErrNone;
	GRAPHICS_ASSERT_ALWAYS(aTarget.Handle(), EDirectGdiPanicActivateWithNullHandle);
	CDirectGdiImageTargetImpl* target = iDriver.GetImageTargetFromHandle(aTarget.Handle());
	GRAPHICS_ASSERT_ALWAYS(target, EDirectGdiPanicNullTargetActivate);
	if (target == iRenderingTarget)
		{
		return KErrNone;
		}
	
	Deactivate();	
	
	iDriver.Activate(target);
	iRenderingTarget = target;
	iDriver.SetCurrentEngine(this);
	iDriver.SetCurrentTarget(target);
	iSize = iRenderingTarget->Size();
	iTargetRegion.Clear();
	iTargetRegion.AddRect(iSize);

	iRegionManager.Initialize(vgGeti(VG_MAX_SCISSOR_RECTS), iSize);
	
	// Set the origin to top-left for path and image rendering. iIdentityMatrix is set so that we have a
	// record of the "identity" transform. After modifying the transform for offsets, rotations, etc, etc
	// just set to iIdentityMatrix to get back to "normal".
	//
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
	vgLoadIdentity();
	vgScale(1, -1);
	vgTranslate(0,  -iSize.iHeight);
	vgGetMatrix(iIdentityMatrix);
	// Set the origin to top-left for image matrix.
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
	vgLoadMatrix(iIdentityMatrix);
	
	// Create the path.
	if (!PreparePath(iVgPath, 1) || (iVgPath == VG_INVALID_HANDLE))
		result = KErrNoMemory;

	// Create the pen object and set it as the object for stroking (OpenVG equivalent of a pen).
	if (result == KErrNone)
		{
		iPen = vgCreatePaint();
		if (iPen != VG_INVALID_HANDLE)
			vgSetPaint(iPen, VG_STROKE_PATH);
		else
			result = KErrNoMemory;
		}

	if (result == KErrNone)
		{		
		// Create the brush object
		// This only need to be done the first time the context is activated
		iBrush = vgCreatePaint();
		if (iBrush != VG_INVALID_HANDLE)
			vgSetPaint(iBrush, VG_FILL_PATH);
		else
			result = KErrNoMemory;
		}
	
	if (result == KErrNone)
		{
		// Create the brush object for Clear operations.
		// This only need to be done the first time the context is activated
		iClearBrush = vgCreatePaint();
		if (iClearBrush == VG_INVALID_HANDLE)
			{
			result = KErrNoMemory;
			}
		}
	
	if (result == KErrNone)
		{
		iTextBrush = vgCreatePaint();
		if (iTextBrush == VG_INVALID_HANDLE)
			{
			result = KErrNoMemory;
			}
		}
	
	if (result == KErrNone)
		{
		result = iDriver.PreAllocateFontGlyphImages();
		}

	if (result == KErrNone)
		{
		SetVgState();
		}

	return result;
	}

/**
@see MDirectGdiEngine::Deactivate()
@see Activate()
 */
void CVgEngine::Deactivate()
	{
	if (iPen != VG_INVALID_HANDLE)
		{
		vgDestroyPaint(iPen);
		}
	if (iBrush != VG_INVALID_HANDLE)
		{
		vgDestroyPaint(iBrush);		
		}
	if (iClearBrush != VG_INVALID_HANDLE)
		{
		vgDestroyPaint(iClearBrush);		
		}
	if (iTextBrush != VG_INVALID_HANDLE)
		{
		vgDestroyPaint(iTextBrush);		
		}
	if (iVgPath != VG_INVALID_HANDLE)
		{
		vgDestroyPath(iVgPath);
		}
	iPen = VG_INVALID_HANDLE;
	iBrush = VG_INVALID_HANDLE;
	iClearBrush = VG_INVALID_HANDLE;
	iVgPath = VG_INVALID_HANDLE;
	iTextBrush = VG_INVALID_HANDLE;
	
	if (iRenderingTarget)
		{
		// Deactivating the render target could potentially unbind the current EGL context
		// which would make the above vgDestroy() calls do nothing, therefore call Deactivate() last.
		iDriver.Deactivate(iRenderingTarget);
		}
	}

/**
Checks whether the engine and target are current. If current, then nothing is done, else all the OpenVG settings 
and EGL context are reapplied. This function is called in every drawing function to ensure that the engine and 
target are current.

If this function returns EFalse, it means any subsequent setting of OpenVG state may be invalid 
and should be avoided as it is setting a null EGL context.

@pre	None.
@post	Applies the current state to OpenVG and is made the active EGL context, if the engine or target is 
        not current.
@return ETrue if as a result of calling this function, the underlying OpenVG context is now current. This
        effectively means whether we have a target or not. EFalse is returned otherwise.
*/
TBool CVgEngine::MakeEngineCurrent()
	{
	TBool vgCurrent = iRenderingTarget!=NULL;
	if(!iDriver.IsCurrentEngine(this) || !iDriver.IsCurrentTarget(iRenderingTarget))
		{
		iDriver.SetCurrentEngine(this);
		iDriver.SetCurrentTarget(iRenderingTarget);
		// Must reactivate the target (i.e. make it current to EGL) before resetting the OpenVG parameters.
		if (iRenderingTarget)
			{
			iDriver.Reactivate(iRenderingTarget);
			SetVgState();
			}
		else
			vgCurrent = EFalse;
		}
	
	return vgCurrent;
	}

/**
@see MDirectGdiEngine::SetOrigin()
 */
void CVgEngine::SetOrigin(const TPoint& aOrigin)
	{
	iOrigin = aOrigin + iDrawOrigin;
	
	if (!MakeEngineCurrent())
		return;

	ResetVgMatrix();
	}

/** 
@see MDrawDeviceOrigin::Set()
*/
TInt CVgEngine::Set(const TPoint& aDrawOrigin)
	{
	TPoint moveOrigin=aDrawOrigin;
 	moveOrigin-=iDrawOrigin;
 	iOrigin+=moveOrigin;
 	
 	//offset clipping region
 	TInt result = KErrNone;
 	RRegion clippingRegion;
 	clippingRegion.Copy(iRegionManager.ClippingRegion());
 	if(!clippingRegion.CheckError())
 		{
 		clippingRegion.Offset(moveOrigin);
 		result = iRegionManager.SetClippingRegion(clippingRegion);
 		}
 	else
 		{
 		result = KErrNoMemory;
 		}
 	
 	if(result != KErrNone)
 		{
 		iDriver.SetError(result);
 		}
 	
 	clippingRegion.Close();
 	
 	iDrawOrigin = aDrawOrigin;
 	return result;
 	}
 
/** 
@see MDrawDeviceOrigin::Get()
*/
void CVgEngine::Get(TPoint& aDrawOrigin)
 	{
 	aDrawOrigin=iDrawOrigin;
   	}

/**
@see MDirectGdiEngine::SetClippingRegion(const TRegion&)
*/
void CVgEngine::SetClippingRegion(const TRegion& aRegion)
	{
	TInt result = KErrNone;
	TRect boundingRect=iTargetRegion.BoundingRect();
	boundingRect.iTl-=iDrawOrigin;
	boundingRect.iBr-=iDrawOrigin;
	if (!aRegion.IsContainedBy(boundingRect))
		{
		result = KErrArgument;
		}
	else
		{
		RRegion clippingRegion;
		clippingRegion.Copy(aRegion);
		if(!clippingRegion.CheckError())
			{
			clippingRegion.Offset(iDrawOrigin);
			result = iRegionManager.SetClippingRegion (clippingRegion);
			}
		else
			{
			result = KErrNoMemory;
			}
		clippingRegion.Close();
		}
	
	if (result != KErrNone)
		{
		iDriver.SetError(result);
		}
	}

/**
@see MDirectGdiEngine::ResetClippingRegion()
*/
void CVgEngine::ResetClippingRegion()
	{
	iRegionManager.Reset();
	
	if (!MakeEngineCurrent())
		return;
		
	vgSeti(VG_SCISSORING, VG_FALSE);
	}

/**
@see MDirectGdiEngine::SetDrawMode()

The generic layer has already checked whether the draw mode is already aMode.
Draw mode is referred to as "blend" mode in OpenVG.
Note that only EDrawModePEN and EDrawModeWriteAlpha style blending are supported by OpenVG.
The default OpenVG blend mode is VG_BLEND_SRC_OVER.
 */
void CVgEngine::SetDrawMode(DirectGdi::TDrawMode aMode)
	{
	iDrawMode = aMode;
	
	if (!MakeEngineCurrent())
		return;

	// Invalid modes are filtered out in the generic layer.
	if (aMode == DirectGdi::EDrawModePEN)
		{
		// Blend the destination with the source using the source alpha for blending if 
		// alpha is available.		
		vgSeti(VG_BLEND_MODE, VG_BLEND_SRC_OVER);
		}
	else // DirectGdi::EDrawModeWriteAlpha
		{
		// Destination colors and alpha are overwritten with source colors and alpha.						
		vgSeti(VG_BLEND_MODE, VG_BLEND_SRC);											
		}
	}

/**
@see MDirectGdiEngine::SetPenColor()
*/
void CVgEngine::SetPenColor(const TRgb& aColor)
	{
	iPenColor = aColor;
	
	if (!MakeEngineCurrent())
		return;

	// Make sure our pen is the current selected pen for stroking before we set the color	
	vgSetPaint(iPen, VG_STROKE_PATH);	
	
	// Set the color
	vgSetParameteri(iPen, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
	SetVgPaintColor(iPen, iPenColor);	
	SetVgPaintColor(iTextBrush, iPenColor);
	}

/**
Set the current pen style for drawing lines. This corresponds to setting the "stroke dash" pattern in OpenVG.

@see MDirectGdiEngine::SetPenStyle()
 */
void CVgEngine::SetPenStyle(DirectGdi::TPenStyle aStyle)
	{
	iPenStyle = aStyle;
	
	if (!MakeEngineCurrent())
		return;
	
	iPaintMode = iPaintMode | VG_STROKE_PATH;		
	
	switch (aStyle)
		{
		case DirectGdi::ENullPen:
			{
			iPaintMode = iPaintMode & (~VG_STROKE_PATH); // Unset stroke bit
			vgSetfv(VG_STROKE_DASH_PATTERN, 0, NULL);
			vgSetf(VG_STROKE_DASH_PHASE, 0.f);
			break;
			}
			
		case DirectGdi::ESolidPen:
			{
			vgSetfv(VG_STROKE_DASH_PATTERN, 0, NULL);	
			vgSetf(VG_STROKE_DASH_PHASE, 0.f);
			break;
			}
			
		case DirectGdi::EDottedPen:
			{
			VGfloat offset = (iPenSize.iWidth > 1) ? 2.f : 0.f;			
			VGfloat dashPattern[2] = {(1*iPenSize.iWidth)-offset, (3*iPenSize.iWidth)+offset};
			vgSetfv(VG_STROKE_DASH_PATTERN, 2, dashPattern);
			vgSetf(VG_STROKE_DASH_PHASE, 1.f);
			break;
			}
			
		case DirectGdi::EDashedPen:
			{
			VGfloat offset = (iPenSize.iWidth > 1) ? 2.f : 0.f;			
			VGfloat dashPattern[2] = {(3*iPenSize.iWidth)-offset, (3*iPenSize.iWidth)+offset};
			vgSetfv(VG_STROKE_DASH_PATTERN, 2, dashPattern);
			vgSetf(VG_STROKE_DASH_PHASE, (iPenSize.iWidth != 1) ? 1.f : 0.f);
			break;
			}
			
		case DirectGdi::EDotDashPen:
			{
			VGfloat offset = (iPenSize.iWidth > 1) ? 2.f : 0.f;			
			VGfloat dashPattern[4] = {(1*iPenSize.iWidth)-offset, (3*iPenSize.iWidth)+offset, (3*iPenSize.iWidth)-offset, (3*iPenSize.iWidth)+offset};
			vgSetfv(VG_STROKE_DASH_PATTERN, 4, dashPattern);
			vgSetf(VG_STROKE_DASH_PHASE, 1.f);
			break;
			}
			
		case DirectGdi::EDotDotDashPen:
			{
			VGfloat offset = (iPenSize.iWidth > 1) ? 2.f : 0.f;			
			VGfloat dashPattern[6] = {(1*iPenSize.iWidth)-offset, (3*iPenSize.iWidth)+offset, (1*iPenSize.iWidth)-offset, (3*iPenSize.iWidth)+offset, (3*iPenSize.iWidth)-offset, (3*iPenSize.iWidth)+offset};
			vgSetfv(VG_STROKE_DASH_PATTERN, 6, dashPattern);
			vgSetf(VG_STROKE_DASH_PHASE, (iPenSize.iWidth != 1) ? 1.f : 0.f);
			break;			
			}
			
		default:
			{
			// Copy BitGdi behaviour here and draw a solid line for any unknown pen style
			vgSetfv(VG_STROKE_DASH_PATTERN, 0, NULL);
			vgSetf(VG_STROKE_DASH_PHASE, 0.f);
			break;		
			}
		}
	}


/**
@see MDirectGdiEngine::SetPenSize()
 */
void CVgEngine::SetPenSize(const TSize& aSize)
	{
	iPenSize = aSize;
	
	if (!MakeEngineCurrent())
		return;

	// VG_STROKE_LINE_WIDTH expects a float	
	// Note that we set the pen size using just the width in the assumption that the pen width
	// and height are normally the same. For the special cases where the pen width and height
	// are different, the pen size is set to (1,1) then scaled to give the effect of a pen with 
	// different width and height. This is done for all functions in this file that draws shapes,  
	// see Plot(), DrawLine(), DrawArc(), DrawRect() etc.
	vgSetf(VG_STROKE_LINE_WIDTH, aSize.iWidth);	
		
	// Re-set the pen style as the pen size has changed, SetPenStyle() uses the pen size to set 
	// the dotted line styles.
	SetPenStyle(iPenStyle);
	}

/**
@see MDirectGdiEngine::SetTextShadowColor()
*/
void CVgEngine::SetTextShadowColor(const TRgb& aColor)
	{
	iTextShadowColor = aColor; //just cache this color
	}


/**
@see MDirectGdiEngine::SetBrushColor()
*/
void CVgEngine::SetBrushColor(const TRgb& aColor)
	{
	iBrushColor = aColor;
	
	if (!MakeEngineCurrent())
		return;
	
	// Convert the color components as that they are between 0.0f and 1.0f
	VGfloat color[4];	
	color[0] = aColor.Red() * KColorConversion;
	color[1] = aColor.Green() * KColorConversion;
	color[2] = aColor.Blue() * KColorConversion;
	color[3] = aColor.Alpha() * KColorConversion;

	// Make sure our brush is the current selected brush for filling before we set the color
	if (iBrushStyle != DirectGdi::ENullBrush)
		vgSetPaint(iBrush, VG_FILL_PATH);	
	
	vgSetParameteri(iBrush, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
	vgSetParameterfv(iBrush, VG_PAINT_COLOR, 4, color);
	
	// Set the clear color and the tile fill color as well as these should both
	// be the same as the brush color
	vgSetParameterfv(iClearBrush, VG_PAINT_COLOR, 4, color);
	vgSetfv(VG_CLEAR_COLOR, 4, color);
	vgSetfv(VG_TILE_FILL_COLOR, 4, color);
	
	// If using a patterned brush, need to reconstruct it as the colours may be out of sync.
	if ((iBrushStyle != DirectGdi::ENullBrush) && (iBrushStyle != DirectGdi::ESolidBrush))
		{
		SetBrushStyle(iBrushStyle);
		}	
	}


/**
The DirectGDI brush styles do not map directly to OpenVG, so brushes for styles > DirectGdi::EPatternedBrush
are created as bitmaps before being set.

@see MDirectGdiEngine::SetBrushColor()
@see CreateStandardBrush()
*/
void CVgEngine::SetBrushStyle(DirectGdi::TBrushStyle aStyle) 
	{	
	iBrushStyle = aStyle;
	
	if (!MakeEngineCurrent())
		return;
	
	TInt standardBrushErr = KErrNone;
	const TInt standardBrushSize = 3;
	const TInt standardBrushArea = standardBrushSize*standardBrushSize;
	const TInt diamondCrossHatchBrushSize = 4;
	const TInt diamondCrossHatchBrushArea = diamondCrossHatchBrushSize*diamondCrossHatchBrushSize;
	
	// Select the brush for drawing any style that is not ENullBrush
	iPaintMode = iPaintMode | VG_FILL_PATH;
	if (aStyle != DirectGdi::ENullBrush)
		vgSetPaint(iBrush, VG_FILL_PATH);

	// Paint using a pattern for all styles that are not ENullBrush or ESolidBrush
	if ((aStyle != DirectGdi::ENullBrush) && (aStyle != DirectGdi::ESolidBrush))
		{		
		vgSetParameteri(iBrush, VG_PAINT_TYPE, VG_PAINT_TYPE_PATTERN);
		vgSetParameteri(iBrush, VG_PAINT_PATTERN_TILING_MODE, VG_TILE_REPEAT);
		}	

	switch (aStyle)
		{
		case DirectGdi::ENullBrush:					
			iPaintMode = iPaintMode & (~VG_FILL_PATH);
			
			// Clear the current brush so that no brush color is drawn
			vgSetPaint(VG_INVALID_HANDLE, VG_FILL_PATH);
			break;			

		case DirectGdi::ESolidBrush:			
			// Paint using a solid color
			vgSetParameteri(iBrush, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
			break;			

		case DirectGdi::EPatternedBrush:						
			// Make sure the user has set a patterned brush for use
			GRAPHICS_ASSERT_ALWAYS(iBrushPatternUser != VG_INVALID_HANDLE, EDirectGdiPanicPatternedBrushNotSet);						
			
			if (NonZeroBrushPatternOrigin())
				{
				// The brush origin is non-zero, update the non-zero origin brush 
				// with the user brush and use that instead of the user brush
				CopyCurrentBrushPatternForNonZeroOrigin();										
				}
			else
				{
				// Set the brush to fill with the user selected bitmap pattern
				vgPaintPattern(iBrush, iBrushPatternUser);
				}
			break;			

		case DirectGdi::EVerticalHatchBrush:
			{
			// The brush fills with vertical hatching  lines going from top to bottom
			TSize size(standardBrushSize, standardBrushSize);
			VGbyte brushPattern[standardBrushArea] = {0,0,1, 0,0,1, 0,0,1};
			standardBrushErr = CreateStandardBrush(size, brushPattern);
			break;
			}
		
		case DirectGdi::EForwardDiagonalHatchBrush:
			{
			// The brush fills with diagonal hatching lines going from bottom left to top right
			TSize size(standardBrushSize, standardBrushSize);
			VGbyte brushPattern[standardBrushArea] = {1,0,0, 0,0,1, 0,1,0};
			standardBrushErr = CreateStandardBrush(size, brushPattern);
			break;
			}
		
		case DirectGdi::EHorizontalHatchBrush:
			{
			// The brush fills with horizontal hatching lines going from left to right
			TSize size(standardBrushSize, standardBrushSize);
			VGbyte brushPattern[standardBrushArea] = {0,0,0, 0,0,0, 1,1,1};
			standardBrushErr = CreateStandardBrush(size, brushPattern);
			break;
			}
		
		case DirectGdi::ERearwardDiagonalHatchBrush:
			{
			// The brush fills with rearward diagonal hatching lines going from top left to bottom right
			TSize size(standardBrushSize, standardBrushSize);
			VGbyte brushPattern[standardBrushArea] = {1,0,0, 0,1,0, 0,0,1};
			standardBrushErr = CreateStandardBrush(size, brushPattern);
			break;
			}
		
		case DirectGdi::ESquareCrossHatchBrush:
			{
			// The brush fills with horizontal and vertical hatching lines going from left to right 
			// plus lines going from top to bottom  giving the effect of a grid of small squares
			TSize size(standardBrushSize, standardBrushSize);
			VGbyte brushPattern[standardBrushArea] = {0,0,1, 0,0,1, 1,1,1};
			standardBrushErr = CreateStandardBrush(size, brushPattern);
			break;
			}
		
		case DirectGdi::EDiamondCrossHatchBrush:
			{
			// The brush fills with forward diagonal and rearward diagonal hatching lines going from  
			// bottom left to top right plus lines going from top left to bottom right giving the effect  
			// of a grid of small diamonds
			// The brush fills with diagonal hatching lines going from bottom left to top right
			TSize size(diamondCrossHatchBrushSize, diamondCrossHatchBrushSize);
			VGbyte brushPattern[diamondCrossHatchBrushArea] = {0,0,1,0, 0,1,0,1, 1,0,0,0, 0,1,0,1};
			standardBrushErr = CreateStandardBrush(size, brushPattern);
			break;	
			}		
		}
	
	// Select the standard brush for all styles > EPatternedBrush
	if (aStyle > DirectGdi::EPatternedBrush)
		{
		if (standardBrushErr == KErrNone)
			{
			if (NonZeroBrushPatternOrigin())
				{
				// The brush origin is non-zero, update the non-zero origin brush 
				// with the standard brush and use that instead of the standard brush
				CopyCurrentBrushPatternForNonZeroOrigin();
				}
			else
				{
				// Use the standard brush region
				vgPaintPattern(iBrush, iBrushPatternStandardRegion);
				}
			}
		else
			{
			iDriver.SetError(standardBrushErr);
			}
		}
	else
		{
		vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_NORMAL);
		}
	}

/**
@see MDirectGdiEngine::SetBrushOrigin()
 */
void CVgEngine::SetBrushOrigin(const TPoint& aOrigin) 
	{
	iBrushOrigin = aOrigin;
	if (NonZeroBrushPatternOrigin())
		{
		// Copy the current brush pattern into iBrushPatternNonZeroOrigin, but shift it to
		// take into account the current non-zero brush origin.
		CopyCurrentBrushPatternForNonZeroOrigin();		
		}
	}


/**
@see MDirectGdiEngine::SetBrushPattern()
 */
TInt CVgEngine::SetBrushPattern(const CFbsBitmap& aPattern) 
	{
	if (aPattern.ExtendedBitmapType() != KNullUid)
		{
		return KErrNotSupported; // Not supported for extended bitmaps
		}
	
	// Destroy any previously set brush pattern
	MakeEngineCurrent();
	ResetBrushPattern();	
	iBrushPatternUser = CreateSourceVGImage(aPattern);
	if (iBrushPatternUser == VG_INVALID_HANDLE)
		{
		return KErrNoMemory;
		}

	iBrushPatternUserSize = aPattern.SizeInPixels();
	iBrushPatternUserBitmapHandle = aPattern.Handle();
	return KErrNone;
	}


/**
@see MDirectGdiEngine::ResetBrushPattern()
 */
void CVgEngine::ResetBrushPattern() 
	{
	MakeEngineCurrent();
	if (iBrushPatternUser != VG_INVALID_HANDLE)
		{
		vgDestroyImage(iBrushPatternUser);
		iBrushPatternUser = VG_INVALID_HANDLE;
		iBrushPatternUserBitmapHandle = KNullHandle;
		iBrushPatternUserSize = TSize(0,0);
		}	
	}

/**
@see MDirectGdiEngine::SetFont()
*/
void CVgEngine::SetFont(TUint32 aFontId) 
	{
	iFontId = aFontId;
	}


/**
@see MDirectGdiEngine::ResetFont()
 */
void CVgEngine::ResetFont() 
	{
	iFontId = 0;
	}

/**
Reset all the VgEngine-specific settings. Generic settings such as paint colour and pen colour
are set by calls from the CDirectGdiContext.
	
@see MDirectGdiEngine::Reset()

@post All VgEngine-specific settings have been reset to their default values.
 */
void CVgEngine::Reset() 
	{
	if (!MakeEngineCurrent())
		return;

	ResetVgMatrix();
	}



/**
@see MDirectGdiEngine::Clear(const TRect&)

@panic DGDIAdapter 62, if the brush for clearing is not valid (debug-only).
*/
void CVgEngine::Clear(const TRect& aRect) 
	{
	MakeEngineCurrent();

	if (255 == iBrushColor.Alpha())
		{
		const TPoint rectOrigin = ConvertToVgCoords(aRect);
		for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
			vgClear(rectOrigin.iX, rectOrigin.iY, aRect.Width(), aRect.Height());
		}
	else
		{
		// If blending is enabled, we cannot use vgClear as it ignores the current blendmode and performs a WriteAlpha.
		// Therefore a clear is done by a filled rectangle.
		
		// The Clear brush should always be a solid brush.
		GRAPHICS_ASSERT_DEBUG(vgGetParameteri(iClearBrush, VG_PAINT_TYPE) == VG_PAINT_TYPE_COLOR, EDirectGdiPanicClearBrushInvalid);
				
		if (PreparePath(iVgPath, 5))
			{
			// Before any vgu command, call SetError() as this stores the current OpenVG error state (if any) 
			// on the driver. Some implementations of vgu clears error state so we'd lose error codes otherwise.
			iDriver.SetError(KErrNone);
			
			VGUErrorCode err = vguRect(iVgPath, aRect.iTl.iX, aRect.iTl.iY, aRect.Width(), aRect.Height());
			if (err == VGU_NO_ERROR)
				{
				// Temporarily set the brush to the clear brush and fill the path.
				vgSetPaint(iClearBrush, VG_FILL_PATH);
				for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
					vgDrawPath(iVgPath, VG_FILL_PATH);
				vgSetPaint(iBrush, VG_FILL_PATH);	
				}
			else
				{
				SetVguError(err);
				}
			}
		}
	}

/**
@see MDirectGdiEngine::Clear()
 */
void CVgEngine::Clear() 
	{
	Clear(TRect(iSize));
	}

/**
@see MDirectGdiEngine::MoveTo()
 */
void CVgEngine::MoveTo(const TPoint& aPoint) 
	{
	iLinePos = aPoint;
	}

/**
@see MDirectGdiEngine::MoveBy()
 */
void CVgEngine::MoveBy(const TPoint& aVector) 
	{
	iLinePos += aVector;
	}

/**
@see MDirectGdiEngine::Plot()
 */
void CVgEngine::Plot(const TPoint& aPoint) 
	{
	MakeEngineCurrent();
	GRAPHICS_ASSERT_DEBUG(vgGeti(VG_STROKE_CAP_STYLE) == VG_CAP_ROUND,  EDirectGdiPanicPenEndCapStyleNotRound);

	
	// If the pen width and height are equal just plot as normal. 
	if (iPenSize.iWidth == iPenSize.iHeight)
		{
		if (PreparePath(iVgPath, 2))
			{
			// A point is plotted by drawing a line with start and end points the same.
			AppendPathCommand(VG_MOVE_TO_ABS, aPoint.iX + 0.5f, aPoint.iY + 0.5f);
			AppendPathCommand(VG_HLINE_TO_REL, 0.f);
			FinishPath(iVgPath);
			
			for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
				vgDrawPath(iVgPath, iPaintMode&VG_STROKE_PATH);
			}
		}
	else if ((iPenSize.iWidth != 0) && (iPenSize.iHeight != 0))
		{
		// Setting a pen with different width and height is not available on OpenVG, so we need to scale 
		// the coordinates we are drawing and apply a scaling matrix that scales by the width and height 
		// of the pen to get the effect of a pen width different width and height.
		if (PreparePath(iVgPath, 2))
			{
			TSize penSize = iPenSize;
			SetPenSize(TSize(1, 1));
			vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
			vgScale(penSize.iWidth, penSize.iHeight);
			
			// A point is plotted by drawing a line with start and end points the same.
			AppendPathCommand(VG_MOVE_TO_ABS, (aPoint.iX + 0.5f)/(float)penSize.iWidth, (aPoint.iY + 0.5f)/(float)penSize.iHeight);
			AppendPathCommand(VG_HLINE_TO_REL, 0.f);
			FinishPath(iVgPath);
			
			vgDrawPath(iVgPath, iPaintMode&VG_STROKE_PATH);
			
			ResetVgMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
			SetPenSize(penSize);
			}								
		}	
	}

/**
@see MDirectGdiEngine::DrawLine()
 */
void CVgEngine::DrawLine(const TPoint& aStart, const TPoint& aEnd)
	{
	MakeEngineCurrent();
	GRAPHICS_ASSERT_DEBUG(vgGeti(VG_STROKE_CAP_STYLE) == VG_CAP_ROUND, EDirectGdiPanicPenEndCapStyleNotRound);
	
    if (iPaintMode == 0)
		{
		return;
		}    
	
	if (PreparePath(iVgPath, 2))
		{
		// If the pen width and height are the same then draw as normal
		if (iPenSize.iWidth == iPenSize.iHeight)
			{
			// 0.5 is appended to all OpenVG drawing co-ordinates as when specifying them, the spec says
			// co-ordinates are relative to pixel boundaries, not pixel centres, so 0,0 is the top left of the
			// top pixel. We need to add 0.5 to specify the centre of pixels.
			
			AppendPathCommand(VG_MOVE_TO_ABS, aStart.iX + 0.5f, aStart.iY + 0.5f);		
			if (aStart.iX == aEnd.iX)
				{
				AppendPathCommand(VG_VLINE_TO_ABS, aEnd.iY + 0.5f);			
				}
			else if (aStart.iY == aEnd.iY)
				{
				AppendPathCommand(VG_HLINE_TO_ABS, aEnd.iX + 0.5f);			
				}
			else
				{
				AppendPathCommand(VG_LINE_TO_ABS, aEnd.iX + 0.5f, aEnd.iY + 0.5f);
				}		
			FinishPath(iVgPath);
			for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
				vgDrawPath(iVgPath, iPaintMode);			
			}
		else if ((iPenSize.iWidth != 0) && (iPenSize.iHeight != 0))
			{
			// Setting a pen with different width and height is not available on OpenVG, so we need to scale 
			// the coordinates we are drawing and apply a scaling matrix that scales by the width and height 
			// of the pen to get the effect of a pen width different width and height.
			TSize penSize = iPenSize;
			SetPenSize(TSize(1, 1));
			vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
			vgScale(penSize.iWidth, penSize.iHeight);
					
			VGfloat scaleX = 1.0f/(float)penSize.iWidth;
			VGfloat scaleY = 1.0f/(float)penSize.iHeight;
			AppendPathCommand(VG_MOVE_TO_ABS, (aStart.iX + 0.5f)*scaleX, (aStart.iY + 0.5f)*scaleY);		
			if (aStart.iX == aEnd.iX)
				{
				AppendPathCommand(VG_VLINE_TO_ABS, (aEnd.iY + 0.5f)*scaleY);			
				}
			else if (aStart.iY == aEnd.iY)
				{
				AppendPathCommand(VG_HLINE_TO_ABS, (aEnd.iX + 0.5f)*scaleX);			
				}
			else
				{
				AppendPathCommand(VG_LINE_TO_ABS, (aEnd.iX + 0.5f)*scaleX, (aEnd.iY + 0.5f)*scaleY);
				}		
			FinishPath(iVgPath);				
			vgDrawPath(iVgPath, iPaintMode);
			
			ResetVgMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
			SetPenSize(penSize);
			}			
		}	

	iLinePos = aEnd;
	}

/**
@see MDirectGdiEngine::DrawLineTo()
 */
void CVgEngine::DrawLineTo(const TPoint& aPoint)
	{
	DrawLine(iLinePos, aPoint);
	}

/**
@see MDirectGdiEngine::DrawLineBy()
 */
void CVgEngine::DrawLineBy(const TPoint& aVector)
	{
	DrawLine(iLinePos, iLinePos + aVector);
	}

/**
@see MDirectGdiEngine::DrawRect()
 */
void CVgEngine::DrawRect(const TRect& aRect)
	{
	MakeEngineCurrent();
	
	// Only draw if we are not painting with a NULL pen and a NULL brush
	if (iPaintMode == 0)
		return;
	
	// If the pen size is larger then 1, make sure we are using the ESolidPen
	// pen style (to match BitGdi behaviour)
	DirectGdi::TPenStyle savedPenStyle = iPenStyle;
	if (((iPenSize.iWidth > 1) || (iPenSize.iHeight > 1)) && (iPenStyle > DirectGdi::ESolidPen))
		SetPenStyle(DirectGdi::ESolidPen);
	
	// Create a copy of the rect. If the pen style is not null, we have to shrink the 
	// width and height of the rect by one pixel at the bottom left corner for conformance.	
	TRect copyRect(aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY-1);
	if (iPenStyle != DirectGdi::ENullPen)
		{
		--copyRect.iBr.iX;
		}
	else
		{
		--copyRect.iTl.iY;
		}

	const TBool symmetricalPenSize = iPenSize.iWidth == iPenSize.iHeight;
	
	// If the pen is so thick it covers the entire area of the rect, don't do the fill, as per BitGdi.
	const TBool penThickerThanRect = (iPenSize.iWidth >= copyRect.Width()) || (iPenSize.iHeight >= copyRect.Height());
	
	// If the pen width and height are the same then draw as normal. If they are different but we should be filling
	// this shape we need to draw the filled area only as normal (not the outline). The outline of the shape is drawn 
	// in the block of code below to allow the effect of a different width and height pen to be applied.
	if (!penThickerThanRect || (iPenStyle == DirectGdi::ENullPen))
		{
		if (symmetricalPenSize || (iPaintMode & VG_FILL_PATH))
			{
			if (PreparePath(iVgPath, 5))
				{
				vgSeti(VG_STROKE_JOIN_STYLE, VG_JOIN_MITER);
				AppendPathCommand(VG_MOVE_TO_ABS, copyRect.iTl.iX + 0.5f, copyRect.iTl.iY + 0.5f);
				AppendPathCommand(VG_HLINE_TO_ABS, copyRect.iBr.iX + 0.5f);
				AppendPathCommand(VG_VLINE_TO_ABS, copyRect.iBr.iY + 0.5f);
				AppendPathCommand(VG_HLINE_TO_ABS, copyRect.iTl.iX + 0.5f);
				AppendPathCommand(VG_CLOSE_PATH);
				FinishPath(iVgPath);
			
				for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
					vgDrawPath(iVgPath, symmetricalPenSize ? iPaintMode : VG_FILL_PATH);
				vgSeti(VG_STROKE_JOIN_STYLE, VG_JOIN_ROUND);
				}
			}
		}
	if((penThickerThanRect && (iPenStyle != DirectGdi::ENullPen)) ||  //we shouldn't draw if pen style is null
		(!symmetricalPenSize &&	(iPaintMode & VG_STROKE_PATH) && (iPenSize.iWidth != 0) && (iPenSize.iHeight != 0)))
		{
		// Setting a pen with different width and height is not available on OpenVG, so we need to scale 
		// the coordinates we are drawing and apply a scaling matrix that scales by the width and height 
		// of the pen to get the effect of a pen width different width and height.
		TSize penSize = iPenSize;
		SetPenSize(TSize(1, 1));
		vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
		vgScale(penSize.iWidth, penSize.iHeight);
			
		if (PreparePath(iVgPath, 5))
			{
			vgSeti(VG_STROKE_JOIN_STYLE, VG_JOIN_MITER);
			VGfloat scaleX = 1.0f/(float)penSize.iWidth;
			VGfloat scaleY = 1.0f/(float)penSize.iHeight;
			AppendPathCommand(VG_MOVE_TO_ABS, (copyRect.iTl.iX + 0.5f)*scaleX, (copyRect.iTl.iY + 0.5f)*scaleY);
			AppendPathCommand(VG_HLINE_TO_ABS, (copyRect.iBr.iX + 0.5f)*scaleX);
			AppendPathCommand(VG_VLINE_TO_ABS, (copyRect.iBr.iY + 0.5f)*scaleY);
			AppendPathCommand(VG_HLINE_TO_ABS, (copyRect.iTl.iX + 0.5f)*scaleX);
			AppendPathCommand(VG_CLOSE_PATH);
			FinishPath(iVgPath);
			for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
				vgDrawPath(iVgPath, VG_STROKE_PATH);
			vgSeti(VG_STROKE_JOIN_STYLE, VG_JOIN_ROUND);
			}
		
		ResetVgMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
		SetPenSize(penSize);
		}

	// Reset the pen style if we changed it above
	if (savedPenStyle != iPenStyle)
		SetPenStyle(savedPenStyle);
	}

/**
@see MDirectGdiEngine::DrawRoundRect()
 */
void CVgEngine::DrawRoundRect(const TRect& aRect, const TSize& aCornerSize) 
	{
	MakeEngineCurrent();
	
	// Only draw if we are not painting with a NULL pen and a NULL brush
	if (iPaintMode == 0)		
		return;
	
	// Create a copy of the rect. If the pen style is not null, we have to shrink the 
	// width and height of the rect by one pixel at the bottom left corner for conformance.	
	TRect copyRect(aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY-1);
	if (iPenStyle != DirectGdi::ENullPen)
		{
		--copyRect.iBr.iX;
		}
	//If the penstyle is null and brush style is not null, then reduce the width and height by
	//two pixels for conformation.
	else if(iBrushStyle != DirectGdi::ENullBrush)
		{
		----copyRect.iBr.iX;		
		--copyRect.iBr.iY;
		}
	else if(iPenStyle == DirectGdi::ENullPen)
		{
		--copyRect.iTl.iY;
		}
	
	// check that the corner size is less than the rectangle size
	if ((aRect.Width() > aCornerSize.iWidth) || (aRect.Height() > aCornerSize.iHeight))
		{									
		// Before any vgu command, call SetError() as this stores the current vg error state (if any) in the 
		// driver. Some implementations of vgu clears error state so we'd lose error codes otherwise.
		iDriver.SetError(KErrNone);
		
		// If the pen width and height are the same then draw as normal. If they are different but we should be filling
		// this shape we need to draw the filled area only as normal (not the outline). The outline of the shape is drawn 
		// in the block of code below to allow the effect of a different width and height pen to be applied.
		if ((iPenSize.iWidth == iPenSize.iHeight) || (iPaintMode & VG_FILL_PATH))
			{
			if (PreparePath(iVgPath, 10))
				{
				VGUErrorCode err = vguRoundRect(iVgPath,
											copyRect.iTl.iX + 0.5f, 
											copyRect.iTl.iY + 0.5f,
											copyRect.Width(), 
											copyRect.Height(),
											aCornerSize.iWidth * 2, 
											aCornerSize.iHeight * 2);	
				
				if (err == VGU_NO_ERROR)
					{
					for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
						vgDrawPath(iVgPath, (iPenSize.iWidth == iPenSize.iHeight) ? iPaintMode : VG_FILL_PATH);	
					}
				else
					{
					SetVguError(err);
					}
				}
			}
		
		if ((iPenSize.iWidth != iPenSize.iHeight)
		&& (iPaintMode & VG_STROKE_PATH)		
		&& (iPenSize.iWidth != 0) && (iPenSize.iHeight != 0))
			{
			// Setting a pen with different width and height is not available on OpenVG, so we need to scale 
			// the coordinates we are drawing and apply a scaling matrix that scales by the width and height 
			// of the pen to get the effect of a pen width different width and height.
			TSize penSize = iPenSize;
			SetPenSize(TSize(1, 1));
			vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
			vgScale(penSize.iWidth, penSize.iHeight);
			
			if (PreparePath(iVgPath, 10))
				{
				VGfloat scaleX = 1.0f/(float)penSize.iWidth;
				VGfloat scaleY = 1.0f/(float)penSize.iHeight;
				VGUErrorCode err = vguRoundRect(iVgPath,
							                (copyRect.iTl.iX + 0.5f) * scaleX, 
							                (copyRect.iTl.iY + 0.5f) * scaleY,
							                copyRect.Width() * scaleX, 
							                copyRect.Height() * scaleY,
							                (aCornerSize.iWidth * 2) * scaleX, 
							                (aCornerSize.iHeight * 2) * scaleY);	
				if (err == VGU_NO_ERROR)
					{
					vgDrawPath(iVgPath, VG_STROKE_PATH);		
					}
				else
					{
					SetVguError(err);
					}
				}
			
			ResetVgMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
			SetPenSize(penSize);
			}
		}
	else
		{
		// Draw an ellipse as the corner size is greater than or equal to the rectangle size
		DrawEllipse(copyRect);
		}
	}

/**
@see MDirectGdiEngine::DrawPolyLine()

@panic DGDIAdapter 27, if the passed point list has zero points (debug-only).
 */
void CVgEngine::DrawPolyLine(const TArray<TPoint>& aPointList)
	{
	DrawPolyLineNoEndPoint(aPointList);
	
	// Do a plot at the end point to improve the appearance. For larger pen-sizes, a plot
	// improves the correlation with BitGDI polylines, giving a subtly more rounded finish.
	if (DirectGdi::ESolidPen == iPenStyle)
		{
		Plot(iLinePos);
		}
	}

/**
@see MDirectGdiEngine::DrawPolyLineNoEndPoint()

@panic DGDIAdapter 27, if the passed point list has zero points (debug-only).
 */
void CVgEngine::DrawPolyLineNoEndPoint(const TArray<TPoint>& aPointList)
	{
	MakeEngineCurrent();

	GRAPHICS_ASSERT_DEBUG(aPointList.Count() > 0, EDirectGdiPanicInvalidPointArray);
	
	DoDrawPoly(aPointList, EFalse);
	}

/**
@see MDirectGdiEngine::DrawPolygon()

@panic DGDIAdapter 26, if the passed fill-rule is invalid (debug-only).
 */
void CVgEngine::DrawPolygon(const TArray<TPoint>& aPoints, DirectGdi::TFillRule aRule)
	{
	MakeEngineCurrent();
	
	GRAPHICS_ASSERT_DEBUG(aPoints.Count() > 0, EDirectGdiPanicInvalidPointArray);
	
	switch(aRule)
	    {
		case DirectGdi::EAlternate:
			vgSeti(VG_FILL_RULE, VG_EVEN_ODD); 
			break;
		case DirectGdi::EWinding: 
			vgSeti(VG_FILL_RULE, VG_NON_ZERO); 
			break;
		default: 
			GRAPHICS_PANIC_DEBUG(EDirectGdiPanicInvalidFillRule);
			return;
	    };
	
	DoDrawPoly(aPoints, ETrue);
	}

/**
Helper function to assist with drawing polygons with DrawPolygon()/DrawPolyLine(). It takes care of 
drawing the array of points given to it. It sets the internal drawing poisition to the last TPoint
in the array.

@see	DrawPolyLine()
@see 	DrawPolygon()

@param	aPoints	Array of points specifying the vertices of the polygon. There must be at least one 
		vertex.
@param  aClosed	If ETrue, the start and end points are joined, and the polygon filled using current 
        brush settings, otherwise just a polyline is drawn.
*/
void CVgEngine::DoDrawPoly(const TArray<TPoint>& aPoints, TBool aClosed)
	{
	GRAPHICS_ASSERT_DEBUG(aPoints.Count() > 0, EDirectGdiPanicInvalidPointArray);	
	const TInt numPoints = aPoints.Count();	
	
	// Set drawing position to last point in the array (regardless of whether the poly is open/closed)
	iLinePos = aPoints[numPoints - 1];
	
	// Set the paint mode depending on whether we are drawing a line (not closed) or a poly (closed)
	VGbitfield paintMode = iPaintMode;
	if (!aClosed)
		{
		paintMode &= ~VG_FILL_PATH;
		}
	
	if (!paintMode)
		{
		return;
		}

	// If the pen width and height are the same then draw as normal. If they are different but we should be filling
	// this shape we need to draw the filled area only as normal (not the outline). The outline of the shape is drawn 
	// in the block of code below to allow the effect of a different width and height pen to be applied.
	if ((iPenSize.iWidth == iPenSize.iHeight) || (paintMode & VG_FILL_PATH))
		{
		if (PreparePath(iVgPath, aClosed ? numPoints + 1 : numPoints))
			{
			AppendPathCommand(VG_MOVE_TO_ABS, aPoints[0].iX + 0.5f, aPoints[0].iY + 0.5f);
			for (TInt point = 0; point < numPoints; ++point)
				{
				AppendPathCommand(VG_LINE_TO_ABS, aPoints[point].iX + 0.5f, aPoints[point].iY + 0.5f);	
				}
			if (aClosed)
				{
				AppendPathCommand(VG_CLOSE_PATH);
				}
			FinishPath(iVgPath);
			for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
				vgDrawPath(iVgPath, (iPenSize.iWidth == iPenSize.iHeight) ? paintMode : VG_FILL_PATH);
			}
		}
	
	if ((iPenSize.iWidth != iPenSize.iHeight)
		&& (paintMode & VG_STROKE_PATH)		
		&& (iPenSize.iWidth != 0) && (iPenSize.iHeight != 0))
		{
		// Setting a pen with different width and height is not available on OpenVG, so we need to scale 
		// the coordinates we are drawing and apply a scaling matrix that scales by the width and height 
		// of the pen to get the effect of a pen width different width and height.
		TSize penSize = iPenSize;
		SetPenSize(TSize(1, 1));
		vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
		vgScale(penSize.iWidth, penSize.iHeight);
		
		if (PreparePath(iVgPath, aClosed ? numPoints + 1 : numPoints))
			{
			AppendPathCommand(VG_MOVE_TO_ABS, (aPoints[0].iX + 0.5f)/(float)penSize.iWidth, (aPoints[0].iY + 0.5f)/(float)penSize.iHeight);
			for (TInt point = 0; point < numPoints; ++point)
				{
				AppendPathCommand(VG_LINE_TO_ABS, (aPoints[point].iX + 0.5f)/(float)penSize.iWidth, (aPoints[point].iY + 0.5f)/(float)penSize.iHeight);	
				}				
			if (aClosed)
				{
				AppendPathCommand(VG_CLOSE_PATH);
				}			
			FinishPath(iVgPath);		
			vgDrawPath(iVgPath, VG_STROKE_PATH);
			}
		
		ResetVgMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
		SetPenSize(penSize);
		}
			
	}

/**
@see MDirectGdiEngine::DrawArc()
@see DrawPie()
@see DrawArc(const TRect&, const TPoint&, const TPoint&, VGUArcType)
 */
void CVgEngine::DrawArc(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd) 
	{	
	DoDrawArc(aRect, aStart, aEnd, VGU_ARC_OPEN);
	}

/**
@see MDirectGdiEngine::DrawPie()
@see DrawArc(const TRect&, const TPoint&, const TPoint&)
@see DrawArc(const TRect&, const TPoint&, const TPoint&, VGUArcType)
 */
void CVgEngine::DrawPie(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd) 
	{	
	DoDrawArc(aRect, aStart, aEnd, VGU_ARC_PIE);
	}

/**
@see MDirectGdiEngine::DrawEllipse()
 */
void CVgEngine::DrawEllipse(const TRect& aRect) 
	{
	// Null brush and pen, draw nothing.
	if (iPaintMode == 0)
		return;
	
	MakeEngineCurrent();	
	VGfloat x = (aRect.iTl.iX + aRect.iBr.iX) * 0.5;
	VGfloat y = (aRect.iTl.iY + aRect.iBr.iY) * 0.5;
	
	// Before any vgu command, call SetError() as this stores the current vg error state (if any) in the 
	// driver. Some implementations of vgu clears error state so we'd lose error codes otherwise.
	iDriver.SetError(KErrNone);
	
	TInt width = aRect.Width();
	TInt height = aRect.Height();
	
	//If the penstyle is null and brush style is not null, then reduce the width and height by
	//two pixels for conformation.
	if(iPenStyle == DirectGdi::ENullPen && iBrushStyle != DirectGdi::ENullBrush)
		{
		width = aRect.Width() > 2 ? aRect.Width() - 2 : 1;
		height = aRect.Height() > 2 ? aRect.Height() - 2 : 1;
		}
	
	// If the pen width and height are the same then draw as normal. If they are different but we should be filling
	// this shape we need to draw the filled area only as normal (not the outline). The outline of the shape is drawn 
	// in the block of code below to allow the effect of a different width and height pen to be applied.
	if ((iPenSize.iWidth == iPenSize.iHeight) || (iPaintMode & VG_FILL_PATH))
		{
		if (PreparePath(iVgPath, 4))
			{
			VGUErrorCode err = vguEllipse(iVgPath, x, y, width, height);
			if (err == VGU_NO_ERROR)
				{
				for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
					vgDrawPath(iVgPath, (iPenSize.iWidth == iPenSize.iHeight) ? iPaintMode : VG_FILL_PATH);
				}
			else
				{
				SetVguError(err);
				}
			}
		}
	
	if ((iPenSize.iWidth != iPenSize.iHeight)
		&& (iPaintMode & VG_STROKE_PATH)		
		&& (iPenSize.iWidth != 0) && (iPenSize.iHeight != 0))
		{
		// Setting a pen with different width and height is not available on OpenVG, so we need to scale 
		// the coordinates we are drawing and apply a scaling matrix that scales by the width and height 
		// of the pen to get the effect of a pen width different width and height.
		TSize penSize = iPenSize;
		SetPenSize(TSize(1, 1));
		vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
		vgScale(penSize.iWidth, penSize.iHeight);
		
		VGfloat scaleX = 1.0f/(float)penSize.iWidth;
		VGfloat scaleY = 1.0f/(float)penSize.iHeight;
		
		if (PreparePath(iVgPath, 4))
			{
			VGUErrorCode err = vguEllipse(iVgPath, x*scaleX, y*scaleY, (float)width*scaleX, (float)height*scaleY);			
			if (err == VGU_NO_ERROR)
				{
				vgDrawPath(iVgPath, VG_STROKE_PATH);
				}
			else
				{
				SetVguError(err);
				}
			}
		
		ResetVgMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
		SetPenSize(penSize);			
		}	
	}

/**
Given a TDisplayMode, returns the closest TDisplayMode that is pixel-for-pixel-compatible
with an OpenVG format, such that the given TDisplayMode may be converted into the result
without loss of colour information.

@param 	aDisplayMode Pixel format to find a match for.

@return Closest TDisplayMode for which there is a OpenVG-compatible match.
 */
static TDisplayMode ClosestVgCompatibleDisplayMode(TDisplayMode aDisplayMode)
	{
	switch (aDisplayMode)
		{
		case EGray2:
		case EGray4:
		case EGray16:
			return EGray256;

		case EColor16:
		case EColor256:
		case EColor4K:
			return EColor64K;
		
		case EColor16M:
			return EColor16MU;
			
		default:
			return aDisplayMode;
		}
	}

/**
Converts a CFbsBitmap into a VGImage.
If the CFbsBitmap is not a volatile bitmap, the VGImage created will be stored in the thread-wide 
VGImage cache. If the CFbsBitmap has been touched (i.e. its data has been changed since it was last 
used), a new VGImage will be created and will replace that currently stored in the cache. An untouched 
bitmap will store the created VGImage in the cache upon first use, and on subsequent use (if it is 
still untouched), will just retrieve the VGImage stored in the cache.

@param aBitmap The CFbsBitmap to create a VGImage from.
@param aImage Returns the VGImage created from the CFbsBitmap.
@param aIsMask True if the CFbsBitmap is to be used as a mask.
@param aOrigin Position of the first pixel in the mask bitmap.

@return ETrue if the VGimage has been stored in the cache, EFalse if not.
*/
TBool CVgEngine::ConvertBitmapToVgImage(const CFbsBitmap& aBitmap, VGImage& aImage, TBool aIsMask, const TPoint& aOrigin)
	{
	TBool createImage =  EFalse;
	TBool storeImageInCache = EFalse;
	TBool imageCached = EFalse;
	// Set the options createImage, storeImageInCache and imageCached depending on
	// whether the bitmap is volatile, has been touched since last used,
	// and whether it already exists in the cache
	if (aBitmap.IsVolatile()) 
		{
		// Source bitmap is volatile and so should not be stored in cache.
		// Therefore create image only.
		createImage = ETrue;
		}
	else //bitmap not volatile
		{
		// Source bitmap has not changed since last used.
		// Retrieve from cache the image created from the bitmap and the touchCount of that image.
		aImage = iDriver.GetVgImageFromCache(aBitmap, aOrigin);
		// If the source bitmap already has an associated VGImage stored in the cache,
		// just use that VGImage.  Otherwise, need to create a VGImage and add it to the cache.
		if (aImage == VG_INVALID_HANDLE)
			{
			// VGImage not in cache
			// Create image, and store in cache
			createImage = ETrue;
			storeImageInCache = ETrue;
			}
		else
			{
			// Image already in cache
			imageCached = ETrue;
			}
		}

	// Create a new VGImage if needed
	if (createImage)
		{
		aImage = CreateSourceVGImage(aBitmap, aIsMask, aOrigin);
		// Error set on creation of VGImage if aImage == VG_INVALID_HANDLE.
		}
	// Store the VGImage in the cache if appropriate
	if (storeImageInCache && aImage != VG_INVALID_HANDLE)
		{
		imageCached = iDriver.AddVgImageToCache(aBitmap, aImage, aOrigin);
		}
	return imageCached;
	}


/**
Transforms coordinates for a TRect from Symbian Graphics to OpenVG surface coordinates.
This is required for OpenVG operations which are not subject to user-to-surface 
coordinate system transformations.

OpenVG coordinates locate the BOTTOM LEFT corner of the object relative to an origin
at the BOTTOM LEFT of the rendering area.

Symbian Graphics coordinates locate the TOP LEFT corner of the object relative to an 
origin located at the TOP LEFT of the rendering area.

@param	aCoord		Top-left of the rectangle, in Symbian graphics coordinates.
@param  aWidth		The width of the desired rectangle.
@param  aHeight		The height of the desired rectangle.
@return A TRect in OpenVG coordinates which describes a rectangle at aCoord with aWidth and aHeight.
*/
TRect CVgEngine::SgMetricsToVgTRect(const TPoint& aCoord, const TInt aWidth, const TInt aHeight) const
	{
	return TRect (TPoint (aCoord.iX + iOrigin.iX, iSize.iHeight - (aCoord.iY + aHeight) - iOrigin.iY), TSize (aWidth, aHeight));
	}

/** 
@see MDirectGdiEngine::BitBlt()
@see BitBltMasked(const TPoint&, const CFbsBitmap&, const TRect&, const CFbsBitmap&, TBool)
@see BitBltMasked(const TPoint&, const CFbsBitmap&, const TRect&, const CFbsBitmap&, const TPoint&)
 */
void CVgEngine::BitBlt(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect)
	{
	if (aSourceBitmap.ExtendedBitmapType() != KNullUid)
		{
		iDriver.SetError(KErrNotSupported); // Not supported for extended bitmaps
		return;
		}
	
	DoVgImageDraw (TRect (aDestPos, aSourceRect.Size()), aSourceBitmap, aSourceRect);
	}

/** 
@see MDirectGdiEngine::DrawBitmap()
@see DrawBitmapMasked()
 */
void CVgEngine::DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect) 
	{
	if (aSourceBitmap.ExtendedBitmapType() != KNullUid)
		{
		iDriver.SetError(KErrNotSupported); // Not supported for extended bitmaps
		return;
		}
	
	DoVgImageDraw (aDestRect, aSourceBitmap, aSourceRect);
	}


/**
Helper method to perform basic VgDrawImage operations, explictly optimised for the case where
the extents of the source image equal the specified source region.

@pre aSource image is a valid VG image handle.
@pre Destination position and/or scaling has already been set in OpenVG. 

@param aDestRect	 	Destination rectangle to draw to.
@param aSourceBitmap	Source bitmap to draw.
@param aSourceRect 		Source rectangle to render. 
*/
void CVgEngine::DoVgImageDraw (const TRect& aDestRect, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect)
	{
	MakeEngineCurrent();
	TRect destRect(aDestRect);
	TRect srcRect(aSourceRect);
	if (!IntersectsClippingRegion (TRect(iOrigin, destRect.Size()))) 
		return;
	
	VGImage sourceImage = VG_INVALID_HANDLE;
	TBool imageCached = ConvertBitmapToVgImage(aSourceBitmap, sourceImage);
	// Error set on creation of VGImage.
	if (sourceImage == VG_INVALID_HANDLE) return;

	vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
	vgTranslate(destRect.iTl.iX, destRect.iTl.iY);
	
	if (aDestRect.Size() != aSourceRect.Size())
		vgScale((VGfloat)destRect.Width()/aSourceRect.Width(), (VGfloat)destRect.Height()/aSourceRect.Height());
	
	if(aSourceBitmap.SizeInPixels() == aSourceRect.Size())
		{
		for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
			vgDrawImage(sourceImage);
		}
	else
		{
		VGImage sourceImageRegion = 
			vgChildImage(
				sourceImage, 
				srcRect.iTl.iX, 
				srcRect.iTl.iY, 
				srcRect.Width(), 
				srcRect.Height());
		
		if (sourceImageRegion != VG_INVALID_HANDLE)
			{
			for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
				vgDrawImage(sourceImageRegion);

			vgDestroyImage(sourceImageRegion);
			}
		}
		
	if (!imageCached) vgDestroyImage (sourceImage);
	ResetVgMatrix(VG_MATRIX_IMAGE_USER_TO_SURFACE);	// Reset the transform matrix.		
	}

/** 
@see MDirectGdiEngine::BitBltMasked()
@see BitBltMasked(const TPoint&, const CFbsBitmap&, const TRect&, const CFbsBitmap&, TBool)
@see BitBlt()
 */
void CVgEngine::BitBltMasked(
		const TPoint& aDestPos,
		const CFbsBitmap& aBitmap,
		const TRect& aSourceRect,
		const CFbsBitmap& aMask,
		const TPoint& aMaskPt)
	{
	if ((aBitmap.ExtendedBitmapType() != KNullUid) || (aMask.ExtendedBitmapType() != KNullUid))
		{
		iDriver.SetError(KErrNotSupported); // Not supported for extended bitmaps
		return;
		}
	
	DoBitBltMasked(aDestPos, aBitmap, aSourceRect, aMask, EFalse, aMaskPt);
	}

/** 
@see MDirectGdiEngine::BitBlt()
@see BitBltMasked(const TPoint&, const CFbsBitmap&, const TRect&, const CFbsBitmap&, const TPoint&)
@see BitBlt()
 */
void CVgEngine::BitBltMasked( 
		const TPoint& aDestPos,
		const CFbsBitmap& aSourceBitmap,
		const TRect& aSourceRect,
		const CFbsBitmap& aMaskBitmap, 
		TBool aInvertMask)
	{
	if ((aSourceBitmap.ExtendedBitmapType() != KNullUid) || (aMaskBitmap.ExtendedBitmapType() != KNullUid))
		{
		iDriver.SetError(KErrNotSupported); // Not supported for extended bitmaps
		return;
		}
	
	DoBitBltMasked(aDestPos, aSourceBitmap, aSourceRect, aMaskBitmap, aInvertMask, TPoint(0, 0));
	}

/**
Helper method for performing BitBltMasked().
Note that aInvertMask is ignored if aMaskPos is not at 0,0.

@see	CDirectGdiContext::BitBlt(const TPoint& aPoint, const CFbsBitmap& aBitmap, const TRect& aSourceRect);
@see	CDirectGdiContext::BitBltMasked(const TPoint&, const CFbsBitmap&,const TRect&, const CFbsBitmap&, const TPoint&);

@param	aDestPos		The destination for the top left corner of the transferred bitmap. 
						It is relative to the top left corner of the destination bitmap, which may be the screen. 
@param	aSourceBitmap	A memory-resident source bitmap.
@param	aSourceRect		A rectangle defining the piece of the bitmap to be drawn, 
						with co-ordinates relative to the top left corner of the bitmap. 
@param	aMaskBitmap		Mask bitmap.
@param	aInvertMask		If EFalse, a source pixel that is masked by a black pixel is not transferred to 
						the destination rectangle. If ETrue, then a source pixel that is masked by a 
						white pixel is not transferred to the destination rectangle. If alpha blending
						is used instead of masking, this flag is ignored and no inversion takes place.
						Note that this parameter is ignored if aMaskPos does not equal TPoint(0,0).
@param	aMaskPos		The point on the mask bitmap to use as the top left corner 
*/
void CVgEngine::DoBitBltMasked (
		const TPoint& aDestPos,
		const CFbsBitmap& aSourceBitmap,
		const TRect& aSourceRect,
		const CFbsBitmap& aMaskBitmap,
		TBool aInvertMask,
		const TPoint& aMaskPos)
	{
	MakeEngineCurrent();
	ResetVgMatrix(VG_MATRIX_IMAGE_USER_TO_SURFACE);
	if (!IntersectsClippingRegion (TRect (aDestPos+iOrigin, aSourceRect.Size()))) 
		return;
	
	VGImage sourceImage = VG_INVALID_HANDLE;
	TBool imageCached = ConvertBitmapToVgImage(aSourceBitmap, sourceImage);
	if (sourceImage == VG_INVALID_HANDLE)
		{
		// Error set on creation of VGImage.
		return;
		}
	
	VGImage maskImage = VG_INVALID_HANDLE;
	TBool maskImageCached = ConvertBitmapToVgImage(aMaskBitmap, maskImage, ETrue, aMaskPos);
	if (maskImage == VG_INVALID_HANDLE)
		{
		// Error set on creation of VGImage.
		if (!imageCached)
			{
			vgDestroyImage(sourceImage);
			}
		return;
		}

	DoVgMaskedImageDraw(
		aDestPos,
		sourceImage,
		aSourceBitmap.SizeInPixels(),
		aSourceRect,
		maskImage,
		aMaskBitmap.SizeInPixels(),
		aSourceRect.Size(),
		aInvertMask);

	if (!maskImageCached)
		{
		vgDestroyImage(maskImage);
		}
	if (!imageCached)
		{
		vgDestroyImage(sourceImage);
		}
	}


/** 
This implementation stretches the mask first, and then performs mask tiling. Another approach is to
tile first and then perform stretching. The latter method requires more memory and stretches
once. Results between these methods are different. When stretching first, all tiles will be completely
uniform. When stretching last, different tiles are affected differently, based on the tile's position
and stretch factor.

@see MDirectGdiEngine::DrawBitmapMasked()
@see DrawBitmap()
 */
void CVgEngine::DrawBitmapMasked(
	const TRect& aDestRect,
	const CFbsBitmap& aSourceBitmap,
	const TRect& aSourceRect,
	const CFbsBitmap& aMaskBitmap,
	TBool aInvertMask)
	{
	if ((aSourceBitmap.ExtendedBitmapType() != KNullUid) || (aMaskBitmap.ExtendedBitmapType() != KNullUid))
		{
		iDriver.SetError(KErrNotSupported); // Not supported for extended bitmaps
		return;
		}
	
	MakeEngineCurrent();
	TRect destRect(aDestRect);
	if (!IntersectsClippingRegion (TRect(iOrigin, destRect.Size())))
		{
		return;
		}

	// Create source image
	VGImage sourceImage = VG_INVALID_HANDLE;
	TBool imageCached = ConvertBitmapToVgImage(aSourceBitmap, sourceImage);
	// Return if VGImage failed to be created (error set on creation of VGImage).
	if (sourceImage == VG_INVALID_HANDLE)
		{
		return;
		}
		
	// Convert aMask to a VGImage.
	VGImage maskImage = VG_INVALID_HANDLE;
	TBool maskImageCached = ConvertBitmapToVgImage(aMaskBitmap, maskImage, ETrue);
	// Error set on creation of VGImage if mask == VG_INVALID_HANDLE

	if (maskImage != VG_INVALID_HANDLE)
		{
		TSize destSize = destRect.Size();
		TSize sourceSize = aSourceRect.Size();
		if ((destSize.iWidth == sourceSize.iWidth) && (destSize.iHeight == sourceSize.iHeight))
			{
			// No scaling of masked bitmap involved
			DoVgMaskedImageDraw(
					destRect.iTl,
					sourceImage,
					aSourceBitmap.SizeInPixels(),
					aSourceRect,
					maskImage,
					aMaskBitmap.SizeInPixels(),
					destSize,
					aInvertMask);
			}
		else
			{
			// Unfortunately, the current implementation of VG does not support
			// mask scaling. So, we render the mask into a VGImage pbuffer surface,
			// and apply user to surface scaling to get a stretch. The stretched
			// mask is then used for rendering.

			// Generate the VGImage to act as a pbuffer surface and receive the stretched mask.
			const TSize maskSizeInPixels = aMaskBitmap.SizeInPixels();
			const VGImageFormat vgFormat = MapToVgDisplayMode(ClosestVgCompatibleDisplayMode(aMaskBitmap.DisplayMode()));
			TInt scaledMaskWidth = Scale(maskSizeInPixels.iWidth,destSize.iWidth,sourceSize.iWidth);
			TInt scaledMaskHeight = Scale(maskSizeInPixels.iHeight,destSize.iHeight,sourceSize.iHeight);
			VGImage stretchedMask = DoVgCreateImage(vgFormat,
											scaledMaskWidth,
											scaledMaskHeight,
											VG_IMAGE_QUALITY_NONANTIALIASED);
	 
			if (stretchedMask != VG_INVALID_HANDLE)
				{
				// Get a configuration handle that is compatible with the mask pixel format.
				EGLConfig utilConfig = 0;
				TInt err = TConfigHelper::GetConfigForFbsBitmap (aMaskBitmap, utilConfig);
				if (err == KErrNone)
					{
					TBool eglSuccess = EFalse;
					EGLDisplay eglDisplay = iDriver.EglDisplay();
					EGLSurface lastSurface = eglGetCurrentSurface(EGL_DRAW);

					// Create a Pbuffer surface from the stretched mask VGImage.
					EGLSurface utilSurface = eglCreatePbufferFromClientBuffer(
						eglDisplay, 
						EGL_OPENVG_IMAGE,
						static_cast<EGLClientBuffer>(stretchedMask), 
						utilConfig,
						NULL);
					
					if (utilSurface != EGL_NO_SURFACE)
						{
						EGLContext lastContext = eglGetCurrentContext();
						// Create config. compatible context.
						EGLContext utilContext = eglCreateContext(eglDisplay, utilConfig, lastContext, NULL);	
					
						if (utilContext != EGL_NO_CONTEXT)
							{
							// Make the utility surface and context current, and stretch the mask.
							if (eglMakeCurrent(eglDisplay, utilSurface, utilSurface, utilContext) == EGL_TRUE)
								{
								// Set up the scaling transform for the current surface.
								vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
								// Scaling factors for x and y.
								VGfloat xScale = ((VGfloat)destSize.iWidth/sourceSize.iWidth);
								VGfloat yScale = ((VGfloat)destSize.iHeight/sourceSize.iHeight);
								vgScale(xScale, yScale);
								
								// Render the stretched mask.
								vgDrawImage(maskImage);
								ResetVgMatrix(VG_MATRIX_IMAGE_USER_TO_SURFACE);
								
								// All done, make current the pre-existing rendering state.
								eglMakeCurrent(eglDisplay, lastSurface, lastSurface, lastContext);
								
								eglSuccess = ETrue;
								}
							eglDestroyContext(eglDisplay, utilContext);
							}
						eglDestroySurface(eglDisplay, utilSurface);
						}	
					
					if (eglSuccess)
						{
						DoVgMaskedImageDraw(destRect.iTl, sourceImage, aSourceBitmap.SizeInPixels(),
											aSourceRect, stretchedMask,	maskSizeInPixels, destSize,	aInvertMask);
						}
					else
						{						
						// coverity[check_return]
						// coverity[unchecked_value]
						LogEglError();
						}
					}				
				else
					{
					iDriver.SetError(err);
					}
				vgDestroyImage (stretchedMask);
				}
			if (!maskImageCached) vgDestroyImage (maskImage);
			}
		}
	if (!imageCached) vgDestroyImage(sourceImage);
	}

/** 
Helper method that implements the core blitting functionality.

@param	aDestPos	The destination for the top left corner of the transferred bitmap. 
					It is relative to the top left corner of the destination bitmap, which may be the screen. 
@param	aSourceImage A valid VGImage to draw.
@param  aSourceImageSize Extents of source bitmap.
@param	aSourceRect	A rectangle defining the piece of the bitmap to be drawn, 
					with co-ordinates relative to the top left corner of the bitmap. 
@param	aScaledMaskImage A valid mask VGImage - pre-scaled, ready for rendering.
@param	aSrcMaskSize The size of the (unscaled) mask image.
@param	aXscale		Scaling factor to apply to x axis (already applied to mask).
@param	aYscale		Scaling factor to apply to y axis (already applied to mask).
@param	aDestSize	The size of the destination (used for calculating any scaling) 
@param	aInvertMask	If EFalse, a source pixel that is masked by a black pixel 
					is not transferred to the destination rectangle. If ETrue, then a source 
					pixel that is masked by a white pixel is not transferred to the destination 
					rectangle. 

@pre	The rendering target has been activated. aBitmap and aMaskBitmap are not NULL and hold Handles. 
		Destination rectangle extents must intersect clipping region.
		aSourceImage is a valid VGImage handle.
@post	Request to draw the masked bitmap content has been accepted. 
		There is no guarantee that the request has been processed when the method returns.
 */
void CVgEngine::DoVgMaskedImageDraw(
		const TPoint& aDestPos,
		VGImage aSourceImage,
		const TRect& aSourceImageSize,
		const TRect& aSourceRect,
		const VGImage aScaledMaskImage,
		const TSize& aSrcMaskSize,
		const TSize& aDestSize,
		TBool aInvertMask) 
	{
	TBool destroySourceImageAtEnd = EFalse;

	if(aSourceImageSize != aSourceRect)
		{
		aSourceImage = 
			vgChildImage(
				aSourceImage, 
				aSourceRect.iTl.iX, 
				aSourceRect.iTl.iY, 
				aSourceRect.Width(), 
				aSourceRect.Height());

		if (aSourceImage == VG_INVALID_HANDLE) return;
		destroySourceImageAtEnd = ETrue;
		}

	TBool maskOK = EFalse;
	TSize sourceSize = aSourceRect.Size();
	TRect destRect(aDestPos, TSize(Scale(aSourceRect.Width(),aDestSize.iWidth,sourceSize.iWidth),
										Scale(aSourceRect.Height(),aDestSize.iHeight,sourceSize.iHeight)));
	// VG does not provide mask tiling...we currently perform multiple
	// vgMask operations to implement tiling. It should be possible to use
	// pattern tiling to render the mask to a surface, convert surface region
	// to VGImage and apply that as a mask (to take advantage of native VG
	// tiling), though cost/benefit is has not yet been determined.
	// NOTE: It may be worth optimising for cases where xScale or yScale equal one.

	TRect destVgRect = SgMetricsToVgTRect(aDestPos, destRect.Width(), destRect.Height());

	if (aScaledMaskImage != VG_INVALID_HANDLE)
		{
		const TSize scaledMaskSize(Scale(aSrcMaskSize.iWidth,aDestSize.iWidth,sourceSize.iWidth),
										Scale(aSrcMaskSize.iHeight,aDestSize.iHeight,sourceSize.iHeight));
		if (scaledMaskSize.iHeight > 0 && scaledMaskSize.iWidth > 0)
			{
			maskOK = ETrue;
			// Determine mask image offset for rendering.
			TInt scaledMaskXOffset = Scale(aSourceRect.iTl.iX%aSrcMaskSize.iWidth,aDestSize.iWidth,sourceSize.iWidth);

			// Sg coordinates are relative to top left, Vg are reletive to bottom left. As we
			// tile from the bottom up we subtract from maskSize.iHeight to get the bottom edge
			// offset.
			TInt scaledMaskYOffset = (aSourceRect.iTl.iY + aSourceRect.Height()) % aSrcMaskSize.iHeight;
			if (scaledMaskYOffset != 0)
				{
				scaledMaskYOffset = Scale(aSrcMaskSize.iHeight-scaledMaskYOffset,aDestSize.iHeight,sourceSize.iHeight);
				}

			// If inverting the mask, we use a difference operation against the existing mask, so 
			// we need to ensure the existing mask is set to the correct state.
			// Fill the existing mask so that it is completly transparent (set to all ones).
			if(aInvertMask)
				{
				vgMask(
					VG_INVALID_HANDLE, 
					VG_FILL_MASK, 
					destVgRect.iTl.iX, 
					destVgRect.iTl.iY, 
					destVgRect.Width(), 
					destVgRect.Height());			
				}
		
			VGMaskOperation vgMaskOp = aInvertMask ? VG_SUBTRACT_MASK : VG_SET_MASK;
			// NOTE: in VG destVgRect.iTl is physically at the bottom and destVgRect.iBr at the top
			for (
				TInt maskY = destVgRect.iTl.iY - scaledMaskYOffset; 
				maskY < destVgRect.iBr.iY; 
				maskY += scaledMaskSize.iHeight)
				{
				for (
					TInt maskX = destVgRect.iTl.iX - scaledMaskXOffset;
					maskX < destVgRect.iBr.iX;
					maskX += scaledMaskSize.iWidth)
					{
					vgMask(
						aScaledMaskImage, 
						vgMaskOp, 
						maskX,
						maskY, 
						scaledMaskSize.iWidth,
						scaledMaskSize.iHeight);
					}
				}
			}
		}		
	
	// Set up translation and scale for the current surface - note that translation must
	// occur first, as is unscaled.
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
	vgTranslate(aDestPos.iX, aDestPos.iY);
	VGfloat xScale = ((VGfloat)aDestSize.iWidth/sourceSize.iWidth);
	VGfloat yScale = ((VGfloat)aDestSize.iHeight/sourceSize.iHeight);
	vgScale(xScale, yScale);
	
	//  Rather than bracketing vgDrawImage with VG_MASKING on/off we may want to always enable masking,
	//  and remove the mask when finished:
	//  vgMask(VG_INVALID_HANDLE, VG_CLEAR_MASK, 0, 0, iRenderingTarget->Size().iWidth, iRenderingTarget->Size().iHeight);
	//  If the mask is not removed in some way, then subsequent rendering operations which intersect with the
	//  masking region will be affected.
	if (maskOK) 
		{
		vgSeti (VG_MASKING, VG_TRUE);
		for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
			vgDrawImage(aSourceImage);
		vgSeti (VG_MASKING, VG_FALSE);
		}
	else
		{
		for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
			vgDrawImage(aSourceImage);
		}
	// Reset the transform matrix.
	ResetVgMatrix(VG_MATRIX_IMAGE_USER_TO_SURFACE);
	
	if (destroySourceImageAtEnd)
		{
		vgDestroyImage (aSourceImage);
		}
	}

/** 
@see MDirectGdiEngine::DrawResource(const TPoint&, const RDirectGdiDrawableSource&, DirectGdi::TGraphicsRotation)
@see DrawResource(const TRect&, const RDirectGdiDrawableSource&, const TRect&, DirectGdi::TGraphicsRotation)
@see DrawResource(const TRect&, const RDirectGdiDrawableSource&, const TDesC8&)
 */
void CVgEngine::DrawResource(const TPoint& aPos, const RDirectGdiDrawableSource& aSource, DirectGdi::TGraphicsRotation aRotation)
	{
	MakeEngineCurrent();
	
	CDirectGdiImageSourceImpl* source = iDriver.GetImageSourceFromHandle(aSource.Handle());
	if (source)
		{			
		const TSize sourceSize = source->Size();				
		
		if ((sourceSize.iWidth > 0) && (sourceSize.iHeight > 0))
			{
			VGImage vgImage = source->VgImage();
			if (vgImage != VG_INVALID_HANDLE)
				{
				vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
				
				TPoint pos(aPos);
				
			
				if (aRotation == DirectGdi::EGraphicsRotationNone ||
					aRotation == DirectGdi::EGraphicsRotation180)
					{
					// Pixel-data in EGLImages appears to be upside down due to the Y-inversion 
					// effect of the Identity matrix.  Therefore must undo the Y-inversion here 
					// and adjust destination rect accordingly.
					pos.iY = iSize.iHeight - aPos.iY - sourceSize.iHeight - iOrigin.iY;
					pos.iX += iOrigin.iX;
					vgLoadIdentity();
					}
				else
					{
					// But if rotation is 90 or 270 degrees we need to mirror in the X-axis
					// and adjust destination translation accordingly.
					vgScale(-1, 1);
					}
							
				switch (aRotation)
					{				
					case DirectGdi::EGraphicsRotation90:
						vgTranslate(-pos.iX, pos.iY);
						vgRotate(90.0f);
						break;
					case DirectGdi::EGraphicsRotation180:
						vgTranslate(pos.iX+sourceSize.iWidth, pos.iY+sourceSize.iHeight);
						vgRotate(180.0f);				
						break;
					case DirectGdi::EGraphicsRotation270:
						vgTranslate(-pos.iX-sourceSize.iHeight, pos.iY+sourceSize.iWidth);
						vgRotate(270.0f);				
						break;
					default:
						// No rotation
						vgTranslate(pos.iX, pos.iY);									
						break;
					}
				
				for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
					vgDrawImage(vgImage);				
				
				ResetVgMatrix(VG_MATRIX_IMAGE_USER_TO_SURFACE);
				}
			}
		}
	}

/** 
@see MDirectGdiEngine::DrawResource(const TRect&, const RDirectGdiDrawableSource&, DirectGdi::TGraphicsRotation)
 */
void CVgEngine::DrawResource(const TRect& aDestRect,
		const RDirectGdiDrawableSource& aSource,
		DirectGdi::TGraphicsRotation aRotation)
	{
	CDirectGdiImageSourceImpl* source = iDriver.GetImageSourceFromHandle(aSource.Handle());
	if (source)
		{		
		TRect srcRect(TPoint(0, 0), source->Size());		
		DoDrawResource(aDestRect, source, srcRect, aRotation);		
		}
	}

/** 
@see MDirectGdiEngine::DrawResource(const TRect&, const RDirectGdiDrawableSource&, const TRect&, DirectGdi::TGraphicsRotation)
 */
void CVgEngine::DrawResource(const TRect& aDestRect,
							const RDirectGdiDrawableSource& aSource,
							const TRect& aSourceRect,
							DirectGdi::TGraphicsRotation aRotation)
	{	
	CDirectGdiImageSourceImpl* source = iDriver.GetImageSourceFromHandle(aSource.Handle());
	if (source)
		{
		// check source rectangle is fully contained within the image resource extent before drawing
		TSize size = source->Size();				
		if ((aSourceRect.iTl.iX < 0) 
			|| (aSourceRect.iTl.iY < 0) 
			|| (aSourceRect.iBr.iX > size.iWidth) 
			|| (aSourceRect.iBr.iY > size.iHeight)
			|| (aSourceRect.Width() <= 0) 
			|| (aSourceRect.Height() <= 0))
			{
			iDriver.SetError(KErrArgument);
			return;
			}
						
		if (((aSourceRect.Width() == aDestRect.Width())
			&& (aSourceRect.Height() == aDestRect.Height())
			&& (aRotation == DirectGdi::EGraphicsRotationNone || aRotation == DirectGdi::EGraphicsRotation180))
			||
			((aSourceRect.Width() == aDestRect.Height())
			&& (aSourceRect.Height() == aDestRect.Width())
			&& (aRotation == DirectGdi::EGraphicsRotation90 || aRotation == DirectGdi::EGraphicsRotation270)))
			{
			// No scaling
			DrawResource(TPoint(aDestRect.iTl.iX, aDestRect.iTl.iY), aSource, aRotation);
			}
		else
			{
			MakeEngineCurrent();			
			DoDrawResource(aDestRect, source, aSourceRect, aRotation);												
			}
		}		
	}

/** 
This method only supports drawing of image sources as Drawables. An attempt to draw
a Drawable that is not an image will result in a panic.

@see MDirectGdiEngine::DrawResource(const TRect&, const RDirectGdiDrawableSource&, const TDesC8&)

@panic DGDIAdapter 1, if an attempt is made to draw a drawable that is not an image source.
 */
void CVgEngine::DrawResource(
			const TRect& aDestRect,
			const RDirectGdiDrawableSource& aSource,
			const TDesC8& /*aParam*/)
	{
	MakeEngineCurrent();
	
	// Check to see if the passed drawable is actually an image as we only support drawing of images at present
	if (iDriver.IsImageSource(aSource.Handle()))
		{
		CDirectGdiImageSourceImpl* source = iDriver.GetImageSourceFromHandle(aSource.Handle());
		if (source)
			{		
			DoDrawResource(aDestRect, source, DirectGdi::EGraphicsRotationNone);
			}
		}
	else
		{
		// This method only supports drawing image sources at present
		GRAPHICS_ASSERT_ALWAYS(0, EDirectGdiPanicNotImplemented);
		}
	}

/**
@see MDirectGdiEngine::BeginDrawGlyph()

Sets the necessary OpenVG and engine state ready for receiving DrawGlyph() commands.
Any OpenVG state that is common for all DrawGlyph() operations are set.
 */
void CVgEngine::BeginDrawGlyph()
	{
	MakeEngineCurrent();	
	vgSetPaint(iTextBrush, VG_FILL_PATH);
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
#ifdef DRAWGLYPH_MULTIPLY_MODE
	vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_MULTIPLY);
#else
	vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_STENCIL);
#endif
	
#ifdef DRAWGLYPH_BUFFERED
	iDrawGlyphCount = 0;
#endif
	}

/**
Draws Glyph image to the given position and orientation. 
The function crops the output image to the clipping rectangle specified as a parameter. 
If the clipping region is defined on the current context, it will also be taken into consideration 

@param	aScrPos				Position to start drawing glyph after rotation has been applied (if necessary).
@param	aChar				Character being drawn. Signifies general Unicode character value.
@param	aGlyphImage			Pointer to the glyph image data.
@param	aGlyphBitmapType	Type of bitmap format.
@param	aGlyphImageSize		Glyph image size.
@param	aScrClipRect			Clipping rect.
@param	aRotation			Rotation specifying how the glyph will be drawn. The number can only be divisible by 90 degrees, 
							i.e. horizontal and vertical rotation.

@see  CFont::TTextDirection
@see MDirectGdiEngine::DrawGlyph()

@pre	The rendering target has been activated.
@post	Request to draw the Glyph has been accepted.
@panic  DGDIAdapter 8, if aClipRect is empty (debug-only).
@panic  DGDIAdapter 61, if font glyph image storage does not exist (debug-only).
*/
void CVgEngine::DrawGlyph(
			const TPoint& aScrPos, 
			const TChar aChar, 
			const TUint8* aGlyphImage,
			const TGlyphBitmapType aGlyphBitmapType,
			const TSize& aGlyphImageSize,
			const TRect& aScrClipRect,			
			const DirectGdi::TGraphicsRotation aRotation)
	{
	GRAPHICS_ASSERT_DEBUG(!aScrClipRect.IsEmpty(), EDirectGdiPanicInvalidRegion);
	GRAPHICS_ASSERT_DEBUG(iFontGlyphImageStorage, EDirectGdiPanicGlyphImageStorageNotCreated);
	
	TPoint pos = aScrPos;
	pos += iDrawOrigin;
	TRect clipRect = aScrClipRect;
	clipRect.iTl += iDrawOrigin;
	clipRect.iBr += iDrawOrigin;
	
	if(	aGlyphImageSize.iHeight <= 0 ||	aGlyphImageSize.iWidth <= 0 ||
		!clipRect.Intersects(iTargetRegion.BoundingRect()) || !iRegionManager.Intersects(clipRect))
		{
		// Just leave silently, as spaces could be passed with empty size.
		return;
		}
	
	// Clip the glyph against the target and the clipping rects.
	// Calculate the axis-aligned bounding box of the glyph.
	TRect glyphBoundingBox;
	switch(aRotation)
		{
		case DirectGdi::EGraphicsRotation90:
			glyphBoundingBox = TRect(TPoint(1+pos.iX-aGlyphImageSize.iHeight, pos.iY), TSize(aGlyphImageSize.iHeight, aGlyphImageSize.iWidth));
			break;
		case DirectGdi::EGraphicsRotation180:
			glyphBoundingBox = TRect(TPoint(pos.iX-aGlyphImageSize.iHeight, pos.iY-aGlyphImageSize.iWidth), aGlyphImageSize);
			break;
		case DirectGdi::EGraphicsRotation270:
			glyphBoundingBox = TRect(TPoint(pos.iX, 1+pos.iY-aGlyphImageSize.iWidth), TSize(aGlyphImageSize.iHeight, aGlyphImageSize.iWidth));
			break;
		default:
			glyphBoundingBox = TRect(pos, aGlyphImageSize);
			break;
		}
	if (!glyphBoundingBox.Intersects(iTargetRegion.BoundingRect()) || !iRegionManager.Intersects(glyphBoundingBox))
		{
		return;
		}
#ifdef DRAWGLYPH_BUFFERED
	iDrawGlyphCommand[iDrawGlyphCount].pos = pos;
	iDrawGlyphCommand[iDrawGlyphCount].aChar = aChar;
	iDrawGlyphCommand[iDrawGlyphCount].aGlyphBitmapType = aGlyphBitmapType;
	iDrawGlyphCommand[iDrawGlyphCount].aGlyphImageSize = aGlyphImageSize;
	iDrawGlyphCommand[iDrawGlyphCount].aClipRect = clipRect;
	iDrawGlyphCommand[iDrawGlyphCount].aRotation = aRotation;
	iDrawGlyphCommand[iDrawGlyphCount].aGlyphImage = const_cast<TUint8*>(aGlyphImage);	
	++iDrawGlyphCount;	
	if (iDrawGlyphCount == KMaxGlyphs)
		{
		FlushDrawGlyphs();
		iDrawGlyphCount = 0;
		}
#else	
	VGImage foreground = VG_INVALID_HANDLE;
	VGImage outline = VG_INVALID_HANDLE;
	VGImage shadow = VG_INVALID_HANDLE;

	TInt err = iFontGlyphImageStorage -> GlyphImage(iFontId, aChar, aGlyphBitmapType, aGlyphImage, aGlyphImageSize, &foreground, &shadow, &outline);
	if(err != KErrNone)
		{
		iDriver.SetError(err);
		if(err == KErrNoMemory)
			{
			if((foreground == VG_INVALID_HANDLE) || 
				((aGlyphBitmapType == EFourColourBlendGlyphBitmap) && ((shadow == VG_INVALID_HANDLE) || (outline == VG_INVALID_HANDLE))))
				{
				return;
				}
			}
		else
			{
			return;
			}
		}
	
	RRegion oldClippingRegion;
	oldClippingRegion.Copy(iRegionManager.ClippingRegion());
	iRegionManager.ClipTo(clipRect);
	
	//  Load the matrix which converts Symbian coordinate system to OpenVG coordinate system
	vgLoadMatrix(Identity());
	if(aRotation == DirectGdi::EGraphicsRotation90)
		{
		vgTranslate(pos.iX+1, pos.iY);
		vgRotate(90.0f);
		}
	else if(aRotation == DirectGdi::EGraphicsRotation270)
		{
		vgTranslate(pos.iX, pos.iY+1);
		vgRotate(270.0f);
		}
	else
		{
		vgTranslate(pos.iX, pos.iY);
		}		

	switch(aGlyphBitmapType)
		{
		case EMonochromeGlyphBitmap:
		case EAntiAliasedGlyphBitmap:
			for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
				vgDrawImage(foreground);
			break;
		case EFourColourBlendGlyphBitmap:
			{
			SetVgPaintColor(iTextBrush, iBrushColor);
			for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
				vgDrawImage(foreground);
	
			SetVgPaintColor(iTextBrush, iTextShadowColor);
			for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
				vgDrawImage(shadow);
	
			SetVgPaintColor(iTextBrush, iPenColor);
			for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
				vgDrawImage(outline);
			break;
			}
		}

	// Restore the clipping manager to its previous state.
	iRegionManager.SetClippingRegion(oldClippingRegion);
	oldClippingRegion.Close();
#endif
	}

/**
@see MDirectGdiEngine::EndDrawGlyph()

Undoes any OpenVG or engine state changes made in BeginDrawGlyph().
 */
void CVgEngine::EndDrawGlyph()
	{
#ifdef DRAWGLYPH_BUFFERED
	FlushDrawGlyphs();
	iDrawGlyphCount = 0;
#endif
	vgLoadMatrix(Identity());
	if (iOrigin != TPoint(0,0))
		{
		vgTranslate(iOrigin.iX, iOrigin.iY);
		}
		
	vgSetPaint(iBrush, VG_FILL_PATH);
	vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_NORMAL);
	}

#ifdef DRAWGLYPH_BUFFERED
void CVgEngine::FlushDrawGlyphs()
	{
	VGImage foreground[KMaxGlyphs];
	Mem::FillZ(foreground, iDrawGlyphCount*sizeof(VGImage));
	VGImage outline[KMaxGlyphs];
	VGImage shadow[KMaxGlyphs];
	
	for (TInt glyph = 0; glyph < iDrawGlyphCount; ++glyph)
		{
		if (foreground[glyph] == 0)
			{			
			TSize aGlyphImageSize = iDrawGlyphCommand[glyph].aGlyphImageSize;
			TChar aChar = iDrawGlyphCommand[glyph].aChar;
			TGlyphBitmapType aGlyphBitmapType = iDrawGlyphCommand[glyph].aGlyphBitmapType;
			TUint8* aGlyphImage = iDrawGlyphCommand[glyph].aGlyphImage;
		
			VGImage foreground1 = VG_INVALID_HANDLE;
			VGImage outline1 = VG_INVALID_HANDLE;
			VGImage shadow1 = VG_INVALID_HANDLE;
		
			TInt err = iFontGlyphImageStorage -> GlyphImage(iFontId, aChar, aGlyphBitmapType, aGlyphImage, aGlyphImageSize, &foreground1, &shadow1, &outline1);
			if(err != KErrNone)
				{
				iDriver.SetError(err);
				if(err == KErrNoMemory)
					{
					if((foreground1 == VG_INVALID_HANDLE) || 
						((aGlyphBitmapType == EFourColourBlendGlyphBitmap) && ((shadow1 == VG_INVALID_HANDLE) || (outline1 == VG_INVALID_HANDLE))))
						{
						return;
						}
					}
				else
					{
					return;
					}
				}
			
			foreground[glyph] = foreground1;
			outline[glyph] = outline1;
			shadow[glyph] = shadow1;
			
			for (TInt nextGlyph = glyph+1; nextGlyph < iDrawGlyphCount; nextGlyph++)
				{
				if (foreground[nextGlyph] == 0)
					{
					if (iDrawGlyphCommand[glyph].SameGlyph(iDrawGlyphCommand[nextGlyph]))
						{
						foreground[nextGlyph] = foreground[glyph];
						outline[nextGlyph] = outline[glyph];
						shadow[nextGlyph] = shadow[glyph];
						}
					}
				}
			}
		}
		
	RRegion oldClippingRegion;
	oldClippingRegion.Copy(iRegionManager.ClippingRegion());
	vgLoadMatrix(Identity());
	TPoint lastPos;
	for (TInt glyph = 0; glyph < iDrawGlyphCount; ++glyph)
		{
		TGlyphBitmapType aGlyphBitmapType = iDrawGlyphCommand[glyph].aGlyphBitmapType;
		TRect aClipRect = iDrawGlyphCommand[glyph].aClipRect;
		DirectGdi::TGraphicsRotation aRotation = iDrawGlyphCommand[glyph].aRotation;
		TPoint aPos = iDrawGlyphCommand[glyph].pos;
		
		iRegionManager.ClipTo(aClipRect);
		
		//  Load the matrix which converts Symbian coordinate system to OpenVG coordinate system
		
		if(aRotation == DirectGdi::EGraphicsRotation90)
			{
			vgTranslate(aPos.iX+1, aPos.iY);
			vgRotate(90.0f);
			}
		else if(aRotation == DirectGdi::EGraphicsRotation270)
			{
			vgTranslate(aPos.iX, aPos.iY+1);
			vgRotate(270.0f);
			}
		else
			{
			//vgTranslate(aPos.iX, aPos.iY);
			vgTranslate(aPos.iX-lastPos.iX, aPos.iY - lastPos.iY);
			lastPos = aPos;
			}		
	
		switch(aGlyphBitmapType)
			{
			case EMonochromeGlyphBitmap:
			case EAntiAliasedGlyphBitmap:
				for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
					vgDrawImage(foreground[glyph]);
				break;
			case EFourColourBlendGlyphBitmap:
				{
				SetVgPaintColor(iTextBrush, iBrushColor);
				for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
					vgDrawImage(foreground[glyph]);
		
				SetVgPaintColor(iTextBrush, iTextShadowColor);
				for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
					vgDrawImage(shadow[glyph]);
		
				SetVgPaintColor(iTextBrush, iPenColor);
				for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
					vgDrawImage(outline[glyph]);
				break;
				}
			}
		
		// Restore the clipping manager to its previous state.
		iRegionManager.SetClippingRegion(oldClippingRegion);
		}
	oldClippingRegion.Close();
	}
#endif

/**
@see MDirectGdiEngine::CopyRect()
 */
void CVgEngine::CopyRect(const TPoint& aOffset, const TRect& aRect)
	{
	MakeEngineCurrent();

	// Transformations, masking and blending are not applied.
	// So need to convert to VG coordinate system.
	// i.e. Need Bottom-Left coord of aRect in VG's coordinates.
	// Also need to allow for drawing engine coordinate system (iOrigin)
	const TPoint sourcePoint = ConvertToVgCoords(TPoint(aRect.iTl.iX, aRect.iBr.iY) + iOrigin);
	// Scissoring is applied to destination, but does not affect reading of pixels.
	for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
		vgCopyPixels(sourcePoint.iX + aOffset.iX, sourcePoint.iY - aOffset.iY,	// destination point
						sourcePoint.iX, sourcePoint.iY,							// source point
						aRect.Width(), aRect.Height());							// size of rect to copy
	}

/**
@see MDirectGdiEngine::ExternalizeL()
@see InternalizeL()
 */
void CVgEngine::ExternalizeL(RWriteStream& aWriteStream) 
	{
	aWriteStream.WriteUint32L(iPaintMode);
	aWriteStream << iLinePos;
	}

/**
@see MDirectGdiEngine::InternalizeL()
@see ExternalizeL()
 */
void CVgEngine::InternalizeL(RReadStream& aReadStream)
	{
	iPaintMode = aReadStream.ReadUint32L();
	aReadStream >> iLinePos;
	}

/**
@see MDirectGdiEngine::GetInterface()
 */
TInt CVgEngine::GetInterface(TUid aInterfaceId, TAny*& aInterface)
	{
	aInterface = NULL;
	TInt err = KErrNone;
	switch (aInterfaceId.iUid)
		{
		case KDirectGdiGetGlyphStorageUid:
			{
			aInterface = static_cast<MFontGlyphImageStorage*> (iDriver.FontGlyphImageStorage());
			break;	
			}
		case KDirectGdiVgImageCacheUid:
			{
			aInterface =  static_cast<MVgImageCache*> (iDriver.VgImageCache());
			break;
			}
		case KDrawDeviceOriginInterfaceID:
			{
			aInterface = static_cast<MDrawDeviceOrigin*>(this);
			break;
			}
		default:
			err = KErrNotSupported;
			break;
		}
	return err;
	}

/**
Converts a point from the Symbian OS graphics coordinate system to the OpenVG coordinate system.

The Symbian OS coordinate system's x-axis increases positively from the origin rightwards.
The y-axis increases positively from the origin downwards.

The OpenVG coordinate system's x-axis increases positively from the origin rightwards.
The y-axis increases positively from the origin upwards.

Therefore a point (X,Y) in the Symbian OS coordinate system would be equivalent to a point
(X',Y') in the OpenVG coordinate system by the following transformations:
X' = X
Y' = (Height of rendering target) - Y

@param	aPoint A point specified in the Symbian OS graphics coordinate system.

@return The point specified in the OpenVG-specific coordinate system.
 */
const TPoint CVgEngine::ConvertToVgCoords(const TPoint& aPoint)
	{
	MakeEngineCurrent();
	TInt targetHeight = iSize.iHeight;
	return TPoint(aPoint.iX, targetHeight - aPoint.iY);
	}

/**
Converts the position of a rectangle from the Symbian OS graphics coordinate system to the 
OpenVG coordinate system.

The Symbian OS coordinate system's x-axis increases positively from the origin rightwards.
The y-axis increases positively from the origin downwards.
A rectangle's position is specified by the top-left coordinate.

The OpenVG coordinate system's x-axis increases positively from the origin rightwards.
The y-axis increases positively from the origin upwards.
A rectangle's position is specified by the bottom-left coordinate.

A point (X,Y) in the Symbian OS coordinate system would be equivalent to point
(X',Y') in the OpenVG coordinate system by the following transformations:
X' = X
Y' = (Height of rendering target) - Y

@param	aRect	A rectangle whose position is to be converted for use in OpenVG.

@return The bottom-left point of the rectangle, specified in the OpenVG specific coordinate system.
 */
const TPoint CVgEngine::ConvertToVgCoords(const TRect& aRect)
	{
	MakeEngineCurrent();
	TInt targetHeight = iSize.iHeight;
	return TPoint(aRect.iTl.iX, targetHeight - aRect.iBr.iY);
	}

/**
Resets the user-to-surface OpenVG matrices to the default system matrix.
 */
void CVgEngine::ResetVgMatrix()
	{
	ResetVgMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
	ResetVgMatrix(VG_MATRIX_IMAGE_USER_TO_SURFACE);
	}

/**
Resets an OpenVG matrix to the default system matrix. Current origin offset applies.
@param aMatrixMode The OpenVG matrix being reset.
 */
void CVgEngine::ResetVgMatrix(const VGMatrixMode aMatrixMode)
	{
	vgSeti(VG_MATRIX_MODE, aMatrixMode);
	//  Load matrix which converts Symbian coordinate system to VG coordinate system
	vgLoadMatrix(Identity());
		
	// Add in translation for drawing engine coordinate system
	vgTranslate(iOrigin.iX, iOrigin.iY);
	}

/**
If aPath is a null handle, a VGPath is created with enough space to hold aExpectedCommandCount commands.
If the path is already a handle to a valid path, the path is simply cleared instead of recreated.
Also allocates memory for two arrays; one to hold all commands we will add to the newly
created path, and one to hold all coordinates we will add to the newly created path.
Commands and coordinates are not added to the path until FinishPath() is called for this path. 
Commands and coordinates are saved for adding to the path when FinishPath() is
called by calling AppendPathCommand(). Any failures are reported to the driver before returning.

@param  aPath A handle to the path being cleared/created.
@param	aExpectedCommandCount The number of commands the path is expected to hold. If this size
		is underestimated, the arrays that hold the current commands and coordinates will grow
		as additional items are added beyond the original estimated size. Once a path is finished, the current
		command and coordinate counters are reset, but the memory remains to cut down on memory allocations.
		
@return EFalse if memory allocation or path creation fails, ETrue otherwise.

@see CVgEngine::AppendPathCommand(VGubyte)
@see CVgEngine::AppendPathCommand(VGubyte, VGfloat)
@see CVgEngine::AppendPathCommand(VGubyte, VGfloat, VGfloat)
@see CVgEngine::PrepareForPathCommand()
@see CVgEngine::FinishPath()
@see CVgEngine::AllocPathCommands()
@see CVgEngine::AllocPathCoords()
 */
TBool CVgEngine::PreparePath(VGPath& aPath, TInt aExpectedCommandCount)
	{	
	TInt err = KErrNone;
	err = AllocPathCommands(aExpectedCommandCount);		
	if(KErrNone == err)
		{
		err = AllocPathCoords(aExpectedCommandCount*2);	// guess at the number of coords needed, this will be reallocated if it is not big enough
		}
	if(KErrNone == err)
		{
		if (aPath != VG_INVALID_HANDLE)
			vgClearPath(aPath, VG_PATH_CAPABILITY_APPEND_TO);
		else
			{
			aPath = vgCreatePath(VG_PATH_FORMAT_STANDARD,
							VG_PATH_DATATYPE_F,
							1.0f, //scale
							0.0f, //bias
							aExpectedCommandCount,
							aExpectedCommandCount*2, //expected coord count
							VG_PATH_CAPABILITY_APPEND_TO);
			
			if (aPath == VG_INVALID_HANDLE)
				err = KErrNoMemory;
			}
		}
	if (KErrNone != err)
		{
		iDriver.SetError(err);
		return EFalse;
		}
	return ETrue;
	}

/**
Adds a path command to the currently saved array of path commands. Any error that occurs is stored
with the driver.

@param aCommand	The VGPathCommand to add to the current path, e.g VG_CLOSE_PATH.

@see CVgEngine::CreatePath()
@see CVgEngine::AppendPathCommand(VGubyte, VGfloat)
@see CVgEngine::AppendPathCommand(VGubyte, VGfloat, VGfloat)
@see CVgEngine::PrepareForPathCommand()
@see CVgEngine::FinishPath()
@see CVgEngine::AllocPathCommands()
@see CVgEngine::AllocPathCoords()
 */
void CVgEngine::AppendPathCommand(VGubyte aCommand)
	{
	TInt err = PrepareForPathCommand(1, 0);
	if (KErrNone == err)
		{
		iPathCommands[iPathCommandCount++] = aCommand;
		}
	else
		{
		iDriver.SetError(err);
		}	
	}

/**
Adds a path command and a single coordinate to the currently saved array of path commands.
Any error that occurs is stored with the driver.  

@param	aCommand The VGPathCommand to add to the current path, e.g VG_HLINE_TO_ABS.
@param	aCoord	 The coordinate to add to the current path, e.g. 10.f.

@see CVgEngine::CreatePath()
@see CVgEngine::AppendPathCommand(VGubyte)
@see CVgEngine::AppendPathCommand(VGubyte, VGfloat, VGfloat)
@see CVgEngine::PrepareForPathCommand()
@see CVgEngine::FinishPath()
@see CVgEngine::AllocPathCommands()
@see CVgEngine::AllocPathCoords()
 */
void CVgEngine::AppendPathCommand(VGubyte aCommand, VGfloat aCoord)
	{
	TInt err = PrepareForPathCommand(1, 1);
	if (KErrNone == err)
		{				
		iPathCommands[iPathCommandCount++] = aCommand;			
		iPathCoords[iPathCoordCount++] = aCoord;		
		}
	else
		{
		iDriver.SetError(err);
		}	
	}

/**
Adds a path command and two coordinates to the currently saved array of path commands. 
Any error that occurs is stored with the driver.

@param	aCommand	The VGPathCommand to add to the current path, e.g VG_MOVE_TO_ABS.
@param	aCoord1		The coordinate to add to the current path, e.g. 10.f.
@param	aCoord2		The coordinate to add to the current path, e.g. 10.f.

@see CVgEngine::CreatePath()
@see CVgEngine::AppendPathCommand(VGubyte)
@see CVgEngine::AppendPathCommand(VGubyte, VGfloat)
@see CVgEngine::PrepareForPathCommand()
@see CVgEngine::FinishPath()
@see CVgEngine::AllocPathCommands()
@see CVgEngine::AllocPathCoords()
 */
void CVgEngine::AppendPathCommand(VGubyte aCommand, VGfloat aCoord1, VGfloat aCoord2)
	{
	TInt err = PrepareForPathCommand(1, 2);
	if (KErrNone == err)
		{			
		iPathCommands[iPathCommandCount++] = aCommand;					
		iPathCoords[iPathCoordCount++] = aCoord1;
		iPathCoords[iPathCoordCount++] = aCoord2;		
		}
	else
		{
		iDriver.SetError(err);
		}	
	}

/**
Allocates memory to store the passed number of commands and coordinates in the saved command and 
coordinate arrays. Any error that occurs is stored with the driver.

@param	aCommandCount	The number of new commands expected to be added to the current command array.
@param	aCoordCount		The number of new commands expected to be added to the current coordinate array.

@return	KErrNoMemory if memory allocation fails, KErrNone otherwise.

@see CVgEngine::CreatePath()
@see CVgEngine::AppendPathCommand(VGubyte)
@see CVgEngine::AppendPathCommand(VGubyte, VGfloat)
@see CVgEngine::AppendPathCommand(VGubyte, VGfloat, VGfloat)
@see CVgEngine::FinishPath()
@see CVgEngine::AllocPathCommands()
@see CVgEngine::AllocPathCoords()
 */
TInt CVgEngine::PrepareForPathCommand(TInt aCommandCount, TInt aCoordCount)
	{
	// Do we need to add space for the new commands and coords?
	TInt err = AllocPathCommands(iPathCommandCount+aCommandCount);
	if (err == KErrNone)
		{
		err = AllocPathCoords(iPathCoordCount+aCoordCount);
		}
	if (err != KErrNone)
		{
		iDriver.SetError(err);
		}
	return err;
	}

/**
Adds the contents of the saved command and coordinate arrays to the passed VGPath ready for drawing.
Clears the counts of saved commands and coordinates.

@param	aPath	The path that the current commands and coordinates are to be added to. 

@see CVgEngine::CreatePath()
@see CVgEngine::AppendPathCommand(VGubyte)
@see CVgEngine::AppendPathCommand(VGubyte, VGfloat)
@see CVgEngine::AppendPathCommand(VGubyte, VGfloat, VGfloat)
@see CVgEngine::PrepareForPathCommand()
@see CVgEngine::AllocPathCommands()
@see CVgEngine::AllocPathCoords()
 */
void CVgEngine::FinishPath(VGPath aPath)
	{
	vgAppendPathData(aPath, iPathCommandCount, iPathCommands, iPathCoords);
	iPathCommandCount = 0;
	iPathCoordCount = 0;
	}

/**
Allocates enough memory to hold aCommandCount path commands, unless the command array already holds
enough memory.

@param	aCommandCount The number of commands to allocate space for in the command array.

@return	KErrNoMemory if memory allocation fails, KErrNone otherwise.

@see CVgEngine::CreatePath(TInt)
@see CVgEngine::AppendPathCommand(VGubyte)
@see CVgEngine::AppendPathCommand(VGubyte, VGfloat)
@see CVgEngine::AppendPathCommand(VGubyte, VGfloat, VGfloat)
@see CVgEngine::PrepareForPathCommand(TInt, TInt)
@see CVgEngine::FinishPath(VGPath)
@see CVgEngine::AllocPathCoords(TInt)
 */
TInt CVgEngine::AllocPathCommands(TInt aCommandCount)
	{
	TInt err = KErrNone;
	if (iCurrentMaxCommands < aCommandCount)
		{
		VGubyte* oldPathCommands = iPathCommands;
		iPathCommands = static_cast<VGubyte*>(User::ReAlloc(iPathCommands, sizeof(VGubyte)*aCommandCount));					
		if (iPathCommands)
			{
			iCurrentMaxCommands = aCommandCount;	
			}	
		else
			{
			delete oldPathCommands;
			iCurrentMaxCommands = 0;
			}
		}
	if(!iPathCommands)
		err = KErrNoMemory;
	return err;	
	}

/**
Allocates enough memory to hold aCoordCount path coordinates, unless the coordinate array already holds
enough memory.

@param	aCoordCount	The number of coordinates to allocate space for in the coordinate array.

@return	KErrNoMemory if memory allocation fails, KErrNone otherwise.

@see CVgEngine::CreatePath()
@see CVgEngine::AppendPathCommand(VGubyte)
@see CVgEngine::AppendPathCommand(VGubyte, VGfloat)
@see CVgEngine::AppendPathCommand(VGubyte, VGfloat, VGfloat)
@see CVgEngine::PrepareForPathCommand()
@see CVgEngine::FinishPath()
@see CVgEngine::AllocPathCommands()
 */
TInt CVgEngine::AllocPathCoords(TInt aCoordCount)
	{
	TInt err = KErrNone;
	if (iCurrentMaxCoords < aCoordCount)
		{
		VGfloat* oldPathCoords = iPathCoords;
		iPathCoords = static_cast<VGfloat*>(User::ReAlloc(iPathCoords, sizeof(VGfloat)*aCoordCount));			
		if (iPathCoords)
			{
			iCurrentMaxCoords = aCoordCount;	
			}	
		else
			{
			delete oldPathCoords;
			iCurrentMaxCoords = 0;
			}
		}
	if(!iPathCoords)
		err = KErrNoMemory;
	return err;	
	}

/**
Helper method for creating a VGImage from a CFbsBitmap. A temporary VGImageFormat-compatible 
copy of the image may be created, if the source bitmap pixel format is not directly supported 
by OpenVG. If there is not enough memory available, the error state on the driver is set to KErrNoMemory.
It sets the error in the driver if out-of-memory occurs when creating a VGImage. OpenVG will set its 
own internal error if creation of VGImage fails.

@param	aSource	The source bitmap.
@param  aFlipY If 'ETrue' then inverts the image in the y axis.
@param  aOrigin An offset from the top-left of the image in which to take the first pixel of the image.

@return	Returns a valid VGImage created using the passed CFbsBitmap. If unable to
		create a valid VGImage then VG_INVALID_HANDLE.
*/
VGImage CVgEngine::CreateSourceVGImage(const CFbsBitmap& aSource, TBool aFlipY, const TPoint& aOrigin)
	{		
	TDisplayMode srcDisplayMode = aSource.DisplayMode();
	TDisplayMode vgCompatibleDisplayMode = ClosestVgCompatibleDisplayMode(srcDisplayMode);
	VGImageFormat imageFormat = MapToVgDisplayMode(vgCompatibleDisplayMode);
	const TSize sourceSize = aSource.SizeInPixels();
	VGImage image = DoVgCreateImage(imageFormat, sourceSize.iWidth, sourceSize.iHeight, VG_IMAGE_QUALITY_NONANTIALIASED);
	if (image != VG_INVALID_HANDLE)
		{
		const TInt pixmapStride = CFbsBitmap::ScanLineLength(sourceSize.iWidth, vgCompatibleDisplayMode);	
		
		// Conversion is performed if changing display mode (pixel format), source is compressed, or want to flip the y orientation.
		if ((vgCompatibleDisplayMode != srcDisplayMode) || (aSource.Header().iCompression != ENoBitmapCompression) || aFlipY)
			{
			// May be worth using a static memory buffer for smaller scan-lines, to avoid overhead of alloc.
			TAny* data = User::Alloc(pixmapStride);
			if (data)
				{
				// Allocate memory and transform source into target format.
				TPoint sourcePoint (0, aFlipY ? (sourceSize.iHeight - 1) : 0);
				TPtr8 targetPoint ((TUint8*)data, pixmapStride, pixmapStride);
				TInt adj = aFlipY ? -1 : 1;
	
				if (aOrigin == TPoint(0,0)) // Not shifted
					{
					for (TInt targetY = 0; targetY < sourceSize.iHeight; targetY++, sourcePoint.iY += adj)
						{
						aSource.GetScanLine(targetPoint, sourcePoint, sourceSize.iWidth, vgCompatibleDisplayMode);
						vgImageSubData(image, data, pixmapStride, imageFormat, 0, targetY, sourceSize.iWidth, 1);
						}
					}
				else if (aOrigin.iX == 0) // Only shifted in Y.
					{
					for (TInt targetY = 0x00; targetY < sourceSize.iHeight; targetY++, sourcePoint.iY += adj)
						{
						aSource.GetScanLine(targetPoint, sourcePoint, sourceSize.iWidth, vgCompatibleDisplayMode);
						vgImageSubData(image, data, pixmapStride, imageFormat, 0, targetY + aOrigin.iY,                      sourceSize.iWidth, 1);
						vgImageSubData(image, data, pixmapStride, imageFormat, 0, targetY + aOrigin.iY - sourceSize.iHeight, sourceSize.iWidth, 1);
						}
					}
				else if (aOrigin.iY == 0) // Only shifted in X.
					{
					for (TInt targetY = 0x00; targetY < sourceSize.iHeight; targetY++, sourcePoint.iY += adj)
						{
						aSource.GetScanLine(targetPoint, sourcePoint, sourceSize.iWidth, vgCompatibleDisplayMode);
						vgImageSubData(image, data, pixmapStride, imageFormat, -aOrigin.iX,                     targetY, sourceSize.iWidth, 1);
						vgImageSubData(image, data, pixmapStride, imageFormat, -aOrigin.iX + sourceSize.iWidth, targetY, sourceSize.iWidth, 1);
						}				
					}
				else						  // Shifted in both X and Y.
					{
					for (TInt targetY = 0; targetY < sourceSize.iHeight; targetY++, sourcePoint.iY += adj)
						{
						aSource.GetScanLine(targetPoint, sourcePoint, sourceSize.iWidth, vgCompatibleDisplayMode);
						vgImageSubData(image, data, pixmapStride, imageFormat, -aOrigin.iX, 					targetY + aOrigin.iY, 						sourceSize.iWidth, 1);
						vgImageSubData(image, data, pixmapStride, imageFormat, -aOrigin.iX, 					targetY + aOrigin.iY - sourceSize.iHeight, sourceSize.iWidth, 1);
						vgImageSubData(image, data, pixmapStride, imageFormat, -aOrigin.iX + sourceSize.iWidth, targetY + aOrigin.iY, 						sourceSize.iWidth, 1);
						vgImageSubData(image, data, pixmapStride, imageFormat, -aOrigin.iX + sourceSize.iWidth, targetY + aOrigin.iY - sourceSize.iHeight, sourceSize.iWidth, 1);
						}
					}
				User::Free(data);
				}
			else
				{
				iDriver.SetError(KErrNoMemory);
				vgDestroyImage(image);
				return VG_INVALID_HANDLE;
				}
			}
		else
			{
			aSource.BeginDataAccess();
			const TInt sourceDataStride = aSource.DataStride();
			if (aOrigin == TPoint(0,0))			// Not shifted
				{
				vgImageSubData(image, aSource.DataAddress(), sourceDataStride, imageFormat, 0, 0, sourceSize.iWidth, sourceSize.iHeight);			
				}
			else
				{
				TUint32* dataAddress = aSource.DataAddress();
				
				if (aOrigin.iX == 0) 		// Only shifted in Y.
					{
					vgImageSubData(image, dataAddress, sourceDataStride, imageFormat, 0, aOrigin.iY,                      sourceSize.iWidth, sourceSize.iHeight);						
					vgImageSubData(image, dataAddress, sourceDataStride, imageFormat, 0, aOrigin.iY - sourceSize.iHeight, sourceSize.iWidth, sourceSize.iHeight);						
					}
				else if (aOrigin.iY == 0) 	// Only shifted in X.
					{
					vgImageSubData(image, dataAddress, sourceDataStride, imageFormat, -aOrigin.iX,                     0, sourceSize.iWidth, sourceSize.iHeight);						
					vgImageSubData(image, dataAddress, sourceDataStride, imageFormat, -aOrigin.iX + sourceSize.iWidth, 0, sourceSize.iWidth, sourceSize.iHeight);						
					}
				else							// Shifted in both X and Y.
					{
					vgImageSubData(image, dataAddress, sourceDataStride, imageFormat, -aOrigin.iX, aOrigin.iY,                      sourceSize.iWidth, sourceSize.iHeight);						
					vgImageSubData(image, dataAddress, sourceDataStride, imageFormat, -aOrigin.iX, aOrigin.iY - sourceSize.iHeight, sourceSize.iWidth, sourceSize.iHeight);							
					vgImageSubData(image, dataAddress, sourceDataStride, imageFormat, -aOrigin.iX + sourceSize.iWidth, aOrigin.iY,                      sourceSize.iWidth, sourceSize.iHeight);						
					vgImageSubData(image, dataAddress, sourceDataStride, imageFormat, -aOrigin.iX + sourceSize.iWidth, aOrigin.iY - sourceSize.iHeight, sourceSize.iWidth, sourceSize.iHeight);							
					}
				}
			aSource.EndDataAccess(ETrue);
			}
		}
	return image;
	}

/**
Creates an image which represents a standard brush pattern (e.g. EVerticalHatchBrush), by
drawing the pattern using the current pen and brush colour to an image.

@param	aPatternSize	The dimensions in pixels of the image to create.
@param	aBrushPattern	An array of 1s and 0s representing the pattern to create, the length
						of this array is aSize.iWidth*aSize.iHeight.	

@return	KErrNone if the brush is created successfully, KErrNoMemory if we fail to create 
		the brush image, KErrArgument if aBrushPattern is invalid.
 */
TInt CVgEngine::CreateStandardBrush(TSize& aPatternSize, VGbyte* aBrushPattern)
	{		
	if (aBrushPattern == NULL)
		return KErrArgument;
	
	TInt err = KErrNone;	
	MakeEngineCurrent();
	
	// The image format to use as the standard brush. If this changes, it also needs changing in 
	// GetCurrentBrush().	
	const VGImageFormat dataFormat = VG_sARGB_8888;
	
	// We want to create the brush to the nearest power of 4 size.
	TSize brushSize((aPatternSize.iWidth+3)&~3, (aPatternSize.iHeight+3)&~3);		
	
	// Allocate some memory to write the brush pattern to.
	TUint32* data = new TUint32[brushSize.iWidth*brushSize.iHeight];
	if (data == NULL)
		err = KErrNoMemory;		
	
	if (err == KErrNone)
		{
		if (iBrushPatternStandardRegion != VG_INVALID_HANDLE)
			{
			vgDestroyImage(iBrushPatternStandardRegion);
			iBrushPatternStandardRegion = VG_INVALID_HANDLE;
			}
		
		// Check the size of the existing standard brush as it can't be re-used if the
		// new brush to be created is a different size.
		if (iBrushPatternStandard != VG_INVALID_HANDLE)
			{
			if (iBrushPatternStandardSize != brushSize)
				{
				vgDestroyImage(iBrushPatternStandard);
				iBrushPatternStandard = VG_INVALID_HANDLE;
				}
			}					
		
		if (iBrushPatternStandard == VG_INVALID_HANDLE)
			{
			// Create an image to draw the brush pattern onto, this will be our offscreen buffer
			iBrushPatternStandardSize = brushSize;
			iBrushPatternStandard = DoVgCreateImage(dataFormat, brushSize.iWidth, brushSize.iHeight, VG_IMAGE_QUALITY_NONANTIALIASED);
			if (iBrushPatternStandard == VG_INVALID_HANDLE)
				err = KErrNoMemory;
			}
		}			

	if (err == KErrNone)
		{
		// Set the colour used for the pen. If not fully opaque and not in WriteAlpha mode,
		// blend the pen colour with the brush colour.
		TRgb penColor = iPenColor;
		if ((iPenColor.Alpha() != 255) && (iDrawMode != DirectGdi::EDrawModeWriteAlpha))
			{
			penColor.SetInternal(
				PMA2NonPMAPixel(
					PMAPixelBlend(
					    NonPMA2PMAPixel(iBrushColor.Internal()), 
						NonPMA2PMAPixel(iPenColor.Internal()))));
			}
		
		// Draw the pattern on to the brush, pixel by pixel.
		for (TInt j = 0; j < aPatternSize.iHeight; ++j)
			{
			for (TInt i = 0; i < aPatternSize.iWidth; ++i)
				{				
				if (aBrushPattern[(j*aPatternSize.iWidth)+i])			
					{					
					data[(j*brushSize.iWidth)+i] = penColor._Color16MA(); 
					}
				else
					{
					data[(j*brushSize.iWidth)+i] = iBrushColor._Color16MA();
					}
				}
			}					
				
		// Copy the pattern to the VGImage so we can set it as the current brush
		vgImageSubData(iBrushPatternStandard,	// the image to copy to		
					data, 						// the source data
					brushSize.iWidth*4,	 		// the stride of the source data
					dataFormat, 				// the format of the source data
					0, 							// x
					0, 							// y
					brushSize.iWidth, 			// width
					brushSize.iHeight);			// height
		iBrushPatternStandardSize = brushSize;
		
		// We only want to use the region of the brush we just created that is the size of the pattern
		iBrushPatternStandardRegion = vgChildImage(iBrushPatternStandard, 
													0, 
													0, 
													aPatternSize.iWidth, 
													aPatternSize.iHeight);
		iBrushPatternStandardRegionSize = aPatternSize;
		}		
	
	// Clean up
	delete [] data;

	return err;
	}

/**
Internal function to return the currently active brush and its properties.
@param aBrush On success, holds the current VGImage brush being used.
@param aSize On success, holds the dimensions of VGImage being used as the brush.
@param aFormat On success, holds the VGImageFormat of the brush. 
@return ETrue if a brush is currently being used, EFalse otherwise.
*/
TBool CVgEngine::GetCurrentBrushPattern(VGImage& aBrush, TSize& aSize, VGImageFormat& aFormat) const
	{
	TBool success = ETrue;
	if (iBrushStyle == DirectGdi::EPatternedBrush)
		{
		aBrush = iBrushPatternUser;
		aSize = iBrushPatternUserSize;
		aFormat = static_cast<VGImageFormat>(vgGetParameteri(aBrush, VG_IMAGE_FORMAT));
		}
	else if (iBrushStyle > DirectGdi::EPatternedBrush)
		{
		aBrush = iBrushPatternStandardRegion;
		aSize = iBrushPatternStandardRegionSize;
		// Currently we only ever use VG_sARGB_8888 for the standard brush format.
		aFormat = VG_sARGB_8888;
		}
	else 
		success = EFalse;
	return success;
	}

/**
Copies the current brush pattern (if a brush pattern is set) into iBrushPatternNonZeroOrigin.
This function should only be used if the current brush origin is not (0,0). When copying the
current brush pattern, it is shifted to take into account the non-zero origin. This shifted brush
pattern should be used for all brush operations while a non-zero origin is set.

@return	KErrNone if successful, KErrNotFound if the brush pattern could not be copied.
 */
TInt CVgEngine::CopyCurrentBrushPatternForNonZeroOrigin()
	{
	MakeEngineCurrent();
	TInt ret = KErrNotFound;
	VGImage brush = VG_INVALID_HANDLE;
	TSize brushSize;
	VGImageFormat imageFormat;
	
	if (GetCurrentBrushPattern(brush, brushSize, imageFormat))
		{
		const TInt width = brushSize.iWidth;
		const TInt height = brushSize.iHeight;
		
		if ((width != 0) && (height != 0))
			{
			if (iBrushPatternNonZeroOrigin != VG_INVALID_HANDLE)
				{
				vgDestroyImage(iBrushPatternNonZeroOrigin);
				iBrushPatternNonZeroOrigin = VG_INVALID_HANDLE;
				}
			
			// Create the brush we are going to copy the current brush into
			iBrushPatternNonZeroOrigin = DoVgCreateImage(imageFormat,
													width,
													height,
													VG_IMAGE_QUALITY_FASTER);
			if (iBrushPatternNonZeroOrigin != VG_INVALID_HANDLE)
				{
				TInt offsetX = width - (iBrushOrigin.iX % width);
				TInt offsetY = height - (iBrushOrigin.iY % height);	
				
				// Top left to bottom right			
				if (offsetX != 0 && offsetY != 0) // check the width and height we are copying are not 0
					vgCopyImage(iBrushPatternNonZeroOrigin, width-offsetX, height-offsetY, brush, 0, 0, offsetX, offsetY, VG_FALSE);
				
				// Top right to bottom left
				if ((width-offsetX) != 0 && offsetY != 0) 
					vgCopyImage(iBrushPatternNonZeroOrigin, 0, height-offsetY, brush, offsetX, 0, width-offsetX, offsetY, VG_FALSE);
				
				// Bottom left to top right
				if (offsetX != 0 && (height-offsetY) != 0) 
					vgCopyImage(iBrushPatternNonZeroOrigin, width-offsetX, 0, brush, 0, offsetY, offsetX, height-offsetY, VG_FALSE);				
				
				// Bottom right to top left
				if ((width-offsetX) != 0 && (height-offsetY) != 0) 
					vgCopyImage(iBrushPatternNonZeroOrigin, 0, 0, brush, offsetX, offsetY, width-offsetX, height-offsetY, VG_FALSE);
				
				// Paint with the new non-zero origin brush
				vgPaintPattern(iBrush, iBrushPatternNonZeroOrigin);
				
				ret = KErrNone;
				}	
			else
				{
				ret = KErrNoMemory;
				}
			}
		}
	
	return ret;
	}

/**
Calculates the angle in degrees formed anti-clockwise between vector V1 and V2, where V1 is a 
horizontal vector to the right from aOriginX, and V2 is the vector (aPointX-aOriginX, aPointY-aOriginY).

@param aOriginX	The x coordinate of a point which represents the origin of our calculation.
@param aOriginY	The y coordinate of a point which represents the origin of our calculation.
@param aPointX	The x coordinate of the point which defines the angle with the origin's x-axis.
@param aPointY 	The y coordinate of the point which defines the angle with the origin's y-axis.
@param aWidth	Width of the rectangle which defines where to draw the ellipse.
@param aHeight 	Height of the rectangle which defines where to draw the ellipse.

@panic DGDIAdapter 1006, if either width or height are less than 1 (debug-only).
@return	The angle in degrees between the vectors V1 and V2 described above.
 */
TReal CVgEngine::GetAngleFromXAxisAnticlockwise(const TReal aOriginX, const TReal aOriginY, const TReal aPointX, const TReal aPointY, const TReal aWidth, const TReal aHeight)
	{
	GRAPHICS_ASSERT_DEBUG((aWidth > 0) && (aHeight > 0), EDirectGdiPanicInvalidParameter);

// The angle is calculated from the radius line that joins the point to the origin.
// The point initially provided defines a line relative to the ellipse.
// But the VG spec states that the required angle is that for a perfect circle
// before that circle is scaled by the bounding rect into an ellipse.
// Therefore, downscale the position of the point relative to the origin, by the
// relative dimensions of the width/height of the ellipse to make it relative to
// the underlying circle. Then use the resulting circle radius line to calculate the angle.

	TReal angle = 0.0f;		
	TReal pointX = aPointX-aOriginX;
	TReal pointY = aPointY-aOriginY;

	const TReal scalingFactor = (aWidth / aHeight);
	pointY *= scalingFactor;
	Math::ATan(angle, pointY, pointX);
	
	// Adjust the angle for Q2 and Q3
	if (pointY < 0)
		angle = (KPi*2)+angle; 
		
	return angle*KRadToDeg;
	}

/**
Method for drawing arcs or pies. An arc is a segment of an ellipse which is defined by a given rectangle. 
The arc is drawn anti-clockwise from the arc start point to the arc end point. The arc start point is the 
intersection between vectors from the centre of the ellipse to the given start position and the ellipse.
The arc end point is defined in the same way.

@param	aRect		The rectangle which defines where to draw the ellipse.
@param	aStart		Position to be used in defining the arc start point.
@param	aEnd		Position to be used in defining the arc end point.
@param 	aArcType	The type of arc to draw; an arc, a chord or a pie.	

@post	Request to draw an arc has been accepted. There is no guarantee that the request 
		has been processed when the method returns.
		
@see	CVgEngine::DrawPie()
@see	CVgEngine::DrawArc()
 */
void CVgEngine::DoDrawArc(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd, VGUArcType aArcType)
	{
	MakeEngineCurrent();
	
	// Only draw if we are not painting with a NULL pen and a NULL brush
	if (iPaintMode == 0)
		return;
		
	// If the pen width and height are the same then draw as normal. If they are different but we should be filling
	// this shape we need to draw the filled area only as normal (not the outline). The outline of the shape is drawn 
	// in the block of code below to allow the effect of a different width and height pen to be applied.
	if ((iPenSize.iWidth == iPenSize.iHeight) || ((iPaintMode & VG_FILL_PATH) && (aArcType == VGU_ARC_PIE)))
		{
		TReal originX = aRect.iTl.iX + (static_cast<TReal>(aRect.Width())*0.5);
		TReal originY = aRect.iTl.iY + (static_cast<TReal>(aRect.Height())*0.5);
		TReal startAngle = GetAngleFromXAxisAnticlockwise(originX, originY, aStart.iX, aStart.iY, aRect.Width(), aRect.Height());
		TReal endAngle = GetAngleFromXAxisAnticlockwise(originX, originY, aEnd.iX, aEnd.iY, aRect.Width(), aRect.Height());		
		TReal extent = endAngle - startAngle;
		
		// The extent defines what direction the arc is drawn in, so make sure we always draw
		// anti-clockwise (-ve extent)
		if (extent > 0.0f)
			extent -= 360.0f;
		
		// If the start and end points are the same, make sure we draw arc all the way round the ellipse
		if ((aStart == aEnd) || (extent > -0.0001f))
			extent = -360.0f;
		
		// Before any vgu command, call SetError() as this stores the current vg error state (if any) in the 
		// driver. Some implementations of vgu clears error state so we'd lose error codes otherwise.
		iDriver.SetError(KErrNone);
		
		if (PreparePath(iVgPath, 5)) // N.B. 5 is just an initial hint as to how large the path may be, not its final size
			{
			VGUErrorCode err = vguArc(iVgPath, 
				originX + 0.5f, 
				originY + 0.5f, 
				aRect.Width(), 
				aRect.Height(), 
				startAngle, 
				extent, 
				aArcType);
			
			if (err == VGU_NO_ERROR)
				{
				VGbitfield paintMode = iPaintMode;		
				if(aArcType == VGU_ARC_OPEN)
					{
					paintMode &= ~VG_FILL_PATH;  
					}
				for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
					vgDrawPath(iVgPath, (iPenSize.iWidth == iPenSize.iHeight) ? paintMode : VG_FILL_PATH);				
				}
			else
				{
				SetVguError(err);
				}
			}
		}
	
	if ((iPenSize.iWidth != iPenSize.iHeight)
	&& (iPaintMode & VG_STROKE_PATH)		
	&& (iPenSize.iWidth != 0) && (iPenSize.iHeight != 0))
		{
		// Setting a pen with different width and height is not available on OpenVG, so we need to scale 
		// the coordinates we are drawing and apply a scaling matrix that scales by the width and height 
		// of the pen to get the effect of a pen width different width and height.
		TSize penSize = iPenSize;
		SetPenSize(TSize(1, 1));
		vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
		vgScale(penSize.iWidth, penSize.iHeight);			
		VGfloat scaleX = 1.0f/(float)penSize.iWidth;
		VGfloat scaleY = 1.0f/(float)penSize.iHeight;
		
		TReal originX = aRect.iTl.iX + (static_cast<TReal>(aRect.Width())*0.5);
		TReal originY = aRect.iTl.iY + (static_cast<TReal>(aRect.Height())*0.5);
		TReal startAngle = GetAngleFromXAxisAnticlockwise(originX, originY, aStart.iX, aStart.iY, aRect.Width(), aRect.Height());
		TReal endAngle = GetAngleFromXAxisAnticlockwise(originX, originY, aEnd.iX, aEnd.iY, aRect.Width(), aRect.Height());		
		TReal extent = endAngle - startAngle;
		
		// The extent defines what direction the arc is drawn in, so make sure we always draw
		// anti-clockwise (-ve extent)
		if (extent > 0.0f)
			extent -= 360.0f;
		
		// If the start and end points are the same, make sure we draw arc all the way round the ellipse
		if ((aStart == aEnd) || (extent > -0.0001f))
			extent = -360.0f;
		
		// Before any vgu command, call SetError() as this stores the current vg error state (if any) in the 
		// driver. Some implementations of vgu clears error state so we'd lose error codes otherwise.
		iDriver.SetError(KErrNone);
		
		if (PreparePath(iVgPath, 5))
			{
			VGUErrorCode err = vguArc(iVgPath, 
				(originX + 0.5f) * scaleX, 
				(originY + 0.5f) * scaleY, 
				(float)aRect.Width() * scaleX, 
				(float)aRect.Height() * scaleY, 
				startAngle, 
				extent, 
				aArcType);
			
			if (err == VGU_NO_ERROR)
				{	
				for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
					vgDrawPath(iVgPath, VG_STROKE_PATH);				
				}
			else
				{
				SetVguError(err);
				}
			}
		
		ResetVgMatrix(VG_MATRIX_PATH_USER_TO_SURFACE);
		SetPenSize(penSize);
		}
	
	}

/**
Helper function for drawing a source as a CDirectGdiImageSource.
 
@see DrawResource(const TRect&, const RDirectGdiDrawableSource&, DirectGdi::TGraphicsRotation).
 */
void CVgEngine::DoDrawResource(const TRect& aDestRect,
					CDirectGdiImageSourceImpl* aSource,
					DirectGdi::TGraphicsRotation aRotation)

	{
	TSize size = aSource->Size();
	TRect srcRect(0, 0, size.iWidth, size.iHeight);
	
	DoDrawResource(aDestRect, aSource, srcRect, aRotation);
	}

/**
Helper function for drawing a source as a CDirectGdiImageSource. 

@see DrawResource(const TRect&, const RDirectGdiDrawableSource&, const TRect&, DirectGdi::TGraphicsRotation). 
 */
void CVgEngine::DoDrawResource(const TRect& aDestRect,
					CDirectGdiImageSourceImpl* aSource,
					const TRect& aSourceRect,
					DirectGdi::TGraphicsRotation aRotation)
	{				 
	// If the source rect is smaller than the actual source size then we need to create a child VGImage to draw
	VGImage vgImage = aSource->VgImage();
	TSize size = aSource->Size();
	if ((aSourceRect.Width() < size.iWidth) || (aSourceRect.Height() < size.iHeight))
		{
		vgImage = vgChildImage(vgImage, aSourceRect.iTl.iX, aSourceRect.iTl.iY, aSourceRect.Width(), aSourceRect.Height());
		}
		
	if (vgImage != VG_INVALID_HANDLE)
		{
		vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);	
		
		TRect destRect(aDestRect);
		TRect sourceRect(aSourceRect);

		if (aRotation == DirectGdi::EGraphicsRotationNone ||
			aRotation == DirectGdi::EGraphicsRotation180)
			{
			// Pixel-data in EGLImages appears to be upside down due to the Y-inversion 
			// effect of the Identity matrix.  Therefore must undo the Y-inversion here 
			// and adjust destination rect accordingly.			
			const TInt destRectHeight = aDestRect.Height();
			destRect.iTl.iY = (iSize.iHeight - aDestRect.iTl.iY - iOrigin.iY) - destRectHeight;
			destRect.iTl.iX += iOrigin.iX;
			destRect.iBr.iX += iOrigin.iX;
			destRect.iBr.iY = destRect.iTl.iY + destRectHeight;
			sourceRect.iBr.iY = aSource->Size().iHeight - sourceRect.iTl.iY;
			sourceRect.iTl.iY = sourceRect.iBr.iY - aSourceRect.Height();
			vgLoadIdentity();
			}
		else
			{
			// But if rotation is 90 or 270 degrees, only need to mirror in the X-axis.
			vgScale(-1, 1);
			}

		VGfloat xScale = 1.f;
		VGfloat yScale = 1.f;				
		VGint posX = destRect.iTl.iX;
		VGint posY = destRect.iTl.iY;		
					
		switch (aRotation)
			{									
			case DirectGdi::EGraphicsRotation90:
				xScale = ((VGfloat)destRect.Width()/(VGfloat)sourceRect.Height());
				yScale = ((VGfloat)destRect.Height()/(VGfloat)sourceRect.Width());																	
				vgTranslate(-posX, posY);
				vgScale(xScale, yScale);
				vgRotate(90.0f);					
				break;
			case DirectGdi::EGraphicsRotation180:								
				xScale = ((VGfloat)destRect.Width()/(VGfloat)sourceRect.Width());
				yScale = ((VGfloat)destRect.Height()/(VGfloat)sourceRect.Height());
				vgTranslate(posX+destRect.Width(), posY+destRect.Height());
				vgScale(xScale, yScale);
				vgRotate(180.0f);												
				break;
			case DirectGdi::EGraphicsRotation270:
				xScale = ((VGfloat)destRect.Width()/(VGfloat)sourceRect.Height());
				yScale = ((VGfloat)destRect.Height()/(VGfloat)sourceRect.Width());
				vgTranslate(-posX-destRect.Width(), (posY+destRect.Height()));
				vgScale(xScale, yScale);
				vgRotate(270.0f);
				break;
			default: // DirectGdi::EGraphicsRotationNone
				xScale = ((VGfloat)destRect.Width()/(VGfloat)sourceRect.Width());
				yScale = ((VGfloat)destRect.Height()/(VGfloat)sourceRect.Height());
				vgTranslate(posX, posY);
				vgScale(xScale, yScale);
				break;
			}																							
		
		for (iRegionManager.Begin(); iRegionManager.ApplyClipRegion(); iRegionManager.Next())
			vgDrawImage(vgImage);
		
		if (vgImage != aSource->VgImage())
			{
			// Created a child VGImage, so destroy after use.
			vgDestroyImage(vgImage);
			}
		
		ResetVgMatrix(VG_MATRIX_IMAGE_USER_TO_SURFACE);
		}
	}

/**
Maps a VGU error code to a Symbian OS error code, and sets the driver's error state. If the 
error is unrecognised, the error is set to KErrGeneral.

@param aErr The return value (error state) from a VGU command.
@post If empty, the driver's error state is updated to the mapped error code.
*/
void CVgEngine::SetVguError(VGUErrorCode aErr)
	{
	switch(aErr)
		{
		case VGU_NO_ERROR:
			break;
		case VGU_OUT_OF_MEMORY_ERROR:
			iDriver.SetError(KErrNoMemory);
			break;
		case VGU_BAD_HANDLE_ERROR:
			iDriver.SetError(KErrBadHandle);
			break;
		case VGU_ILLEGAL_ARGUMENT_ERROR:
			iDriver.SetError(KErrArgument);
			break;
		case VGU_PATH_CAPABILITY_ERROR:
			iDriver.SetError(KErrNotSupported);
			break;
		default:
			iDriver.SetError(KErrGeneral);
			break;
		}
	}	

/**
Helper method for creating a VGImage. This method clears the VG image cache in an attempt to  
reclaim some memory if creation of a VGImage fails due to no memory being available, 
it then tries to create the image again. If image creation fails again it then clears the glyph 
cache and tries to create the image again. If image creation still fails, OpenVG is forced to 
complete all currently outstanding drawing requests, so that any OpenVG objects marked for deletion,
such as VGImages that are currently waiting to be drawn, are freed. This is an attempt to make sure 
that images are still displayed in low memory conditions. Use this method instead of calling 
vgCreateImage() directly. Clearing the VG image cache in this way will have a negative effect on 
performance regarding speed, but it is more important that an attempt is made to draw something 
when memory is low.

@param aFormat The pixel format of the image to be created
@param aWidth The width of the image to be created
@param aHeight The height of the image to be created
@param aAllowedQuality One of the VGImageQuality flags

@return A VGImage handle if the image was created successfully, VG_INVALID_HANDLE otherwise 
 */
VGImage CVgEngine::DoVgCreateImage(VGImageFormat aFormat, VGint aWidth, VGint aHeight, VGbitfield aAllowedQuality)
	{
	const TInt oldVgError = CDirectGdiDriverImpl::GetVgError();
	VGImage imageRet = vgCreateImage(aFormat, aWidth, aHeight, aAllowedQuality);
	
	if (imageRet == VG_INVALID_HANDLE)
		{
		// If the new error is anything other than KErrNoMemory, there is nothing that can be done.
		TInt newVgError = CDirectGdiDriverImpl::GetVgError();		
		if (newVgError != KErrNoMemory)
			{
			iDriver.SetError(oldVgError != KErrNone ? oldVgError : newVgError);
			return imageRet;
			}

		// From here on, we are assuming any failure to create the image is due to OOM.
		if (iDriver.VgImageCache())
			{
			// Delete all the images that are currently in the cache then try and create the image again
			iDriver.VgImageCache()->ResetCache();
			imageRet = vgCreateImage(aFormat, aWidth, aHeight, aAllowedQuality);
			}
		if ((imageRet == VG_INVALID_HANDLE) && iFontGlyphImageStorage)
			{
			// Clear the glyph cache as well then try and create the image again
			iFontGlyphImageStorage->CleanGlyphImageCache();
			imageRet = vgCreateImage(aFormat, aWidth, aHeight, aAllowedQuality);
			}
		if ((imageRet == VG_INVALID_HANDLE))
			{
			// Finally, force completion of any outstanding drawing, may free any VGImages marked for deletion.
			// Empty the current OpenVG error state before calling Finish(), as Finish() may call SetError(),
			// and could prematurely set the driver error state to KErrNoMemory.
			//coverity[check_return]
			//coverity[unchecked_value]
			vgGetError();
			iDriver.Finish();
			imageRet = vgCreateImage(aFormat, aWidth, aHeight, aAllowedQuality);
			}

		// If the above worked, empty any OpenVG error state set by any failed attempts to create the image.
		if (imageRet != VG_INVALID_HANDLE)
			{
			//coverity[check_return]
			//coverity[unchecked_value]
			vgGetError();
			}
		}
	
	// Reset the error code to the original VG error code. If oldVgError is KErrNone, 
	// SetError() will use the current OpenVG error state.
	iDriver.SetError(oldVgError);
	
	return imageRet;
	}

/**
Helper method for setting the colour property of a VGPaint object from a TRgb structure.

@param aPaint The VGPaint object to change the colour property of.
@param aColor The colour to set the paint to.
*/ 
void CVgEngine::SetVgPaintColor(VGPaint& aPaint, const TRgb& aColor)
	{
	// Need to swap from internal ARGB to RGBA for OpenVG.
	const TUint argb = aColor.Internal();
	const VGuint rgba = ((argb & 0xFFFFFF) << 8) + ((argb & 0xFF000000) >> 24);
	vgSetColor(aPaint, rgba);
	}
