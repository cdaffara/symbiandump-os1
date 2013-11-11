// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This test stub has been manually created based on the .def file generator
// utility available in ../group/openvg11
//
//

#include <VG/openvg.h>
#include <VG/vgu.h>

#include <e32def.h>
#include <e32uid.h>
    
#ifdef __cplusplus
extern "C" {
#endif


// --------------------------------------------------------------------------------------------------------------------------------------------
// OpenVG VG functions
EXPORT_C void  vgAppendPath(VGPath /*dstPath*/, VGPath /*srcPath*/) { }
EXPORT_C void  vgAppendPathData(VGPath /*dstPath*/, VGint /*numSegments*/, const VGubyte * /*pathSegments*/, const void * /*pathData*/) { }
EXPORT_C VGImage  vgChildImage(VGImage /*parent*/, VGint /*x*/, VGint /*y*/, VGint /*width*/, VGint /*height*/) { return 0; }
EXPORT_C void  vgClear(VGint /*x*/, VGint /*y*/, VGint /*width*/, VGint /*height*/) { }
EXPORT_C void  vgClearGlyph(VGFont /*font*/, VGuint /*glyphIndex*/) { }
EXPORT_C void  vgClearImage(VGImage /*image*/, VGint /*x*/, VGint /*y*/, VGint /*width*/, VGint /*height*/) { }
EXPORT_C void  vgClearPath(VGPath /*path*/, VGbitfield /*capabilities*/) { }
EXPORT_C void  vgColorMatrix(VGImage /*dst*/, VGImage /*src*/, const VGfloat * /*matrix*/) { }
EXPORT_C void  vgConvolve(VGImage /*dst*/, VGImage /*src*/, VGint /*kernelWidth*/, VGint /*kernelHeight*/, VGint /*shiftX*/, VGint /*shiftY*/, const VGshort * /*kernel*/, VGfloat /*scale*/, VGfloat /*bias*/, VGTilingMode /*tilingMode*/) { }
EXPORT_C void  vgCopyImage(VGImage /*dst*/, VGint /*dx*/, VGint /*dy*/, VGImage /*src*/, VGint /*sx*/, VGint /*sy*/, VGint /*width*/, VGint /*height*/, VGboolean /*dither*/) { }
EXPORT_C void  vgCopyMask(VGMaskLayer /*maskLayer*/, VGint /*sx*/, VGint /*sy*/, VGint /*dx*/, VGint /*dy*/, VGint /*width*/, VGint /*height*/) { }
EXPORT_C void  vgCopyPixels(VGint /*dx*/, VGint /*dy*/, VGint /*sx*/, VGint /*sy*/, VGint /*width*/, VGint /*height*/) { }
EXPORT_C VGFont  vgCreateFont(VGint /*glyphCapacityHint*/) { return 0; }
EXPORT_C VGImage  vgCreateImage(VGImageFormat /*format*/, VGint /*width*/, VGint /*height*/, VGbitfield /*allowedQuality*/) { return 0; }
EXPORT_C VGMaskLayer  vgCreateMaskLayer(VGint /*width*/, VGint /*height*/) { return 0; }
EXPORT_C VGPaint  vgCreatePaint(void) { return 0; }
EXPORT_C VGPath  vgCreatePath(VGint /*pathFormat*/, VGPathDatatype /*datatype*/, VGfloat /*scale*/, VGfloat /*bias*/, VGint /*segmentCapacityHint*/, VGint /*coordCapacityHint*/, VGbitfield /*capabilities*/) { return 0; }
EXPORT_C void  vgDestroyFont(VGFont /*font*/) { }
EXPORT_C void  vgDestroyImage(VGImage /*image*/) { }
EXPORT_C void  vgDestroyMaskLayer(VGMaskLayer /*maskLayer*/) { }
EXPORT_C void  vgDestroyPaint(VGPaint /*paint*/) { }
EXPORT_C void  vgDestroyPath(VGPath /*path*/) { }
EXPORT_C void  vgDrawGlyph(VGFont /*font*/, VGuint /*glyphIndex*/, VGbitfield /*paintModes*/, VGboolean /*allowAutoHinting*/) { }
EXPORT_C void  vgDrawGlyphs(VGFont /*font*/, VGint /*glyphCount*/, const VGuint * /*glyphIndices*/, const VGfloat * /*adjustments_x*/, const VGfloat * /*adjustments_y*/, VGbitfield /*paintModes*/, VGboolean /*allowAutoHinting*/) { }
EXPORT_C void  vgDrawImage(VGImage /*image*/) { }
EXPORT_C void  vgDrawPath(VGPath /*path*/, VGbitfield /*paintModes*/) { }
EXPORT_C void  vgFillMaskLayer(VGMaskLayer /*maskLayer*/, VGint /*x*/, VGint /*y*/, VGint /*width*/, VGint /*height*/, VGfloat /*value*/) { }
EXPORT_C void  vgFinish(void) { }
EXPORT_C void  vgFlush(void) { }
EXPORT_C void  vgGaussianBlur(VGImage /*dst*/, VGImage /*src*/, VGfloat /*stdDeviationX*/, VGfloat /*stdDeviationY*/, VGTilingMode /*tilingMode*/) { }
EXPORT_C VGuint  vgGetColor(VGPaint /*paint*/) { return 0; }
EXPORT_C VGErrorCode  vgGetError(void) { return VG_ILLEGAL_ARGUMENT_ERROR; }
EXPORT_C VGfloat  vgGetf(VGParamType /*type*/) { return 0; }
EXPORT_C void  vgGetfv(VGParamType /*type*/, VGint /*count*/, VGfloat * /*values*/) { }
EXPORT_C VGint  vgGeti(VGParamType /*type*/) { return 0; }
EXPORT_C void  vgGetImageSubData(VGImage /*image*/, void * /*data*/, VGint /*dataStride*/, VGImageFormat /*dataFormat*/, VGint /*x*/, VGint /*y*/, VGint /*width*/, VGint /*height*/) { }
EXPORT_C void  vgGetiv(VGParamType /*type*/, VGint /*count*/, VGint * /*values*/) { }
EXPORT_C void  vgGetMatrix(VGfloat * /*m*/) { }
EXPORT_C VGPaint  vgGetPaint(VGPaintMode /*paintMode*/) { return 0; }
EXPORT_C VGfloat  vgGetParameterf(VGHandle /*object*/, VGint /*paramType*/) { return 0; }
EXPORT_C void  vgGetParameterfv(VGHandle /*object*/, VGint /*paramType*/, VGint /*count*/, VGfloat * /*values*/) { }
EXPORT_C VGint  vgGetParameteri(VGHandle /*object*/, VGint /*paramType*/) { return 0; }
EXPORT_C void  vgGetParameteriv(VGHandle /*object*/, VGint /*paramType*/, VGint /*count*/, VGint * /*values*/) { }
EXPORT_C VGint  vgGetParameterVectorSize(VGHandle /*object*/, VGint /*paramType*/) { return 0; }
EXPORT_C VGImage  vgGetParent(VGImage /*image*/) { return 0; }
EXPORT_C VGbitfield  vgGetPathCapabilities(VGPath /*path*/) { return 0; }
EXPORT_C void  vgGetPixels(VGImage /*dst*/, VGint /*dx*/, VGint /*dy*/, VGint /*sx*/, VGint /*sy*/, VGint /*width*/, VGint /*height*/) { }
EXPORT_C const VGubyte *  vgGetString(VGStringID /*name*/) { return NULL; }
EXPORT_C VGint  vgGetVectorSize(VGParamType /*type*/) { return 0; }
EXPORT_C VGHardwareQueryResult  vgHardwareQuery (VGHardwareQueryType /*key*/, VGint /*setting*/) { return VG_HARDWARE_UNACCELERATED; }
EXPORT_C void  vgImageSubData(VGImage /*image*/, const void * /*data*/, VGint /*dataStride*/, VGImageFormat /*dataFormat*/, VGint /*x*/, VGint /*y*/, VGint /*width*/, VGint /*height*/) { }
EXPORT_C VGboolean  vgInterpolatePath(VGPath /*dstPath*/, VGPath /*startPath*/, VGPath /*endPath*/, VGfloat /*amount*/) { return VG_FALSE; }
EXPORT_C void  vgLoadIdentity(void) { }
EXPORT_C void  vgLoadMatrix(const VGfloat * /*m*/) { }
EXPORT_C void  vgLookup(VGImage /*dst*/, VGImage /*src*/, const VGubyte * /*redLUT*/, const VGubyte * /*greenLUT*/, const VGubyte * /*blueLUT*/, const VGubyte * /*alphaLUT*/, VGboolean /*outputLinear*/, VGboolean /*outputPremultiplied*/) { }
EXPORT_C void  vgLookupSingle(VGImage /*dst*/, VGImage /*src*/, const VGuint * /*lookupTable*/, VGImageChannel /*sourceChannel*/, VGboolean /*outputLinear*/, VGboolean /*outputPremultiplied*/) { }
EXPORT_C void  vgMask(VGHandle /*mask*/, VGMaskOperation /*operation*/, VGint /*x*/, VGint /*y*/, VGint /*width*/, VGint /*height*/) { }
EXPORT_C void  vgModifyPathCoords(VGPath /*dstPath*/, VGint /*startIndex*/, VGint /*numSegments*/, const void * /*pathData*/) { }
EXPORT_C void  vgMultMatrix(const VGfloat * /*m*/) { }
EXPORT_C void  vgPaintPattern(VGPaint /*paint*/, VGImage /*pattern*/) { }
EXPORT_C void  vgPathBounds(VGPath /*path*/, VGfloat * /*minX*/, VGfloat * /*minY*/, VGfloat * /*width*/, VGfloat * /*height*/) { }
EXPORT_C VGfloat vgPathLength(VGPath /*path*/, VGint /*startSegment*/, VGint /*numSegments*/) { return 0; }
EXPORT_C void  vgPathTransformedBounds(VGPath /*path*/, VGfloat * /*minX*/, VGfloat * /*minY*/, VGfloat * /*width*/, VGfloat * /*height*/) { }
EXPORT_C void  vgPointAlongPath(VGPath /*path*/, VGint /*startSegment*/, VGint /*numSegments*/, VGfloat /*distance*/, VGfloat * /*x*/, VGfloat * /*y*/, VGfloat * /*tangentX*/, VGfloat * /*tangentY*/) { }
EXPORT_C void  vgReadPixels(void * /*data*/, VGint /*dataStride*/, VGImageFormat /*dataFormat*/, VGint /*sx*/, VGint /*sy*/, VGint /*width*/, VGint /*height*/) { }
EXPORT_C void  vgRemovePathCapabilities(VGPath /*path*/, VGbitfield /*capabilities*/) { }
EXPORT_C void  vgRenderToMask(VGPath /*path*/, VGbitfield /*paintModes*/, VGMaskOperation /*operation*/) { }
EXPORT_C void  vgRotate(VGfloat /*angle*/) { }
EXPORT_C void  vgScale(VGfloat /*sx*/, VGfloat /*sy*/) { }
EXPORT_C void  vgSeparableConvolve(VGImage /*dst*/, VGImage /*src*/, VGint /*kernelWidth*/, VGint /*kernelHeight*/, VGint /*shiftX*/, VGint /*shiftY*/, const VGshort * /*kernelX*/, const VGshort * /*kernelY*/, VGfloat /*scale*/, VGfloat /*bias*/, VGTilingMode /*tilingMode*/) { }
EXPORT_C void  vgSetColor(VGPaint /*paint*/, VGuint /*rgba*/) { }
EXPORT_C void  vgSetf (VGParamType /*type*/, VGfloat /*value*/) { }
EXPORT_C void  vgSetfv(VGParamType /*type*/, VGint /*count*/, const VGfloat * /*values*/) { }
EXPORT_C void  vgSetGlyphToImage(VGFont /*font*/, VGuint /*glyphIndex*/, VGImage /*image*/, const VGfloat /*glyphOrigin*/ [2], const VGfloat /*escapement*/[2]) { }
EXPORT_C void  vgSetGlyphToPath(VGFont /*font*/, VGuint /*glyphIndex*/, VGPath /*path*/, VGboolean /*isHinted*/, const VGfloat /*glyphOrigin*/[2], const VGfloat /*escapement*/[2]) { }
EXPORT_C void  vgSeti (VGParamType /*type*/, VGint /*value*/) { }
EXPORT_C void  vgSetiv(VGParamType /*type*/, VGint /*count*/, const VGint * /*values*/) { }
EXPORT_C void  vgSetPaint(VGPaint /*paint*/, VGbitfield /*paintModes*/) { }
EXPORT_C void  vgSetParameterf(VGHandle /*object*/, VGint /*paramType*/, VGfloat /*value*/) { }
EXPORT_C void  vgSetParameterfv(VGHandle /*object*/, VGint /*paramType*/, VGint /*count*/, const VGfloat * /*values*/) { }
EXPORT_C void  vgSetParameteri(VGHandle /*object*/, VGint /*paramType*/, VGint /*value*/) { }
EXPORT_C void  vgSetParameteriv(VGHandle /*object*/, VGint /*paramType*/, VGint /*count*/, const VGint * /*values*/) { }
EXPORT_C void  vgSetPixels(VGint /*dx*/, VGint /*dy*/, VGImage /*src*/, VGint /*sx*/, VGint /*sy*/, VGint /*width*/, VGint /*height*/) { }
EXPORT_C void  vgShear(VGfloat /*shx*/, VGfloat /*shy*/) { }
EXPORT_C void  vgTransformPath(VGPath /*dstPath*/, VGPath /*srcPath*/) { }
EXPORT_C void  vgTranslate(VGfloat /*tx*/, VGfloat /*ty*/) { }
EXPORT_C void  vgWritePixels(const void * /*data*/, VGint /*dataStride*/, VGImageFormat /*dataFormat*/, VGint /*dx*/, VGint /*dy*/, VGint /*width*/, VGint /*height*/) { }




// --------------------------------------------------------------------------------------------------------------------------------------------
// END: OpenVG VG
// --------------------------------------------------------------------------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif
