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
* Implementation of OpenVG interface
*/
#include <openvg.h>
#include "openvginternal.h"

extern "C" {

EXPORT_C VGErrorCode   vgGetError(void)
   {
   return do_vgGetError();
   }

EXPORT_C void   vgFlush(void)
   {
   return do_vgFlush();
   }

EXPORT_C void   vgFinish(void)
   {
   return do_vgFinish();
   }

EXPORT_C void  vgSetf (VGParamType type, VGfloat value)
   {
   return do_vgSetf(type,value);
   }

EXPORT_C void   vgSeti (VGParamType type, VGint value)
   {
   return do_vgSeti(type,value);
   }

EXPORT_C void   vgSetfv(VGParamType type, VGint count ,const VGfloat* values)
   {
   return do_vgSetfv(type,count,values);
   }

EXPORT_C void   vgSetiv(VGParamType type, VGint count ,const VGint* values)
   {
   return do_vgSetiv(type,count,values);
   }

EXPORT_C VGfloat   vgGetf(VGParamType type)
   {
   return do_vgGetf(type);
   }

EXPORT_C VGint     vgGeti(VGParamType type)
   {
   return do_vgGeti(type);
   }

EXPORT_C VGint     vgGetVectorSize(VGParamType type)
   {
   return do_vgGetVectorSize(type);
   }

EXPORT_C void      vgGetfv(VGParamType type, VGint count ,VGfloat* values)
   {
   return do_vgGetfv(type,count,values);
   }

EXPORT_C void      vgGetiv(VGParamType type, VGint count ,VGint* values)
   {
   return do_vgGetiv(type,count,values);
   }

EXPORT_C void   vgSetParameterf(VGHandle object ,VGint paramType ,VGfloat value)
   {
   return do_vgSetParameterf(object,paramType,value);
   }

EXPORT_C void   vgSetParameteri(VGHandle object ,VGint paramType ,VGint value)
   {
   return do_vgSetParameteri(object,paramType,value);
   }

EXPORT_C void   vgSetParameterfv(VGHandle object ,VGint paramType ,VGint count ,const VGfloat* values)
   {
   return do_vgSetParameterfv(object,paramType,count,values);
   }

EXPORT_C void   vgSetParameteriv(VGHandle object ,VGint paramType ,VGint count ,const VGint* values)
   {
   return do_vgSetParameteriv(object,paramType,count,values);
   }

EXPORT_C VGfloat   vgGetParameterf(VGHandle object ,VGint paramType)
   {
   return do_vgGetParameterf(object,paramType);
   }

EXPORT_C VGint   vgGetParameteri(VGHandle object ,VGint paramType)
   {
   return do_vgGetParameteri(object,paramType);
   }

EXPORT_C VGint   vgGetParameterVectorSize(VGHandle object ,VGint paramType)
   {
   return do_vgGetParameterVectorSize(object,paramType);
   }

EXPORT_C void   vgGetParameterfv(VGHandle object ,VGint paramType ,VGint count ,VGfloat* values)
   {
   return do_vgGetParameterfv(object,paramType,count,values);
   }

EXPORT_C void   vgGetParameteriv(VGHandle object ,VGint paramType ,VGint count ,VGint* values)
   {
   return do_vgGetParameteriv(object,paramType,count,values);
   }

EXPORT_C void   vgLoadIdentity(void)
   {
   return do_vgLoadIdentity();
   }

EXPORT_C void   vgLoadMatrix(const VGfloat* m)
   {
   return do_vgLoadMatrix(m);
   }

EXPORT_C void   vgGetMatrix(VGfloat* m)
   {
   return do_vgGetMatrix(m);
   }

EXPORT_C void   vgMultMatrix(const  VGfloat* m)
   {
   return do_vgMultMatrix(m);
   }

EXPORT_C void   vgTranslate(VGfloat tx, VGfloat ty)
   {
   return do_vgTranslate(tx,ty);
   }

EXPORT_C void   vgScale(VGfloat sx, VGfloat sy)
   {
   return do_vgScale(sx,sy);
   }

EXPORT_C void   vgShear(VGfloat shx, VGfloat shy)
   {
   return do_vgShear(shx,shy);
   }

EXPORT_C void   vgRotate(VGfloat angle)
   {
   return do_vgRotate(angle);
   }

EXPORT_C void   vgMask(VGHandle mask, VGMaskOperation operation ,VGint x, VGint y ,VGint width, VGint height)
   {
   return do_vgMask(mask,operation,x,y,width,height);
   }

EXPORT_C void   vgRenderToMask(VGPath path ,VGbitfield paintModes ,VGMaskOperation operation)
   {
   return do_vgRenderToMask(path,paintModes,operation);
   }

EXPORT_C VGMaskLayer   vgCreateMaskLayer(VGint width, VGint height)
   {
   return do_vgCreateMaskLayer(width,height);
   }

EXPORT_C void   vgDestroyMaskLayer(VGMaskLayer maskLayer)
   {
   return do_vgDestroyMaskLayer(maskLayer);
   }

EXPORT_C void   vgFillMaskLayer(VGMaskLayer maskLayer ,VGint x, VGint y ,VGint width, VGint height ,VGfloat value)
   {
   return do_vgFillMaskLayer(maskLayer,x,y,width,height,value);
   }

EXPORT_C void   vgCopyMask(VGMaskLayer maskLayer ,VGint sx, VGint sy ,VGint dx, VGint dy ,VGint width, VGint height)
   {
   return do_vgCopyMask(maskLayer,sx,sy,dx,dy,width,height);
   }

EXPORT_C void   vgClear(VGint x, VGint y ,VGint width, VGint height)
   {
   return do_vgClear(x,y,width,height);
   }

EXPORT_C VGPath   vgCreatePath(VGint pathFormat ,VGPathDatatype datatype ,VGfloat scale, VGfloat bias ,VGint segmentCapacityHint ,VGint coordCapacityHint ,VGbitfield capabilities)
   {
   return do_vgCreatePath(pathFormat,datatype,scale,bias,segmentCapacityHint,coordCapacityHint,capabilities);
   }

EXPORT_C void   vgClearPath(VGPath path, VGbitfield capabilities)
   {
   return do_vgClearPath(path,capabilities);
   }

EXPORT_C void   vgDestroyPath(VGPath path)
   {
   return do_vgDestroyPath(path);
   }

EXPORT_C void   vgRemovePathCapabilities(VGPath path ,VGbitfield capabilities)
   {
   return do_vgRemovePathCapabilities(path,capabilities);
   }

EXPORT_C VGbitfield   vgGetPathCapabilities(VGPath path)
   {
   return do_vgGetPathCapabilities(path);
   }

EXPORT_C void   vgAppendPath(VGPath dstPath, VGPath srcPath)
   {
   return do_vgAppendPath(dstPath,srcPath);
   }

EXPORT_C void   vgAppendPathData(VGPath dstPath ,VGint numSegments ,const  VGubyte* pathSegments ,const  void* pathData)
   {
   return do_vgAppendPathData(dstPath,numSegments,pathSegments,pathData);
   }

EXPORT_C void   vgModifyPathCoords(VGPath dstPath ,VGint startIndex ,VGint numSegments ,const void* pathData)
   {
   return do_vgModifyPathCoords(dstPath,startIndex,numSegments,pathData);
   }

EXPORT_C void   vgTransformPath(VGPath dstPath, VGPath srcPath)
   {
   return do_vgTransformPath(dstPath,srcPath);
   }

EXPORT_C VGboolean   vgInterpolatePath(VGPath dstPath ,VGPath startPath ,VGPath endPath ,VGfloat amount)
   {
   return do_vgInterpolatePath(dstPath,startPath,endPath,amount);
   }

EXPORT_C VGfloat   vgPathLength(VGPath path ,VGint startSegment ,VGint numSegments)
   {
   return do_vgPathLength(path,startSegment,numSegments);
   }

EXPORT_C void   vgPointAlongPath(VGPath path ,VGint startSegment ,VGint numSegments ,VGfloat distance ,VGfloat* x, VGfloat* y ,VGfloat* tangentX ,VGfloat* tangentY)
   {
   return do_vgPointAlongPath(path,startSegment,numSegments,distance,x,y,tangentX,tangentY);
   }

EXPORT_C void   vgPathBounds(VGPath path ,VGfloat* minX ,VGfloat* minY ,VGfloat* width ,VGfloat* height)
   {
   return do_vgPathBounds(path,minX,minY,width,height);
   }

EXPORT_C void   vgPathTransformedBounds(VGPath path ,VGfloat* minX ,VGfloat* minY ,VGfloat* width ,VGfloat* height)
   {
   return do_vgPathTransformedBounds(path,minX,minY,width,height);
   }

EXPORT_C void   vgDrawPath(VGPath path, VGbitfield paintModes)
   {
   return do_vgDrawPath(path,paintModes);
   }

EXPORT_C VGPaint   vgCreatePaint(void)
   {
   return do_vgCreatePaint();
   }

EXPORT_C void   vgDestroyPaint(VGPaint paint)
   {
   return do_vgDestroyPaint(paint);
   }

EXPORT_C void   vgSetPaint(VGPaint paint, VGbitfield paintModes)
   {
   return do_vgSetPaint(paint,paintModes);
   }

EXPORT_C VGPaint   vgGetPaint(VGPaintMode paintMode)
   {
   return do_vgGetPaint(paintMode);
   }

EXPORT_C void   vgSetColor(VGPaint paint, VGuint rgba)
   {
   return do_vgSetColor(paint,rgba);
   }

EXPORT_C VGuint   vgGetColor(VGPaint paint)
   {
   return do_vgGetColor(paint);
   }

EXPORT_C void   vgPaintPattern(VGPaint paint, VGImage pattern)
   {
   return do_vgPaintPattern(paint,pattern);
   }

EXPORT_C VGImage   vgCreateImage(VGImageFormat format ,VGint width, VGint height ,VGbitfield allowedQuality)
   {
   return do_vgCreateImage(format,width,height,allowedQuality);
   }

EXPORT_C void   vgDestroyImage(VGImage image)
   {
   return do_vgDestroyImage(image);
   }

EXPORT_C void   vgClearImage(VGImage image ,VGint x, VGint y ,VGint width, VGint height)
   {
   return do_vgClearImage(image,x,y,width,height);
   }

EXPORT_C void   vgImageSubData(VGImage image ,const void* data ,VGint dataStride ,VGImageFormat dataFormat ,VGint x, VGint y ,VGint width, VGint height)
   {
   return do_vgImageSubData(image,data,dataStride,dataFormat,x,y,width,height);
   }

EXPORT_C void   vgGetImageSubData(VGImage image ,void* data ,VGint dataStride ,VGImageFormat dataFormat ,VGint x, VGint y ,VGint width, VGint height)
   {
   return do_vgGetImageSubData(image,data,dataStride,dataFormat,x,y,width,height);
   }

EXPORT_C VGImage   vgChildImage(VGImage parent ,VGint x, VGint y ,VGint width, VGint height)
   {
   return do_vgChildImage(parent,x,y,width,height);
   }

EXPORT_C VGImage   vgGetParent(VGImage image)
   {
   return do_vgGetParent(image);
   }

EXPORT_C void   vgCopyImage(VGImage dst, VGint dx, VGint dy ,VGImage src, VGint sx, VGint sy ,VGint width, VGint height ,VGboolean dither)
   {
   return do_vgCopyImage(dst,dx,dy,src,sx,sy,width,height,dither);
   }

EXPORT_C void   vgDrawImage(VGImage image)
   {
   return do_vgDrawImage(image);
   }

EXPORT_C void   vgSetPixels(VGint dx, VGint dy ,VGImage src, VGint sx, VGint sy ,VGint width, VGint height)
   {
   return do_vgSetPixels(dx,dy,src,sx,sy,width,height);
   }

EXPORT_C void   vgWritePixels(const void* data, VGint dataStride ,VGImageFormat dataFormat ,VGint dx, VGint dy ,VGint width, VGint height)
   {
   return do_vgWritePixels(data,dataStride,dataFormat,dx,dy,width,height);
   }

EXPORT_C void   vgGetPixels(VGImage dst, VGint dx, VGint dy ,VGint sx, VGint sy ,VGint width, VGint height)
   {
   return do_vgGetPixels(dst,dx,dy,sx,sy,width,height);
   }

EXPORT_C void   vgReadPixels(void* data, VGint dataStride ,VGImageFormat dataFormat ,VGint sx, VGint sy ,VGint width, VGint height)
   {
   return do_vgReadPixels(data,dataStride,dataFormat,sx,sy,width,height);
   }

EXPORT_C void   vgCopyPixels(VGint dx, VGint dy ,VGint sx, VGint sy ,VGint width, VGint height)
   {
   return do_vgCopyPixels(dx,dy,sx,sy,width,height);
   }

EXPORT_C VGFont   vgCreateFont(VGint glyphCapacityHint)
   {
   return do_vgCreateFont(glyphCapacityHint);
   }

EXPORT_C void   vgDestroyFont(VGFont font)
   {
   return do_vgDestroyFont(font);
   }

EXPORT_C void   vgSetGlyphToPath(VGFont font ,VGuint glyphIndex ,VGPath path ,VGboolean isHinted ,const VGfloat glyphOrigin [2] ,const VGfloat escapement[2])
   {
   return do_vgSetGlyphToPath(font,glyphIndex,path,isHinted,glyphOrigin,escapement);
   }

EXPORT_C void   vgSetGlyphToImage(VGFont font ,VGuint glyphIndex ,VGImage image ,const VGfloat glyphOrigin [2] ,const VGfloat escapement[2])
   {
   return do_vgSetGlyphToImage(font,glyphIndex,image,glyphOrigin,escapement);
   }

EXPORT_C void   vgClearGlyph(VGFont font ,VGuint glyphIndex)
   {
   return do_vgClearGlyph(font,glyphIndex);
   }

EXPORT_C void   vgDrawGlyph(VGFont font ,VGuint glyphIndex ,VGbitfield paintModes ,VGboolean allowAutoHinting)
   {
   return do_vgDrawGlyph(font,glyphIndex,paintModes,allowAutoHinting);
   }

EXPORT_C void   vgDrawGlyphs(VGFont font ,VGint glyphCount ,const VGuint* glyphIndices ,const VGfloat* adjustments_x ,const VGfloat* adjustments_y ,VGbitfield paintModes ,VGboolean allowAutoHinting)
   {
   return do_vgDrawGlyphs(font,glyphCount,glyphIndices,adjustments_x,adjustments_y,paintModes,allowAutoHinting);
   }

EXPORT_C void   vgColorMatrix(VGImage dst, VGImage src ,const VGfloat* matrix)
   {
   return do_vgColorMatrix(dst,src,matrix);
   }

EXPORT_C void   vgConvolve(VGImage dst, VGImage src ,VGint kernelWidth, VGint kernelHeight ,VGint shiftX, VGint shiftY ,const VGshort* kernel ,VGfloat scale ,VGfloat bias ,VGTilingMode tilingMode)
   {
   return do_vgConvolve(dst,src,kernelWidth,kernelHeight,shiftX,shiftY,kernel,scale,bias,tilingMode);
   }

EXPORT_C void   vgSeparableConvolve(VGImage dst, VGImage src ,VGint kernelWidth ,VGint kernelHeight ,VGint shiftX, VGint shiftY ,const VGshort* kernelX ,const VGshort* kernelY ,VGfloat scale ,VGfloat bias ,VGTilingMode tilingMode)
   {
   return do_vgSeparableConvolve(dst,src,kernelWidth,kernelHeight,shiftX,shiftY,kernelX,kernelY,scale,bias,tilingMode);
   }

EXPORT_C void   vgGaussianBlur(VGImage dst, VGImage src ,VGfloat stdDeviationX ,VGfloat stdDeviationY ,VGTilingMode tilingMode)
   {
   return do_vgGaussianBlur(dst,src,stdDeviationX,stdDeviationY,tilingMode);
   }

EXPORT_C void   vgLookup(VGImage dst, VGImage src ,const VGubyte* redLUT ,const VGubyte* greenLUT ,const VGubyte* blueLUT ,const VGubyte* alphaLUT ,VGboolean outputLinear ,VGboolean outputPremultiplied)
   {
   return do_vgLookup(dst,src,redLUT,greenLUT,blueLUT,alphaLUT,outputLinear,outputPremultiplied);
   }

EXPORT_C void   vgLookupSingle(VGImage dst, VGImage src ,const VGuint* lookupTable ,VGImageChannel sourceChannel ,VGboolean outputLinear ,VGboolean outputPremultiplied)
   {
   return do_vgLookupSingle(dst,src,lookupTable,sourceChannel,outputLinear,outputPremultiplied);
   }

EXPORT_C VGHardwareQueryResult   vgHardwareQuery        (VGHardwareQueryType key ,VGint setting)
   {
   return do_vgHardwareQuery(key,setting);
   }

EXPORT_C const VGubyte *   vgGetString(VGStringID name)
   {
   return do_vgGetString(name);
   }
}

