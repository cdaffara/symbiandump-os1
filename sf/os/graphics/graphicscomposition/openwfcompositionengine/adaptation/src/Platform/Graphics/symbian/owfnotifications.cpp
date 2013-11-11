// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and/or associated documentation files (the
// "Materials"), to deal in the Materials without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Materials, and to
// permit persons to whom the Materials are furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Materials.
//
// THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
//
// Description:
// Display Channel adaptation for owfscreen
// 
//
#include "WF/wfc.h"
#include "wfcstructs.h"
#include "wfcdevice.h"

#include "wfcdevice.h"
#include "wfcelement.h"
#include "wfccontext.h"
#include "wfcimageprovider.h"

#include "owfarray.h"
#include "owfmemory.h"
#include "owfmutex.h"
#include "owfscreen.h"
#include "owftypes.h"
#include "owfobject.h"

#include "owfdebug.h"

#include "owfnotifications.h"
#include <graphics/symbianstream.h>

#include "owfdisplaycontext.h"

#ifdef __cplusplus
extern "C" {
#endif

static void
owfSymContextSourceStreamUpdated(SymbianStreamType stream,
                                 OWFint event,
                                 void* data,
                                 void* param);

static void
owfSymDeviceSetUpdateFlags(WFC_CONTEXT* context, OWFNativeStreamType stream, WFCint updateFlags);

static WFCint
owfSymDeviceProcessAllCallBacks(WFC_CONTEXT* context, WFCint events);

static void
owfSymDeviceSetVisibilityState(WFC_CONTEXT* context, OWFNativeStreamType stream, WFCint visibility);

static OWFint owfGetScreenNumber(WFC_CONTEXT* context)
{
    OWF_ASSERT(context);
    return context->type == WFC_CONTEXT_TYPE_ON_SCREEN ? context->screenNumber : -context->handle;
}

static void
owfSymContextSourceStreamUpdated(SymbianStreamType stream,
                                 OWFint event,
                                 void* data,
                                 void* param)
{
    OWF_ASSERT(data);

    (void) stream; /* suppress compiler warning */
 
    switch (event)
    {
    case ESOWF_ObserverReturnDefaultEvent:
        if (param)
        {
            SYMOWF_DEFAULT_EVENT_PARAM* parameter = (SYMOWF_DEFAULT_EVENT_PARAM*) param;
            if ((parameter->length) == sizeof(SYMOWF_DEFAULT_EVENT_PARAM))
            {
                parameter->event = ESOWF_EventUpdated;
            }
        }
        return;
        
    case ESOWF_EventUpdated:
        {
            WFC_CONTEXT*            context = NULL;
            context = CONTEXT(data);
            OWF_ASSERT(context);
            OWF_ASSERT(context->device);
            
            OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(context->displayContext);
            
            if (param)
                {
                SYMOWF_CONTENT_UPDATED_PARAM* parameter = (SYMOWF_CONTENT_UPDATED_PARAM*) param;
                OWFNativeStreamType owfStream = (OWFNativeStreamType) stream;
                
                if ((parameter->length) == sizeof(SYMOWF_CONTENT_UPDATED_PARAM))
                    {
                        switch (parameter->id)
                            {
                            case SYM_CONTENT_UPDATE_BEGIN:
                                {
                                    OWF_Mutex_Lock(&context->updateFlagMutex);
                                    NFPRINT(("+++ BEGIN CONTENT UPDATE"));
                                    
                                    parameter->immediateAvailable = 0;
                                    parameter->immediateVisibility = SYM_CONTENT_VISIBLE_NOT_SET;
                                    if (pDispCtx->iCompositionOngoing &&
                                        context->device &&
                                        pDispCtx->iPreviousCommit == SYM_CONTENT_PREVIOUS_COMMIT_FALSE)
                                    {
                                        TInt i = 0;
                                        parameter->immediateAvailable = 1;
                                        parameter->serialNumber = pDispCtx->iSerialNumber;
                                        for (i = 0; i < context->device->providers.length; i++)
                                        {
                                            WFC_IMAGE_PROVIDER* provider;
    
                                            provider = IMAGE_PROVIDER(OWF_Array_GetItemAt(&context->device->providers, i));
                                            
                                            if (provider->type == WFC_IMAGE_SOURCE && 
                                                provider->streamHandle == owfStream && 
                                                context == provider->owner)
                                            {
                                                parameter->immediateVisibility = provider->contentVisible;
                                                break;
                                            }
                                        }
                                    }
                                    //the mutex is acquired!
                                }
                                return;
                                
                            case SYM_CONTENT_UPDATE_END:
                                ++context->sourceUpdateCount;
                                pDispCtx->iEventServerUpdate |= parameter->par;
                                owfSymDeviceSetUpdateFlags(context, owfStream, parameter->par);
                                OWF_Mutex_Unlock(&context->updateFlagMutex);
                                NFPRINT(("+++ END CONTENT UPDATE"));
                                break;
                                
                            case SYM_CONTENT_UPDATE:
                                NFPRINT(("+++ CONTENT UPDATE"));
                                OWF_Mutex_Lock(&context->updateFlagMutex);
                                ++context->sourceUpdateCount;
                                pDispCtx->iEventServerUpdate |= parameter->par;
                                owfSymDeviceSetUpdateFlags(context, owfStream, parameter->par);
                                OWF_Mutex_Unlock(&context->updateFlagMutex);
                                break;
                            default:
                                break;
                            }
                    }
                else
                    {
                    NFPRINT(("+++ WFC_Context_SourceStreamUpdated parameter length error(%d)",
                            parameter->length));
                    
                    }
                }
            else
                {
                // Old, legacy, behaviour, not sure if really needed
                NFPRINT(("+++ LEGACY CONTENT UPDATE"));
                OWF_Mutex_Lock(&context->updateFlagMutex);
                ++context->sourceUpdateCount;
                OWF_Mutex_Unlock(&context->updateFlagMutex);
                }
        }
        return;
    default:
        return;
    }
}

static void
owfSymDeviceSetUpdateFlags(WFC_CONTEXT* context, OWFNativeStreamType stream, WFCint updateFlags)
{
    WFC_DEVICE* device = NULL;
    WFCint      i;
    
    OWF_ASSERT(context && (context->device));

    device = context->device;
    
    for (i = 0; i < device->providers.length; i++)
    {
        WFC_IMAGE_PROVIDER* provider = IMAGE_PROVIDER(OWF_Array_GetItemAt(&device->providers, i));
    
        if (provider->type == WFC_IMAGE_SOURCE && provider->streamHandle == stream && context == provider->owner)
        {
            NFPRINT(("+++ owfSymDeviceSetUpdateFlags set context updated stream(0x%x) provider(0x%x) flags(0x%x)", stream, provider, updateFlags));
            provider->contentUpdated |= updateFlags;
        }
    }
}

static void
owfSymDeviceSetVisibilityState(WFC_CONTEXT* context, OWFNativeStreamType stream, WFCint visibility)
{
    WFC_DEVICE* device = NULL;
    WFCint      i;
    
    OWF_ASSERT(context && (context->device));

    device = context->device;
    
    for (i = 0; i < device->providers.length; i++)
    {
        WFC_IMAGE_PROVIDER* provider = IMAGE_PROVIDER(OWF_Array_GetItemAt(&device->providers, i));
        
        if (provider->type == WFC_IMAGE_SOURCE && provider->streamHandle == stream && context == provider->owner)
        {
            provider->contentVisible = visibility;
        }
    }
}

static WFCint
owfSymDeviceProcessAllCallBacks(WFC_CONTEXT* context, WFCint events)
{
    WFC_DEVICE* device = NULL;
    WFCint      i;
    WFCint newMask = 0;
    NFPRINT(("+++ START owfSymDeviceProcessAllCallBacks events=0x%x", events));
    
    if (!context || !(context->device))
    {
        return newMask;
    }
    device = context->device;
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(context->displayContext);

    for (i = 0; i < device->providers.length; i++)
    {
        WFC_IMAGE_PROVIDER* source = IMAGE_PROVIDER(OWF_Array_GetItemAt(&device->providers, i));
        
        if (source->contentVisible == SYM_CONTENT_VISIBLE_NOT_SET)
        {
            source->contentVisible = SYM_CONTENT_NOT_VISIBLE;
        }
        
        NFPRINT(("+++ owfSymDeviceProcessAllCallBacks provider=0x%x v=%d t=%d o=%d u=0x%x c=0x%x ", 
                 source, 
                 source->contentVisible == SYM_CONTENT_VISIBLE, 
                 source->type == WFC_IMAGE_SOURCE, 
                 context == source->owner,
                 source->contentUpdated,
                 source->composed));
        
        if (source->contentVisible == SYM_CONTENT_VISIBLE && 
            source->type == WFC_IMAGE_SOURCE && 
            context == source->owner && 
            source->contentUpdated & events &&
            source->composed)
        {
            NFPRINT(("+++ START owfSymDeviceProcessAllCallBacks process all notifications handle=0x%x", 
                    source->streamHandle));
        
            SymbianStreamProcessNotifications((SymbianStreamType)source->streamHandle, 
                                              events & source->contentUpdated, 
                                              owfGetScreenNumber(context),
                                              pDispCtx->iSerialNumber,
                                              &newMask);

            source->visited |= events;
        
            source->contentUpdated &= ~events;
            source->contentUpdated |= newMask;
            source->composed = 0;
        }
    }
    
    for (i = 0; i < device->providers.length; i++)
    {
        WFC_IMAGE_PROVIDER* source = IMAGE_PROVIDER(OWF_Array_GetItemAt(&device->providers, i));
        
        if (!source->visited && 
            source->type == WFC_IMAGE_SOURCE && 
            context == source->owner && 
            (source->contentUpdated & events))
        {
            NFPRINT(("+++ START owfSymDeviceProcessAllCallBacks check visible handle=0x%x", 
                    source->streamHandle));
                SymbianStreamCheckVisible((SymbianStreamType)source->streamHandle,
                                          events & source->contentUpdated,
                                          owfGetScreenNumber(context),
                                          pDispCtx->iSerialNumber);
                
                source->contentUpdated &= ~events;
            }
        source->visited = 0;
    }
    
    return newMask;
}


OWF_API_CALL OWFboolean
owfSymDeviceInitialise(WFC_IMAGE_PROVIDER* source)
{
    WFC_CONTEXT* context = NULL;
    WFC_DEVICE* device = NULL;
    WFCint i = 0;
    OWFNativeStreamType stream;
    
    if (!source || !(source->owner) || ! (((WFC_CONTEXT*)source->owner)->device))
    {
        return OWF_FALSE;
    }
   
    context = (WFC_CONTEXT*)source->owner;
    device = context->device;
    
    if (source->type == WFC_IMAGE_MASK)
    {
        return OWF_TRUE;
    }
    
    stream = source->streamHandle;
    
    source->contentVisible = SYM_CONTENT_VISIBLE_NOT_SET;
    source->visited = 0;
    source->composed = 0;
    
    // forward check to copy the visibility state
    for (i = 0; i < device->providers.length; i++)
    {
        WFC_IMAGE_PROVIDER* provider = IMAGE_PROVIDER(OWF_Array_GetItemAt(&device->providers, i));
        
        if (provider->type == WFC_IMAGE_SOURCE && provider->streamHandle == stream && context == provider->owner)
        {
            source->contentVisible = provider->contentVisible;
            break;
        }
    }
    
    SymbianStreamAddExtendedObserver((SymbianStreamType)source->streamHandle, 
                                  owfSymContextSourceStreamUpdated, 
                                  context, 
                                  owfGetScreenNumber(context),
                                  ESOWF_EventUpdated);
    return OWF_TRUE;

}

OWF_API_CALL void
owfSymDeviceDestroy(WFC_DEVICE* device, WFC_IMAGE_PROVIDER* source, WFCint position)
{
    WFCint i = 0;
    OWFboolean foundAnotherSource = OWF_FALSE;

    OWF_ASSERT(device);
    OWF_ASSERT(source);

    if (source->type == WFC_IMAGE_SOURCE)
    {
        OWFNativeStreamType  stream = source->streamHandle;
        WFC_CONTEXT* context = (WFC_CONTEXT*)source->owner;
        OWF_ASSERT(context);
        for (i = 0; i < device->providers.length; i++)
        {
            if (i != position)
            {
                WFC_IMAGE_PROVIDER* newObject = (WFC_IMAGE_PROVIDER*)(OWF_Array_GetItemAt(&device->providers, i));
                if ((stream == newObject->streamHandle) && 
                    (context == newObject->owner) && 
                    (newObject->type == WFC_IMAGE_SOURCE))
                {
                    foundAnotherSource = OWF_TRUE;
                    break;
                }
            }
        }
        
        if (!foundAnotherSource)
        {
        WFCint screenNumber = owfGetScreenNumber(context);
        SymbianStreamRemoveObserver((SymbianStreamType)stream,
                                            context,
                                            ESOWF_EventUpdated);
        }
    }
}

OWF_API_CALL OWFboolean
owfSymElementNotifications(WFC_CONTEXT* context, WFC_ELEMENT* element)
{
    WFC_IMAGE_PROVIDER* source = NULL;
    OWFNativeStreamType stream = OWF_INVALID_HANDLE;
    
    OWF_ASSERT(context && element && (context->device) && (element->source));
    
    source = IMAGE_PROVIDER(element->source);
    
    if (source->type != WFC_IMAGE_SOURCE)
    {
        return OWF_TRUE;
    }
    
    OWF_Mutex_Lock(&context->updateFlagMutex);
    source->composed = source->contentUpdated | ESOWF_EventComposed;
    NFPRINT(("+++ END owfSymElementNotifications composed s=0x%x u=0x%x", source, source->composed));
    
    if (source->contentVisible != SYM_CONTENT_VISIBLE_NOT_SET)
    {
        OWF_Mutex_Unlock(&context->updateFlagMutex);
        return OWF_TRUE;
    }
    
    stream = source->streamHandle;
    
    if (!element->skipCompose)
        {
        owfSymDeviceSetVisibilityState(context, stream, SYM_CONTENT_VISIBLE);
        }
    
    OWF_Mutex_Unlock(&context->updateFlagMutex);
    
    return OWF_TRUE;
}

OWF_API_CALL void
owfSymDeviceResetVisibilityState(WFC_CONTEXT* context)
{
    WFC_DEVICE* device = NULL;
    WFCint      i;
    
    OWF_ASSERT(context && (context->device));

    device = context->device;
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(context->displayContext);
    
    OWF_Mutex_Lock(&context->updateFlagMutex);
    pDispCtx->iPreviousCommit = SYM_CONTENT_PREVIOUS_COMMIT_TRUE;
    context->sourceUpdateCount++;
    for (i = 0; i < device->providers.length; i++)
    {
        WFC_IMAGE_PROVIDER* provider = IMAGE_PROVIDER(OWF_Array_GetItemAt(&device->providers, i));
        
        if (context == provider->owner)
        {
            provider->contentVisible = SYM_CONTENT_VISIBLE_NOT_SET;
        }
    }
    OWF_Mutex_Unlock(&context->updateFlagMutex);
}

OWF_API_CALL WFCint
owfSymProcessAllNotifications(WFC_CONTEXT* context)
{
    WFCint newContextUpdateEvents = 0;
    
    if (!context )
    {
        NFPRINT(("+++ END owfSymProcessAllNotifications (context = 0)"));
        return 0;
    }
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(context->displayContext);
    
    NFPRINT(("+++ START owfSymProcessAllNotifications events=0x%x", pDispCtx->iEventServerUpdate));
    OWF_Mutex_Lock(&context->updateFlagMutex);
    pDispCtx->iCompositionOngoing = WFC_FALSE;
    
    if (pDispCtx->iEventServerUpdate || pDispCtx->iPreviousCommit == SYM_CONTENT_PREVIOUS_COMMIT_TRUE)
    {
        newContextUpdateEvents |= owfSymDeviceProcessAllCallBacks(context, pDispCtx->iEventServerUpdate);
    
        pDispCtx->iEventServerUpdate = newContextUpdateEvents;
        
        if (newContextUpdateEvents)
        {
            context->sourceUpdateCount++;
        }
    }
    
    pDispCtx->iPreviousCommit = SYM_CONTENT_PREVIOUS_COMMIT_FALSE;
    OWF_Mutex_Unlock(&context->updateFlagMutex);
    
    NFPRINT(("+++ END owfSymProcessAllNotifications newEvents=0x%x", newContextUpdateEvents));
    return newContextUpdateEvents;
}

#ifdef __cplusplus
}
#endif

