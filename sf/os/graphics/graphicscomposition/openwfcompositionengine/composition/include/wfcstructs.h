/* Copyright (c) 2009 The Khronos Group Inc.
 * Portions copyright (c) 2009-2010  Nokia Corporation and/or its subsidiary(-ies)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */


/*! \ingroup wfc
 *  \file wfcstructs.h
 *
 *  \brief Composition SI data structures
 */

#ifndef WFCSTRUCTS_H_
#define WFCSTRUCTS_H_

#include "WF/wfc.h"

#include "owftypes.h"
#include "owfimage.h"
#include "owfattributes.h"
#include "owfmessagequeue.h"
#include "owfthread.h"
#include "owfmutex.h"
#include "owfarray.h"
#include "owfpool.h"
#include "owflinkedlist.h"
#include "owfdisplaycontextgeneral.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct WFC_CONTEXT_;


typedef struct WFC_DEVICE_ {
    WFCDevice               deviceId;
    WFCHandle               handle;
    WFCErrorCode            latestUnreadError;
    OWF_ARRAY               contexts;
    OWF_ARRAY               providers;
    OWF_ARRAY               elements;
    OWF_MUTEX               mutex;
} WFC_DEVICE;

typedef struct PHYSICAL_DEVICE_ {
            OWF_ARRAY iDeviceInstanceArray;
            OWFint gDeviceHandleID;
} PHYSICAL_DEVICE;

typedef struct {
    OWFNativeStreamBuffer   targetBuffer;
    /* */
    void*                   targetPixels;
    /* Mapped to the external format target image buffer each frame */
    OWF_IMAGE*              targetImage;
    /* Mapped to the internal format target image to compose to */
    OWF_IMAGE*              internalTargetImage;
    
    /* The unrotated target buffer */ 
    OWF_IMAGE_INST          unrotatedTargetImage;
    /* The rotated version of the target buffer for hardware rotation, 
     * or a de-rotated version of the internal buffer into another scratch buffer for software rotation
     */ 
    OWF_IMAGE_INST          rotatedTargetImage;
    /* The internal target buffer composed to for 0 and 180 degree rotation */
    OWF_IMAGE_INST          unrotatedInternalTargetImage;
    /* The internal target buffer composed to for 90 and 270 degree rotation */
    OWF_IMAGE_INST          rotatedInternalTargetImage;
} WFC_CONTEXT_STATE;

/*!
Scratch buffers needed:
1 for cropped source image
1 for cropped mask image
1 for scaled element image
1 for mask
*/
#define SCRATCH_BUFFER_COUNT    5

typedef struct {
    /*! elements, ordered by depth; starting from bottom */
    struct WFC_CONTEXT_*    context;
    OWF_NODE*               elements;
} WFC_SCENE;


/*!
 Element's composition state parameters.
 */
typedef struct {
    /*! original source image and mask image - inputs to the first stage */
    OWF_IMAGE*              originalSourceImage;
    OWF_IMAGE*              originalMaskImage;
    
    /*! converted source image -
       results of the first stage in the 
       composition */
    OWF_IMAGE_INST          convertedSourceImage;
    
    /*! cropped source image - result of cropping stage */
    OWF_IMAGE_INST          croppedSourceImage;

    /*! mirrored source intermediate image - temp buffer used in mirroring stage */
    OWF_IMAGE_INST          rotatedSourceIntermediateImage;
    
    /*! mirrored source image - result of mirroring stage */
    OWF_IMAGE_INST          flippedSourceImage;
    
    /*! rotated source image - result of rotation stage */
    OWF_IMAGE_INST          rotatedSourceImage;
    
    /*! these are the "final size" (i.e. destination
       size) versions of the previous. used in
       the blending stage */
    OWF_IMAGE_INST          scaledSourceImage;
    OWF_RECTANGLE           scaledSrcRect;
    OWF_IMAGE_INST          maskImage;

    
    /*! support for blending operation */
    OWF_BLEND_INFO          blendInfo;
    
    
    /*! source and target extents */
    WFCfloat                sourceRect[4]; 
    WFCfloat                destinationRect[4];
    OWF_RECTANGLE           dstRect;
    
    /*! source fp viewport */
    WFCfloat                transformedSourceRect[4];
    
    /*! oversized integer crop */
    OWF_RECTANGLE           oversizedCropRect;
    
    /* Other attributes copied from element */
    OWFsubpixel             globalAlpha;
    WFCScaleFilter          sourceScaleFilter;
    WFCbitfield             transparencyTypes;
    /*! rotation method */
    WFCRotation             rotation;
    /*! flipping */
    WFCboolean              sourceFlip;

} WFC_ELEMENT_STATE;

typedef enum
{
    WFC_IMAGE_SOURCE,
    WFC_IMAGE_MASK
} WFC_IMAGE_PROVIDER_TYPE;

