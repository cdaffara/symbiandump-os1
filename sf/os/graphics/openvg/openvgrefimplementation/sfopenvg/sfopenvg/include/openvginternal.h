/*
* Copyright (c) 2009 Symbian Foundation Ltd
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Symbian Foundation Ltd - initial contribution.
* 
* Contributors:
*
* Description:
* Implementation of a OpenVG like interface. Basically every OpenVG API names but prefixed with do_ e.g. do_vgSetf
*/

#ifndef OPENVGINTERNAL_H
#define OPENVGINTERNAL_H

#include <openvg.h>

/*
 * defines set of internal vgu functions that are 1:1 with the published openvg functions. 
 * E.g. vgSetf -> do_vgSetf
 * 
 * These do_XXX fucntions are called from the libopenvg.dll (which is a shim) to be processed here
 */

IMPORT_C VGErrorCode do_vgGetError(void);
IMPORT_C void do_vgFlush(void);
IMPORT_C void do_vgFinish(void);
IMPORT_C void do_vgSetf(VGParamType type, VGfloat value);
IMPORT_C void do_vgSeti(VGParamType type, VGint value);
IMPORT_C void do_vgSetfv(VGParamType type, VGint count ,const VGfloat* values);
IMPORT_C void do_vgSetiv(VGParamType type, VGint count ,const VGint* values);
IMPORT_C VGfloat do_vgGetf(VGParamType type);
IMPORT_C VGint do_vgGeti(VGParamType type);
IMPORT_C VGint do_vgGetVectorSize(VGParamType type);
IMPORT_C void do_vgGetfv(VGParamType type, VGint count ,VGfloat* values);
IMPORT_C void do_vgGetiv(VGParamType type, VGint count ,VGint* values);
IMPORT_C void do_vgSetParameterf(VGHandle object ,VGint paramType ,VGfloat value);
IMPORT_C void do_vgSetParameteri(VGHandle object ,VGint paramType ,VGint value);
IMPORT_C void do_vgSetParameterfv(VGHandle object ,VGint paramType ,VGint count ,const VGfloat* values);
IMPORT_C void do_vgSetParameteriv(VGHandle object ,VGint paramType ,VGint count ,const VGint* values);
IMPORT_C VGfloat do_vgGetParameterf(VGHandle object ,VGint paramType);
IMPORT_C VGint do_vgGetParameteri(VGHandle object ,VGint paramType);
IMPORT_C VGint do_vgGetParameterVectorSize(VGHandle object ,VGint paramType);
IMPORT_C void do_vgGetParameterfv(VGHandle object ,VGint paramType ,VGint count ,VGfloat* values);
IMPORT_C void do_vgGetParameteriv(VGHandle object ,VGint paramType ,VGint count ,VGint* values);
IMPORT_C void do_vgLoadIdentity(void);
IMPORT_C void do_vgLoadMatrix(const VGfloat* m);
IMPORT_C void do_vgGetMatrix(VGfloat* m);
IMPORT_C void do_vgMultMatrix(const  VGfloat* m);
IMPORT_C void do_vgTranslate(VGfloat tx, VGfloat ty);
IMPORT_C void do_vgScale(VGfloat sx, VGfloat sy);
IMPORT_C void do_vgShear(VGfloat shx, VGfloat shy);
IMPORT_C void do_vgRotate(VGfloat angle);
IMPORT_C void do_vgMask(VGHandle mask, VGMaskOperation operation ,VGint x, VGint y ,VGint width, VGint height);
IMPORT_C void do_vgRenderToMask(VGPath path ,VGbitfield paintModes ,VGMaskOperation operation);
IMPORT_C VGMaskLayer do_vgCreateMaskLayer(VGint width, VGint height);
IMPORT_C void do_vgDestroyMaskLayer(VGMaskLayer maskLayer);
IMPORT_C void do_vgFillMaskLayer(VGMaskLayer maskLayer ,VGint x, VGint y ,VGint width, VGint height ,VGfloat value);
IMPORT_C void do_vgCopyMask(VGMaskLayer maskLayer ,VGint sx, VGint sy ,VGint dx, VGint dy ,VGint width, VGint height);
IMPORT_C void do_vgClear(VGint x, VGint y ,VGint width, VGint height);
IMPORT_C VGPath do_vgCreatePath(VGint pathFormat ,VGPathDatatype datatype ,VGfloat scale, VGfloat bias ,VGint segmentCapacityHint ,VGint coordCapacityHint ,VGbitfield capabilities);
IMPORT_C void do_vgClearPath(VGPath path, VGbitfield capabilities);
IMPORT_C void do_vgDestroyPath(VGPath path);
IMPORT_C void do_vgRemovePathCapabilities(VGPath path ,VGbitfield capabilities);
IMPORT_C VGbitfield do_vgGetPathCapabilities(VGPath path);
IMPORT_C void do_vgAppendPath(VGPath dstPath, VGPath srcPath);
IMPORT_C void do_vgAppendPathData(VGPath dstPath ,VGint numSegments ,const  VGubyte* pathSegments ,const  void* pathData);
IMPORT_C void do_vgModifyPathCoords(VGPath dstPath ,VGint startIndex ,VGint numSegments ,const void* pathData);
IMPORT_C void do_vgTransformPath(VGPath dstPath, VGPath srcPath);
IMPORT_C VGboolean do_vgInterpolatePath(VGPath dstPath ,VGPath startPath ,VGPath endPath ,VGfloat amount);
IMPORT_C VGfloat do_vgPathLength(VGPath path ,VGint startSegment ,VGint numSegments);
IMPORT_C void do_vgPointAlongPath(VGPath path ,VGint startSegment ,VGint numSegments ,VGfloat distance ,VGfloat* x, VGfloat* y ,VGfloat* tangentX ,VGfloat* tangentY);
IMPORT_C void do_vgPathBounds(VGPath path ,VGfloat* minX ,VGfloat* minY ,VGfloat* width ,VGfloat* height);
IMPORT_C void do_vgPathTransformedBounds(VGPath path ,VGfloat* minX ,VGfloat* minY ,VGfloat* width ,VGfloat* height);
IMPORT_C void do_vgDrawPath(VGPath path, VGbitfield paintModes);
IMPORT_C VGPaint do_vgCreatePaint(void);
IMPORT_C void do_vgDestroyPaint(VGPaint paint);
IMPORT_C void do_vgSetPaint(VGPaint paint, VGbitfield paintModes);
IMPORT_C VGPaint do_vgGetPaint(VGPaintMode paintMode);
IMPORT_C void do_vgSetColor(VGPaint paint, VGuint rgba);
IMPORT_C VGuint do_vgGetColor(VGPaint paint);
IMPORT_C void do_vgPaintPattern(VGPaint paint, VGImage pattern);
IMPORT_C VGImage do_vgCreateImage(VGImageFormat format ,VGint width, VGint height ,VGbitfield allowedQuality);
IMPORT_C void do_vgDestroyImage(VGImage image);
IMPORT_C void do_vgClearImage(VGImage image ,VGint x, VGint y ,VGint width, VGint height);
IMPORT_C void do_vgImageSubData(VGImage image ,const void* data ,VGint dataStride ,VGImageFormat dataFormat ,VGint x, VGint y ,VGint width, VGint height);
IMPORT_C void do_vgGetImageSubData(VGImage image ,void* data ,VGint dataStride ,VGImageFormat dataFormat ,VGint x, VGint y ,VGint width, VGint height);
IMPORT_C VGImage do_vgChildImage(VGImage parent ,VGint x, VGint y ,VGint width, VGint height);
IMPORT_C VGImage do_vgGetParent(VGImage image);
IMPORT_C void do_vgCopyImage(VGImage dst, VGint dx, VGint dy ,VGImage src, VGint sx, VGint sy ,VGint width, VGint height ,VGboolean dither);
IMPORT_C void do_vgDrawImage(VGImage image);
IMPORT_C void do_vgSetPixels(VGint dx, VGint dy ,VGImage src, VGint sx, VGint sy ,VGint width, VGint height);
IMPORT_C void do_vgWritePixels(const void* data, VGint dataStride ,VGImageFormat dataFormat ,VGint dx, VGint dy ,VGint width, VGint height);
IMPORT_C void do_vgGetPixels(VGImage dst, VGint dx, VGint dy ,VGint sx, VGint sy ,VGint width, VGint height);
IMPORT_C void do_vgReadPixels(void* data, VGint dataStride ,VGImageFormat dataFormat ,VGint sx, VGint sy ,VGint width, VGint height);
IMPORT_C void do_vgCopyPixels(VGint dx, VGint dy ,VGint sx, VGint sy ,VGint width, VGint height);
IMPORT_C VGFont do_vgCreateFont(VGint glyphCapacityHint);
IMPORT_C void do_vgDestroyFont(VGFont font);
IMPORT_C void do_vgSetGlyphToPath(VGFont font ,VGuint glyphIndex ,VGPath path ,VGboolean isHinted ,const VGfloat glyphOrigin [2] ,const VGfloat escapement[2]);
IMPORT_C void do_vgSetGlyphToImage(VGFont font ,VGuint glyphIndex ,VGImage image ,const VGfloat glyphOrigin [2] ,const VGfloat escapement[2]);
IMPORT_C void do_vgClearGlyph(VGFont font ,VGuint glyphIndex);
IMPORT_C void do_vgDrawGlyph(VGFont font ,VGuint glyphIndex ,VGbitfield paintModes ,VGboolean allowAutoHinting);
IMPORT_C void do_vgDrawGlyphs(VGFont font ,VGint glyphCount ,const VGuint* glyphIndices ,const VGfloat* adjustments_x ,const VGfloat* adjustments_y ,VGbitfield paintModes ,VGboolean allowAutoHinting);
IMPORT_C void do_vgColorMatrix(VGImage dst, VGImage src ,const VGfloat* matrix);
IMPORT_C void do_vgConvolve(VGImage dst, VGImage src ,VGint kernelWidth, VGint kernelHeight ,VGint shiftX, VGint shiftY ,const VGshort* kernel ,VGfloat scale ,VGfloat bias ,VGTilingMode tilingMode);
IMPORT_C void do_vgSeparableConvolve(VGImage dst, VGImage src ,VGint kernelWidth ,VGint kernelHeight ,VGint shiftX, VGint shiftY ,const VGshort* kernelX ,const VGshort* kernelY ,VGfloat scale ,VGfloat bias ,VGTilingMode tilingMode);
IMPORT_C void do_vgGaussianBlur(VGImage dst, VGImage src ,VGfloat stdDeviationX ,VGfloat stdDeviationY ,VGTilingMode tilingMode);
IMPORT_C void do_vgLookup(VGImage dst, VGImage src ,const VGubyte* redLUT ,const VGubyte* greenLUT ,const VGubyte* blueLUT ,const VGubyte* alphaLUT ,VGboolean outputLinear ,VGboolean outputPremultiplied);
IMPORT_C void do_vgLookupSingle(VGImage dst, VGImage src ,const VGuint* lookupTable ,VGImageChannel sourceChannel ,VGboolean outputLinear ,VGboolean outputPremultiplied);
IMPORT_C VGHardwareQueryResult do_vgHardwareQuery(VGHardwareQueryType key ,VGint setting);
IMPORT_C const VGubyte * do_vgGetString(VGStringID name);

#endif
