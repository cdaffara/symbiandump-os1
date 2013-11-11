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
 *  \file wfccontext.c
 *
 *  \brief SI Context handling
 */

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#include <WF/wfc.h>
#include <EGL/egl.h>

#include "wfccontext.h"
#include "wfcpipeline.h"

#include "owfscreen.h"
#include "owfdisplaycontextgeneral.h"
#include "owfnotifications.h"


/*! maximum number of elements per scene */
#define MAX_ELEMENTS            512
/*! maximum number of scenes per context */
#define MAX_SCENES              3


#define CONTEXT_SCENE_POOL_SIZE     MAX_SCENES
#define CONTEXT_ELEMENT_POOL_SIZE   MAX_SCENES * MAX_ELEMENTS
#define CONTEXT_NODE_POOL_SIZE      2 * CONTEXT_ELEMENT_POOL_SIZE

/*! almost 2^31 */
#define MAX_DELAY               2100000000

/*! 15ms */
#define AUTO_COMPOSE_DELAY      15000
#define FIRST_CONTEXT_HANDLE    2000

#ifdef __cplusplus
extern "C" {
#endif

static WFCHandle                nextContextHandle = FIRST_CONTEXT_HANDLE;

typedef enum
{
    WFC_MESSAGE_NONE,
    WFC_MESSAGE_QUIT,
    WFC_MESSAGE_COMPOSE,
    WFC_MESSAGE_COMMIT,
    WFC_MESSAGE_FENCE_1_DISPLAY,
    WFC_MESSAGE_FENCE_2_SYNCOBJECT,
    WFC_MESSAGE_ACTIVATE,
    WFC_MESSAGE_DEACTIVATE,
    WFC_MESSAGE_START_COUNTDOWN,
    WFC_MESSAGE_CANCEL
} WFC_MESSAGES;

static void*
WFC_Context_ComposerThread(void* data);


/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
void WFC_CONTEXT_Ctor(void* self)
{
    self = self;
}

/*---------------------------------------------------------------------------*/
static WFCboolean
WFC_Context_CreateState(WFC_CONTEXT* context)
    {   /* Must be called late enough that scratch buffers can be mapped and hardware rotation capability queried */
    OWF_IMAGE_FORMAT        fInt,
                            fExt;
    WFCint                  stride = 0;
    OWF_ASSERT(context);

    DPRINT(("WFC_Context_CreateContextState"));

    owfNativeStreamGetHeader(context->stream,
                             NULL, NULL, &stride, &fExt, NULL);
    /* internal image used as intermediate target */
    fInt.pixelFormat    = OWF_IMAGE_ARGB_INTERNAL;
    fInt.linear         = fExt.linear;
    fInt.premultiplied  = fExt.premultiplied;
    fInt.rowPadding     = 1;
     
    if (context->type == WFC_CONTEXT_TYPE_ON_SCREEN)
        {
        /* The unrotated target buffer: Can't get real address without locking for writing!  NO STRIDE */
        context->state.unrotatedTargetImage=OWF_Image_Create(context->targetWidth,context->targetHeight,&fExt,context->scratchBuffer[2],0);
        /* The rotated version of the target buffer for hardware rotation
         * or a de-rotated version of the internal buffer into another scratch buffer for software rotation
         */ 
        if (OWF_Screen_Rotation_Supported(context->displayContext))
            {   /* The rotated version of the target buffer for hardware rotation */
            context->state.rotatedTargetImage=OWF_Image_Create(context->targetHeight,context->targetWidth,&fExt,context->scratchBuffer[2],0);
            }
        else
            {   /* a de-rotated version of the internal buffer into another scratch buffer for software rotation */
            context->state.rotatedTargetImage=OWF_Image_Create(context->targetWidth,context->targetHeight,&fInt,context->scratchBuffer[1],0);
            }
        }
    else
        {
        /* The unrotated target buffer: Can't get real address without locking for writing!  STRIDE HONOURED */
        context->state.unrotatedTargetImage=OWF_Image_Create(context->targetWidth,context->targetHeight,&fExt,context->scratchBuffer[2],stride);
        /* a de-rotated version of the internal buffer into another scratch buffer for software rotation */
        context->state.rotatedTargetImage=OWF_Image_Create(context->targetWidth,context->targetHeight,&fInt,context->scratchBuffer[1],0);
        }
    /* The internal target buffer composed to for 0 and 180 degree rotation */
    context->state.unrotatedInternalTargetImage=OWF_Image_Create(context->targetWidth,context->targetHeight,&fInt,context->scratchBuffer[0],stride);
    /* The internal target buffer composed to for 90 and 270 degree rotation */
    context->state.rotatedInternalTargetImage=OWF_Image_Create(context->targetHeight,context->targetWidth,&fInt,context->scratchBuffer[0],stride);

    if (context->state.unrotatedTargetImage && context->state.rotatedTargetImage && context->state.unrotatedInternalTargetImage && context->state.rotatedInternalTargetImage)
        {
        return WFC_TRUE;
        }
    else
        {
        return WFC_FALSE;
        }
    }
/*---------------------------------------------------------------------------*/
static void
WFC_Context_DestroyState(WFC_CONTEXT* context)
    {
    /* The unrotated target buffer */ 
    OWF_Image_Destroy(context->state.unrotatedTargetImage);
    /* The rotated version of the target buffer for hardware rotation, 
     * or a de-rotated version of the internal buffer into another scratch buffer for software rotation
     */ 
    OWF_Image_Destroy(context->state.rotatedTargetImage);
    /* The internal target buffer composed to for 0 and 180 degree rotation */
    OWF_Image_Destroy(context->state.unrotatedInternalTargetImage);
    /* The internal target buffer composed to for 90 and 270 degree rotation */
    OWF_Image_Destroy(context->state.rotatedInternalTargetImage);
    
    }
/*---------------------------------------------------------------------------
 * Should only be accessed indirectly by calls to WFC_Device_DestroyContext or
 * WFC_Device_DestroyContexts
 *----------------------------------------------------------------------------*/
void WFC_CONTEXT_Dtor(void* self)
{
    OWFint                  ii = 0;
    WFC_CONTEXT*            context = NULL;

    OWF_ASSERT(self);
    DPRINT(("WFC_CONTEXT_Dtor(%p)", self));

    context = CONTEXT(self);

    OWF_ASSERT(context);
    
    WFC_Pipeline_DestroyState(context);
    WFC_Context_DestroyState(context);
    
    OWF_MessageQueue_Destroy(&context->composerQueue);

    /* make the stream destroyable */
    if (context->stream != OWF_INVALID_HANDLE)
        {
        owfNativeStreamSetProtectionFlag(context->stream, OWF_FALSE);
        owfNativeStreamDestroy(context->stream);
        }
    context->stream = OWF_INVALID_HANDLE;

    OWF_AttributeList_Destroy(&context->attributes);

    for (ii = 0; ii < SCRATCH_BUFFER_COUNT; ii++)
    {
        OWF_Image_FreeData(context->displayContext, &context->scratchBuffer[ii]);
        context->scratchBuffer[ii] = 0;
    }

    OWF_DisplayContext_Destroy(context->screenNumber, context->displayContext);
    context->displayContext = NULL;    

    OWF_Pool_Destroy(context->scenePool);
    OWF_Pool_Destroy(context->elementPool);
    OWF_Pool_Destroy(context->nodePool);

    OWF_Semaphore_Destroy(&context->compositionSemaphore);
    OWF_Semaphore_Destroy(&context->commitSemaphore);
    OWF_Mutex_Destroy(&context->updateFlagMutex);
    OWF_Mutex_Destroy(&context->sceneMutex);
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Context_Shutdown(WFC_CONTEXT* context)
{
    OWF_ASSERT(context);
    DPRINT(("WFC_Context_Shutdown(context = %d)", context->handle));

    DPRINT(("Waiting for composer thread termination"));
    if (context->composerThread)
        {
        OWF_Message_Send(&context->composerQueue, WFC_MESSAGE_QUIT, 0);
        OWF_Thread_Join(context->composerThread, NULL);
        OWF_Thread_Destroy(context->composerThread);
        }
    
    context->composerThread = NULL;

    if (context->device)
    {
        /* #4604: added guard condition */
        WFC_Device_DestroyContextElements(context->device, context);
        WFC_Device_DestroyContextImageProviders(context->device, context);
    }

    WFC_Scene_Destroy(context->workScene);
    WFC_Scene_Destroy(context->snapshotScene);
    WFC_Scene_Destroy(context->committedScene);
    context->workScene = NULL;
    context->snapshotScene = NULL;
    context->committedScene = NULL;    
}

/*---------------------------------------------------------------------------
 *  Initialize context attributes
 *----------------------------------------------------------------------------*/
OWF_API_CALL OWF_ATTRIBUTE_LIST_STATUS
WFC_Context_InitializeAttributes(WFC_CONTEXT* context,
                                 WFCContextType type)
{
    OWF_ATTRIBUTE_LIST_STATUS attribError=ATTR_ERROR_NONE;
    OWF_ASSERT(context);
    /* initialize attributes/properties */
    if (context->stream)
    {
        owfNativeStreamGetHeader(context->stream,
                                 &context->targetWidth,
                                 &context->targetHeight,
                                 NULL, NULL, NULL);
    }
    context->type               = type;
    context->rotation           = WFC_ROTATION_0;
    context->backgroundColor    = 0x000000FF;
    context->lowestElement      = WFC_INVALID_HANDLE;


    OWF_AttributeList_Create(&context->attributes,
                             WFC_CONTEXT_TYPE,
                             WFC_CONTEXT_BG_COLOR);
    attribError=OWF_AttributeList_GetError(&context->attributes);
    if (attribError!=ATTR_ERROR_NONE)
        {
        OWF_ASSERT(attribError==ATTR_ERROR_NO_MEMORY);
        return attribError;
        }

  
    /* The composition code reads the member variables directly, 
     * not via the attribute engine.
     */
    OWF_Attribute_Initi(&context->attributes,
                        WFC_CONTEXT_TYPE,
                        (WFCint*) &context->type,
                        OWF_TRUE);

    OWF_Attribute_Initi(&context->attributes,
                        WFC_CONTEXT_TARGET_WIDTH,
                        &context->targetWidth,
                        OWF_TRUE);

    OWF_Attribute_Initi(&context->attributes,
                        WFC_CONTEXT_TARGET_HEIGHT,
                        &context->targetHeight,
                        OWF_TRUE);

    OWF_Attribute_Initi(&context->attributes,
                        WFC_CONTEXT_ROTATION,
                        (WFCint*) &context->rotation,
                        OWF_FALSE);

    OWF_Attribute_Initi(&context->attributes,
                        WFC_CONTEXT_BG_COLOR,
                        (WFCint*) &context->backgroundColor,
                        OWF_FALSE);

    OWF_Attribute_Initi(&context->attributes,
                        WFC_CONTEXT_LOWEST_ELEMENT,
                        (OWFint*) &context->lowestElement,
                        OWF_TRUE);
    attribError=OWF_AttributeList_GetError(&context->attributes);
	
	/* After commit to working, writable attribute abstracted variables
	must not be written to directly. */
    OWF_AttributeList_Commit(&context->attributes,
                             WFC_CONTEXT_TYPE,
                             WFC_CONTEXT_BG_COLOR,
		             WORKING_ATTR_VALUE_INDEX );
	
    return attribError;
}


/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
static WFC_CONTEXT*
WFC_Context_Initialize(WFC_CONTEXT* context,
                       WFC_DEVICE* device,
                       WFCNativeStreamType stream,
                       WFCContextType type,
                       WFCint screenNum)
{
    void*                   scratch[SCRATCH_BUFFER_COUNT];
    OWFint                  err2    = 0;
    OWFint                  ii      = 0,
                            nbufs   = 0;
    OWFint                  fail    = 0;
    OWF_ATTRIBUTE_LIST_STATUS attribStatus = ATTR_ERROR_NONE;
    OWF_ASSERT(context);
    OWF_ASSERT(device);

    DPRINT(("WFC_Context_Initialize(%p,%p,%d,%d)", context, device, type, screenNum));

    context->type               = type;
    context->device             = device;
    context->handle             = nextContextHandle;
    context->screenNumber       = screenNum;
    context->activationState    = WFC_CONTEXT_STATE_PASSIVE;
    context->sourceUpdateCount  = 0;
    context->clientElementCount = 0;
    ++nextContextHandle;

    context->displayContext = OWF_DisplayContext_Create(screenNum);
    if (context->displayContext == NULL)
        {
        DPRINT(("WFC_Context_Initialize(): Could not create display Context"));
        return NULL;
        }
    
    context->eglPrivateSignalSync = (TFPtrEglPrivateSignalSyncNok) eglGetProcAddress("egl_Private_SignalSyncNOK");
    if (! context->eglPrivateSignalSync)
        {
        DPRINT(("WFC_Context_Initialize(): Missing EGL extension - egl_Private_SignalSyncNOK extension"));   
        return NULL;
        }

    err2 =OWF_MessageQueue_Init(&context->composerQueue);
    if (err2 != 0)
        {
        DPRINT(("WFC_Context_Initialize(): Cannot initialise the message queue err(%d)", err2));   
        return NULL;
        }
    
    context->composerThread = OWF_Thread_Create(WFC_Context_ComposerThread, context);
    if (!(context->composerThread))
        {
        /* must call these to remove references to context */
        DPRINT(("WFC_Context_Initialize(): Failed to create thread!"));
        return NULL;
        }

    OWF_ComposerThread_RendezvousWait(context->displayContext);
    
    /*the following section of the code could be pushed to adaptation in future*/
    if (type == WFC_CONTEXT_TYPE_ON_SCREEN)
    {
       OWF_IMAGE_FORMAT        imageFormat;
       OWF_SCREEN              screen;
       WFCint width = 0;
       WFCint height = 0;
       WFCint normalSize = 0;
       WFCint flippedSize = 0;
       WFCNativeStreamType stream;
    
       /* Set up stream for sending data to screen */
       
       if (!OWF_Screen_GetHeader(context->displayContext, &screen))
       {
           DPRINT(("WFC_Context_Initialize(): Could not retrieve the screen parameters"));
           WFC_Context_Shutdown(context);
           return NULL;
       }
    
       /* Set on-screen pixel format */
       imageFormat.pixelFormat     = OWF_SURFACE_PIXEL_FORMAT;
       imageFormat.premultiplied   = OWF_SURFACE_PREMULTIPLIED;
       imageFormat.linear          = OWF_SURFACE_LINEAR;
       imageFormat.rowPadding      = OWF_SURFACE_ROWPADDING;
    
       width = screen.normal.width;
       height = screen.normal.height;
       
       normalSize = screen.normal.height * screen.normal.stride;
       flippedSize = screen.flipped.height * screen.flipped.stride;
       
       if (flippedSize > normalSize)
           {
           width = screen.flipped.width;
           height = screen.flipped.height;
           }
       
       stream = owfNativeStreamCreateImageStream(width,
                                                 height,
                                                 &imageFormat,
                                                 1);
    
       if (stream)
       {
           WFC_Context_SetTargetStream(context, stream);
           
           /* At this point the stream's refcount is 2 - we must decrement
            * it by one to ensure that the stream is destroyed when the
            * context (that "owns" it) is destroyed.
            */
           owfNativeStreamRemoveReference(stream);
       }
       else
       {
           DPRINT(("WFC_Context_Initialize(): cannot create internal target stream"));
           WFC_Context_Shutdown(context);
           return NULL;
       }
    }
    else
    {
       WFC_Context_SetTargetStream(context, stream);
    }
    
    nbufs = SCRATCH_BUFFER_COUNT-1;
    for (ii = 0; ii < nbufs; ii++)
    {
        scratch[ii] = OWF_Image_AllocData(context->displayContext, MAX_SOURCE_WIDTH,
                MAX_SOURCE_HEIGHT,
                OWF_IMAGE_ARGB_INTERNAL);
        fail = fail || (scratch[ii] == NULL);
    }

    /*
     * allocate one-channel buffer for alpha
     * obs! this assumes sizeof(OWFsubpixel) is 4.
     */
    scratch[nbufs] = OWF_Image_AllocData(context->displayContext, MAX_SOURCE_WIDTH,
                                         MAX_SOURCE_HEIGHT,
                                         OWF_IMAGE_L32);
    fail = fail || (scratch[nbufs] == NULL);
    
    if (fail)
    {
        for (ii = 0; ii < SCRATCH_BUFFER_COUNT; ii++)
        {
            OWF_Image_FreeData(context->displayContext, &scratch[ii]);
        }
        WFC_Context_Shutdown(context);
        return NULL;
    }

    for (ii = 0; ii < SCRATCH_BUFFER_COUNT; ii++)
    {
        context->scratchBuffer[ii] = scratch[ii];
    }
    
    if (    OWF_Semaphore_Init(&context->compositionSemaphore, 1)
        ||  OWF_Semaphore_Init(&context->commitSemaphore, 1)
        ||  OWF_Mutex_Init(&context->updateFlagMutex)
        ||  OWF_Mutex_Init(&context->sceneMutex)
        
        )
        {
        DPRINT(("WFC_Context_Initialize(): Could not create mutexes and semaphores!"));
        WFC_Context_Shutdown(context);
        return NULL;
        }

    if (!WFC_Pipeline_CreateState(context) || !WFC_Context_CreateState(context))
         {
         DPRINT(("WFC_Context_Initialize(): Could not create pipeline state object"));
         WFC_Context_Shutdown(context);
         return NULL;
         }
    

    attribStatus= WFC_Context_InitializeAttributes(context, type);

    if (attribStatus!=ATTR_ERROR_NONE)
        {
        WFC_Context_Shutdown(context);
        return NULL;
        }
    
    
    context->scenePool = OWF_Pool_Create(sizeof(WFC_SCENE),
                                         CONTEXT_SCENE_POOL_SIZE);
    context->elementPool = OWF_Pool_Create(sizeof(WFC_ELEMENT),
                                           CONTEXT_ELEMENT_POOL_SIZE);
    context->nodePool = OWF_Pool_Create(sizeof(OWF_NODE),
                                        CONTEXT_NODE_POOL_SIZE);
    if (!( context->scenePool &&
          context->nodePool && context->elementPool))
    {
        WFC_Context_Shutdown(context);
        return NULL;
    }

    DPRINT(("  Creating scenes"));
    context->workScene = WFC_Scene_Create(context);
    context->committedScene = WFC_Scene_Create(context);
    context->snapshotScene = NULL;

    /* snapshotScene is initialized in InvokeCommit */

    /* context's refcount is now 3 */

    if (!(context->workScene && context->committedScene &&
          context->nodePool && context->elementPool))
    {
        WFC_Context_Shutdown(context);
        return NULL;
    }

    return context;
}

/*---------------------------------------------------------------------------
 *  Create new context on device
 *
 *  \param device Device on which the context should be created
 *  \param type Context type (on- or off-screen)
 *
 *  \return New context object or NULL in case of failure
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFC_CONTEXT*
WFC_Context_Create(WFC_DEVICE* device,
                   WFCNativeStreamType stream,
                   WFCContextType type,
                   WFCint screenNum)
{
    WFC_CONTEXT*            context = NULL;

    OWF_ASSERT(device);
    context = CREATE(WFC_CONTEXT);

    if (context)
    {
        if (!WFC_Context_Initialize(context, device, stream, type, screenNum))
        {
            DESTROY(context);
        }
    }
    return context;
}

/*---------------------------------------------------------------------------
 *  Setup context rendering target
 *
 *  \param context Context
 *  \param stream Target stream to use for rendering
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Context_SetTargetStream(WFC_CONTEXT* context,
                            OWFNativeStreamType stream)
{
    OWF_ASSERT(context);
    context->stream = stream;

    owfNativeStreamAddReference(stream);

    owfNativeStreamGetHeader(stream,
                             &context->targetWidth, &context->targetHeight,
                             NULL, NULL, NULL);
}

static OWFboolean
WFC_FastpathCheckTransparency(WFCbitfield transparencyTypes, WFCfloat globalAlpha, OWF_PIXEL_FORMAT sourceFormat)
    {
    if ((transparencyTypes & WFC_TRANSPARENCY_ELEMENT_GLOBAL_ALPHA) && (globalAlpha != 255.0f))
        {
        DPRINT(("=== WFC_FastpathCheckTransparency - Failed global alfa(%f) check", globalAlpha));
        return OWF_FALSE;
        }

    if ((transparencyTypes & WFC_TRANSPARENCY_SOURCE) && (sourceFormat != OWF_IMAGE_XRGB8888))
        {
        DPRINT(("=== WFC_FastpathCheckTransparency - Failed transparency check types=0x%x format=0x%x", 
                transparencyTypes, sourceFormat));
        return OWF_FALSE;
        }
    

    return OWF_TRUE;
    }

static OWFboolean
WFC_FastpathCheckGeometry(WFC_CONTEXT* context, WFC_ELEMENT* element)
    {
    OWFint sourceWidth = 0;
    OWFint sourceHeight = 0;
    OWFint destWidth = 0;
    OWFint destHeight = 0;
    OWFint targetWidth = 0;
    OWFint targetHeight = 0;
    
    OWF_ASSERT(context);
    OWF_ASSERT(element);

    if ((element->srcRect[0] != 0) || (element->srcRect[1] != 0))
        {
        DPRINT(("=== WFC_FastpathCheckGeometry(context = %p) - FAILED Source Position Check", context));
        return OWF_FALSE;
        }
    
    if ((element->dstRect[0] != 0) || (element->dstRect[1] != 0))
        {
        DPRINT(("=== WFC_FastpathCheckGeometry(context = %p) - FAILED Destination Position Check", context));
        return OWF_FALSE;
        }
    
    if(element->sourceFlip)
        {
        DPRINT(("=== WFC_FastpathCheckGeometry(context = %p) - FAILED Source Flip Check", context));
        return OWF_FALSE;
        }
    
    if(element->sourceRotation == WFC_ROTATION_0)
        {
        sourceWidth = element->srcRect[2];
        sourceHeight = element->srcRect[3];
        }
    else
        {
        DPRINT(("=== WFC_FastpathCheckGeometry(context = %p) - FAILED Source Rotation (0x%x) Check", 
                context, element->sourceRotation));
        return OWF_FALSE;
        }
    
    destWidth = element->dstRect[2];
    destHeight = element->dstRect[3];
    
    if ((sourceWidth != destWidth) || (sourceHeight != destHeight))
       {
       DPRINT(("=== WFC_FastpathCheckGeometry(context = %p) - FAILED Non-scaling Check", context));
       return OWF_FALSE;
       }

    if (context->rotation == WFC_ROTATION_0 || OWF_Screen_Rotation_Supported(context->displayContext))
        {
        if (context->rotation == WFC_ROTATION_0 || context->rotation == WFC_ROTATION_180)
            {
            targetWidth = context->targetWidth;
            targetHeight = context->targetHeight;
            }
        else
            {
            targetWidth = context->targetHeight;
            targetHeight = context->targetWidth;
            }
        
        if (destWidth == targetWidth && destHeight == targetHeight)
            {
            return OWF_TRUE;
            }
        else
            {
            DPRINT(("=== WFC_FastpathCheckGeometry(context = %p) - Failed Dimensions Check", context));
            }
        }
    else
        {
        DPRINT(("=== WFC_FastpathCheckGeometry(context = %p) - Failed Supported Rotations Check", context));
        }
    
    return OWF_FALSE;
    }

/**
 * Check if the current scene is candidate for fastpath optimisation.
 * Fastpath optimisation means the topmost visible layer will be passed direct to the 
 * lower level for presentation on the display without being composed.
 * There are two questions:
 * - Is the scene itself suitable for fastpathing?
 * - Can the surface selected for fastpathing be presented directly by the display?
 * This function will check the scene (eg reject if the top stream is non-opaque or smaller than the screen)
 *
 * @param context context object containing the scene to be checked.
 **/

/* [Not in doxygen]
 * The first time MOpenWFC_RI_Display_Update::SetTopLayerSurface (or SetLayerSurface) is called 
 * with a different stream handle, it can fail indicating the display cannot accept the stream.
 * The compositor will then have to immediately compose that frame as normal, and should continue
 * to perform normal composition until the scene changes to present a different stream as fastpath candidate.  
 * 
 * There is a middle ground, e.g. can the hardware handle over-sized images, or do scaling or do rotation?
 * SetTopLayerSurface accepts an optional list of imperfect attributes to be checked by the adaptation.
 * By WFC_Context_CheckFastpath only listing the attributes that are considered imperfect,
 * and SetLayerSurface rejecting fastpath for any attribute IDs that it doesn't recognise,
 * safe independent extensibility is assured. 
 */
static void
WFC_Context_CheckFastpath(WFC_CONTEXT* context)
    {
    WFC_ELEMENT* element = NULL;
    OWF_ASSERT(context);
    
    DPRINT(("WFC_Context_CheckFastpath(context = %p) - Check Fastpath", context));
    if ((context->type != WFC_CONTEXT_TYPE_ON_SCREEN) ||
        OWF_DisplayContext_FastpathChecked(context->displayContext))
        {
        return;
        }
    
    // Simple case-fast path top most layer
    // More complex case, fast-path top most VISIBLE, OPAQUE layer.
    OWF_DisplayContext_DisableFastpath(context->displayContext);
    OWF_DisplayContext_SetFastpathChecked(context->displayContext);
    // Find top layer
    element = WFC_Scene_TopMostElement(context->committedScene);
    if (element && element->source && element->skipCompose == WFC_FALSE)
        {
    
        if (element->mask)
            {
            DPRINT(("=== WFC_Context_CheckFastpath(context = %p) - FAILED elemenent includes mask", context));
            return;
            }
        
        OWF_ASSERT(element->source->lockedStream.image);
        
        OWF_ASSERT(element->source->streamHandle != OWF_INVALID_HANDLE);
        
        if (!WFC_FastpathCheckGeometry(context, element))
            {
            return;
            }
        
        if (!WFC_FastpathCheckTransparency(element->transparencyTypes, 
                                           element->globalAlpha, 
                                           element->source->lockedStream.image->format.pixelFormat))
            {
            return;
            }
        
        OWF_DisplayContext_EnableFastpath(context->displayContext, element->source->streamHandle);
        DPRINT(("=== WFC_Context_CheckFastpath(context = %p) - Fastpath Enabled", context));
        }
    }

/*---------------------------------------------------------------------------
 * Checks if the given stream would be valid as an off-screen context target.
 * 
 * Checks that the format can be rendered TO.
 * Also checks that the image size is acceptable (within the scratch buffers).
 * This is called before the context is created so is effectively a "static" context method.
 * 
 *  \param stream Target stream to use for rendering
 ---------------------------------------------------------------------------*/

OWF_API_CALL WFCboolean
WFC_Context_IsValidTargetStream(OWFNativeStreamType stream)
{
    OWFint      width,height;
    OWF_IMAGE_FORMAT    format;
    owfNativeStreamGetHeader(stream,
                         &width, &height,
                         NULL, &format, NULL);
    if (width>MAX_SOURCE_WIDTH)
        {
        return WFC_FALSE;
        }
    if (height>MAX_SOURCE_HEIGHT)
        {
        return WFC_FALSE;
        }
    return (WFCboolean)OWF_Image_IsValidDestinationFormat(&format);
}

/*---------------------------------------------------------------------------
 * Checks that the image size is acceptable (within the scratch buffers).
 * This is called before the source is created.
 * 
 *  \param stream source stream to use for rendering.
 ---------------------------------------------------------------------------*/

OWF_API_CALL WFCboolean
WFC_Context_IsValidSourceStream(OWFNativeStreamType stream)
{
    OWFint      width,height;
    owfNativeStreamGetHeader(stream,
                         &width, &height,
                         NULL, NULL, NULL);
    if ((width+2) * (height+2) > MAX_SOURCE_WIDTH * MAX_SOURCE_HEIGHT)
        {
        return WFC_FALSE;
        }
    return WFC_TRUE;
}

/*---------------------------------------------------------------------------
 *  Find element from current scene
 *
 *  \param context Context object
 *  \param element Element to find
 *
 *  \return Element object or NULL if element hasn't been inserted
 *  into current scene.
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFC_ELEMENT*
WFC_Context_FindElement(WFC_CONTEXT* context,
                        WFCElement element)
{
    OWF_ASSERT(context);
    return WFC_Scene_FindElement(context->workScene, element);
}

/*---------------------------------------------------------------------------
 *  Commit context scene graph changes
 *
 *  \param context Context to commit
 *----------------------------------------------------------------------------*/
static void
WFC_Context_DoCommit(WFC_CONTEXT* context)
{
    OWF_ASSERT(context);
    DPRINT(("WFC_Context_DoCommit(context = %p)", context));

    OWF_ASSERT(context->snapshotScene);


    DPRINT(("COMMIT: Committing attribute list changes"));

    DPRINT(("COMMIT: Acquiring mutex"));
    OWF_Mutex_Lock(&context->sceneMutex);

    /* comitting scene attribute changes */
    DPRINT(("COMMIT: Committing scene attribute changes"));
    OWF_AttributeList_Commit(&context->attributes,
                             WFC_CONTEXT_TYPE,
                             WFC_CONTEXT_BG_COLOR,COMMITTED_ATTR_VALUE_INDEX);


    /* resolve sources and masks */
    DPRINT(("COMMIT: Committing scene changes"));
    WFC_Scene_Commit(context->snapshotScene);
    DPRINT(("COMMIT: Destroying old committed scene"));
    WFC_Scene_Destroy(context->committedScene);
    DPRINT(("COMMIT: Setting new snapshot scene as committed one."));
    context->committedScene = context->snapshotScene;
    context->snapshotScene = NULL;

    // reset the visibility flags
    owfSymDeviceResetVisibilityState(context);
    
    DPRINT(("COMMIT: Unlocking mutex"));
    OWF_Mutex_Unlock(&context->sceneMutex);

    DPRINT(("COMMIT: Signaling commit semaphore"));
    /* signal we're ready */
    OWF_Semaphore_Post(&context->commitSemaphore);
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
static OWFboolean
WFC_Context_LockTargetForWriting(WFC_CONTEXT* context)
{
    OWF_ASSERT(context);

    DPRINT(("WFC_Context_LockTargetForWriting"));

    context->state.targetBuffer =
        owfNativeStreamAcquireWriteBuffer(context->stream);
    
    if (!context->state.targetBuffer)
        {
        DPRINT(("Failed to WFC_Context_LockTargetForWriting owfNativeStreamAcquireWriteBuffer"));
        return OWF_FALSE;
        }
    context->state.targetPixels =
        owfNativeStreamGetBufferPtr(context->stream,
                                    context->state.targetBuffer);

    if ((WFC_ROTATION_0   == context->rotation || WFC_ROTATION_180 == context->rotation) ||
        !OWF_Screen_Rotation_Supported(context->displayContext))
    {
        /* final target, in target format */
        context->state.targetImage =context->state.unrotatedTargetImage;
    }
    else
    {
        /* final target, in target format */
        /* fExt stride/padding value is the rotated value */
        context->state.targetImage = context->state.rotatedTargetImage;
    }
    OWF_Image_SetPixelBuffer(context->state.targetImage,context->state.targetPixels);
    
    if (context->state.targetImage==NULL)
        {
        OWF_ASSERT(WFC_FALSE);
        }

    /* take context rotation into account. */
    if (WFC_ROTATION_0   == context->rotation ||
        WFC_ROTATION_180 == context->rotation)
    {
        context->state.internalTargetImage = context->state.unrotatedInternalTargetImage;
    }
    else
    {
        context->state.internalTargetImage = context->state.rotatedInternalTargetImage;
    }

    if (context->state.internalTargetImage==NULL)
        {
        OWF_ASSERT(WFC_FALSE);
        }
    return OWF_TRUE;
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
static void
WFC_Context_UnlockTarget(WFC_CONTEXT* context,OWFboolean aDoPost)
{
    OWF_ROTATION rotation = OWF_ROTATION_0;

    OWF_ASSERT(context);
    DPRINT(("WFC_Context_UnlockTarget"));
    DPRINT(("  Unlocking target stream=%d, buffer=%d",
            context->stream, context->state.targetBuffer));

    owfNativeStreamReleaseWriteBuffer(context->stream,
                                      context->state.targetBuffer,
                                      EGL_NO_DISPLAY,
                                      NULL);

    
    if (aDoPost)
    {
        switch (context->rotation)
        {
            case WFC_ROTATION_0:
            {
                rotation = OWF_ROTATION_0;
                break;
            }
            case WFC_ROTATION_90:
            {
                rotation = OWF_ROTATION_90;
                break;
            }
            case WFC_ROTATION_180:
            {
                rotation = OWF_ROTATION_180;
                break;
            }
            case WFC_ROTATION_270:
            {
                rotation = OWF_ROTATION_270;
                break;
            }
            default:
            {
                OWF_ASSERT(0);
            }
        }
        
        if (!OWF_Screen_Post_Topmost_Layer(context->displayContext, context->stream, rotation))
            {   //getting a fail here is bad... display did not accept the composition buffer.
            DPRINT(("WFC_Context_UnlockTarget - OWF_Screen_Post_Topmost_Layer failed for composition stream"));
           OWF_ASSERT(0);
            }
    }
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
static void
WFC_Context_PrepareComposition(WFC_CONTEXT* context)
{
    OWFsubpixel             r, g, b, a;

    OWF_ASSERT(context);

    /* the first thing to do is to lock target stream and fetch
       write buffer to it. fetching blocks until one is available,
       but since only one stream can be target to only one context
       at the time, no stalls should occur */
    WFC_Context_LockTargetForWriting(context);

    /* prepare for composition by "clearing the table" with
       background color.  */

    r = (OWFsubpixel) OWF_RED_MAX_VALUE * ((context->backgroundColor >> 24) &
        0xFF) / OWF_BYTE_MAX_VALUE;
    g = (OWFsubpixel) OWF_GREEN_MAX_VALUE * ((context->backgroundColor >> 16) &
        0xFF) / OWF_BYTE_MAX_VALUE;
    b = (OWFsubpixel) OWF_BLUE_MAX_VALUE * ((context->backgroundColor >> 8) &
        0xFF) / OWF_BYTE_MAX_VALUE;
    a = (OWFsubpixel) OWF_ALPHA_MAX_VALUE * (context->backgroundColor & 0xFF) /
        OWF_BYTE_MAX_VALUE;

    r = r * a / OWF_ALPHA_MAX_VALUE;
    g = g * a / OWF_ALPHA_MAX_VALUE;
    b = b * a / OWF_ALPHA_MAX_VALUE;

    OWF_Image_Clear(context->state.internalTargetImage, r, g, b, a);
}



/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
static void
WFC_Context_FinishComposition(WFC_CONTEXT* context)
{
    OWF_ROTATION            rotation = OWF_ROTATION_0;
    OWFboolean              screenRotation;

    OWF_ASSERT(context);

    screenRotation = OWF_Screen_Rotation_Supported(context->displayContext);
    /* re-use scratch buffer 1 for context rotation */
    if (WFC_ROTATION_0   == context->rotation || screenRotation)
    {
 
        if (screenRotation)
        {
            if (WFC_ROTATION_90   == context->rotation || WFC_ROTATION_270   == context->rotation)
                {
                owfSetStreamFlipState(context->stream, OWF_TRUE);
                }
            else
                {
                owfSetStreamFlipState(context->stream, OWF_FALSE);
                }
        }
        OWF_Image_DestinationFormatConversion(context->state.targetImage, context->state.internalTargetImage);
    }
    else
    {
        switch (context->rotation)
        {
            case WFC_ROTATION_0:
            {
                rotation = OWF_ROTATION_0;
                break;
            }
            case WFC_ROTATION_90:
            {
                rotation = OWF_ROTATION_90;
                break;
            }
            case WFC_ROTATION_180:
            {
                rotation = OWF_ROTATION_180;
                break;
            }
            case WFC_ROTATION_270:
            {
                rotation = OWF_ROTATION_270;
                break;
            }
            default:
            {
                OWF_ASSERT(0);
            }
        }
     
        /* rotate */
        OWF_Image_Rotate(context->state.rotatedTargetImage,
                         context->state.internalTargetImage,
                         rotation);

        /* Note: support of different target formats  can be put here */

        OWF_Image_DestinationFormatConversion(context->state.targetImage, context->state.rotatedTargetImage);
    }
    WFC_Context_UnlockTarget(context,(context->type==WFC_CONTEXT_TYPE_ON_SCREEN)?OWF_TRUE:OWF_FALSE);
}

/*!---------------------------------------------------------------------------
 * \brief Actual composition routine.
 *  Mainly just calls other functions that executes different stages of
 *  the composition pipeline.
 *  \param context Context to compose.
 *----------------------------------------------------------------------------*/
static void
WFC_Context_DoCompose(WFC_CONTEXT* context)
{
    WFC_SCENE*              scene   = NULL;
    OWF_NODE*               node    = NULL;

    OWF_ASSERT(context);

    OWF_Mutex_Lock(&context->updateFlagMutex);

    OWF_DisplayContext_IncrementSerialNumber(context->displayContext);
    
    OWF_DisplayContext_SetCompositionOngoing(context->displayContext, WFC_TRUE);
    context->sourceUpdateCount = 0;
    OWF_Mutex_Unlock(&context->updateFlagMutex);
    
    DPRINT(("WFC_Context_Compose"));
    /* Composition always uses the committed version
     * of the scene.
     */

    WFC_Scene_LockSourcesAndMasks(context->committedScene);
    
    OWF_Mutex_Lock(&context->sceneMutex);
    
    WFC_Context_CheckFastpath(context);
    if (OWF_DisplayContext_FastpathEnabled(context->displayContext))
        {
        WFCboolean targetStreamAccessed;
        OWFboolean screenRotation;
        screenRotation = OWF_Screen_Rotation_Supported(context->displayContext);
        if (WFC_Context_Active(context))
            {   //Full fastpath is only supported for autonomous composition 
            OWFNativeStreamType stream = OWF_INVALID_HANDLE;
            OWF_ROTATION rotation = OWF_ROTATION_0;
    
            DPRINT(("== WFC_Context_DoCompose(context = %p) - Fastpathing", context));
    
            stream = OWF_DisplayContext_FastpathStream(context->displayContext);
            if (screenRotation)
                {
                switch (context->rotation)
                    {
                    case WFC_ROTATION_0:
                        {
                        rotation = OWF_ROTATION_0;
                        break;
                        }
                    case WFC_ROTATION_90:
                        {
                        rotation = OWF_ROTATION_90;
                        break;
                        }
                    case WFC_ROTATION_180:
                        {
                        rotation = OWF_ROTATION_180;
                        break;
                        }
                    case WFC_ROTATION_270:
                        {
                        rotation = OWF_ROTATION_270;
                        break;
                        }
                    default:
                        {
                        OWF_ASSERT(0);
                        rotation = OWF_ROTATION_0;
                        }
                    }
                }
    
            if (!OWF_Screen_Post_Topmost_Layer(context->displayContext, stream, rotation))
                {
            
                DPRINT(("WFC_Context_Compose calls OWF_DisplayContext_DisableFastpath because !OWF_Screen_Post_Topmost_Layer()"));
                OWF_DisplayContext_DisableFastpath(context->displayContext);
                //If fastpath is disabled here then we need to compose properly this cycle
                }
            }
        targetStreamAccessed = OWF_DisplayContext_InternalStreamAccessed(context->displayContext);
        if (OWF_DisplayContext_FastpathEnabled(context->displayContext) && ( targetStreamAccessed || !WFC_Context_Active(context) ))
            {   //Fastpath in non-autonomous composition just does a simple copy and post.
            DPRINT(("== WFC_Context_DoCompose(context = %p) -   fastpath copy target", context));
            if (WFC_Context_LockTargetForWriting(context))
                {
                OWFboolean copy;
                if (screenRotation)
                    {
                    if (WFC_ROTATION_90 == context->rotation || WFC_ROTATION_270 == context->rotation)
                        {
                        owfSetStreamFlipState(context->stream, OWF_TRUE);
                        }
                    else
                        {
                        owfSetStreamFlipState(context->stream, OWF_FALSE);
                        }
                    }
                copy=OWF_DisplayContext_CopyFastpathedStreamToTargetStream(context);
                if (!WFC_Context_Active(context))
                    {
                    if (!copy)
                        {
                        DPRINT(("WFC_Context_Compose calls OWF_DisplayContext_DisableFastpath because !OWF_DisplayContext_CopyFastpathedStreamToTargetStream()"));
                        OWF_DisplayContext_DisableFastpath(context->displayContext);
                        //If fastpath is disabled here then we need to compose properly this cycle
                        }
                    }
                else
                    {
                    copy=OWF_FALSE;
                    }
              
                WFC_Context_UnlockTarget(context,copy);
                }
            else
                {
                //If non-autonomous, then the lock target is required.
                OWF_ASSERT(WFC_Context_Active(context));
                }

            }
        if (OWF_DisplayContext_FastpathEnabled(context->displayContext))
            {
            WFC_ELEMENT* topmostElement = NULL;
            topmostElement = WFC_Scene_TopMostElement(context->committedScene);
            owfSymElementNotifications(context, topmostElement);
            }
        }
    if (!OWF_DisplayContext_FastpathEnabled(context->displayContext))
        {
        DPRINT(("== WFC_Context_DoCompose(context = %p) -   Composing", context));
        WFC_Context_PrepareComposition(context);

        scene = context->committedScene;
        OWF_ASSERT(scene);
    
        for (node = scene->elements; NULL != node; node = node->next)
            {
            
            WFC_ELEMENT*            element = NULL;
            WFC_ELEMENT_STATE*      elementState = NULL;
            element = ELEMENT(node->data);
    
            if (element->skipCompose)
                {
                 /* this element is somehow degraded, its source is missing or
                  * something else; skip to next element */
                DPRINT(("  *** Skipping element %d", element->handle));
                continue;
                }
    
            DPRINT(("  Composing element %d", element->handle));
    
            /* BeginComposition may fail e.g. if the element's destination
             * rectangle is something bizarre, i.e. causes overflows or
             * something.
             */
            if ((elementState=WFC_Pipeline_BeginComposition(context, element))!=NULL)
                {
                owfSymElementNotifications(context, element);
                
                WFC_Pipeline_ExecuteSourceConversionStage(context, elementState);
                WFC_Pipeline_ExecuteCropStage(context, elementState);
                WFC_Pipeline_ExecuteFlipStage(context, elementState);
                WFC_Pipeline_ExecuteRotationStage(context, elementState);
                WFC_Pipeline_ExecuteScalingStage(context, elementState);
                WFC_Pipeline_ExecuteBlendingStage(context, elementState);
                    
                WFC_Pipeline_EndComposition(context, element,elementState);
                }
            }
    
        WFC_Context_FinishComposition(context);
        DPRINT(("=== WFC_Context_DoCompose(context = %p) - Diplayed Composition", context));
        }
    
    WFC_Scene_UnlockSourcesAndMasks(context->committedScene);
    owfSymProcessAllNotifications(context);
    OWF_Mutex_Unlock(&context->sceneMutex);
    
    OWF_Semaphore_Post(&context->compositionSemaphore);
}

/*---------------------------------------------------------------------------
 *  Activate/deactivate auto-composition on context
 *
 *  \param context Context
 *  \param act Auto-composition enable/disable
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Context_Activate(WFC_CONTEXT* context,
                     WFCboolean act)
{
    OWF_ASSERT(context);

    DPRINT(("WFC_Context_Activate: %s", (act) ? "activate" : "deactivate"));

    if (act && !WFC_Context_Active(context))
    {
        DPRINT(("WFC_Context_Activate: WFC_CONTEXT_STATE_PASSIVE: activating"));
        context->activationState = WFC_CONTEXT_STATE_ACTIVATING;

        /* moved from composer loop - updates must be allowed
         * immediately after activation
         */
        WFC_Device_EnableContentNotifications(context->device,
                                              context,
                                              WFC_TRUE);

        OWF_Message_Send(&context->composerQueue,
                         WFC_MESSAGE_ACTIVATE,
                         0);
    }
    else if (!act && WFC_Context_Active(context))
    {
        DPRINT(("WFC_Context_Activate: WFC_CONTEXT_STATE_ACTIVE: deactivating"));
        context->activationState = WFC_CONTEXT_STATE_DEACTIVATING;
        OWF_Message_Send(&context->composerQueue,
                         WFC_MESSAGE_DEACTIVATE,
                         0);
    }
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFCboolean
WFC_Context_InvokeComposition(WFC_DEVICE* device,
                              WFC_CONTEXT* context,
                              WFCboolean wait)
{
    WFCint              status = 0;

    OWF_ASSERT(context);
    OWF_ASSERT(device);

    device = device; /* suppress the compiler warning */

    status = OWF_Semaphore_TryWait(&context->compositionSemaphore);
    if (status)
    {
        if (!wait)
        {
            /* busy; processing last request */
            return WFC_FALSE;
        }
        /* wait previous frame composition to finish */
        OWF_Semaphore_Wait(&context->compositionSemaphore);
    }

    /* compositionSemaphore is posted/signaled in WFC_Context_Compose()
    after frame has been successfully composed */
    OWF_Message_Send(&context->composerQueue, WFC_MESSAGE_COMPOSE, 0);

    return WFC_TRUE;
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFCErrorCode
WFC_Context_InvokeCommit(WFC_DEVICE* device,
                         WFC_CONTEXT* context,
                         WFCboolean wait)
{
    WFCint              status = 0;

    OWF_ASSERT(context);
    OWF_ASSERT(device);

    device = device; /* suppress the compiler warning */

    /* first check if there're inconsistensies in the scene */
    if (WFC_Scene_HasConflicts(context->workScene))
    {
        DPRINT(("WFC_Context_InvokeCommit: scene has inconsistensies"));
        return WFC_ERROR_INCONSISTENCY;
    }

    /* then commit - always asynchronously */
    status = OWF_Semaphore_TryWait(&context->commitSemaphore);
    DPRINT(("COMMIT: Commit semaphore status = %d", status));
    if (status)
    {
        if (!wait)
        {
            DPRINT(("COMMIT: Busy; exiting."));
            /* busy; processing last commit */
            return WFC_ERROR_BUSY;
        }

        DPRINT(("COMMIT: Waiting for previous commit to finish."));
        /* wait previous commit to finish */
        OWF_Semaphore_Wait(&context->commitSemaphore);
    }

    /* comitting scene attribute changes */
    DPRINT(("COMMIT: Cloning scene attribute changes"));
    OWF_AttributeList_Commit(&context->attributes,
                             WFC_CONTEXT_TYPE,
                             WFC_CONTEXT_BG_COLOR,SNAPSHOT_ATTR_VALUE_INDEX);

    DPRINT(("COMMIT: Cloning scene"));
    /* take snapshot of the current working copy - it will
     * be the new committed scene */
    context->snapshotScene = WFC_Scene_Clone(context->workScene);

    DPRINT(("COMMIT: Sending commit request"));
    /* invoke async commit */
    OWF_Message_Send(&context->composerQueue, WFC_MESSAGE_COMMIT, 0);
    return WFC_ERROR_NONE;
}

/*---------------------------------------------------------------------------
 *  \param context
 *  \param dpy
 *  \param sync
 *----------------------------------------------------------------------------*/
OWF_API_CALL void
WFC_Context_InsertFence(WFC_CONTEXT* context,
                        WFCEGLDisplay dpy,
                        WFCEGLSync sync)
{
    OWF_ASSERT(context);
    OWF_ASSERT(sync);

    DPRINT(("WFC_Context_InsertFence: Sending fence sync: 0x%08x", sync));
    
    OWF_Message_Send(&context->composerQueue, WFC_MESSAGE_FENCE_1_DISPLAY, (void*) dpy);
    OWF_Message_Send(&context->composerQueue, WFC_MESSAGE_FENCE_2_SYNCOBJECT, sync);
}

/*---------------------------------------------------------------------------
 *  Insert element into context's scene
 *
 *  \param context
 *  \param element
 *  \param subordinate
 *
 *  \return WFCErrorCode
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFCErrorCode
WFC_Context_InsertElement(WFC_CONTEXT* context,
                          WFCElement element,
                          WFCElement subordinate)
{
    WFC_ELEMENT*            object = NULL;
    WFCErrorCode            result = WFC_ERROR_BAD_HANDLE;

    OWF_ASSERT(context);

    object = WFC_Device_FindElement(context->device, element);

    if (NULL != object && CONTEXT(object->context) == context)
    {
        /* set the sharing flag as the element will be
         * shared between the device and working copy scene.
         * this is to tell the scene that it must not destroy
         * this element.
         */
        object->shared = WFC_TRUE;
        result = WFC_Scene_InsertElement(context->workScene,
                                         object,
                                         subordinate);

        context->lowestElement = WFC_Scene_LowestElement(context->workScene);
    }
    return result;
}

/*---------------------------------------------------------------------------
 *  Remove element from context's scene
 *
 *  \param context
 *  \param element
 *
 *  \return WFCErrorCode
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFCErrorCode
WFC_Context_RemoveElement(WFC_CONTEXT* context,
                          WFCElement element)
{
    WFC_ELEMENT*            elemento = NULL;

    OWF_ASSERT(context);

    elemento = WFC_Context_FindElement(context, element);

    if (elemento)
    {
        WFC_Scene_RemoveElement(context->workScene, element);
        /* the element is no longer shared, as it only resides
         * in device from this point on
         */
        elemento->shared = WFC_FALSE;
        context->lowestElement = WFC_Scene_LowestElement(context->workScene);
    }

    return WFC_ERROR_NONE;
}

/*!
 *  \brief IncreaseClientElementCount
 *
 *  \param context
 *  \return positive increased current element count; negative or zero indicating the count can't be increased
 */
OWF_API_CALL WFCint
WFC_Context_IncreaseClientElementCount(WFC_CONTEXT* context)
    {
    /* This implementation simply caps the number of elements allocated to the client
     * to 1/3 of the total elements.
     * A cleverer strategy would allow the client to create more elements 
     * so long as the number added to the scene *2 did not exceed the spare pool size.
     * These failure points are also a good place to consider increasing the pool size
     */
    if (context->clientElementCount>=MAX_ELEMENTS)
        {
        return -context->clientElementCount;
        }
    else
        {
        return ++context->clientElementCount;
        }
    }

/*!
 *  \brief DecreaseClientElementCount
 *
 *  \param context
 *  \return positive or zero decreased current element count; negative indicating the count can't be decreased - p0robably already zero
 */
OWF_API_CALL WFCint
WFC_Context_DecreaseClientElementCount(WFC_CONTEXT* context)
    {
    /* note that a negative return indicates that decrements are not matched with increments */ 
    return --context->clientElementCount;
    }

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFCErrorCode
WFC_Context_GetElementAbove(WFC_CONTEXT* context,
                            WFCElement element,
                            WFCElement* result)
{
    WFC_ELEMENT*            object = NULL;
    WFCErrorCode            error = WFC_ERROR_ILLEGAL_ARGUMENT;

    OWF_ASSERT(context);
    OWF_ASSERT(result);

    object = WFC_Context_FindElement(context, element);

    if (object)
    {
        WFCElement          temp;

        temp = WFC_Scene_GetNeighbourElement(context->workScene, element, 1);
        *result = temp;
        error = WFC_ERROR_NONE;
    }
    return error;
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFCErrorCode
WFC_Context_GetElementBelow(WFC_CONTEXT* context,
                            WFCElement element,
                            WFCElement* result)
{
    WFC_ELEMENT*            object = NULL;
    WFCErrorCode            error = WFC_ERROR_ILLEGAL_ARGUMENT;

    OWF_ASSERT(context);
    OWF_ASSERT(result);

    object = WFC_Context_FindElement(context, element);
    if (object)
    {
        WFCElement          temp;

        temp = WFC_Scene_GetNeighbourElement(context->workScene, element, -1);
        *result = temp;
        error = WFC_ERROR_NONE;
    }
    return error;
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFCErrorCode
WFC_Context_GetAttribi(WFC_CONTEXT* context,
                       WFCContextAttrib attrib,
                       WFCint* value)
{
    WFCint                  temp = 0;
    OWF_ATTRIBUTE_LIST_STATUS   err;
    WFCErrorCode            result = WFC_ERROR_NONE;

    OWF_ASSERT(context);
    OWF_ASSERT(value);

    temp    = OWF_Attribute_GetValuei(&context->attributes, attrib);
    err     = OWF_AttributeList_GetError(&context->attributes);

    if (err != ATTR_ERROR_NONE)
    {
        result = WFC_ERROR_BAD_ATTRIBUTE;
    }
    else
    {
        *value = temp;
    }
    return result;
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFCErrorCode
WFC_Context_SetAttribi(WFC_CONTEXT* context,
                       WFCContextAttrib attrib,
                       WFCint value)
{
    WFCErrorCode                result = WFC_ERROR_NONE;

    OWF_ASSERT(context);

    /* check value */
    switch (attrib)
    {
        case WFC_CONTEXT_BG_COLOR:
        {
            OWFint              alpha;

            /*
             *  Color format is RGBA NOT RGBA.
             */
            alpha = value & 0xFF;

            if (WFC_CONTEXT_TYPE_ON_SCREEN == context->type)
            {
                /* the only allowed value for on-screen context
                 * background alpha is 255 */
                if (alpha != 255)
                {
                    result = WFC_ERROR_ILLEGAL_ARGUMENT;
                }
            }
            break;
        }

        case WFC_CONTEXT_ROTATION:
        {
            if (!(WFC_ROTATION_0 == value ||
                  WFC_ROTATION_90 == value ||
                  WFC_ROTATION_180 == value ||
                  WFC_ROTATION_270 == value))
            {
               result = WFC_ERROR_ILLEGAL_ARGUMENT;
            }
            else
            {
                OWF_DisplayContext_ResetFastpathCheck(context->displayContext);
            }
            break;
        }

        case WFC_CONTEXT_TYPE:
        case WFC_CONTEXT_TARGET_HEIGHT:
        case WFC_CONTEXT_TARGET_WIDTH:
        case WFC_CONTEXT_LOWEST_ELEMENT:
        {
            result = WFC_ERROR_BAD_ATTRIBUTE;
            break;
        }

        case WFC_CONTEXT_FORCE_32BIT:
        default:
        {
            result = WFC_ERROR_BAD_ATTRIBUTE;
            break;
        }
    }

    if (WFC_ERROR_NONE == result)
    {
        OWF_ATTRIBUTE_LIST_STATUS   error;

        /* try changing the value */
        OWF_Attribute_SetValuei(&context->attributes, attrib, value);
        error = OWF_AttributeList_GetError(&context->attributes);

        /* transform errors */
        switch (error) {
            case ATTR_ERROR_ACCESS_DENIED:
            case ATTR_ERROR_INVALID_ATTRIBUTE:
            {
                result = WFC_ERROR_BAD_ATTRIBUTE;
                break;
            }
            default:
            {
                break;
            }
        }
    }

    return result;
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFCErrorCode
WFC_Context_GetAttribfv(WFC_CONTEXT* context,
                        WFCContextAttrib attrib,
                        WFCint count,
                        WFCfloat* values)
{
    WFCErrorCode            result = WFC_ERROR_NONE;

    OWF_ASSERT(context);
    OWF_ASSERT(values);

    switch (attrib)
    {
        case WFC_CONTEXT_BG_COLOR:
        {
            if (4 != count)
            {
                result = WFC_ERROR_ILLEGAL_ARGUMENT;
            }
            else
            {
                OWFuint32      color;
                OWF_ATTRIBUTE_LIST_STATUS   err;

                color = OWF_Attribute_GetValuei(&context->attributes, attrib);
                err = OWF_AttributeList_GetError(&context->attributes);

                if (err != ATTR_ERROR_NONE)
                {
                    result = WFC_ERROR_BAD_ATTRIBUTE;
                    break;
                }

                /* extract color channels and convert to float */
                values[0] = (WFCfloat) (color >> 24) /
                            (WFCfloat) OWF_BYTE_MAX_VALUE;
                values[1] = (WFCfloat) ((color >> 16) & 0xFF) /
                            (WFCfloat) OWF_BYTE_MAX_VALUE;
                values[2] = (WFCfloat) ((color >> 8) & 0xFF) /
                            (WFCfloat) OWF_BYTE_MAX_VALUE;
                values[3] = (WFCfloat) (color & 0xFF) /
                            (WFCfloat) OWF_BYTE_MAX_VALUE;
            }
            break;
        }

        default:
        {
            result = WFC_ERROR_BAD_ATTRIBUTE;
            break;
        }
    }

    return result;
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFCErrorCode
WFC_Context_SetAttribfv(WFC_CONTEXT* context,
                        WFCContextAttrib attrib,
                        WFCint count,
                        const WFCfloat* values)
{
    WFCErrorCode            result = WFC_ERROR_NONE;

    OWF_ASSERT(context);
    OWF_ASSERT(values);

    switch (attrib)
    {
        case WFC_CONTEXT_BG_COLOR:
        {
            if (4 != count)
            {
                result = WFC_ERROR_ILLEGAL_ARGUMENT;
            }
            else
            {
                OWFuint32    color;

                /* Every color component value must fall within range [0, 1] */
                if (INRANGE(values[0], 0.0f, 1.0f) &&
                    INRANGE(values[1], 0.0f, 1.0f) &&
                    INRANGE(values[2], 0.0f, 1.0f) &&
                    INRANGE(values[3], 0.0f, 1.0f))
                {
                    color = (((OWFuint32) floor(values[0] * 255)) << 24) |
                            (((OWFuint32) floor(values[1] * 255)) << 16) |
                            (((OWFuint32) floor(values[2] * 255)) << 8) |
                            ((OWFuint32) floor(values[3] * 255));

                    /* delegate to integer accessor - it'll check the
                     * the rest of the value and update it eventually  */
                    result = WFC_Context_SetAttribi(context, attrib, color);
                }
                else
                {
                    result = WFC_ERROR_ILLEGAL_ARGUMENT;
                }
            }
            break;
        }

        default:
        {
            result = WFC_ERROR_BAD_ATTRIBUTE;
            break;
        }
    }

    return result;
}


static void
WFC_Context_AutoComposer(WFC_CONTEXT* context)
{
    OWF_Mutex_Lock(&context->updateFlagMutex);
    if (context->sourceUpdateCount > 0)
    {
        DPRINT(("WFC_Context_ComposerThread: %d sources were updated, "
                "invoking composition\n",
                context->sourceUpdateCount));

        /* reset update counter */

        OWF_Mutex_Unlock(&context->updateFlagMutex);
        WFC_Context_DoCompose(context);
    }
    else
    {
        OWF_Mutex_Unlock(&context->updateFlagMutex);
    }
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
static void*
WFC_Context_ComposerThread(void* data)
{
    WFC_CONTEXT*            context = (WFC_CONTEXT*) data;
    OWFboolean screenCreated = OWF_TRUE;
    OWF_MESSAGE             msg;


    OWF_ASSERT(context);
    DPRINT(("WFC_Context_ComposerThread starting"));

    memset(&msg, 0, sizeof(OWF_MESSAGE));
    
    if (context->type == WFC_CONTEXT_TYPE_ON_SCREEN)
        {
        screenCreated = OWF_Screen_Create(context->screenNumber, context->displayContext);
        }
    
    OWF_ComposerThread_Rendezvous(context->displayContext);

    OWF_ComposerThread_RendezvousDestroy(context->displayContext);
    
    while (context->device && msg.id != WFC_MESSAGE_QUIT && screenCreated)
    {
        OWFint              err = -1;

        if (WFC_CONTEXT_STATE_ACTIVE == context->activationState)
        {
            err = OWF_Message_Wait(&context->composerQueue,
                                   &msg,
                                   AUTO_COMPOSE_DELAY);

            WFC_Context_AutoComposer(context);
        }
        else
        {
            DPRINT(("  ComposerThread waiting for message"));
            err = OWF_Message_Wait(&context->composerQueue, &msg, WAIT_FOREVER);
        }

        if (0 == err)
        {
            switch (msg.id)
            {
                case WFC_MESSAGE_ACTIVATE:
                {
                    DPRINT(("****** ENABLING AUTO-COMPOSITION ******"));
                    context->activationState = WFC_CONTEXT_STATE_ACTIVE;
                    break;
                }

                case WFC_MESSAGE_DEACTIVATE:
                {
                    /* cancel possible countdown so that update won't occur
                     * after deactivation */
                    DPRINT(("****** DISABLING AUTO-COMPOSITION ******"));
                    WFC_Device_EnableContentNotifications(context->device,
                                                          context,
                                                          WFC_FALSE);
                    context->activationState = WFC_CONTEXT_STATE_PASSIVE;
                    if (OWF_DisplayContext_FastpathEnabled(context->displayContext))
                        {
                        DPRINT(("COMMIT: Invoking fastpath recomposition after deactivate"));
                        WFC_Context_DoCompose(context);
                        }
                    break;
                }

                case WFC_MESSAGE_COMMIT:
                {
                    DPRINT(("****** COMMITTING SCENE CHANGES ******"));

                    DPRINT(("COMMIT: Invoking DoCommit"));
                    WFC_Context_DoCommit(context);
                    OWF_DisplayContext_ResetFastpathCheck(context->displayContext);
                    if (!WFC_Context_Active(context))
                    {
                        DPRINT(("COMMIT: Context is inactive, composition "
                                "not needed.", context->handle));
                        break;
                    }
                    else
                    {
                        /* context is active; compose immediately after
                         * commit has completed */

                        DPRINT(("COMMIT: Invoking composition after commit"));
                    }
                    /* FLOW THROUGH */
                }

                case WFC_MESSAGE_COMPOSE:
                {
                    DPRINT(("****** COMPOSING SCENE ******"));

                    WFC_Context_DoCompose(context);

                    break;
                }
                
                case WFC_MESSAGE_FENCE_1_DISPLAY:
                {
                    DPRINT(("****** STORING EGLDISPLAY (%p) ******", msg.data));

                    context->nextSyncObjectDisplay = (WFCEGLDisplay)msg.data;
                    break;
                }

                case WFC_MESSAGE_FENCE_2_SYNCOBJECT:
                {
                    DPRINT(("****** BREAKING FENCE (%p) ******", msg.data));
                    OWF_ASSERT(context->eglPrivateSignalSync);

                    context->eglPrivateSignalSync(context->nextSyncObjectDisplay,
                                     (WFCEGLSync) msg.data,
                                     EGL_SIGNALED_KHR);
                    break;
                }
            }
        }
    }

    if (context->type == WFC_CONTEXT_TYPE_ON_SCREEN)
        {
        OWF_Screen_Destroy(context->displayContext);
        }
    /* Release any use of EGL by this thread. */
    eglReleaseThread();

    DPRINT(("WFC_Context_ComposerThread terminating"));
    OWF_Thread_Exit(NULL);
    return NULL;
}

/*---------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------*/
OWF_API_CALL WFCboolean
WFC_Context_Active(WFC_CONTEXT* context)
{
    OWF_ASSERT(context);

    return (WFC_CONTEXT_STATE_ACTIVE == context->activationState ||
           WFC_CONTEXT_STATE_ACTIVATING == context->activationState) ? WFC_TRUE : WFC_FALSE;
}

#ifdef __cplusplus
}
#endif