typedef struct 
{
    OWFNativeStreamBuffer   buffer;
    OWFint                  lockCount;
    OWF_IMAGE_INST          image;

} WFC_LOCK_STREAM;

typedef struct
{
    WFCHandle               handle;
    WFC_IMAGE_PROVIDER_TYPE type;
    OWFNativeStreamType     streamHandle;
    void*                   owner;
    WFCint                  contentUpdated;
    WFCint                  contentVisible;
    WFCint                  composed;
    WFCint                  visited;
    WFC_LOCK_STREAM         lockedStream;

} WFC_IMAGE_PROVIDER;

typedef struct {
    WFCElement              handle;
    WFC_DEVICE*             device;
    struct WFC_CONTEXT_*     context;
    /*! element attributes */
    WFCfloat                srcRect[4];
    WFCfloat                dstRect[4];
    WFCboolean              sourceFlip;
    WFCRotation             sourceRotation;
    WFCScaleFilter          sourceScaleFilter;
    WFCbitfield             transparencyTypes;
    WFCfloat                globalAlpha;
    WFCMask                 maskHandle;
    WFCSource               sourceHandle;

    /*! client scene and composed scene dereferenced image providers */
    WFC_IMAGE_PROVIDER*     mask;
    WFC_IMAGE_PROVIDER*     source;
    WFC_IMAGE_PROVIDER*     cachedMask;
    WFC_IMAGE_PROVIDER*     cachedSource;

    /*! shared element? (must not be destroyed by a scene) */
    WFCboolean              shared;

    /*! set in WFC_Element_BeginComposition to indicate whether
       the element should be included in composition */
    WFCboolean              skipCompose;
    /*! Set in WFC_Element_BeginComposition to indicate whether 
     * the mask stream should be included in composition */
    WFCboolean              maskComposed;
} WFC_ELEMENT;

typedef enum
{
    WFC_CONTEXT_STATE_PASSIVE,
    WFC_CONTEXT_STATE_ACTIVATING,
    WFC_CONTEXT_STATE_ACTIVE,
    WFC_CONTEXT_STATE_DEACTIVATING
} WFC_CONTEXT_ACTIVATION_STATE;

typedef EGLint (*TFPtrEglPrivateSignalSyncNok) (EGLDisplay dpy, EGLSyncKHR sync, EGLenum mode);
#define INITIAL_CONTEXT_SERIAL_NUMBER 0
typedef struct WFC_CONTEXT_ {
    WFCContext              handle;
    WFC_DEVICE*             device;

    /*! work-in-progress scene */
    WFC_SCENE*              workScene;
    WFC_SCENE*              committedScene;
    WFC_SCENE*              snapshotScene;

    /*! pools for resource allocation */
    OWF_POOL*               scenePool;
    OWF_POOL*               nodePool;
    OWF_POOL*               elementPool;

    /*! screen number for on-screen contexts */
    WFCint                  screenNumber;

    /*!  scratch buffers used in composition to store per-element
        intermediate results */
    void*                   scratchBuffer[SCRATCH_BUFFER_COUNT];

    /*! onscreen/offscreen write stream */
    OWFNativeStreamType     stream;
    WFCContextType          type;
    WFCint                  targetWidth;
    WFCint                  targetHeight;
    WFCRotation             rotation;
    OWFuint32               backgroundColor;
    WFCElement              lowestElement;
    OWF_ATTRIBUTE_LIST      attributes;
    WFCint                  clientElementCount;

    /*! timing & synchronization */
    OWF_MESSAGE_QUEUE       composerQueue;
    OWF_THREAD              composerThread;
    WFC_CONTEXT_ACTIVATION_STATE       activationState;
    OWF_SEMAPHORE           compositionSemaphore;
    OWF_SEMAPHORE           commitSemaphore;
    OWF_MUTEX               updateFlagMutex;
    OWF_MUTEX               sceneMutex;
    WFCint                  sourceUpdateCount;
   
    WFC_CONTEXT_STATE       state;
    OWF_DISPCTX             displayContext;
    
    WFCEGLDisplay           nextSyncObjectDisplay;
    WFC_ELEMENT_STATE       prototypeElementState;    
    TFPtrEglPrivateSignalSyncNok    eglPrivateSignalSync;
} WFC_CONTEXT;


#define IMAGE_PROVIDER(x)   ((WFC_IMAGE_PROVIDER*)(x))
#define ELEMENT(x)          ((WFC_ELEMENT*)(x))
#define CONTEXT(x)          ((WFC_CONTEXT*)(x))
#define DEVICE(x)           ((WFC_DEVICE*)(x))
#define SCENE(x)            ((WFC_SCENE*)(x))
#define STREAM(x)           ((OWF_STREAM*)(x))

#ifdef __cplusplus
}
#endif

#endif /*STRUCTS_H_*/

