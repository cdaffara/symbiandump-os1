// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of Test class for OpenWfc Native Stream 
// 
//

#include <graphics/symbianstream.h>
#include <graphics/streammap.h>
#include <graphics/compositionsurfaceupdate.h>
#include <graphics/extensioncontainer.h>
#include <graphics/suerror.h>
#include <test/extendtef.h>
#include <dispchannel.h>
#include <hal.h>
#include <test/singletontestexithelper.inl>
#include "tnativestream.h"
#include "surfaceutility.h"

#ifdef EGLSYNCHELPER_INCLUDED
    #include <EGL/egl.h>
    #include "eglsynchelper.h"
#endif

#define BUFFER_READ_HANDLE_BASE  0x100
#define BUFFER_WRITE_HANDLE_BASE 0x200
#define INDEX_TO_READ_HANDLE(x)  ((SymbianStreamBuffer) ((x)+BUFFER_READ_HANDLE_BASE))
#define INDEX_TO_WRITE_HANDLE(x)  ((SymbianStreamBuffer) ((x)+BUFFER_WRITE_HANDLE_BASE))
#define BUFFER_READ_HANDLE_TO_INDEX(x)  (TInt) (x > 0 ? (x&0xFF) : (x-BUFFER_READ_HANDLE_BASE))
#define BUFFER_WRITE_HANDLE_TO_INDEX(x)  (TInt) (x > 0 ? (x&0xFF) : (x-BUFFER_WRITE_HANDLE_BASE))
#define WFC_INVALID_HANDLE NULL
#define KGrowCleanupStack   12
#define KCompositorVersion 0x01023456
#define KCompositorVersionMajor 0x1
#define KCompositorVersionMinor 0x2
#define KCompositorVersionRevision 0x3456

void PopHeap(void* aHeapPtr)
    {
    User::SwitchHeap((RHeap*)aHeapPtr);
    }

void GrowCleanupStackL()
    {
    TInt n = KGrowCleanupStack;
    while(n--)
        {
        CleanupStack::PushL((CBase*)NULL);
        }
    CleanupStack::Pop(KGrowCleanupStack);
    }

// Helper functions
/* supported external image formats */
enum OWF_PIXEL_FORMAT   {
    OWF_IMAGE_NOT_SUPPORTED = 0,
    OWF_IMAGE_ARGB8888      = 0x8888,
    OWF_IMAGE_XRGB8888      = 0xf888,
    OWF_IMAGE_RGB888        = 0x888,
    OWF_IMAGE_RGB565        = 0x565,
    OWF_IMAGE_L32           = 0xA32,
    OWF_IMAGE_L16           = 0xA16,
    OWF_IMAGE_L8            = 0xA8,
    OWF_IMAGE_L1            = 0xA1,
    OWF_IMAGE_ARGB_INTERNAL = 0x666 /* OWFpixel rep */
} ;

struct CTestNativeStream::OWF_IMAGE_FORMAT{
    OWF_PIXEL_FORMAT        pixelFormat;
    bool              linear;
    bool              premultiplied;
    int                  rowPadding; /* row alignment, in bytes */
} ;

TInt CTestNativeStream::BytesPerPixel(TUidPixelFormat aPixelFormat)
    {
    switch (aPixelFormat)
        {
        case EUidPixelFormatXRGB_8888:
        case EUidPixelFormatARGB_8888:
        case EUidPixelFormatBGRX_8888:
        case EUidPixelFormatXBGR_8888:
        case EUidPixelFormatBGRA_8888:
        case EUidPixelFormatABGR_8888:  
        case EUidPixelFormatABGR_8888_PRE:
        case EUidPixelFormatARGB_8888_PRE:
        case EUidPixelFormatBGRA_8888_PRE:
        case EUidPixelFormatARGB_2101010:
        case EUidPixelFormatABGR_2101010:
            return 4;
        case EUidPixelFormatBGR_888:
        case EUidPixelFormatRGB_888:
            return 3;
        case EUidPixelFormatXRGB_4444:
        case EUidPixelFormatARGB_4444:
        case EUidPixelFormatXBGR_4444:
        case EUidPixelFormatRGB_565:
        case EUidPixelFormatBGR_565:
        case EUidPixelFormatARGB_1555:
        case EUidPixelFormatXRGB_1555:
        case EUidPixelFormatARGB_8332:
        case EUidPixelFormatBGRX_5551:
        case EUidPixelFormatBGRA_5551:
        case EUidPixelFormatBGRA_4444:
        case EUidPixelFormatBGRX_4444:
        case EUidPixelFormatAP_88:
            return  2;
        case EUidPixelFormatRGB_332:
        case EUidPixelFormatBGR_332:
        case EUidPixelFormatA_8:
        case EUidPixelFormatL_8:
            return  1;
        case EUidPixelFormatP_8:
            return -1;
        case EUidPixelFormatP_4:
        case EUidPixelFormatL_4:
            return -2;
        case EUidPixelFormatL_2:
        case EUidPixelFormatP_2:
            return -4;
        case EUidPixelFormatL_1 :
            return -8;
        default:
            {
            return 0;
            }
        }
    }

/*****************************************
 * Helper Creates Surface from OWF spec 
 * 
 *
 */
TSurfaceId CTestNativeStream::helperCreateSurfaceL(khronos_int32_t width,
        khronos_int32_t height,
        const OWF_IMAGE_FORMAT*   format,
        khronos_int32_t nbufs,
        TUidPixelFormat overridePixelFormat)
	{
	RSurfaceManager::TSurfaceCreationAttributesBuf bf;
	RSurfaceManager::TSurfaceCreationAttributes& b = bf();
	
	TBool premultiplied = format->premultiplied;
	OWF_PIXEL_FORMAT pixelFormat = format->pixelFormat;
	khronos_int32_t bytesPerPixel = 0;
	
	if (overridePixelFormat != EUidPixelFormatUnknown)
	    {
        bytesPerPixel = BytesPerPixel(overridePixelFormat);
        b.iAlignment = 4;
        b.iPixelFormat = overridePixelFormat;
	    }
	else
	    {
        switch(pixelFormat)
            {
            case OWF_IMAGE_RGB565:
                b.iPixelFormat = EUidPixelFormatRGB_565;
                bytesPerPixel = 2;
                b.iAlignment = 4;
                break;
            case OWF_IMAGE_ARGB8888:
                {
                if (premultiplied)
                    {
                    b.iPixelFormat = EUidPixelFormatARGB_8888_PRE;
                    }
                else
                    {
                    b.iPixelFormat = EUidPixelFormatARGB_8888;
                    }
                bytesPerPixel = 4;
                b.iAlignment = 4;
                break;
                }
            case OWF_IMAGE_XRGB8888 :
                b.iPixelFormat = EUidPixelFormatXRGB_8888;
                bytesPerPixel = 4;
                b.iAlignment = 4;
                break;
            case OWF_IMAGE_L8 :
                b.iPixelFormat = EUidPixelFormatA_8;
                bytesPerPixel = 1;
                b.iAlignment = 4;
                break;
            case OWF_IMAGE_L1 :
                b.iPixelFormat = EUidPixelFormatL_1;
                bytesPerPixel = -8;
                b.iAlignment = 4;
                break;
            default:
                User::Leave(KErrNotSupported);	
                break;
            }
	    }
	
	b.iSize.iWidth = width;
	b.iSize.iHeight = height;
	b.iBuffers = nbufs;								// number of buffers in the surface
	b.iOffsetToFirstBuffer = 0;						// way of reserving space before the surface pixel data
	if (bytesPerPixel >= 0)
	    {
	    b.iStride = bytesPerPixel * width;	// number of bytes between start of one line and start of next	
	    }
	else
	    {
	    b.iStride = (width-(bytesPerPixel+1)) / (-bytesPerPixel);
	    }
	b.iContiguous = EFalse;
	b.iMappable = ETrue;
	
	TSurfaceId surface = TSurfaceId::CreateNullId();
	User::LeaveIfError(iUtility->Manager().CreateSurface(bf, surface));
	
	return surface;
	}

SymbianStreamType CTestNativeStream::helperCreateImageStream(khronos_int32_t width,
                                 khronos_int32_t height,
                                 const OWF_IMAGE_FORMAT*  format,
                                 khronos_int32_t nbufs,
                                 TUidPixelFormat overridePixelFormat)
	{
	TSurfaceId surface;
	
	TRAPD(err,surface = helperCreateSurfaceL(width, height, format, nbufs, overridePixelFormat));
	if (err)
	    {
	    return WFC_INVALID_HANDLE;
	    }
	SymbianStreamType ns;
	SymbianStreamAcquire(&surface, &ns);
	
	iUtility->Manager().CloseSurface(surface);
	
	return ns;
	}

RSemaphore      gSemaphore;
RSemaphore      gSemaphore2;

TGlobalNativeStreamVar	gVarInstance={0};
const TGlobalNativeStreamVar& TGlobalNativeStreamVar::Instance()
	{
	return gVarInstance;
	}

void TGlobalNativeStreamVar::SetSurfaceID(TSurfaceId aSurfaceID)
    {
    iSurfaceID = aSurfaceID;
    }

void TGlobalNativeStreamVar::SetTestComplete(TBool aTestComplete)
    {
    iTestComplete = aTestComplete;
    }

void TGlobalNativeStreamVar::SetBuffers(TInt aBuffers)
    {
    iBuffers = aBuffers;
    }

TSurfaceId TGlobalNativeStreamVar::SurfaceID() const
    {
    return iSurfaceID;
    }

TBool TGlobalNativeStreamVar::TestComplete() const
    {
    return iTestComplete;
    }

TInt TGlobalNativeStreamVar::Buffers() const
    {
    return iBuffers;
    }

/*
 * CTestNativeStream implementation
 * 
 * 
 * 
 * 
 */

CTestNativeStream::CTestNativeStream(): iUtility(this)
	{
	// No implementation required
	}

CTestNativeStream::~CTestNativeStream()
	{
	DeleteOwfSingletons();
	}

void CTestNativeStream::SetupL()
	{
	TRAPD(err_FailedToCreateSurfaceUtility, iUtility = CSurfaceUtility::NewL( NULL ));
	ASSERT_EQUALS(err_FailedToCreateSurfaceUtility,KErrNone);
	DefineOwfSingletonKeys();
	}

/**
 * test Suite furniture 
 **/
void CTestNativeStream::TearDownL()
	{
	delete iUtility();
	}

/**
WFC context callback function invoked by native stream when the stream's content
is updated. For testing, we simply call a class member function that checks that the
correct native stream handle and events mask were supplied.
*/
void CTestNativeStream::SourceStreamUpdatedCallback(
        SymbianStreamType aNs, khronos_int32_t aEvents, void* aData, void* aParam)
    {
    (void)aData;
    if (aEvents == ESOWF_ObserverReturnDefaultEvent && aParam)
        {
        SYMOWF_DEFAULT_EVENT_PARAM* parameter = (SYMOWF_DEFAULT_EVENT_PARAM*) aParam;
        if ((parameter->length) == sizeof(SYMOWF_DEFAULT_EVENT_PARAM))
            {
            parameter->event = ESOWF_EventUpdated;
            }
        return;
        }
        
    ASSERT(CTestNativeStream::iTester);
    CTestNativeStream::iTester->CheckSourceStreamUpdated(aNs, aParam);
    }

void CTestNativeStream::CheckSourceStreamUpdated(SymbianStreamType aNs, void* aParam)
    {
    RHeap *h1 = &(User::Heap());
    if (aParam)
        {
        ASSERT_EQUALS(iNs, aNs);
        SYMOWF_CONTENT_UPDATED_PARAM* param = (SYMOWF_CONTENT_UPDATED_PARAM*) aParam;
        ASSERT_TRUE(param->id == SYM_CONTENT_UPDATE_BEGIN || 
                    param->id == SYM_CONTENT_UPDATE_END ||
                    param->id == SYM_CONTENT_UPDATE);

        iSourceStreamUpdatedCalled++;
        iStreamUpdatedParameter = param->id; 
        switch(param->id)
            {
            case SYM_CONTENT_UPDATE_BEGIN:
                param->immediateAvailable = iImmediateAvailable;
                param->immediateVisibility = iImmediateVisible;
                param->serialNumber = iStreamUpdatedSerialNumber;
                break;
                
            case SYM_CONTENT_UPDATE:
            case SYM_CONTENT_UPDATE_END:
                ASSERT_EQUALS(iExpectedSourceStreamUpdatedEventMask, param->par);        
                iContextUpdatedFlags |= param->par & (~ESOWF_EventUpdated);    
                break;
            default:
                break;
            }
        }
    else
        {
        iSourceStreamUpdatedCalled++;
        }
        
    }

/**
Remove the native stream notifications. The creator of the source is responsible
for destroying the native stream. 

Now with the new SymbianStreamRemoveObserver function we need to pass in an observer
*/
void CTestNativeStream::RemoveNsNotifications()
    {
    SymbianStreamRemoveObserver(iNs, &iScreenNo, ESOWF_EventAvailable);
    SymbianStreamRemoveObserver(iNs, &iScreenNo, ESOWF_EventDisplayed);
    SymbianStreamRemoveObserver(iNs, &iScreenNo, ESOWF_EventDisplayedX);
    }

CTestNativeStream* CTestNativeStream::iTester = NULL;

// Create a suite of all the tests
CTestSuite* CTestNativeStream::CreateSuiteL(const TDesC& aName)
	{
	SymbianStreamRegisterScreenNotifications(0, 10, KCompositorVersion);
	SUB_SUITE_OPT(CTestNativeStream,NULL);

		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0100L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0101L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0102L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0103L);
		
		// Test with 1, 2 and 3 buffers
		ADD_TEST_STEP_PARAM_RANGE(GRAPHICS_OPENWFC_NATIVESTREAM_0104L,1,4);
		ADD_TEST_STEP_PARAM_RANGE(GRAPHICS_OPENWFC_NATIVESTREAM_0105L,1,4);
		ADD_TEST_STEP_PARAM_RANGE(GRAPHICS_OPENWFC_NATIVESTREAM_0106L,1,4);
		
		// Concurrent tests
		ADD_TEST_STEP_PARAM_RANGE(CreateSharedNativeStreamL,1,4);
		ADD_THIS_TEST_STEP(DestroySharedNativeStreamL);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0107_1L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0107_2L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0107_3L);
		
		// Test SUS with OpenWF pipeline
		ADD_TEST_STEP_PARAM_RANGE(GRAPHICS_OPENWFC_NATIVESTREAM_0108L, 1, 4);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0109L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0110L);
		
		// Test various cases for Native Stream callbacks
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0111L);
		// Test multithreaded cases for Native Stream callbacks
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0112_1L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0112_2L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0112_3L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0112_4L);
		
		// Notification tests
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0113L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0114L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0115L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0116L);
		
		// Notification cancel
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0117_1L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0117_2L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0117_3L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0117_4L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0118_1L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0118_2L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0118_3L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0118_4L);
		
		// Notification overflow
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0119_1L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0119_2L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0119_3L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0120_1L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0120_2L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0120_3L);
		
		// Notifications cancelled due to removal of source observer
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0122L);
		
		// Sym native stream add/remove observers
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0130L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0131L);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0132L);
		
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0133L);

		// Test with 1, 2 and 3 buffers
		ADD_TEST_STEP_PARAM_RANGE(GRAPHICS_OPENWFC_NATIVESTREAM_0140L,1,4);
		ADD_TEST_STEP_PARAM_RANGE(GRAPHICS_OPENWFC_NATIVESTREAM_0141L,1,4);
		ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0142L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0143L);
        ADD_THIS_TEST_STEP(GRAPHICS_OPENWFC_NATIVESTREAM_0144L);

	END_SUITE;	
	
	}

// This handles any non-member uses of the extended ASSERT_XXX macros
void TefUnitFailLeaveL()
	{
	User::Leave(KErrTEFUnitFail);
	}

SymbianStreamType CTestNativeStream::NsCheckL(const TSurfaceId aId, TInt aCheck, TBool aFind)
	{
	// Acquire (create OR find) the stream
	SymbianStreamType ns = NULL;
	TInt err;
	if (aFind)
		{
		err = (TInt)SymbianStreamFind(&aId, &ns);
		}
	else
		{
		err = SymbianStreamAcquire(&aId, &ns);
		}
	ASSERT_TRUE(err == KErrNone);
	// Check the hash map count
	ASSERT_EQUALS((COpenWfcStreamMap::InstanceL().Count()), aCheck);
    SymbianStreamBuffer bufferHandle;
    err = SymbianStreamAcquireReadBuffer(ns,&bufferHandle);
    ASSERT_TRUE(err == KErrNone);
    long bufferIndex;
    const TSurfaceId* checkId = NULL;
    err = SymbianStreamGetBufferId(ns,bufferHandle,&bufferIndex,&checkId);
    ASSERT_TRUE(err == KErrNone);
    SymbianStreamReleaseReadBuffer(ns,bufferHandle);
	ASSERT_NOT_NULL(checkId);
	ASSERT_EQUALS(*checkId, aId);
	return ns;
	}

void CTestNativeStream::CreateSharedNativeStreamL(TInt aBuffers)
	{
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    GrowCleanupStackL();
    
	CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
	
	gVarInstance.SetTestComplete(EFalse);
	gVarInstance.SetBuffers(aBuffers);
	
	TSize surfaceSize(TSize(100,100));
	khronos_int32_t width = surfaceSize.iWidth;
	khronos_int32_t height = surfaceSize.iHeight;
	
	OWF_IMAGE_FORMAT pixelFormat =
		{
		OWF_IMAGE_ARGB8888,
		ETrue,
		ETrue,
		4
		};	
	
	// Create the first stream
	SymbianStreamType ns=helperCreateImageStream(width,
													height,
													&pixelFormat,
													aBuffers);
	ASSERT_TRUE(ns);
	
    SymbianStreamBuffer bufferHandle;
    SymbianStreamAcquireReadBuffer(ns,&bufferHandle);
    long bufferIndex;
    const TSurfaceId* checkId = NULL;
    TInt err = SymbianStreamGetBufferId(ns,bufferHandle,&bufferIndex,&checkId);
    ASSERT_TRUE(err == KErrNone);
    SymbianStreamReleaseReadBuffer(ns,bufferHandle);
	gVarInstance.SetSurfaceID(*checkId);
    gSemaphore.CreateLocal(1);
    gSemaphore.Wait(); 
    gSemaphore2.CreateLocal(1);
    gSemaphore2.Wait(); 
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
	}

void CTestNativeStream::DestroySharedNativeStreamL()
	{
	GrowCleanupStackL();
	
	CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(COpenWfcStreamMap::InstanceL().GetMainHeap())));
	
    gSemaphore2.Close();
	gSemaphore.Close();
	TSurfaceId id = gVarInstance.SurfaceID();
	SymbianStreamType ns;
	TInt err = SymbianStreamFind(&id,&ns);
	ASSERT_TRUE(err == KErrNone);
	ASSERT_TRUE(ns);
	// Decrease stream's reference count by one. This removes reference added by owfNativeStreamFind()
	SymbianStreamRemoveReference(ns);
	// Decrease stream's reference count by one to make reference count zero, and destroy the stream
	SymbianStreamRemoveReference(ns);
	CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
	}

// Test observers
void TestUpdateCallback(SymbianStreamType aStream, khronos_int32_t aEvent, void* aData, void* aParam)
    {
    (void) aStream;
    switch (aEvent)
        {
        case ESOWF_ObserverReturnDefaultEvent:
            if (aParam)
                {
                SYMOWF_DEFAULT_EVENT_PARAM* parameter = (SYMOWF_DEFAULT_EVENT_PARAM*) aParam;
                if ((parameter->length) == sizeof(SYMOWF_DEFAULT_EVENT_PARAM))
                    {
                    parameter->event = ESOWF_EventUpdated;
                    }
                }
            return;
            
        case ESOWF_EventUpdated:
            {
            TInt* localNumber = (TInt*)aData;
             ++(*localNumber);
            }
            break;
        default:
            break;
        }
    }

// Test compose target
void TestComposedCallback(SymbianStreamType aStream, khronos_int32_t aEvent, void* aData, void* aParam)
    {
    (void) aStream;
    switch (aEvent)
        {
        case ESOWF_ObserverReturnDefaultEvent:
            if (aParam)
                {
                SYMOWF_DEFAULT_EVENT_PARAM* parameter = (SYMOWF_DEFAULT_EVENT_PARAM*) aParam;
                if ((parameter->length) == sizeof(SYMOWF_DEFAULT_EVENT_PARAM))
                    {
                    parameter->event = ESOWF_EventComposed;
                    }
                }
            return;
            
        case ESOWF_EventComposed:
            {
            TInt* localNumber = (TInt*)aData;
             ++(*localNumber);
            }
            break;
        default:
            break;
        }
    }

/**
@SYMTestCaseID			GFX_OPENWFC_NATIVESTREAM_0100
@SYMTestCaseDesc		Create a native stream using SymbianStreamAcquire()
@SYMREQ					
@SYMPREQ				PREQ2400
@SYMTestType			CT
@SYMTestPriority		
@SYMTestPurpose			Verify native stream objects can be created and persist unique surface ID values
@SYMTestActions	
		Create two surfaces,
		Create two streams from the surfaces
		Read back the surface Ids from the streams
@SYMTestExpectedResults
		The surface IDs should be non-null and unique
		The streams should be non-null and unique
		The returned surface Ids should match the constructed IDs
 **/		
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0100L()
	{
	RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
	TRAPD(err, GrowCleanupStackL());
	ASSERT_TRUE(err == KErrNone);
	
	TInt count = COpenWfcStreamMap::InstanceL().Count();
	TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100),EUidPixelFormatARGB_8888_PRE,400,2);
	ASSERT_FALSE(surface.IsNull());
	CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
	SymbianStreamType ns;
	err = SymbianStreamAcquire(&surface, &ns);
	ASSERT_TRUE(err == KErrNone);
    SymbianStreamBuffer bufferHandle;
    err = SymbianStreamAcquireReadBuffer(ns, &bufferHandle);
    ASSERT_TRUE(err == KErrNone);
    long bufferIndex;
    const TSurfaceId* getId = NULL;
    err = SymbianStreamGetBufferId(ns,bufferHandle,&bufferIndex,&getId);
    ASSERT_TRUE(err == KErrNone);
    TInt ChunkHandle = 100;
    err = SymbianStreamGetChunkHandle(ns, &ChunkHandle);
    ASSERT_TRUE(err == KErrNone);    
    err = SymbianStreamReleaseReadBuffer(ns,bufferHandle);
    ASSERT_TRUE(err == KErrNone);    
	ASSERT_EQUALS(*getId,surface);
	CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
	
	TSurfaceId surface2=iUtility->CreateSurfaceL(TSize(100,100),EUidPixelFormatARGB_8888_PRE,400,2);
	ASSERT_FALSE(surface2.IsNull());
	ASSERT_NOT_EQUALS(surface,surface2);
	CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
	SymbianStreamType ns2;
	err = SymbianStreamAcquire(&surface2,&ns2);
	ASSERT_TRUE(err == KErrNone);
	ASSERT_TRUE(ns2);
	ASSERT_FALSE(SymbianStreamSame(ns, ns2));
	ASSERT_EQUALS((COpenWfcStreamMap::InstanceL().Count()), count + 2);
	err = SymbianStreamAcquireReadBuffer(ns2,&bufferHandle);
	ASSERT_TRUE(err == KErrNone);
    const TSurfaceId* getId2 = NULL;
    err = SymbianStreamGetBufferId(ns2,bufferHandle,&bufferIndex,&getId2);
    ASSERT_TRUE(err == KErrNone);
    err = SymbianStreamReleaseReadBuffer(ns2,bufferHandle);
    ASSERT_TRUE(err == KErrNone);
	ASSERT_NOT_NULL(getId2);
	ASSERT_EQUALS(*getId2,surface2);
	
	SymbianStreamRemoveReference(ns);
	ASSERT_EQUALS((COpenWfcStreamMap::InstanceL().Count()), count + 1);
	CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
	iUtility->DestroySurface(surface);
	
	CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
	SymbianStreamRemoveReference(ns2);
	ASSERT_EQUALS((COpenWfcStreamMap::InstanceL().Count()), count);
	CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
	iUtility->DestroySurface(surface2);
	}

/**
@SYMTestCaseID			GFX_OPENWFC_NATIVESTREAM_0101
@SYMTestCaseDesc		Create a native stream using SymbianStreamCreateImageStream()
@SYMREQ					
@SYMPREQ				PREQ2400
@SYMTestType			CT
@SYMTestPriority		
@SYMTestPurpose			Verify native stream objects can be created and persist unique surface ID values
@SYMTestActions	
		Create two streams from the parameters passed in
		Read back the surface Ids from the streams
@SYMTestExpectedResults
		The surface IDs should be non-null and unique
		The streams should be non-null and unique
 **/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0101L()
	{
	RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
	TRAPD(err, GrowCleanupStackL());
	ASSERT_TRUE(err == KErrNone);
	
	TSize surfaceSize(TSize(100,100));
	TInt width = surfaceSize.iWidth;
	TInt height = surfaceSize.iHeight;
	TInt buffers = 2;

	OWF_IMAGE_FORMAT pixelFormat =
		{
		OWF_IMAGE_ARGB8888,
	    ETrue,
	    ETrue,
	    4
	    };	
	
	CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
	
	// Create the first stream
	SymbianStreamType ns=helperCreateImageStream(width,
													height,
													&pixelFormat,
													buffers);
	ASSERT_TRUE(ns);
    SymbianStreamBuffer bufferHandle;
    err = SymbianStreamAcquireReadBuffer(ns,&bufferHandle);
    ASSERT_TRUE(err == KErrNone);
    long bufferIndex;
    const TSurfaceId* getId = NULL;
    err = SymbianStreamGetBufferId(ns,bufferHandle,&bufferIndex,&getId);
    ASSERT_TRUE(err == KErrNone);
    err = SymbianStreamReleaseReadBuffer(ns,bufferHandle);
    ASSERT_TRUE(err == KErrNone);
	
	// Create the second stream
	SymbianStreamType ns2=helperCreateImageStream(width,
													height,
													&pixelFormat,
													buffers);
	ASSERT_TRUE(ns2);
	
    err = SymbianStreamAcquireReadBuffer(ns2,&bufferHandle);
    ASSERT_TRUE(err == KErrNone);
    const TSurfaceId* getId2 = NULL;
    err = SymbianStreamGetBufferId(ns2,bufferHandle,&bufferIndex,&getId2);
    ASSERT_TRUE(err == KErrNone);
    err = SymbianStreamReleaseReadBuffer(ns2,bufferHandle);
    ASSERT_TRUE(err == KErrNone);
	ASSERT_NOT_NULL(getId2);
	
	ASSERT_NOT_EQUALS(ns,ns2);
	ASSERT_NOT_EQUALS(getId,getId2);
	
	SymbianStreamRemoveReference(ns);
	SymbianStreamRemoveReference(ns2);
	
	CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
	}

/**
@SYMTestCaseID			GFX_OPENWFC_NATIVESTREAM_0102
@SYMTestCaseDesc		Create a native stream 2
@SYMREQ					
@SYMPREQ				PREQ2400
@SYMTestType			CT
@SYMTestPriority		
@SYMTestPurpose			Verify native stream objects can be created and persist unique surface ID values
@SYMTestActions	
		Create two surfaces,
		Create two streams from the surfaces
		Read back the surface Ids from the streams
		Acquire multiple time the native streams
		Find native streams
@SYMTestExpectedResults
		The surface IDs should be non-null and unique
		The streams should be non-null and unique
		The returned surface Ids should match the constructed IDs
		The hash map counter should be updated accordingly when native streams are created or destroyed
 **/		
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0102L()
	{
	RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
	TRAPD(err, GrowCleanupStackL());
	ASSERT_TRUE(err == KErrNone);
	
	COpenWfcStreamMap& singleton = COpenWfcStreamMap::InstanceL();
	TInt check = COpenWfcStreamMap::InstanceL().Count() + 1;
	// Create the first surface
	TSurfaceId surfaceId1 = iUtility->CreateSurfaceL(TSize(100, 100), EUidPixelFormatARGB_8888_PRE, 400, 2);
	ASSERT_FALSE(surfaceId1.IsNull());
	
	CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
	SymbianStreamType ns1 = NsCheckL(surfaceId1, check, EFalse);
	
	TSurfaceId surfaceId2;
	surfaceId2.CreateNullId();
	SymbianStreamType ns2;
	err = SymbianStreamFind(&surfaceId2,&ns2);
	ASSERT_TRUE(err == KErrNotFound);
	ASSERT_FALSE(ns2);
	CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
	
	// Create the second surface
	surfaceId2 = iUtility->CreateSurfaceL(TSize(100, 100), EUidPixelFormatARGB_8888_PRE, 400, 2);
	ASSERT_FALSE(surfaceId2.IsNull()); 
	
	CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
	err = SymbianStreamFind(&surfaceId2,&ns2);
	ASSERT_TRUE(err == KErrNotFound);
	ASSERT_FALSE(ns2);
	
	ns2 = NsCheckL(surfaceId2, ++check, EFalse);
	
	NsCheckL(surfaceId1, check, EFalse);
	
	NsCheckL(surfaceId1, check, ETrue);
	
	NsCheckL(surfaceId2, check, ETrue);
	
	SymbianStreamRemoveReference(ns1);
	ASSERT_EQUALS((singleton.Count()), check);
	
	SymbianStreamRemoveReference(ns2);
	ASSERT_EQUALS((singleton.Count()), check);
	
	SymbianStreamRemoveReference(ns2);
	ASSERT_EQUALS((singleton.Count()), --check);

	SymbianStreamRemoveReference(ns1);
	ASSERT_EQUALS((singleton.Count()), check);
	
	SymbianStreamRemoveReference(ns1);
	ASSERT_EQUALS((singleton.Count()), --check);
	CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
	
	iUtility->DestroySurface(surfaceId1);
	iUtility->DestroySurface(surfaceId2);
	
	CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
	err = SymbianStreamFind(&surfaceId2,&ns2);
	ASSERT_TRUE(err == KErrNotFound);
	ASSERT_FALSE(ns2);
	CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
	}

struct SupportedFormats
    {
    TUidPixelFormat symbianPixelFormat;
    } supportedFormats[]=
    {
        EUidPixelFormatXRGB_8888,
        EUidPixelFormatARGB_8888,
        EUidPixelFormatBGRX_8888,
        EUidPixelFormatXBGR_8888,
        EUidPixelFormatBGRA_8888,
        EUidPixelFormatABGR_8888, 
        EUidPixelFormatABGR_8888_PRE,
        EUidPixelFormatARGB_8888_PRE,
        EUidPixelFormatBGRA_8888_PRE,
        EUidPixelFormatARGB_2101010,
        EUidPixelFormatABGR_2101010,
        EUidPixelFormatBGR_888,
        EUidPixelFormatRGB_888,
        EUidPixelFormatXRGB_4444,
        EUidPixelFormatARGB_4444,
        EUidPixelFormatXBGR_4444,
        EUidPixelFormatRGB_565,
        EUidPixelFormatBGR_565,
        EUidPixelFormatARGB_1555,
        EUidPixelFormatXRGB_1555,
        EUidPixelFormatARGB_8332,
        EUidPixelFormatBGRX_5551,
        EUidPixelFormatBGRA_5551,
        EUidPixelFormatBGRA_4444,
        EUidPixelFormatBGRX_4444,
        EUidPixelFormatAP_88,
        EUidPixelFormatRGB_332,
        EUidPixelFormatBGR_332,
        EUidPixelFormatA_8,
        EUidPixelFormatL_8,
        EUidPixelFormatP_8,
        EUidPixelFormatP_4,
        EUidPixelFormatL_4,
        EUidPixelFormatL_2,
        EUidPixelFormatP_2,
        EUidPixelFormatL_1
    };

/**
@SYMTestCaseID			GFX_OPENWFC_NATIVESTREAM_0103
@SYMTestCaseDesc		Retrieve stream attributes using SymbianStreamGetHeader()
@SYMREQ					
@SYMPREQ				PREQ2400
@SYMTestType			CT
@SYMTestPriority		
@SYMTestPurpose			Verify native stream object attributes can be retrieved.
@SYMTestActions	
		Create a native stream based on a supported image/pixel format
		Retrieve all of the stream attributes
		Retreive none of the stream attributes
@SYMTestExpectedResults
		The retrieved attributes should match the parameters used to create the surface stream
		Retrieving no attributes should not cause a crash
 **/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0103L()
	{
	// Native stream attributes
    TSize surfaceSize(TSize(100,50));
    khronos_int32_t width = surfaceSize.iWidth;
    khronos_int32_t height = surfaceSize.iHeight;
    khronos_int32_t numBuffers = 2;
    
    for (TInt x=0; x<sizeof(supportedFormats)/sizeof(supportedFormats[0]); x++)
        {
        INFO_PRINTF2(_L("Pixel format %x"),supportedFormats[x]);
        khronos_int32_t streamPixelSize = BytesPerPixel(supportedFormats[x].symbianPixelFormat);
        
        OWF_IMAGE_FORMAT pixelFormat =
            {
            OWF_IMAGE_NOT_SUPPORTED,
            EFalse,
            EFalse,
            2
            };  
        
        SymbianStreamType ns=helperCreateImageStream(width,
                                                     height,
                                                     &pixelFormat,
                                                     numBuffers,
                                                     supportedFormats[x].symbianPixelFormat);      //will ignore pixelFormat
        ASSERT_TRUE(ns);
        
        // Store the retrieved attributes
        TInt32 attWidth = 0;
        TInt32 attHeight = 0;
        TInt32 attStreamStride = 0;
        TInt32 attStreamFormat = EUidPixelFormatUnknown;
        TInt32 attStreamPixelSize = 0;
        
        SymbianStreamGetHeader(ns, &attWidth, &attHeight, &attStreamStride, &attStreamFormat, &attStreamPixelSize);		
            
        ASSERT_EQUALS(attWidth, width);
        ASSERT_EQUALS(attHeight, height);
        ASSERT_EQUALS((TInt32)attStreamFormat, (TInt32)supportedFormats[x].symbianPixelFormat);
        if (BytesPerPixel(supportedFormats[x].symbianPixelFormat) > 0)
            {
            ASSERT_EQUALS(attStreamStride, (streamPixelSize * width));
            }
        else
            {
            ASSERT_EQUALS(attStreamStride, (width-(streamPixelSize+1)) / (-streamPixelSize));
            }
        ASSERT_EQUALS(attStreamPixelSize, (TInt32)BytesPerPixel(supportedFormats[x].symbianPixelFormat));	
            
        SymbianStreamGetHeader(ns, NULL, NULL, NULL, NULL, NULL);
        
        SymbianStreamRemoveReference(ns);
        }
	}

/**
@SYMTestCaseID			GFX_OPENWFC_NATIVESTREAM_0104
@SYMTestCaseDesc		Acquire write/read buffers and retrieve the buffer ptr (single threaded test).
@SYMREQ					
@SYMPREQ				PREQ2400
@SYMTestType			CT
@SYMTestPriority		
@SYMTestPurpose         Verify owfNativeStreamAcquireWriteBuffer(), owfNativeStreamReleaseWriteBuffer(),
                        owfNativeStreamAcquireReadBuffer() and owfNativeStreamReleaseReadBuffer() methods
						work as expected.
						
@SYMTestActions	
		Create a native stream 
		For each buffer:
		- Acquire the write buffer (wB)
		- Get the write buffer ptr (pWB)
		- Release the write buffer
		- Acquire the read buffer (rB)
		- Get the read buffer ptr (pRB)
		- Release the read buffer	
		Repeat for each buffer. Finally:
		Acquire the write buffer
		Release the write buffer	
@SYMTestExpectedResults
		For each buffer of the native stream, check:
		- The read buffer (rB) should be the same as the write buffer (wB)
		- The read buffer address (pRB) should be the same as the write buffer address (pWB)
		If the number of buffers > 1, check:
		- The write buffer number from the previous acquire write buffer call should not be the same 
		as the write buffer number from the next acquire write buffer call
		The final acquire/release write/read calls should check:
		The write buffer number should acquire the first buffer number
 **/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0104L(TInt aNumBuffers)
	{
	RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
	TRAPD(err, GrowCleanupStackL());
	ASSERT_TRUE(err == KErrNone);

	CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
	
	ASSERT_TRUE(aNumBuffers > 0);
	
	TSize surfaceSize(TSize(100,100));
	khronos_int32_t width = surfaceSize.iWidth;
	khronos_int32_t height = surfaceSize.iHeight;
	
	OWF_IMAGE_FORMAT pixelFormat =
		{
		OWF_IMAGE_ARGB8888,
	    ETrue,
	    ETrue,
	    aNumBuffers
	    };	
	
	SymbianStreamType ns=helperCreateImageStream(width,
													height,
													&pixelFormat,
													aNumBuffers);
	ASSERT_TRUE(ns);
	
	TUint8 *pWriteBuffer = NULL;
	TUint8 *pReadBuffer = NULL;
	TUint8 *pPrevWriteBuffer = pWriteBuffer;
	khronos_int32_t writeBuffer;
	khronos_int32_t readBuffer;
	khronos_int32_t bufferIndexWrite;
	khronos_int32_t bufferIndexRead;
	khronos_int32_t bufferIndexWriteFirst;
	khronos_int32_t bufferIndexWriteFinal;
	khronos_int32_t finalWriteBuffer;
	const TSurfaceId* getId = NULL;
	
	TInt bufferCount = aNumBuffers;
	
	// Loop through the buffers
	for (TInt count=0; count<bufferCount; count++)
		{
		// Acquire the write buffer
		err = SymbianStreamAcquireWriteBuffer(ns,&writeBuffer);
		ASSERT_TRUE(err == KErrNone);
		err = SymbianStreamGetBufferId(ns,writeBuffer,&bufferIndexWrite,&getId);
		ASSERT_TRUE(err == KErrNone);
		
		if (count == 0)
		    {
		    bufferIndexWriteFirst = bufferIndexWrite;
		    }
		
		err = SymbianStreamGetBufferPointer(ns,writeBuffer,reinterpret_cast<void**>(&pWriteBuffer));
		ASSERT_TRUE(err == KErrNone);
		ASSERT_NOT_NULL(pWriteBuffer);
		
		err = SymbianStreamReleaseWriteBuffer(ns,writeBuffer);
		ASSERT_TRUE(err == KErrNone);
		
		// Acquire the read buffer
		err = SymbianStreamAcquireReadBuffer(ns,&readBuffer);
		ASSERT_TRUE(err == KErrNone);
		err = SymbianStreamGetBufferId(ns,writeBuffer,&bufferIndexRead,&getId);
		ASSERT_TRUE(err == KErrNone);
		
		err = SymbianStreamGetBufferPointer(ns,readBuffer,reinterpret_cast<void**>(&pReadBuffer));
		ASSERT_TRUE(err == KErrNone);
		ASSERT_NOT_NULL(pReadBuffer);
		
		err = SymbianStreamReleaseReadBuffer(ns,readBuffer);
		ASSERT_TRUE(err == KErrNone);
		
		// Life-cycle checks
		ASSERT_EQUALS(bufferIndexWrite, bufferIndexRead)
		ASSERT_SAME(pWriteBuffer, pReadBuffer);
		
		if (count > 0)
			{
			ASSERT_NOT_SAME(pWriteBuffer, pPrevWriteBuffer);
			}
		
		pPrevWriteBuffer = pWriteBuffer;
		}	

	// The next acquire write/reads should return the first buffer (0).

	err = SymbianStreamAcquireWriteBuffer(ns,&finalWriteBuffer);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamGetBufferId(ns,finalWriteBuffer,&bufferIndexWriteFinal,&getId);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamReleaseWriteBuffer(ns,finalWriteBuffer);
	ASSERT_TRUE(err == KErrNone);
	
	// Final checks
	ASSERT_EQUALS(bufferIndexWriteFinal, bufferIndexWriteFirst);
	
	SymbianStreamRemoveReference(ns);
	
	CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
	}

/**
@SYMTestCaseID			GFX_OPENWFC_NATIVESTREAM_0105
@SYMTestCaseDesc		Create a native stream and acquire a write buffer which is written to (single threaded test)
@SYMREQ					
@SYMPREQ				PREQ2400
@SYMTestType			CT
@SYMTestPriority		
@SYMTestPurpose			Verify native stream buffers can be written to and read from.
@SYMTestActions	
		Create a surface and fill it Red
		Create a native stream surface with the same surface properties (size, pixel format etc.)
		Acquire the native stream write buffer
		Fill the native stream surface Red
		Release the write buffer
		Acquire the native stream read buffer
		Compare the pixel data of the surface to the native stream surface
		Fill the Blue
		Compare the pixel data of the surface to the native stream surface
@SYMTestExpectedResults
		The surface pixel data is the same as the native stream surface pixel data
		After the surface pixel data is changed to Blue, the native stream surface should not be the same
 **/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0105L(TInt aNumBuffers)
	{
	RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
	TRAPD(err, GrowCleanupStackL());
	ASSERT_TRUE(err == KErrNone);
	
	// Surface properties
	TSize surfaceSize(TSize(100,100));
	
	// Create the comparison surface and fill it Red
	TSurfaceId surface;
	TRAP(err, surface = iUtility->CreateSurfaceL(TSize(surfaceSize.iWidth,surfaceSize.iHeight), 
			EUidPixelFormatXRGB_8888, surfaceSize.iWidth * 4));
	
	TRAP(err, iUtility->FillSurfaceL(surface, 0, KRgbRed));
	
	// Native stream
	khronos_int32_t width = surfaceSize.iWidth;
	khronos_int32_t height = surfaceSize.iHeight;
	
	OWF_IMAGE_FORMAT pixelFormatXRGB888 =
		{
		OWF_IMAGE_XRGB8888,
	    ETrue,
	    EFalse,
	    4
	    };	
	
	CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
	
	SymbianStreamType ns1=helperCreateImageStream(width,
													height,
													&pixelFormatXRGB888,
													aNumBuffers);
	ASSERT_TRUE(ns1);
	
	
    SymbianStreamBuffer bufferHandle;
    err = SymbianStreamAcquireReadBuffer(ns1,&bufferHandle);
    ASSERT_TRUE(err == KErrNone);
    long bufferIndex;
    const TSurfaceId* nSurface = NULL;
    err = SymbianStreamGetBufferId(ns1,bufferHandle,&bufferIndex,&nSurface);
    ASSERT_TRUE(err == KErrNone);
    err = SymbianStreamReleaseReadBuffer(ns1,bufferHandle);
    ASSERT_TRUE(err == KErrNone);
	ASSERT_NOT_NULL(nSurface);
	
	TSurfaceId* nsSurface = const_cast<TSurfaceId*>(nSurface);
	
	// Acquire write buffer. With 3 buffers we should return buffer 1
	khronos_int32_t writeBuffer1;
	err = SymbianStreamAcquireWriteBuffer(ns1,&writeBuffer1);
	ASSERT_TRUE(err == KErrNone);
	
	TUint8 *pWriteBuffer1 = NULL;
	err = SymbianStreamGetBufferPointer(ns1,writeBuffer1,reinterpret_cast<void**>(&pWriteBuffer1));
	ASSERT_TRUE(err == KErrNone);
	ASSERT_NOT_NULL(pWriteBuffer1);
	
	TRAP(err, iUtility->FillNativeStreamSurfaceL(*nsSurface, pWriteBuffer1, KRgbRed));
	
	err = SymbianStreamReleaseWriteBuffer(ns1, writeBuffer1);
	ASSERT_TRUE(err == KErrNone);
	
	// Now we should compare to see if the pixels are the same
	khronos_int32_t readBuffer1;
	err = SymbianStreamAcquireReadBuffer(ns1,&readBuffer1);
	ASSERT_TRUE(err == KErrNone);
	
	TUint8 *pReadBuffer1 = NULL;
	err = SymbianStreamGetBufferPointer(ns1,readBuffer1,reinterpret_cast<void**>(&pReadBuffer1));
	ASSERT_TRUE(err == KErrNone);
	ASSERT_NOT_NULL(pReadBuffer1);
	CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
	ASSERT_TRUE(iUtility->CompareSurfacesL(surface, 0, *nsSurface, pReadBuffer1));	
	
	// Finally, change the surface to blue. The pixels should now be different
	TRAP(err, iUtility->FillSurfaceL(surface, 0, KRgbBlue));
	ASSERT_FALSE(iUtility->CompareSurfacesL(surface, 0, *nsSurface, pReadBuffer1));	
	
	CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
	err = SymbianStreamReleaseReadBuffer(ns1,readBuffer1);
	ASSERT_TRUE(err == KErrNone);
	SymbianStreamRemoveReference(ns1);
	CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
	}

/**
@SYMTestCaseID			GFX_OPENWFC_NATIVESTREAM_0106
@SYMTestCaseDesc		Negative test - Attempt to acquire a write buffer when the write buffer has not been released (single threaded test)
@SYMREQ					
@SYMPREQ				PREQ2400
@SYMTestType			CT
@SYMTestPriority		
@SYMTestPurpose			Verify an invalid handle is returned to the calling thread if the write buffer has not been released
@SYMTestActions	
		Create a native stream surface with 1 buffer
		Acquire the native stream write buffer
		Acquire the native stream write buffer again
@SYMTestExpectedResults
		The 2nd acquire write buffer call should return OWF_INVALID_HANDLE if not single buffered.
 **/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0106L(TInt aNumBuffers)
	{
	RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
	TRAPD(err, GrowCleanupStackL());
	ASSERT_TRUE(err == KErrNone);

	CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
	
	// Create the first stream (ARGB_8888_PRE)
	TSize surfaceSize(TSize(100,100));
	khronos_int32_t width = surfaceSize.iWidth;
	khronos_int32_t height = surfaceSize.iHeight;
	
	OWF_IMAGE_FORMAT pixelFormatARGB888Pre =
		{
		OWF_IMAGE_ARGB8888,
	    ETrue,
	    EFalse,
	    4
	    };	
	
	SymbianStreamType ns1=helperCreateImageStream(width,
													height,
													&pixelFormatARGB888Pre,
													aNumBuffers);
	ASSERT_TRUE(ns1);
	
	// Acquire write buffer. With just 1 buffer we should return buffer 0
	khronos_int32_t writeBuffer1;
	err = SymbianStreamAcquireWriteBuffer(ns1,&writeBuffer1);
	ASSERT_TRUE(err != KErrBadHandle);
	
	// Try and acquire the write buffer again before we have released it
	khronos_int32_t writeBuffer2;
	err = SymbianStreamAcquireWriteBuffer(ns1,&writeBuffer2);
	ASSERT_TRUE(err != KErrBadHandle);
	if (aNumBuffers == 1)
	    {
	    ASSERT_EQUALS(writeBuffer2, writeBuffer1); 	    
	    }
	else
	    {
	    ASSERT_EQUALS(writeBuffer2, (khronos_int32_t)0); 	    
	    }
	
	err = SymbianStreamReleaseWriteBuffer(ns1,writeBuffer1);
	ASSERT_TRUE(err != KErrBadHandle);
	
	err = SymbianStreamAcquireWriteBuffer(ns1, &writeBuffer2);
	ASSERT_TRUE(err != KErrBadHandle);
	TUint8 *pWriteBuffer2 = NULL;
	err = SymbianStreamGetBufferPointer(ns1,writeBuffer2,reinterpret_cast<void**>(&pWriteBuffer2));
	ASSERT_TRUE(err != KErrBadHandle);
	ASSERT_NOT_NULL(pWriteBuffer2);
	
	SymbianStreamRemoveReference(ns1);
	
	CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
	}

/**
@SYMTestCaseID			GFX_OPENWFC_NATIVESTREAM_0107
@SYMTestCaseDesc		Attempt to acquire a write buffer when the write buffer has not been released (multi threaded tests)
@SYMREQ					
@SYMPREQ				PREQ2400
@SYMTestType			CT
@SYMTestPriority		
@SYMTestPurpose			Verify an invalid handle is returned to the calling thread if the write buffer has not been released
@SYMTestActions	
		Create a shared native stream surface to be used by multiple threads
		Thread 1 acquires the shared native stream and acquires the write buffer
		Thread 2 acquires the shared native stream and attempts to acquire the write buffer
		Thread 3 acquires the shared native stream and attempts to acquire the write buffer
		Thread 1 releases the write buffer
		Thread 2 acquires the write buffer
		Thread 2 releases the write buffer
@SYMTestExpectedResults
		OWF_INVALID_HANDLE returned when Thread 2 and Thread 3 attempt to acquire the write buffer
		when Thread 1 has already acquired it
 **/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0107_1L()
	{
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
        
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
	INFO_PRINTF2(_L("** GRAPHICS_OPENWFC_NATIVESTREAM_0107 - %i buffers **"), gVarInstance.Buffers());
	INFO_PRINTF1(_L("Thread 1 - start"));
	TSurfaceId surface = gVarInstance.SurfaceID();
	SymbianStreamType ns;
	err = SymbianStreamAcquire(&surface,&ns);
	ASSERT_TRUE(err == KErrNone);
	ASSERT_TRUE(ns);
	
	khronos_int32_t writeBuffer1;
	err = SymbianStreamAcquireWriteBuffer(ns,&writeBuffer1);
	ASSERT_TRUE(err != KErrBadHandle);
	
	khronos_int32_t bufferIndex;
	const TSurfaceId* getId = NULL;
	err = SymbianStreamGetBufferId(ns,writeBuffer1,&bufferIndex,&getId);
	ASSERT_TRUE(err == KErrNone);
	ASSERT_EQUALS(*getId, surface);
	ASSERT_EQUALS(bufferIndex, (khronos_int32_t)1);
	INFO_PRINTF2(_L("Thread 1 - Write buffer %i acquired"), bufferIndex);
	
	gSemaphore.Signal(2); // Thread 2 and 3 ready to run
	
	gSemaphore2.Wait();
    gSemaphore2.Wait();     // Wait for both threads to signal
	err = SymbianStreamReleaseWriteBuffer(ns,writeBuffer1);
	ASSERT_TRUE(err == KErrNone);
	SymbianStreamRemoveReference(ns);
	INFO_PRINTF2(_L("Thread 1 - Write buffer %i released"), bufferIndex);

	gSemaphore.Signal();	
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one    
	}

void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0107_2L()
	{
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
        
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
	gSemaphore.Wait(); // Semaphore count becomes -1
	INFO_PRINTF1(_L("Thread 2 - Start"));
	TSurfaceId surface = gVarInstance.SurfaceID();
	SymbianStreamType ns;
	err = SymbianStreamAcquire(&surface,&ns);
	ASSERT_TRUE(err == KErrNone);
	ASSERT_TRUE(ns);

	khronos_int32_t writeBuffer1;
	err = SymbianStreamAcquireWriteBuffer(ns,&writeBuffer1);
	ASSERT_TRUE(err != KErrBadHandle);
	INFO_PRINTF1(_L("Thread 2 - Attempt to acquire the write buffer"));
	ASSERT_FALSE(writeBuffer1);
	INFO_PRINTF1(_L("Thread 2 - Write buffer already in use by Thread 1!"));
    
	gSemaphore2.Signal();
	
	gSemaphore.Wait();
	
	khronos_int32_t writeBuffer2;
	err = SymbianStreamAcquireWriteBuffer(ns,&writeBuffer2);
	ASSERT_TRUE(err != KErrBadHandle);
	khronos_int32_t bufferIndex;
	const TSurfaceId* getId = NULL;
	err = SymbianStreamGetBufferId(ns,writeBuffer2,&bufferIndex,&getId);
	ASSERT_TRUE(err == KErrNone);
	ASSERT_FALSE(bufferIndex);
	INFO_PRINTF2(_L("Thread 2 - Write buffer %i acquired"), bufferIndex);
	
	err = SymbianStreamReleaseWriteBuffer(ns,writeBuffer2);
	ASSERT_TRUE(err == KErrNone);
	SymbianStreamRemoveReference(ns);
	INFO_PRINTF2(_L("Thread 2 - Write buffer %i released"), bufferIndex);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one    
	}

void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0107_3L()
	{
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
        
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
	gSemaphore.Wait(); // Semaphore count is -2
	INFO_PRINTF1(_L("Thread 3 - start"));
	TSurfaceId surface = gVarInstance.SurfaceID();
	SymbianStreamType ns;
	err = SymbianStreamAcquire(&surface,&ns);
	ASSERT_TRUE(err == KErrNone);
	ASSERT_TRUE(ns);

 	khronos_int32_t writeBuffer1;
 	err = SymbianStreamAcquireWriteBuffer(ns,&writeBuffer1);
 	ASSERT_TRUE(err != KErrBadHandle);
 	INFO_PRINTF1(_L("Thread 3 - Attempt to acquire the write buffer"));
	ASSERT_FALSE(writeBuffer1);
	INFO_PRINTF1(_L("Thread 3 - Write buffer already in use by Thread 1!"));

    gSemaphore2.Signal();

    SymbianStreamRemoveReference(ns);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one    
	}


/**
@SYMTestCaseID			GFX_OPENWFC_NATIVESTREAM_0108
@SYMTestCaseDesc		Content updates on valid and invalid buffers
@SYMREQ					
@SYMPREQ				PREQ2400
@SYMTestType			CT
@SYMTestPriority		
@SYMTestPurpose			Verify native streams can handle content updates for valid and invalid buffer numbers
@SYMTestActions	
		Create a surface with 4 buffers,
		Create a stream from this surface,
		Add an observer to listen out for content updates to the stream,
		Set valid and invalid buffer numbers to be used in content updates
@SYMTestExpectedResults
		For valid buffers, the read buffers should be set correctly and observer 
		callback method is called.
 **/		
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0108L(TInt aBuffers)
	{
	aBuffers = 3;
	RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
	TRAPD(err, GrowCleanupStackL());
	ASSERT_TRUE(err == KErrNone);

	iScreenNo = 0;
	TInt localNumber = 0;
	
	TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, aBuffers);
	ASSERT_FALSE(surface.IsNull());
	
	CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
	SymbianStreamType ns;
	err = SymbianStreamAcquire(&surface,&ns);
    ASSERT_TRUE(err == KErrNone);
	ASSERT_TRUE(ns);
	
	err = SymbianStreamAddObserver(ns, TestUpdateCallback, &localNumber);
	ASSERT_TRUE(err == KErrNone);
	khronos_int32_t bufferIndex = 0; 
	khronos_int32_t buffersIndex = 0;
	khronos_int32_t lastValidBufnum = 0;
	
	const TSurfaceId* getId = NULL;
    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
	// Valid inputs
	for (TInt i = 0; i < aBuffers; ++i)
		{
		buffersIndex = (aBuffers + i) % aBuffers;

 		
        updateProxy->ContentUpdated(surface,                    //aSurface
                                    buffersIndex,               //aBuffer
                                    NULL,                       //aRegion
                                    NULL,                       //aStatusConsumed
                                    NULL,                       //aStatusDisplayed
                                    NULL,                       //aTimeStamp
                                    NULL,                       //aStatusDispXTimes
                                    NULL                        //aDisplayedXTimes
                                    );	
		
		// The test can pass without a delay when running locally, but fails on ONB. Insert a delay here temporarily to see if it makes any difference
        User::After(10000); //10 ms delay
		khronos_int32_t readBuffer;
		err = SymbianStreamAcquireReadBuffer(ns,&readBuffer);
		ASSERT_TRUE(err == KErrNone);
		err = SymbianStreamGetBufferId(ns,readBuffer,&bufferIndex,&getId);
		ASSERT_TRUE(err == KErrNone);
		err = SymbianStreamReleaseReadBuffer(ns, readBuffer);
		ASSERT_TRUE(err == KErrNone);
		ASSERT_TRUE(bufferIndex == buffersIndex);
		
        // The test can pass without a delay when running locally, but fails on ONB. Insert a delay here temporarily to see if it makes any difference
        User::After(10000); //10 ms delay
		ASSERT_EQUALS((i+1),localNumber);
		}
	
	// Reset number for negative tests
	localNumber = 0;
	lastValidBufnum = buffersIndex;
	
	// Invalid inputs
	TInt invalidBuffersIndex[] = {-1, -256, aBuffers+29, 10000, -10000, aBuffers};
	TInt size = sizeof(invalidBuffersIndex) / sizeof(invalidBuffersIndex[0]);
	
	for (TInt i = 0; i < size; ++i)
		{
		updateProxy->ContentUpdated(surface,                    //aSurface
                                    invalidBuffersIndex[i],     //aBuffer
                                    NULL,                       //aRegion
                                    NULL,                       //aStatusConsumed
                                    NULL,                       //aStatusDisplayed
                                    NULL,                       //aTimeStamp
                                    NULL,                       //aStatusDispXTimes
                                    NULL                        //aDisplayedXTimes
                                    );
        
		User::After(10000); //10 ms delay
		khronos_int32_t readBuffer;
		err = SymbianStreamAcquireReadBuffer(ns,&readBuffer);
		ASSERT_TRUE(err == KErrNone);
		err = SymbianStreamGetBufferId(ns,readBuffer,&bufferIndex,&getId);
		ASSERT_TRUE(err == KErrNone);
		err = SymbianStreamReleaseReadBuffer(ns, readBuffer);
		ASSERT_TRUE(err == KErrNone);
		ASSERT_TRUE(bufferIndex == lastValidBufnum);
		ASSERT_EQUALS(0,localNumber);
		}
	SymbianStreamRemoveReference(ns);
	CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
	}

/**
@SYMTestCaseID			GFX_OPENWFC_NATIVESTREAM_0109
@SYMTestCaseDesc		Test multiple acquire/release read/write buffer calls
@SYMREQ					
@SYMPREQ				PREQ2400
@SYMTestType			CT
@SYMTestPriority		
@SYMTestPurpose			Verify the buffer index is correctly set
@SYMTestActions	
		Create a surface with 4 buffers,
		Call acquire/release read/write buffer functions and to Submit an update to the surface
@SYMTestExpectedResults
		Verify that acquire/release read/write buffer functions honour SUS update
		Verify that the update callback function is called when the native stream is updated
 **/		
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0109L()
	{
	RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
	TRAPD(err, GrowCleanupStackL());
	ASSERT_TRUE(err == KErrNone);
	
	iScreenNo = 0;
	TInt localNumber = 0;
	TInt count = 0;
	khronos_int32_t bufferIndexRead;
	khronos_int32_t bufferIndexWrite;
	
	TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
	ASSERT_FALSE(surface.IsNull());
	
	CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
	SymbianStreamType ns;
	err = SymbianStreamAcquire(&surface,&ns);
	ASSERT_TRUE(err == KErrNone);
	ASSERT_TRUE(ns);
	
	err = SymbianStreamAddObserver(ns, TestComposedCallback, &localNumber);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamAddObserver(ns, TestUpdateCallback, &localNumber);
	ASSERT_TRUE(err == KErrNone);
	
	khronos_int32_t readBuffer1;
	err = SymbianStreamAcquireReadBuffer(ns,&readBuffer1);
	ASSERT_TRUE(err == KErrNone);
	const TSurfaceId* getId = NULL;
	err = SymbianStreamGetBufferId(ns,readBuffer1,&bufferIndexRead,&getId);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamReleaseReadBuffer(ns, readBuffer1);
	ASSERT_TRUE(err == KErrNone);
	ASSERT_TRUE(bufferIndexRead == 0);
	
	khronos_int32_t writeBuffer1;
	err = SymbianStreamAcquireWriteBuffer(ns,&writeBuffer1);
	ASSERT_TRUE(err == KErrNone);
	
	khronos_int32_t readBuffer2;
	err = SymbianStreamAcquireReadBuffer(ns,&readBuffer2);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamGetBufferId(ns,readBuffer2,&bufferIndexRead,&getId);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamReleaseReadBuffer(ns, readBuffer2);
	ASSERT_TRUE(err == KErrNone);
	ASSERT_TRUE(bufferIndexRead == 0);
	err = SymbianStreamReleaseWriteBuffer(ns,writeBuffer1);
	ASSERT_TRUE(err == KErrNone);
	
	ASSERT_EQUALS((++count),localNumber);
	
	khronos_int32_t readBuffer3;
	err = SymbianStreamAcquireReadBuffer(ns,&readBuffer3);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamGetBufferId(ns,readBuffer3,&bufferIndexRead,&getId);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamReleaseReadBuffer(ns, readBuffer3);
	ASSERT_TRUE(err == KErrNone);
	ASSERT_TRUE(bufferIndexRead == 1);
	
	khronos_int32_t writeBuffer2;
	err = SymbianStreamAcquireWriteBuffer(ns,&writeBuffer2);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamGetBufferId(ns,writeBuffer2,&bufferIndexWrite,&getId);
	ASSERT_TRUE(err == KErrNone);
	ASSERT_TRUE((bufferIndexRead + 1) == bufferIndexWrite);
	
	khronos_int32_t readBuffer4;
	err = SymbianStreamAcquireReadBuffer(ns,&readBuffer4);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamGetBufferId(ns,readBuffer4,&bufferIndexRead,&getId);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamReleaseReadBuffer(ns, readBuffer4);
	ASSERT_TRUE(err == KErrNone);
	ASSERT_TRUE(bufferIndexRead == (bufferIndexWrite - 1));

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                NULL,                       //aStatusConsumed
                                NULL,                       //aStatusDisplayed
                                NULL,                       //aTimeStamp
                                NULL,                       //aStatusDispXTimes
                                NULL                        //aDisplayedXTimes
                                );  	
	
	ASSERT_EQUALS((++count),localNumber);
	
	khronos_int32_t readBuffer5;
	err = SymbianStreamAcquireReadBuffer(ns,&readBuffer5);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamGetBufferId(ns,readBuffer5,&bufferIndexRead,&getId);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamReleaseReadBuffer(ns, readBuffer5);
	ASSERT_TRUE(err == KErrNone);
	ASSERT_TRUE(bufferIndexRead == 0);
	err = SymbianStreamReleaseWriteBuffer(ns,writeBuffer2);
	ASSERT_TRUE(err == KErrNone);
	
	ASSERT_EQUALS((++count),localNumber);
	
	khronos_int32_t readBuffer6;
	err = SymbianStreamAcquireReadBuffer(ns,&readBuffer6);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamGetBufferId(ns,readBuffer6,&bufferIndexRead,&getId);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamReleaseReadBuffer(ns, readBuffer6);
	ASSERT_TRUE(err == KErrNone);
	ASSERT_TRUE(bufferIndexRead == 0);
	
	khronos_int32_t writeBuffer3;
	err = SymbianStreamAcquireWriteBuffer(ns,&writeBuffer3);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamGetBufferId(ns,writeBuffer3,&bufferIndexWrite,&getId);
	ASSERT_TRUE(err == KErrNone);
	ASSERT_TRUE(bufferIndexWrite == 1);
	err = SymbianStreamReleaseWriteBuffer(ns,writeBuffer3);
	ASSERT_TRUE(err == KErrNone);
	
	khronos_int32_t readBuffer7;
	err = SymbianStreamAcquireReadBuffer(ns,&readBuffer7);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamGetBufferId(ns,readBuffer7,&bufferIndexRead,&getId);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamReleaseReadBuffer(ns, readBuffer7);
	ASSERT_TRUE(err == KErrNone);
	ASSERT_TRUE(bufferIndexRead == 1);
	
	SymbianStreamRemoveReference(ns);
	CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
	}

/**
@SYMTestCaseID			GFX_OPENWFC_NATIVESTREAM_0110
@SYMTestCaseDesc		Verify that the update observer callback function is called after a
                        acquireWriteBuffer/releaseWriteBuffer call.
@SYMREQ					
@SYMPREQ				PREQ2400
@SYMTestType			CT
@SYMTestPriority		
@SYMTestPurpose			
@SYMTestActions	
		Create a surface with 1 buffer
		Add an observer to listen out for content updates to the stream
		Call acquire/release write buffer functions
@SYMTestExpectedResults
		Verify that the release write buffer function notifies any observers listening and the 
		observer callback function is called.
 **/		
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0110L()
	{
	RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
	TRAPD(err, GrowCleanupStackL());
	ASSERT_TRUE(err == KErrNone);
	
	TInt localNumber = 0;
	
	TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 1);
	ASSERT_FALSE(surface.IsNull());
	
	CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
	SymbianStreamType ns;
	err = SymbianStreamAcquire(&surface,&ns);
	ASSERT_TRUE(err == KErrNone);
	ASSERT_TRUE(ns);
	
	err = SymbianStreamAddObserver(ns, TestComposedCallback, &localNumber);
	ASSERT_TRUE(err == KErrNone);
	
	khronos_int32_t writeBuffer1;
	err = SymbianStreamAcquireWriteBuffer(ns,&writeBuffer1);
	ASSERT_TRUE(err == KErrNone);
	err = SymbianStreamReleaseWriteBuffer(ns,writeBuffer1);
	ASSERT_TRUE(err == KErrNone);
	
	if (localNumber == 0)
		{
		User::After(1000000);
		}
	
	ASSERT_TRUE(localNumber == 1);    
	SymbianStreamRemoveReference(ns);
	CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
	}

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0111
@SYMTestCaseDesc        Tests various cases including some negative cases for Native Stream callbacks
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        
@SYMTestPurpose         
@SYMTestActions 
        Create a surface with 1 buffer
        Add an observer N times
        Remove the same observer M (M <= N) times
        Register Null observer
        Unregister something that was never registered
@SYMTestExpectedResults
        Verify that the observer is called (N - M) times
        Verify that error case behaviour is correct
 **/        
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0111L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0;
    
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 1);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    SymbianStreamType ns;
    err = SymbianStreamAcquire(&surface,&ns);
    ASSERT_TRUE(err == KErrNone);    
    ASSERT_TRUE(ns);
    
    TInt localNumber = 0;
    err = SymbianStreamAddObserver(ns, NULL, &localNumber);
    ASSERT_TRUE(err == KErrBadHandle);
    err = SymbianStreamRemoveObserver(ns, &localNumber, ESOWF_EventComposed);
    ASSERT_TRUE(err == KErrNotFound);
    err = SymbianStreamRemoveObserver(ns, NULL, ESOWF_EventComposed);
    ASSERT_TRUE(err == KErrNotFound);
    err = SymbianStreamAddObserver(ns, TestUpdateCallback, &localNumber);
    ASSERT_TRUE(err == KErrNone);
    err = SymbianStreamRemoveObserver(ns, &localNumber, ESOWF_EventUpdated);
    ASSERT_TRUE(err == KErrNone);
    err = SymbianStreamRemoveObserver(ns, &localNumber, ESOWF_EventUpdated);
    ASSERT_TRUE(err == KErrNotFound);
    
#define N_TIMES 7
#define M_TIMES 3
    
    localNumber = 0;
    TInt k = N_TIMES;
    while (k--)
        {
        err = SymbianStreamAddObserver(ns, TestUpdateCallback, &localNumber);
        ASSERT_TRUE(err != KErrBadHandle);
        }
    k = M_TIMES;

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                NULL,                       //aStatusConsumed
                                NULL,                       //aStatusDisplayed
                                NULL,                       //aTimeStamp
                                NULL,                       //aStatusDispXTimes
                                NULL                        //aDisplayedXTimes
                                );  
    
    User::After(10000);
    ASSERT_TRUE(localNumber == 1);
    
    while (k--)
        {
        err = SymbianStreamRemoveObserver(ns, &localNumber, ESOWF_EventUpdated);
        ASSERT_TRUE(err != KErrBadHandle);
        }
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                NULL,                       //aStatusConsumed
                                NULL,                       //aStatusDisplayed
                                NULL,                       //aTimeStamp
                                NULL,                       //aStatusDispXTimes
                                NULL                        //aDisplayedXTimes
                                );      
    
    ASSERT_TRUE(localNumber == 1);
    
    SymbianStreamRemoveReference(ns);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0112
@SYMTestCaseDesc        Tests multithreaded cases for Native Stream callbacks
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        
@SYMTestPurpose         
@SYMTestActions 
        Create 3 threads and call add, remove, notify observers from
        respective threads multiple times
@SYMTestExpectedResults
        Verify that the observers work correctly in multithreaded environment
 **/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0112_1L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
        
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_NATIVESTREAM_0112_1L: Thread 1 start, Register Observer"));
    TSurfaceId surface = gVarInstance.SurfaceID();
    SymbianStreamType ns;
    err = SymbianStreamAcquire(&surface,&ns);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(ns);
    
    gVarInstance.iMultithreadCounter = 0;

    TTimeIntervalMicroSeconds32 delay(32000), zeroDelay(0);
    while (delay >= zeroDelay)
        {
        INFO_PRINTF2(_L("Thread 1 is going to add another observer after %i microsecond"), delay.Int());
        User::AfterHighRes(delay);
        SymbianStreamAddObserver(ns, TestUpdateCallback, &gVarInstance.iMultithreadCounter);
        delay = delay.Int() - 43;
        }
    TInt count = 50;
    while (count--)
        {
        INFO_PRINTF1(_L("Thread 1 is going to add another observer without delay"));
        SymbianStreamAddObserver(ns, TestUpdateCallback, &gVarInstance.iMultithreadCounter);
        }
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_NATIVESTREAM_0112_1L: Thread 1 exits"));
    
    SymbianStreamRemoveReference(ns);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0112_2L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);

    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_NATIVESTREAM_0112_2L: Thread 2 start, Unregister Observer"));
    TSurfaceId surface = gVarInstance.SurfaceID();
    SymbianStreamType ns;
    err = SymbianStreamAcquire(&surface,&ns);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(ns);
    
    TTimeIntervalMicroSeconds32 delay(32000), zeroDelay(0);
    while (delay > zeroDelay)
        {
        INFO_PRINTF2(_L("Thread 2 is going to remove one observer after %i microsecond"), delay.Int());
        User::AfterHighRes(delay);
        SymbianStreamRemoveObserver(ns, &gVarInstance.iMultithreadCounter, ESOWF_EventUpdated);
        delay = delay.Int() - 49;
        }
    TInt count = 50;
    while (count--)
        {
        INFO_PRINTF1(_L("Thread 2 is going to remove one observer without delay"));
        SymbianStreamRemoveObserver(ns, &gVarInstance.iMultithreadCounter, ESOWF_EventUpdated);
        }
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_NATIVESTREAM_0112_2L: Thread 2 exits"));    
    SymbianStreamRemoveReference(ns);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0112_3L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);

    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_NATIVESTREAM_0112_2L: Thread 3 start, Notify Observer"));
    TSurfaceId surface = gVarInstance.SurfaceID();
    SymbianStreamType ns;
    err = SymbianStreamAcquire(&surface,&ns);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(ns);
    
    khronos_int32_t bufferIndex;
    TTimeIntervalMicroSeconds32 delay(32000), zeroDelay(0);
    while (delay > zeroDelay)
        {
        khronos_int32_t writeBuffer;
        err = SymbianStreamAcquireWriteBuffer(ns,&writeBuffer);
        ASSERT_TRUE(err == KErrNone);
        const TSurfaceId* getId = NULL;
        err = SymbianStreamGetBufferId(ns,writeBuffer,&bufferIndex,&getId);
        ASSERT_TRUE(err == KErrNone);
        INFO_PRINTF2(_L("Thread 3 - Write buffer %i acquired"), bufferIndex);
        
        INFO_PRINTF2(_L("Thread 3 going to send notification after %i second"), delay.Int());
        User::AfterHighRes(delay);
        delay = delay.Int() - 58;
        err = SymbianStreamReleaseWriteBuffer(ns, writeBuffer);
        ASSERT_TRUE(err == KErrNone);
        }
    TInt count = 50;
    while (count--)
        {
        khronos_int32_t writeBuffer;
        err = SymbianStreamAcquireWriteBuffer(ns,&writeBuffer);
        ASSERT_TRUE(err == KErrNone);
        const TSurfaceId* getId = NULL;
        err = SymbianStreamGetBufferId(ns,writeBuffer,&bufferIndex,&getId);
        ASSERT_TRUE(err == KErrNone);
        INFO_PRINTF2(_L("Thread 3 - Write buffer %i acquired"), bufferIndex);
        
        INFO_PRINTF1(_L("Thread 3 going to send notification without delay"));
        err = SymbianStreamReleaseWriteBuffer(ns, writeBuffer);
        ASSERT_TRUE(err == KErrNone);
        }
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_NATIVESTREAM_0112_2L: Thread 3 exits"));
    
    SymbianStreamRemoveReference(ns);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**
Submit updates to the native stream whilst observers are being added and removed. 
Do not run in parallel with 0112_3L 
*/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0112_4L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);

    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_NATIVESTREAM_0112_2L: Thread 3 start, Notify Observer"));
    
    TTimeIntervalMicroSeconds32 delay(32000), zeroDelay(0);
    
    iScreenNo = 0;
    TRequestStatus displayedStatus, availableStatus, displayedXStatus;
    TUint32 timeStamp = 0;
    TInt bufferNo = 0;
    TInt numBuffers = 2;
    TInt displayedX = 5;

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    while (delay > zeroDelay)
        {        
        INFO_PRINTF2(_L("Thread 0112_4 submitting update after %i second"), delay.Int());
        User::AfterHighRes(delay);
        delay = delay.Int() - 58;                
        
        updateProxy->ContentUpdated(gVarInstance.SurfaceID(),                    //aSurface
                                    bufferNo,                   //aBuffer
                                    NULL,                       //aRegion
                                    &availableStatus,           //aStatusConsumed
                                    &displayedStatus,           //aStatusDisplayed
                                    &timeStamp,                 //aTimeStamp
                                    &displayedXStatus,          //aStatusDispXTimes
                                    &displayedX                 //aDisplayedXTimes
                                    );                
        
        bufferNo = (bufferNo + 1) % numBuffers;        
        }   
    
    TInt count = 50;
    while (count--)
        {
        INFO_PRINTF1(_L("Thread 0112_4 Set notifications"));
        INFO_PRINTF1(_L("Thread 0112_4 submitting update without delay"));

        updateProxy->ContentUpdated(gVarInstance.SurfaceID(),   //aSurface
                                    bufferNo,                   //aBuffer
                                    NULL,                       //aRegion
                                    &availableStatus,           //aStatusConsumed
                                    &displayedStatus,           //aStatusDisplayed
                                    &timeStamp,                 //aTimeStamp
                                    &displayedXStatus,          //aStatusDispXTimes
                                    &displayedX                 //aDisplayedXTimes
                                    );         
        
        bufferNo = (bufferNo + 1) % numBuffers;
        }    
    INFO_PRINTF1(_L("GRAPHICS_OPENWFC_NATIVESTREAM_0112_4L: Thread exits"));
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0113
@SYMTestCaseDesc        Test displayed notification
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Tests an end to end displayed notification.
@SYMTestActions

     1.  Create a surface
     2.  Create a native stream for the surface
     3.  Register the test class as an observer for ESOWF_EventUpdated to act as a fake composer
     4.  Register for displayed notifications
     5.  Submit an update to the surface
     6.  The observer function should be invoked and verifies that stream and event parameters.
     7.  The tester simulates the end of a composition by firing owfNativeStreanProcessNotifications
     8.  Wait on the displayed request status. 
     9.  Remove the source stream updated observer
     10. Destroy the native stream.
     
     The test is then repeated but the compositor claims the native stream is not visible.
        
@SYMTestExpectedResults
    No errors, displayed status completed with KErrNone.
        
 **/

void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0113L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iSourceStreamUpdatedCalled = 0;
    iImmediateAvailable = EFalse;
    iImmediateVisible = SYM_CONTENT_NOT_VISIBLE;
    iContextUpdatedFlags = 0;
    iScreenNo = 0;    
    
    for (TInt i = 0; i < 2; ++i)
        {
        TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
        ASSERT_FALSE(surface.IsNull());
        CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
        
        CTestNativeStream::iTester = this;
        TBool visible = (i == 0);
        
        RHeap* threadHeap3 = &User::Heap();
        err = SymbianStreamAcquire(&surface,&iNs);
        ASSERT_TRUE(err == KErrNone);
        ASSERT_TRUE(iNs);
        
        iExpectedSourceStreamUpdatedEventMask = 0;
        err = SymbianStreamAddObserver(iNs, SourceStreamUpdatedCallback, this);
        ASSERT_TRUE(err == KErrNone);
        
        TRequestStatus statusDisplayed;
        TUint32 displayedTime;    

        CExtensionContainer* updateExtension = NULL;
        err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
        ASSERT_TRUE(err == KErrNone);
        MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
        ASSERT_NOT_NULL(updateProxy);
        
        iSourceStreamUpdatedCalled = 0;
        updateProxy->ContentUpdated(surface,                    //aSurface
                                    0,                          //aBuffer
                                    NULL,                       //aRegion
                                    NULL,                       //aStatusConsumed
                                    &statusDisplayed,           //aStatusDisplayed
                                    &displayedTime,             //aTimeStamp
                                    NULL,                       //aStatusDispXTimes
                                    NULL                        //aDisplayedXTimes
                                    );
        
        ASSERT_TRUE(iSourceStreamUpdatedCalled == 1);
        err = SymbianStreamRemoveObserver(iNs, this, ESOWF_EventUpdated);
        ASSERT_TRUE(err == KErrNone);
    
        iExpectedSourceStreamUpdatedEventMask = ESOWF_EventDisplayed;
        err = SymbianStreamAddExtendedObserver(iNs, SourceStreamUpdatedCallback, this, iScreenNo, ESOWF_EventUpdated);
        ASSERT_TRUE(err == KErrNone);
    
        updateProxy->ContentUpdated(surface,                    //aSurface
                                    0,                          //aBuffer
                                    NULL,                       //aRegion
                                    NULL,                       //aStatusConsumed
                                    &statusDisplayed,           //aStatusDisplayed
                                    &displayedTime,             //aTimeStamp
                                    NULL,                       //aStatusDispXTimes
                                    NULL                        //aDisplayedXTimes
                                    );
        
        ASSERT_TRUE(iSourceStreamUpdatedCalled);
        
        // Pretend that a composition has occured
        ++iStreamUpdatedSerialNumber; 
        if (visible)
            {
            khronos_int32_t newNotificationsMask = 0;
            SymbianStreamProcessNotifications(iNs, 
                                                ESOWF_EventDisplayed,
                                                iScreenNo,
                                                iStreamUpdatedSerialNumber, 
                                                &newNotificationsMask);
    
            // No updates during composition so newNotificationMask should still be zero 
            ASSERT_TRUE(newNotificationsMask == 0);
            }        
        // Simulate multiple sources
        SymbianStreamCheckVisible(iNs, ESOWF_EventDisplayed, iScreenNo, iStreamUpdatedSerialNumber); 
        SymbianStreamCheckVisible(iNs, ESOWF_EventDisplayed, iScreenNo, iStreamUpdatedSerialNumber);
        
        // Make sure displayed event was completed
        User::WaitForRequest(statusDisplayed);
        if (visible)
            {
            ASSERT_EQUALS(statusDisplayed.Int(), KErrNone);
            }
        else
            {
            ASSERT_EQUALS(statusDisplayed.Int(), KErrNotVisible);
            }
        
        err = SymbianStreamRemoveObserver(iNs, this, ESOWF_EventUpdated);
        ASSERT_TRUE(err == KErrNone);
        SymbianStreamRemoveReference(iNs);
        CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one   
        iUtility->DestroySurface(surface);
        }
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0114
@SYMTestCaseDesc        Test available notification
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Tests an end to end available notification.
@SYMTestActions

     1.  Create a surface
     2.  Create a native stream for the surface
     3.  Register the test class as an observer for ESOWF_EventUpdated to act as a fake composer
     4.  Register for displayed notifications
     5.  Submit an update to the surface
     6.  The observer function should be invoked and verifies that stream and event parameters.
     7.  The tester simulates the end of a composition by firing owfNativeStreanProcessNotifications
     8.  Verify that available notification has not been sent.
     9.  Send another display update to change the read buffer to buffer 1
     10. Verify that the source-stream updated callback is invoked.
     11. The tester simulates the end of a composition by firing owfNativeStreanProcessNotifications
     12. Wait for available status to be completed
     13. Remove the source stream updated observer
     14. Destroy the native stream.
     
     The test is then repeated but the compositor claims the native stream is not visible.
        
@SYMTestExpectedResults
    No errors, available status completed with KErrNone.
        
 **/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0114L()
    {  
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0;
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iSourceStreamUpdatedCalled = 0;
    // during compositio or first time after a commit
    iImmediateAvailable = EFalse;
    iImmediateVisible = SYM_CONTENT_VISIBLE;
    
    for (TInt i = 0; i < 2; ++i)
        {
        TBool visible = (i == 0);
        TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
        ASSERT_FALSE(surface.IsNull());
        CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
        
        CTestNativeStream::iTester = this;
        
        err = SymbianStreamAcquire(&surface,&iNs);
        ASSERT_TRUE(err == KErrNone);
        ASSERT_TRUE(iNs);
        
        err = SymbianStreamAddExtendedObserver(iNs, SourceStreamUpdatedCallback, this, iScreenNo, ESOWF_EventUpdated);
        ASSERT_TRUE(err == KErrNone);
        
        TRequestStatus statusAvailable;
        iSourceStreamUpdatedCalled = 0;

        CExtensionContainer* updateExtension = NULL;
        err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
        ASSERT_TRUE(err == KErrNone);
        MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
        ASSERT_NOT_NULL(updateProxy);
        
        ASSERT_TRUE(iSourceStreamUpdatedCalled == 0);
        
        iExpectedSourceStreamUpdatedEventMask = 0;
        //we are during a composition
        ++iStreamUpdatedSerialNumber;
        updateProxy->ContentUpdated(surface,                    //aSurface
                                    0,                          //aBuffer
                                    NULL,                       //aRegion
                                    &statusAvailable,           //aStatusConsumed
                                    NULL,                       //aStatusDisplayed
                                    NULL,                       //aTimeStamp
                                    NULL,                       //aStatusDispXTimes
                                    NULL                        //aDisplayedXTimes
                                    );
        
        // Verify that the context's callback is invoked when SubmitUpdate is called.
        ASSERT_TRUE(iSourceStreamUpdatedCalled == 2);
        
        // Available should only be compled when submit update is called with a different buffer no.
        // if the stream is multi-buffered.
        ASSERT_EQUALS(statusAvailable.Int(), KRequestPending);
        
        // Pretend that a composition has occured
        khronos_int32_t newNotificationsMask = 0;
        if (visible)
            {            
            SymbianStreamProcessNotifications(iNs, 
                                              0, 
                                              iScreenNo,
                                              iStreamUpdatedSerialNumber,
                                              &newNotificationsMask);
    
            // No updates during composition so newNotificationMask should still be zero 
            ASSERT_TRUE(newNotificationsMask == 0);
            SymbianStreamProcessNotifications(iNs, 
                                              0, 
                                              iScreenNo,
                                              iStreamUpdatedSerialNumber,
                                              &newNotificationsMask);
    
            // No updates during composition so newNotificationMask should still be zero 
            ASSERT_TRUE(newNotificationsMask == 0);
            }
        SymbianStreamCheckVisible(iNs, ESOWF_EventAvailable, iScreenNo, iStreamUpdatedSerialNumber); 
        SymbianStreamCheckVisible(iNs, ESOWF_EventAvailable, iScreenNo, iStreamUpdatedSerialNumber);        
                
        // Available for buffer zero should not be completed yet
        ASSERT_EQUALS(statusAvailable.Int(), KRequestPending);
        
        // Update and switch to buffer 1
        iExpectedSourceStreamUpdatedEventMask = ESOWF_EventAvailable;
        //we are during a composition
        ++iStreamUpdatedSerialNumber;
        updateProxy->ContentUpdated(surface,                    //aSurface
                                    1,                          //aBuffer
                                    NULL,                       //aRegion
                                    NULL,                       //aStatusConsumed
                                    NULL,                       //aStatusDisplayed
                                    NULL,                       //aTimeStamp
                                    NULL,                       //aStatusDispXTimes
                                    NULL                        //aDisplayedXTimes
                                    );
        
        ASSERT_TRUE(iSourceStreamUpdatedCalled == 4);
        
        // Consume update on buffer 1. This should make buffer 0 available
        if (visible)
            {
            SymbianStreamProcessNotifications(iNs, 
                                                ESOWF_EventAvailable, 
                                                iScreenNo,
                                                iStreamUpdatedSerialNumber,
                                                &newNotificationsMask);
            
            ASSERT_TRUE(newNotificationsMask == 0);

            SymbianStreamProcessNotifications(iNs, 
                                                0, 
                                                iScreenNo,
                                                ++iStreamUpdatedSerialNumber,
                                                &newNotificationsMask);
            ASSERT_TRUE(newNotificationsMask == 0);
            }
        SymbianStreamCheckVisible(iNs, ESOWF_EventAvailable, iScreenNo, iStreamUpdatedSerialNumber); 
        SymbianStreamCheckVisible(iNs, ESOWF_EventAvailable, iScreenNo, iStreamUpdatedSerialNumber);        
        
        // Make sure displayed event was completed
        User::WaitForRequest(statusAvailable);
        if (visible)
            {
            ASSERT_EQUALS(statusAvailable.Int(), KErrNone);
            }
        else
            {
            ASSERT_EQUALS(statusAvailable.Int(), KErrNotVisible);
            }

        err = SymbianStreamRemoveObserver(iNs, this, ESOWF_EventUpdated);
        ASSERT_TRUE(err == KErrNone);
        SymbianStreamRemoveReference(iNs);
        CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one    
        iUtility->DestroySurface(surface);
        }
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0115
@SYMTestCaseDesc        Test displayed x times notification
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Verify that the surface stream adaptation processes displayed x times
                        notifications correctly.
@SYMTestActions

     1.  Create a surface
     2.  Create a native stream for the surface
     3.  Register the test class as an observer for ESOWF_EventUpdated to act as a fake composer
     4.  Register for displayed notifications
     5.  Submit an update to the surface
     6.  The observer function should be invoked and verifies that stream and event parameters.
     7.  The tester simulates the end of a composition by firing SymbianStreamProcessNotifications
     8.  If X has not been reached yet verify that displayed x status is not completed then goto 6. 
         Otherwise, goto step 9.   
     9.  Verify displayed-x-times status is completed with KErrNone
     10. Remove the observer
     11. Destroy the native stream.
     
     The test is then repeated but the compositor claims the native stream is not visible.
        
@SYMTestExpectedResults
    No errors, displayed-x-times status completed with KErrNone.
        
*/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0115L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0;  
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iImmediateAvailable = EFalse;
    iImmediateVisible = SYM_CONTENT_VISIBLE;
    
    for (TInt i = 0; i < 2; ++i)
        {
        TBool visible = (i == 0);        
        iSourceStreamUpdatedCalled = 0;
        TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
        ASSERT_FALSE(surface.IsNull());
        
        CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
        err = SymbianStreamAcquire(&surface, &iNs);
        ASSERT_TRUE(err == KErrNone);
        ASSERT_TRUE(iNs);
    
        err = SymbianStreamAddExtendedObserver(iNs, SourceStreamUpdatedCallback, this, iScreenNo, ESOWF_EventUpdated);
        ASSERT_TRUE(err == KErrNone);
        
        TRequestStatus statusDisplayedX;
        TInt X = 5;

        CExtensionContainer* updateExtension = NULL;
        err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
        ASSERT_TRUE(err == KErrNone);
        MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
        ASSERT_NOT_NULL(updateProxy);
        
        iExpectedSourceStreamUpdatedEventMask = ESOWF_EventDisplayedX;
        
        updateProxy->ContentUpdated(surface,                    //aSurface
                                    0,                          //aBuffer
                                    NULL,                       //aRegion
                                    NULL,                       //aStatusConsumed
                                    NULL,                       //aStatusDisplayed
                                    NULL,                       //aTimeStamp
                                    &statusDisplayedX,          //aStatusDispXTimes
                                    &X                          //aDisplayedXTimes
                                    );        
        
        ASSERT_TRUE(iSourceStreamUpdatedCalled == 2);
        
        
        khronos_int32_t events = ESOWF_EventDisplayedX;
        for (TInt i = 0; i < X; ++i)
            {            
            // Pretend that a composition has occured
            ++iStreamUpdatedSerialNumber;
            
            if (visible)
                {
                khronos_int32_t newNotificationsMask = 0;                        
                SymbianStreamProcessNotifications(iNs, 
                                                    events, 
                                                    iScreenNo, 
                                                    iStreamUpdatedSerialNumber,
                                                    &newNotificationsMask);
            
                SymbianStreamCheckVisible(iNs, events, iScreenNo, iStreamUpdatedSerialNumber); 
                SymbianStreamCheckVisible(iNs, events, iScreenNo, iStreamUpdatedSerialNumber);   
                
                if (i < X -1)
                    {
                    ASSERT_TRUE(newNotificationsMask == ESOWF_EventDisplayedX);
                    ASSERT_EQUALS(statusDisplayedX.Int(), KRequestPending);
                    }
                else
                    {
                    ASSERT_TRUE(newNotificationsMask == 0);
                    User::WaitForRequest(statusDisplayedX);
                    ASSERT_EQUALS(statusDisplayedX.Int(), KErrNone);            
                    }
                }
            else
                {
                SymbianStreamCheckVisible(iNs, events, iScreenNo, iStreamUpdatedSerialNumber); 
                SymbianStreamCheckVisible(iNs, events, iScreenNo, iStreamUpdatedSerialNumber);            
                User::WaitForRequest(statusDisplayedX);
                ASSERT_EQUALS(statusDisplayedX.Int(), KErrNotVisible);
                break;
                }
            }
            
        err = SymbianStreamRemoveObserver(iNs, this, ESOWF_EventUpdated);
        ASSERT_TRUE(err == KErrNone);
        SymbianStreamRemoveReference(iNs);
        CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
        
        iUtility->DestroySurface(surface);
        }
    }


/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0116
@SYMTestCaseDesc        Test all notifications together
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Verify that the surface stream adaptation processes displayed x times
                        notifications correctly.
@SYMTestActions

     1.  Create a surface
     2.  Create a native stream for the surface
     3.  Register the test class as an observer for ESOWF_EventUpdated to act as a fake composer
     4.  Register available, displayed and displayed-x-times notifications.
     5.  Submit an update to the surface
     6.  Wait for the displayed notification to complete.
     7.  The observer function should be invoked and verifies that stream and event parameters.
     8.  The tester simulates the end of a composition by firing SymbianStreamProcessNotifications
     9.  If X has not been reached yet verify that displayedx and available status is not completed 
         then goto 5; otherwise, goto step 11.   
     10. Verify displayed-x-times status is completed with KErrNone
     11. Submit an update on a different buffer number
     12. The tester simulates the end of a composition by firing SymbianStreamProcessNotifications
     13. Verify that the available status is completed with KErrNone
     14. Destroy the native stream.
        
@SYMTestExpectedResults
    No errors, displayed-x-times status completed with KErrNone.
        
*/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0116L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0; 
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iSourceStreamUpdatedCalled = 0;
    iImmediateAvailable = EFalse;
    iImmediateVisible = SYM_CONTENT_VISIBLE;   
    
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    err = SymbianStreamAcquire(&surface, &iNs);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(iNs);
    
    err = SymbianStreamAddExtendedObserver(iNs, SourceStreamUpdatedCallback, this, iScreenNo, ESOWF_EventUpdated);
    ASSERT_TRUE(err == KErrNone);
    
    TRequestStatus statusDisplayedX;
    TRequestStatus statusAvailable;
    TRequestStatus statusDisplayed;
        
    TInt X = 5;
    TUint32 displayedTime = 0;

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    // the composition is signalled as ongoing (busy system)
    iImmediateAvailable = EFalse;
    // we expect, initially that the composer is asked to check only for the displayed notifications
    iExpectedSourceStreamUpdatedEventMask = ESOWF_EventDisplayedX | ESOWF_EventDisplayed;
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                &statusAvailable,           //aStatusConsumed
                                &statusDisplayed,           //aStatusDisplayed
                                &displayedTime,             //aTimeStamp
                                &statusDisplayedX,          //aStatusDispXTimes
                                &X                          //aDisplayedXTimes
                                );     
    
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 2);
    // simulating the ongoing composition, the processing to be deferred for the following one
    khronos_int32_t newNotificationsMask = 0;
    SymbianStreamProcessNotifications(iNs, 
                                        iExpectedSourceStreamUpdatedEventMask, 
                                        iScreenNo,
                                        iStreamUpdatedSerialNumber++,
                                        &newNotificationsMask);

    ASSERT_TRUE(newNotificationsMask == iExpectedSourceStreamUpdatedEventMask);
    
    for (TInt i = 0; i < X; ++i)
        {                                
        // Pretend that a composition has occured
                
        khronos_int32_t events = newNotificationsMask;
        // we process the expected notifications
        newNotificationsMask = 0;
        SymbianStreamProcessNotifications(iNs, 
                                            events, 
                                            iScreenNo,
                                            iStreamUpdatedSerialNumber++,
                                            &newNotificationsMask);
                
        // No updates during composition so newNotificationMask should still be zero 
         
        if (i == 0)
            {
            User::WaitForRequest(statusDisplayed);
            ASSERT_EQUALS(statusDisplayed.Int(), KErrNone);
            }
        
        
        if (i < X - 1)
            {
            ASSERT_TRUE(newNotificationsMask == ESOWF_EventDisplayedX);
            // Displayed X times for buffer zero should not be completed yet
            ASSERT_EQUALS(statusDisplayedX.Int(), KRequestPending);
            ASSERT_EQUALS(statusAvailable.Int(), KRequestPending);
            }
        else
            {
            ASSERT_TRUE(newNotificationsMask == 0);
            User::WaitForRequest(statusDisplayedX);
            ASSERT_EQUALS(statusDisplayedX.Int(), KErrNone);            
            }
        }
    
    // the composition is, still, signalled as ongoing (busy system)
    iImmediateAvailable = EFalse;
    // we expect, initially that the composer is asked to check only for the displayed notifications
    iExpectedSourceStreamUpdatedEventMask = ESOWF_EventAvailable;
    iUtility->SubmitUpdate(KAllScreens, surface, 1, 0);
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 4);
    
    SymbianStreamProcessNotifications(iNs, 
                                        iExpectedSourceStreamUpdatedEventMask, 
                                        iScreenNo,
                                        iStreamUpdatedSerialNumber++,
                                        &newNotificationsMask);
    
    User::WaitForRequest(statusAvailable);
    ASSERT_EQUALS(statusAvailable.Int(), KErrNone);    
    ASSERT_TRUE(newNotificationsMask == 0);
    
    SymbianStreamRemoveReference(iNs);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    
    iUtility->DestroySurface(surface);
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0117_1
@SYMTestCaseDesc        Test displayed notification is cancelled when there are no registered SUS 
                        notification observers
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Verify that the surface stream adaptation cancels the displayed notification
                        when the native stream has not registered any observers for SUS notifications
@SYMTestActions

     1.  Create a surface
     2.  Create a native stream for the surface
     3.  Register the test class as an observer for ESOWF_EventUpdated to act as a fake composer
     4.  Do not add any SUS notification observers to the native stream
     5.  Register for displayed notification.
     6.  Submit an update to the surface
     7.  The observer function should be invoked and verifies that stream and event parameters.
     8.  The tester simulates the end of a composition by firing SymbianStreamProcessNotifications
     9.  Verify displayed status is completed with KErrCancel
     10. Destroy the native stream.
        
@SYMTestExpectedResults
    No errors, displayed status completed with KErrCancel.
        
*/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0117_1L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0;    
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iSourceStreamUpdatedCalled = 0;
    iImmediateVisible = SYM_CONTENT_VISIBLE;
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    err = SymbianStreamAcquire(&surface, &iNs);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(iNs);
    
    // Do not add observers for SUS notifications!
    
    iExpectedSourceStreamUpdatedEventMask = 0;
    err = SymbianStreamAddObserver(iNs, SourceStreamUpdatedCallback, this);
    ASSERT_TRUE(err == KErrNone);
    
    TRequestStatus statusDisplayed;
    TUint32 timeStamp = 0;

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                NULL,                       //aStatusConsumed
                                &statusDisplayed,           //aStatusDisplayed
                                &timeStamp,                 //aTimeStamp
                                NULL,                       //aStatusDispXTimes
                                NULL                        //aDisplayedXTimes
                                );    
    
    // Verify that the context's callback is invoked when SubmitUpdate is called.
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 1);
    
    // Pretend that a composition has occured
    khronos_int32_t notificationsMask = 0;
    khronos_int32_t newNotificationsMask = 0;
    
    SymbianStreamProcessNotifications(iNs, 
                                        ESOWF_EventAvailable | ESOWF_EventDisplayed |ESOWF_EventDisplayedX, 
                                        iScreenNo,
                                        ++iStreamUpdatedSerialNumber,
                                        &newNotificationsMask);
    
    // No updates during composition so newNotificationMask should still be zero 
    ASSERT_EQUALS(newNotificationsMask, notificationsMask);
    
    // The displayed notification should be cancelled as we have no observers registered
    ASSERT_EQUALS(statusDisplayed.Int(), KErrCancel);
    ASSERT_TRUE(timeStamp == 0);
    
    SymbianStreamRemoveReference(iNs);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0117_2
@SYMTestCaseDesc        Test displayed x times notification is cancelled when there are no registered SUS 
                        notification observers
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Verify that the surface stream adaptation cancels the displayed x times notification
                        when the native stream has not registered any observers for SUS notifications
@SYMTestActions

     1.  Create a surface
     2.  Create a native stream for the surface
     3.  Register the test class as an observer for ESOWF_EventUpdated to act as a fake composer
     4.  Do not add any SUS notification observers to the native stream
     5.  Register for displayed x times notification.
     6.  Submit an update to the surface
     7.  The observer function should be invoked and verifies that stream and event parameters.
     8.  The tester simulates the end of a composition by firing SymbianStreamProcessNotifications
     9.  Verify displayed x time status is completed with KErrCancel
     10. Destroy the native stream.
        
@SYMTestExpectedResults
    No errors, displayed x times status completed with KErrCancel.
        
*/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0117_2L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0;    
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iSourceStreamUpdatedCalled = 0;
    iImmediateVisible = SYM_CONTENT_VISIBLE;
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    err = SymbianStreamAcquire(&surface, &iNs);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(iNs);
    
    // Do not add observers for SUS notifications!
    
    iExpectedSourceStreamUpdatedEventMask = 0;
    err = SymbianStreamAddObserver(iNs, SourceStreamUpdatedCallback, this);
    ASSERT_TRUE(err == KErrNone);
    
    TRequestStatus statusDisplayedX;
    TInt X = 5;

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                NULL,                       //aStatusConsumed
                                NULL,                       //aStatusDisplayed
                                NULL,                       //aTimeStamp
                                &statusDisplayedX,          //aStatusDispXTimes
                                &X                          //aDisplayedXTimes
                                );     

    // Verify that the context's callback is invoked when SubmitUpdate is called.
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 1);
    
    User::WaitForRequest(statusDisplayedX);
    ASSERT_EQUALS(statusDisplayedX.Int(), KErrCancel);
    
    for (TInt i = 0; i < X; ++i)
        {
        // Pretend that a composition has occured
        khronos_int32_t newNotificationsMask = 0;
        SymbianStreamProcessNotifications(iNs, 
                                            ESOWF_EventAvailable | ESOWF_EventDisplayed |ESOWF_EventDisplayedX, 
                                            iScreenNo,
                                            ++iStreamUpdatedSerialNumber,
                                            &newNotificationsMask);
        
        // No updates during composition so newNotificationMask should still be zero 
        ASSERT_TRUE(newNotificationsMask == 0);
        }
   
    SymbianStreamRemoveReference(iNs);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0117_3
@SYMTestCaseDesc        Test available notification is cancelled when there are no registered SUS 
                        notification observers
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Verify that the surface stream adaptation cancels the available notification
                        when the native stream has not registered any observers for SUS notifications
@SYMTestActions

     1.  Create a surface
     2.  Create a native stream for the surface
     3.  Register the test class as an observer for ESOWF_EventUpdated to act as a fake composer
     4.  Do not add any SUS notification observers to the native stream
     5.  Register for available notification.
     6.  Submit an update to the surface
     7.  The observer function should be invoked and verifies that stream and event parameters.
     8.  The tester simulates the end of a composition by firing SymbianStreamProcessNotifications
     9.  Verify available status is completed with KErrCancel
     10. Destroy the native stream.
        
@SYMTestExpectedResults
    No errors, available status completed with KErrCancel.
        
*/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0117_3L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0;    
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iSourceStreamUpdatedCalled = 0;
    iImmediateVisible = SYM_CONTENT_VISIBLE;
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    err = SymbianStreamAcquire(&surface, &iNs);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(iNs);
    
    // Do not add observers for SUS notifications!
    
    iExpectedSourceStreamUpdatedEventMask = 0;
    err = SymbianStreamAddObserver(iNs, SourceStreamUpdatedCallback, this);
    ASSERT_TRUE(err == KErrNone);
    
    TRequestStatus statusAvailable;

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                &statusAvailable,           //aStatusConsumed
                                NULL,                       //aStatusDisplayed
                                NULL,                       //aTimeStamp
                                NULL,                       //aStatusDispXTimes
                                NULL                        //aDisplayedXTimes
                                ); 
    
    // Verify that the context's callback is invoked when SubmitUpdate is called.
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 1);
    
    // Available should only be completed when submit update is called with a different buffer no.
    // But, when there are no registered SUS observers, the notification should complete immediately
    // with KErrCancel
    ASSERT_EQUALS(statusAvailable.Int(), KErrCancel);
    
    // Pretend that a composition has occured
    khronos_int32_t newNotificationsMask = 0;
    SymbianStreamProcessNotifications(iNs, 
                                        ESOWF_EventAvailable | ESOWF_EventDisplayed |ESOWF_EventDisplayedX, 
                                        iScreenNo,
                                        ++iStreamUpdatedSerialNumber,
                                        &newNotificationsMask);
    
    ASSERT_TRUE(newNotificationsMask == 0);
    
    // Update and switch to buffer 1
    iExpectedSourceStreamUpdatedEventMask = 0;
    iUtility->SubmitUpdate(KAllScreens, surface, 1, 0);    
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 2);
    
    // Consume update on buffer 1. This should make buffer 0 available
    SymbianStreamProcessNotifications(iNs, 
                                        ESOWF_EventAvailable | ESOWF_EventDisplayed |ESOWF_EventDisplayedX, 
                                        iScreenNo,
                                        ++iStreamUpdatedSerialNumber,
                                        &newNotificationsMask);
    
    ASSERT_TRUE(newNotificationsMask == 0);
    
    User::WaitForRequest(statusAvailable);
    ASSERT_EQUALS(statusAvailable.Int(), KErrCancel);
   
    SymbianStreamRemoveReference(iNs);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**  
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0117_4
@SYMTestCaseDesc        Test all notifications are cancelled when there are no registered SUS 
                        notification observers
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Verify that the surface stream adaptation cancels all notifications
                        when the native stream has not registered any observers for SUS notifications
@SYMTestActions

     1.  Create a surface
     2.  Create a native stream for the surface
     3.  Register the test class as an observer for ESOWF_EventUpdated to act as a fake composer
     4.  Do not add any SUS notification observers to the native stream
     5.  Register for displayed, available and displayed x times notifications
     6.  Submit an update to the surface
     7.  The observer function should be invoked and verifies that stream and event parameters
     8.  The tester simulates the end of a composition by firing SymbianStreamProcessNotifications
     9.  Verify all notification statuses completed with KErrCancel
     10. Destroy the native stream.
        
@SYMTestExpectedResults
    No errors, all notification statuses completed with KErrCancel.
        
*/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0117_4L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0;    
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iSourceStreamUpdatedCalled = 0;
    iImmediateVisible = SYM_CONTENT_VISIBLE;
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    err = SymbianStreamAcquire(&surface, &iNs);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(iNs);
    
    // Do not add observers for SUS notifications!
    
    iExpectedSourceStreamUpdatedEventMask = 0;
    err = SymbianStreamAddObserver(iNs, SourceStreamUpdatedCallback, this);
    ASSERT_TRUE(err == KErrNone);
    
    TRequestStatus statusDisplayedX;
    TRequestStatus statusAvailable;
    TRequestStatus statusDisplayed;
    TInt X = 5;
    TUint32 displayedTime = 0;

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                &statusAvailable,           //aStatusConsumed
                                &statusDisplayed,           //aStatusDisplayed
                                &displayedTime,             //aTimeStamp
                                &statusDisplayedX,          //aStatusDispXTimes
                                &X                          //aDisplayedXTimes
                                );     

    // Verify that the context's callback is invoked when SubmitUpdate is called.
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 1);
    
    for (TInt i = 0; i < X; ++i)
        {                                
        // Pretend that a composition has occured
        khronos_int32_t newNotificationsMask = 0;
                
        SymbianStreamProcessNotifications(iNs, 
                                            ESOWF_EventAvailable | ESOWF_EventDisplayed |ESOWF_EventDisplayedX, 
                                            iScreenNo,
                                            ++iStreamUpdatedSerialNumber,
                                            &newNotificationsMask);
                
        // No updates during composition so newNotificationMask should still be zero 
        ASSERT_TRUE(newNotificationsMask == 0);
                
        if (i == 0)
            {
            User::WaitForRequest(statusDisplayed);
            User::WaitForRequest(statusDisplayedX);
            User::WaitForRequest(statusAvailable);
            }

        ASSERT_EQUALS(statusAvailable.Int(), KErrCancel);            
        ASSERT_EQUALS(statusDisplayed.Int(), KErrCancel);            
        ASSERT_EQUALS(statusDisplayedX.Int(), KErrCancel);
        }
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                NULL,                       //aStatusConsumed
                                NULL,                       //aStatusDisplayed
                                NULL,                       //aTimeStamp
                                NULL,                       //aStatusDispXTimes
                                NULL                        //aDisplayedXTimes
                                );    
    
    // Verify that the context's callback is invoked when SubmitUpdate is called.
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 2);
    
    khronos_int32_t newNotificationsMask = 0;
    SymbianStreamProcessNotifications(iNs, 
                                        ESOWF_EventAvailable | ESOWF_EventDisplayed |ESOWF_EventDisplayedX, 
                                        iScreenNo,
                                        ++iStreamUpdatedSerialNumber,
                                        &newNotificationsMask);
    
    ASSERT_EQUALS(statusAvailable.Int(), KErrCancel);
    ASSERT_TRUE(displayedTime == 0);
    
    SymbianStreamRemoveReference(iNs);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0118_1
@SYMTestCaseDesc        Test displayed notification is cancelled when content updated is called on  
                        surface which is not registered with a native stream
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Verify that the surface stream adaptation cancels the displayed notification
                        added to a surface which does not have a native stream associated with it
@SYMTestActions

     1.  Create a surface
     2.  Register for displayed notification.
     3.  Submit an update to the surface
     4.  Verify displayed status is completed with KErrSurfaceNotRegistered
        
@SYMTestExpectedResults
    No errors, displayed status completed with KErrSurfaceNotRegistered.
        
*/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0118_1L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0;    
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iSourceStreamUpdatedCalled = 0;
    iImmediateVisible = SYM_CONTENT_VISIBLE;
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    TRequestStatus statusDisplayed;
    TUint32 timeStamp = 0;

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                NULL,                       //aStatusConsumed
                                &statusDisplayed,           //aStatusDisplayed
                                &timeStamp,                 //aTimeStamp
                                NULL,                       //aStatusDispXTimes
                                NULL                        //aDisplayedXTimes
                                );    
    
    User::WaitForRequest(statusDisplayed);
    
    // The displayed notification should return with a surface registration error as there 
    // isn't a native stream registered with surface...
    ASSERT_EQUALS(statusDisplayed.Int(), KErrSurfaceNotRegistered);
    ASSERT_TRUE(timeStamp == 0);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0118_2
@SYMTestCaseDesc        Test displayed x times notification is cancelled when content updated is called on  
                        surface which is not registered with a native stream
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Verify that the surface stream adaptation cancels the displayed x times notification
                        added to a surface which does not have a native stream associated with it
@SYMTestActions

     1.  Create a surface
     2.  Register for displayed x times notification.
     3.  Submit an update to the surface
     4.  Verify displayed x times status is completed with KErrSurfaceNotRegistered
        
@SYMTestExpectedResults
    No errors, displayed x times status completed with KErrSurfaceNotRegistered.
        
*/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0118_2L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0;    
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iSourceStreamUpdatedCalled = 0;
    iImmediateVisible = SYM_CONTENT_VISIBLE;
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    
    // Do not create a native stream for surface!
    
    TRequestStatus statusDisplayedXTimes;
    TInt X = 5;

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                NULL,                       //aStatusConsumed
                                NULL,                       //aStatusDisplayed
                                NULL,                       //aTimeStamp
                                &statusDisplayedXTimes,     //aStatusDispXTimes
                                &X                          //aDisplayedXTimes
                                );
    
    User::WaitForRequest(statusDisplayedXTimes);
    
    // The displayed x times notification should return with a surface registration error as there 
    // isn't a native stream registered with surface...
    ASSERT_EQUALS(statusDisplayedXTimes.Int(), KErrSurfaceNotRegistered);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0118_3
@SYMTestCaseDesc        Test available notification is cancelled when content updated is called on  
                        surface which is not registered with a native stream
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Verify that the surface stream adaptation cancels the available notification
                        added to a surface which does not have a native stream associated with it
@SYMTestActions

     1.  Create a surface
     2.  Rregister for available notification.
     3.  Submit an update to the surface
     4.  Verify available status is completed with KErrSurfaceNotRegistered
        
@SYMTestExpectedResults
    No errors, available status completed with KErrSurfaceNotRegistered.
        
*/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0118_3L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0;    
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iSourceStreamUpdatedCalled = 0;
    iImmediateVisible = SYM_CONTENT_VISIBLE;
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    
    // Do not create a native stream for surface!
    
    TRequestStatus statusAvailable;

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                &statusAvailable,           //aStatusConsumed
                                NULL,                       //aStatusDisplayed
                                NULL,                       //aTimeStamp
                                NULL,                       //aStatusDispXTimes
                                NULL                        //aDisplayedXTimes
                                );
    
    User::WaitForRequest(statusAvailable);  
    
    // The available notification should return with a surface registration error as there 
    // isn't a native stream registered with surface...
    ASSERT_EQUALS(statusAvailable.Int(), KErrSurfaceNotRegistered);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0118_4
@SYMTestCaseDesc        Test all notifications are cancelled when content updated is called on  
                        surface which is not registered with a native stream
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Verify that the surface stream adaptation cancels all notifications
                        added to a surface which does not have a native stream associated with it
@SYMTestActions

     1.  Create a surface
     2.  Register for displayed, available and displayed x times notifications.
     3.  Submit an update to the surface
     4.  Verify all notification statuses completed with KErrSurfaceNotRegistered
        
@SYMTestExpectedResults
    No errors, all notification statuses completed with KErrSurfaceNotRegistered.
        
*/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0118_4L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0;    
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iSourceStreamUpdatedCalled = 0;
    iImmediateVisible = SYM_CONTENT_VISIBLE;
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    
    // Do not create a native stream for surface!
    
    TRequestStatus statusDisplayed, statusAvailable, statusDisplayedXTimes;
    TInt X = 5;
    TUint32 timeStamp = 0;

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                &statusAvailable,           //aStatusConsumed
                                &statusDisplayed,           //aStatusDisplayed
                                &timeStamp,                 //aTimeStamp
                                &statusDisplayedXTimes,     //aStatusDispXTimes
                                &X                          //aDisplayedXTimes
                                ); 
    
    User::WaitForRequest(statusDisplayed);
    User::WaitForRequest(statusAvailable);
    User::WaitForRequest(statusDisplayedXTimes);
    
    // All notifications should return with a surface registration error as there 
    // isn't a native stream registered with surface...
    ASSERT_EQUALS(statusDisplayed.Int(), KErrSurfaceNotRegistered);
    ASSERT_EQUALS(statusAvailable.Int(), KErrSurfaceNotRegistered);
    ASSERT_EQUALS(statusDisplayedXTimes.Int(), KErrSurfaceNotRegistered);
    ASSERT_TRUE(timeStamp == 0);
    
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0119_1
@SYMTestCaseDesc        Test overflow conditions on notifications for displayed
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Verify that the surface stream adaptation correctly handles duplicate displayed 
                        notifications registered by SUS for content updates on the same screen and buffer
@SYMTestActions

     1.  Create a surface
     2.  Create a native stream for the surface
     3.  Register the test class as an observer for ESOWF_EventUpdated to act as a fake composer
     4.  Register for displayed notification (d1)
     5.  Submit an update to the surface
     6.  Register for displayed notification (d2)
     7.  Submit an update to the surface
     8.  The observer function should be invoked and verifies that stream and event parameters.
     9.  The tester simulates the end of a composition by firing SymbianStreamProcessNotifications
     10. Verify d1 status is completed with KErrOverflow and d2 status is completed with KErrNone
     11. Destroy the native stream
        
@SYMTestExpectedResults
    No errors, d1 status completed with KErrOverflow, d2 status completed with KErrNone.
        
*/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0119_1L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0;    
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iSourceStreamUpdatedCalled = 0;
    iImmediateVisible = SYM_CONTENT_VISIBLE;
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    err = SymbianStreamAcquire(&surface, &iNs);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(iNs);;
    
    iExpectedSourceStreamUpdatedEventMask = ESOWF_EventDisplayed;
    err = SymbianStreamAddExtendedObserver(iNs, SourceStreamUpdatedCallback, this, iScreenNo, ESOWF_EventUpdated);
    ASSERT_TRUE(err == KErrNone);
    
    TRequestStatus statusDisplayed1, statusDisplayed2;
    TUint32 timeStamp1 = 0;
    TUint32 timeStamp2 = 0;

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                NULL,                       //aStatusConsumed
                                &statusDisplayed1,          //aStatusDisplayed
                                &timeStamp1,                //aTimeStamp
                                NULL,                       //aStatusDispXTimes
                                NULL                        //aDisplayedXTimes
                                );
    
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 2);
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                NULL,                       //aStatusConsumed
                                &statusDisplayed2,          //aStatusDisplayed
                                &timeStamp2,                //aTimeStamp
                                NULL,                       //aStatusDispXTimes
                                NULL                        //aDisplayedXTimes
                                );
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 4);
    
    User::WaitForRequest(statusDisplayed1);
    ASSERT_EQUALS(statusDisplayed1.Int(), KErrOverflow);
    ASSERT_TRUE(timeStamp1 == 0);
    
    // Verify that the context's callback is invoked when SubmitUpdate is called.
    ASSERT_TRUE(iSourceStreamUpdatedCalled);
    
    // Pretend that a composition has occured
    khronos_int32_t newNotificationsMask = 0;
    SymbianStreamProcessNotifications(iNs, 
                                        iExpectedSourceStreamUpdatedEventMask, 
                                        iScreenNo,
                                        ++iStreamUpdatedSerialNumber,
                                        &newNotificationsMask);
    
    ASSERT_TRUE(newNotificationsMask == 0);
    
    // Make sure displayed event was completed
    User::WaitForRequest(statusDisplayed2);
    ASSERT_EQUALS(statusDisplayed2.Int(), KErrNone);
    ASSERT_TRUE(timeStamp2);
    
    SymbianStreamRemoveReference(iNs);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0119_2
@SYMTestCaseDesc        Test overflow conditions on notifications for displayed-x-times
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Verify that the surface stream adaptation correctly handles duplicate displayed 
                        x times notifications registered by SUS for content updates on the same screen
                        and buffer
@SYMTestActions

     1.  Create a surface
     2.  Create a native stream for the surface
     3.  Register the test class as an observer for ESOWF_EventUpdated to act as a fake composer
     4.  Register for displayed x times notification (d1)
     5.  Submit an update to the surface
     6.  Register for displayed x times notification (d2)
     7.  Submit an update to the surface
     8.  The observer function should be invoked and verifies that stream and event parameters.
     9.  The tester simulates the end of a composition by firing SymbianStreamProcessNotifications
     10. Verify d1 status is completed with KErrOverflow and d2 status is completed with KErrNone
     11. Destroy the native stream
        
@SYMTestExpectedResults
    No errors, d1 status completed with KErrOverflow, d2 status completed with KErrNone.
        
*/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0119_2L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0;    
    TInt X = 5;
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iSourceStreamUpdatedCalled = 0;
    iImmediateVisible = SYM_CONTENT_VISIBLE;
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    err = SymbianStreamAcquire(&surface, &iNs);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(iNs);
    
    iExpectedSourceStreamUpdatedEventMask = ESOWF_EventDisplayedX;
    err = SymbianStreamAddExtendedObserver(iNs, SourceStreamUpdatedCallback, this, iScreenNo, ESOWF_EventUpdated);
    ASSERT_TRUE(err == KErrNone);
    
    TRequestStatus statusDisplayedX1, statusDisplayedX2;

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                NULL,                       //aStatusConsumed
                                NULL,                       //aStatusDisplayed
                                NULL,                       //aTimeStamp
                                &statusDisplayedX1,         //aStatusDispXTimes
                                &X                          //aDisplayedXTimes
                                );
    
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 2);
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                NULL,                       //aStatusConsumed
                                NULL,                       //aStatusDisplayed
                                NULL,                       //aTimeStamp
                                &statusDisplayedX2,         //aStatusDispXTimes
                                &X                          //aDisplayedXTimes
                                );
    
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 4);
    
    User::WaitForRequest(statusDisplayedX1);
    ASSERT_EQUALS(statusDisplayedX1.Int(), KErrOverflow);
    
    for (TInt i = 0; i < X; ++i)
        {
        ASSERT_EQUALS(statusDisplayedX2.Int(), KRequestPending);
        
        // Pretend that a composition has occured
        khronos_int32_t newNotificationsMask = 0;
        SymbianStreamProcessNotifications(iNs, 
                                            ESOWF_EventDisplayedX, 
                                            iScreenNo,
                                            ++iStreamUpdatedSerialNumber,
                                            &newNotificationsMask);        

        if (i < X -1)
            {
            ASSERT_TRUE(newNotificationsMask == ESOWF_EventDisplayedX);
            }
        else
            {
            ASSERT_TRUE(newNotificationsMask == 0);
            }
        }
    
    // Make sure displayed event was completed
    User::WaitForRequest(statusDisplayedX2);
    ASSERT_EQUALS(statusDisplayedX2.Int(), KErrNone);
    
    SymbianStreamRemoveReference(iNs);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0119_3
@SYMTestCaseDesc        Test overflow conditions on notifications for available
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Verify that the surface stream adaptation correctly handles duplicate available 
                        notifications registered by SUS for content updates on the same screen and buffer
@SYMTestActions

     1.  Create a surface
     2.  Create a native stream for the surface
     3.  Register the test class as an observer for ESOWF_EventUpdated to act as a fake composer
     4.  Register for available notification (a1)
     5.  Submit an update to the surface
     6.  Register for available notification (a2)
     7.  Submit an update to the surface
     8.  The observer function should be invoked and verifies that stream and event parameters.
     9.  The tester simulates the end of a composition by firing SymbianStreamProcessNotifications
     10. Verify d1 status is completed with KErrOverflow and d2 status is completed with KErrNone
     11. Destroy the native stream
        
@SYMTestExpectedResults
    No errors, a1 status completed with KErrOverflow, a2 status completed with KErrNone.
        
*/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0119_3L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0;    
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iSourceStreamUpdatedCalled = 0;
    iImmediateAvailable = EFalse;
    iImmediateVisible = SYM_CONTENT_VISIBLE;
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    err = SymbianStreamAcquire(&surface, &iNs);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(iNs);
    
    err = SymbianStreamAddExtendedObserver(iNs, SourceStreamUpdatedCallback, this, iScreenNo, ESOWF_EventUpdated);
    ASSERT_TRUE(err == KErrNone);
    
    TRequestStatus statusAvailable1, statusAvailable2, statusAvailable3;

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    iExpectedSourceStreamUpdatedEventMask = 0;
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                &statusAvailable1,          //aStatusConsumed
                                NULL,                       //aStatusDisplayed
                                NULL,                       //aTimeStamp
                                NULL,                       //aStatusDispXTimes
                                NULL                        //aDisplayedXTimes
                                );    
    
    // Verify that the context's callback is invoked when SubmitUpdate is called.
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 2);
    
    iExpectedSourceStreamUpdatedEventMask = ESOWF_EventAvailable;
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                &statusAvailable2,          //aStatusConsumed
                                NULL,                       //aStatusDisplayed
                                NULL,                       //aTimeStamp
                                NULL,                       //aStatusDispXTimes
                                NULL                        //aDisplayedXTimes
                                );   
    
    // Verify that the context's callback is invoked when SubmitUpdate is called.
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 4);
    
    iExpectedSourceStreamUpdatedEventMask = ESOWF_EventAvailable;
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                &statusAvailable3,          //aStatusConsumed
                                NULL,                       //aStatusDisplayed
                                NULL,                       //aTimeStamp
                                NULL,                       //aStatusDispXTimes
                                NULL                        //aDisplayedXTimes
                                );   
    
    // Verify that the context's callback is invoked when SubmitUpdate is called.
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 6);
    
    User::WaitForRequest(statusAvailable1);
    ASSERT_EQUALS(statusAvailable1.Int(), KErrOverflow);
    
    // Pretend that a composition has occured
    khronos_int32_t newNotificationsMask = 0;
    SymbianStreamProcessNotifications(iNs, 
                                        ESOWF_EventAvailable, 
                                        iScreenNo,
                                        iStreamUpdatedSerialNumber,
                                        &newNotificationsMask);
    
    ASSERT_TRUE(newNotificationsMask == 0);
    
    // Make sure displayed event was completed
    User::WaitForRequest(statusAvailable2);
    ASSERT_EQUALS(statusAvailable2.Int(), KErrNone);
    
    SymbianStreamRemoveReference(iNs);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0120_1
@SYMTestCaseDesc        Test overflow conditions on notifications for displayed
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Verify that the surface stream adaptation correctly handles duplicate notifications
                        registered by SUS for content updates on different buffers
@SYMTestActions

     1.  Create a surface
     2.  Create a native stream for the surface
     3.  Register the test class as an observer for ESOWF_EventUpdated to act as a fake composer
     4.  Register for displayed notification (d1)
     5.  Submit an update to the surface on buffer 0
     6.  Register for displayed notification (d2)
     7.  Submit an update to the surface on buffer 1
     8.  The observer function should be invoked and verifies that stream and event parameters.
     9.  The tester simulates the end of a composition by firing SymbianStreamProcessNotifications
     10. Verify d1 status is completed with KErrOverflow and d2 status is completed with KErrNone
     11. Destroy the native stream
        
@SYMTestExpectedResults
    No errors, d1 status completed with KErrOverflow, d2 status completed with KErrNone.
        
*/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0120_1L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0;    
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iSourceStreamUpdatedCalled = 0;
    iImmediateVisible = SYM_CONTENT_VISIBLE;
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    err = SymbianStreamAcquire(&surface, &iNs);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(iNs);
    
    err = SymbianStreamAddExtendedObserver(iNs, SourceStreamUpdatedCallback, this, iScreenNo, ESOWF_EventUpdated);
    ASSERT_TRUE(err == KErrNone);
    iExpectedSourceStreamUpdatedEventMask = ESOWF_EventDisplayed;
    
    TRequestStatus statusDisplayed1, statusDisplayed2;
    TUint32 timeStamp1 = 0;
    TUint32 timeStamp2 = 0;

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                NULL,                       //aStatusConsumed
                                &statusDisplayed1,          //aStatusDisplayed
                                &timeStamp1,                //aTimeStamp
                                NULL,                       //aStatusDispXTimes
                                NULL                        //aDisplayedXTimes
                                );
    
    // Verify that the context's callback is invoked when SubmitUpdate is called.
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 2);
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                1,                          //aBuffer
                                NULL,                       //aRegion
                                NULL,                       //aStatusConsumed
                                &statusDisplayed2,          //aStatusDisplayed
                                &timeStamp2,                //aTimeStamp
                                NULL,                       //aStatusDispXTimes
                                NULL                        //aDisplayedXTimes
                                );  
    
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 4);
    
    User::WaitForRequest(statusDisplayed1);
    ASSERT_EQUALS(statusDisplayed1.Int(), KErrOverflow);
    
    // Pretend that a composition has occured
    khronos_int32_t newNotificationsMask = 0;
    SymbianStreamProcessNotifications(iNs, 
                                        iExpectedSourceStreamUpdatedEventMask, 
                                        iScreenNo,
                                        ++iStreamUpdatedSerialNumber,
                                        &newNotificationsMask);
    
    ASSERT_TRUE(newNotificationsMask == 0);
    
    // Make sure displayed event was completed
    User::WaitForRequest(statusDisplayed2);
    ASSERT_EQUALS(statusDisplayed2.Int(), KErrNone);
    
    SymbianStreamRemoveReference(iNs);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }


/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0120_2
@SYMTestCaseDesc        Test overflow conditions on notifications for displayed-x-times
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Verify that the surface stream adaptation correctly handles duplicate notifications
                        registered by SUS for content updates on different buffers
@SYMTestActions

     1.  Create a surface
     2.  Create a native stream for the surface
     3.  Register the test class as an observer for ESOWF_EventUpdated to act as a fake composer
     4.  Register for displayed-x-times notification (d1)
     5.  Submit an update to the surface on buffer 0
     6.  Register for displayed-x-times notification (d2)
     7.  Submit an update to the surface on buffer 1
     8.  The observer function should be invoked and verifies that stream and event parameters.
     9.  Verify d1 status is completed with KErrOverflow
     10. The tester simulates the end of a composition by firing SymbianStreamProcessNotifications
     11. Step 11. is repeated 5 times to simulate 5 compositions.
     12. Verify that d2 status is completed with KErrNone
     13. Destroy the native stream
        
@SYMTestExpectedResults
    No errors, d1 status completed with KErrOverflow, d2 status completed with KErrNone.
        
*/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0120_2L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0;    
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iSourceStreamUpdatedCalled = 0;
    TInt X = 5;
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    err = SymbianStreamAcquire(&surface, &iNs);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(iNs);
    
    err = SymbianStreamAddExtendedObserver(iNs, SourceStreamUpdatedCallback, this, iScreenNo, ESOWF_EventUpdated);
    ASSERT_TRUE(err == KErrNone);
    iExpectedSourceStreamUpdatedEventMask = ESOWF_EventDisplayedX;
    
    TRequestStatus statusDisplayedX1, statusDisplayedX2;

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                NULL,                       //aStatusConsumed
                                NULL,                       //aStatusDisplayed
                                NULL,                       //aTimeStamp
                                &statusDisplayedX1,         //aStatusDispXTimes
                                &X                          //aDisplayedXTimes
                                );
    
    // Verify that the context's callback is invoked when SubmitUpdate is called.
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 2);
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                1,                          //aBuffer
                                NULL,                       //aRegion
                                NULL,                       //aStatusConsumed
                                NULL,                       //aStatusDisplayed
                                NULL,                       //aTimeStamp
                                &statusDisplayedX2,         //aStatusDispXTimes
                                &X                          //aDisplayedXTimes
                                );
    
    // Verify that the context's callback is invoked when SubmitUpdate is called.
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 4);
    
    User::WaitForRequest(statusDisplayedX1);
    ASSERT_EQUALS(statusDisplayedX1.Int(), KErrOverflow);
    
    for (TInt i = 0; i < X; ++i)
        {
        ASSERT_EQUALS(statusDisplayedX2.Int(), KRequestPending);
        
        // Pretend that a composition has occured
        khronos_int32_t newNotificationsMask = 0;
        SymbianStreamProcessNotifications(iNs, 
                                            iExpectedSourceStreamUpdatedEventMask, 
                                            iScreenNo,
                                            ++iStreamUpdatedSerialNumber,
                                            &newNotificationsMask);
        if (i < X -1)
            {
            ASSERT_TRUE(newNotificationsMask == ESOWF_EventDisplayedX);
            }
        else
            {
            ASSERT_TRUE(newNotificationsMask == 0);
            }
        }
    
    // Make sure displayed event was completed
    User::WaitForRequest(statusDisplayedX2);
    ASSERT_EQUALS(statusDisplayedX2.Int(), KErrNone);
    
    SymbianStreamRemoveReference(iNs);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0120_3
@SYMTestCaseDesc        Test overflow conditions on notifications for available
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Verify that the surface stream adaptation correctly handles duplicate notifications
                        registered by SUS for content updates on different buffers
@SYMTestActions

     1.  Create a surface
     2.  Create a native stream for the surface
     3.  Register the test class as an observer for ESOWF_EventUpdated to act as a fake composer
     4.  Register for available notification (d1)
     5.  Submit an update to the surface on buffer 0
     6.  Register for available notification (d2)
     7.  Submit an update to the surface on buffer 1
     8.  The observer function should be invoked and verifies that stream and event parameters.
     9.  The tester simulates the end of a composition by firing SymbianStreamProcessNotifications
     10. Verify d1 status is completed with KErrOverflow and d2 status is completed with KErrNone
     11. Destroy the native stream
        
@SYMTestExpectedResults
    No errors, d1 status completed with KErrOverflow, d2 status completed with KErrNone.
        
*/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0120_3L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0;    
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iSourceStreamUpdatedCalled = 0;
    //force composition, otherwise we may never be able to experience the overflow
    iImmediateAvailable = EFalse;
    iImmediateVisible = SYM_CONTENT_VISIBLE;
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    err = SymbianStreamAcquire(&surface, &iNs);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(iNs);
    
    err = SymbianStreamAddExtendedObserver(iNs, SourceStreamUpdatedCallback, this, iScreenNo, ESOWF_EventUpdated);
    ASSERT_TRUE(err == KErrNone);
    
    TRequestStatus statusAvailable1, statusAvailable2, statusAvailable3;

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    iExpectedSourceStreamUpdatedEventMask = 0;

    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                &statusAvailable1,          //aStatusConsumed
                                NULL,                       //aStatusDisplayed
                                NULL,                       //aTimeStamp
                                NULL,                       //aStatusDispXTimes
                                NULL                        //aDisplayedXTimes
                                );
    
    // Verify that the context's callback is invoked when SubmitUpdate is called.
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 2);
    
    iExpectedSourceStreamUpdatedEventMask = ESOWF_EventAvailable;
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                1,                          //aBuffer
                                NULL,                       //aRegion
                                &statusAvailable2,          //aStatusConsumed
                                NULL,                       //aStatusDisplayed
                                NULL,                       //aTimeStamp
                                NULL,                       //aStatusDispXTimes
                                NULL                        //aDisplayedXTimes
                                );
    
    // Verify that the context's callback is invoked when SubmitUpdate is called.
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 4);
   
    iExpectedSourceStreamUpdatedEventMask = ESOWF_EventAvailable;
    
    updateProxy->ContentUpdated(surface,                    //aSurface
                                2,                          //aBuffer
                                NULL,                       //aRegion
                                &statusAvailable3,          //aStatusConsumed
                                NULL,                       //aStatusDisplayed
                                NULL,                       //aTimeStamp
                                NULL,                       //aStatusDispXTimes
                                NULL                        //aDisplayedXTimes
                                );
    
    // Verify that the context's callback is invoked when SubmitUpdate is called.
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 6);
   
    // Theoretically KErrNone could be returned because buffer 0 is actually available.
    // However, this would be a change in behaviour.
    User::WaitForRequest(statusAvailable1);
    ASSERT_EQUALS(statusAvailable1.Int(), KErrOverflow);
    
    // Pretend that a composition has occured
    khronos_int32_t newNotificationsMask = 0;
    SymbianStreamProcessNotifications(iNs, 
                                        iExpectedSourceStreamUpdatedEventMask, 
                                        iScreenNo,
                                        iStreamUpdatedSerialNumber,
                                        &newNotificationsMask);
    
    ASSERT_TRUE(newNotificationsMask == 0);
    
    // Make sure displayed event was completed
    User::WaitForRequest(statusAvailable2);
    ASSERT_EQUALS(statusAvailable2.Int(), KErrNone);
    
    SymbianStreamRemoveReference(iNs);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }



/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0122
@SYMTestCaseDesc        
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        High
@SYMTestPurpose         Tests an end to end displayed notification.
@SYMTestActions

     1.  Create a surface.
     2.  Create a native stream for the surface.
     3.  Register an observer for content-update events.
     4.  Register notifications for displayed, available and displayed-x-times.
     5.  Submit an update to the surface
     6.  Remove the content-updated observer.
     7.  Wait for the request status objects to be completed.
        
@SYMTestExpectedResults
    The notification status objects are completed with KErrCancel.
        
 **/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0122L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    iScreenNo = 0;    
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iSourceStreamUpdatedCalled = 0;
    CTestNativeStream::iTester = this;
    iImmediateVisible = SYM_CONTENT_VISIBLE;
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    err = SymbianStreamAcquire(&surface, &iNs);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(iNs);
    
    err = SymbianStreamAddExtendedObserver(iNs, SourceStreamUpdatedCallback, this, iScreenNo, ESOWF_EventUpdated);
    ASSERT_TRUE(err == KErrNone);
    
    TRequestStatus statusDisplayed, statusAvailable, statusDisplayedX;
    TUint32 displayedTime = 0;
    TInt X = 42;

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(iScreenNo,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* updateProxy=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(updateProxy);
    
    iExpectedSourceStreamUpdatedEventMask = ESOWF_EventDisplayed | ESOWF_EventDisplayedX;

    updateProxy->ContentUpdated(surface,                    //aSurface
                                0,                          //aBuffer
                                NULL,                       //aRegion
                                &statusAvailable,           //aStatusConsumed
                                &statusDisplayed,           //aStatusDisplayed
                                &displayedTime,             //aTimeStamp
                                &statusDisplayedX,          //aStatusDispXTimes
                                &X                          //aDisplayedXTimes
                                );     
    
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 2);
    err = SymbianStreamRemoveObserver(iNs, this, ESOWF_EventUpdated);
    ASSERT_TRUE(err == KErrNone);
    
    User::WaitForRequest(statusDisplayed);
    ASSERT_EQUALS(statusDisplayed.Int(), KErrCancel);
    User::WaitForRequest(statusAvailable);
    ASSERT_EQUALS(statusAvailable.Int(), KErrCancel);
    User::WaitForRequest(statusDisplayedX);
    ASSERT_EQUALS(statusDisplayedX.Int(), KErrCancel);
    
    SymbianStreamRemoveReference(iNs);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    
    iUtility->DestroySurface(surface);
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0130
@SYMTestCaseDesc        Add/remove Sym observers - positive test cases
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        
@SYMTestPurpose         Verify that Sym observers can be added, removed and re-added
@SYMTestActions 
        Create a surface with 1 buffer
        Add a Sym observer (displayed notification) 
        Remove the Sym observer
        Add the Sym observer
@SYMTestExpectedResults
        KErrNone is returned when the Sym observer is added
        KErrNone is returned when the Sym observer is removed
        KErrNone is returned when the Sym observer is re-added
 **/ 
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0130L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 1);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    SymbianStreamType ns;
    err = SymbianStreamAcquire(&surface,&ns);
    ASSERT_TRUE(err == KErrNone);    
    ASSERT_TRUE(ns);
    
    TInt32 screenNumber = 0;
    TInt localnumber = 0;
    err = SymbianStreamAddExtendedObserver(ns, TestUpdateCallback, &localnumber, screenNumber, ESOWF_EventDisplayed);
    ASSERT_TRUE(err == KErrNone);
    err = SymbianStreamRemoveObserver(ns, &localnumber, ESOWF_EventDisplayed);
    ASSERT_TRUE(err == KErrNone);
    err = SymbianStreamAddExtendedObserver(ns, TestUpdateCallback, &localnumber, screenNumber, ESOWF_EventDisplayed);
    ASSERT_TRUE(err == KErrNone);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0131
@SYMTestCaseDesc        Add/remove Sym observers - negative test cases
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        
@SYMTestPurpose         Verify that Sym observers reject invalid parameters
@SYMTestActions 
        Create a surface with 1 buffer
        Add an invalid Sym observer (invalid event flag)
        Remove a non-existent Sym observer
        Add a valid Sym observer
        Add the same Sym observer
        Remove the valid Sym observer
        Remove the same Sym observer
@SYMTestExpectedResults
        KErrArgument is returned and the observer list is un-changed
        KErrNotFound is returned and the observer list is un-changed
        KErrOverflow is returned when the Sym observer is added again and the observer list is un-changed
        KErrNotFound is returned when the Sym observer is removed again the observer list is un-changed
 **/ 
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0131L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 1);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    SymbianStreamType ns;
    err = SymbianStreamAcquire(&surface, &ns);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(ns);
    
    TInt32 screenNumber = 0;
    err = SymbianStreamAddExtendedObserver(ns, NULL, NULL, screenNumber, ESOWF_NoEvent);
    ASSERT_TRUE(err == KErrArgument);
    err = SymbianStreamRemoveObserver(ns, &screenNumber, ESOWF_EventDisplayed);
    ASSERT_TRUE(err == KErrNotFound);
    
    // add the same observer twice
    err = SymbianStreamAddExtendedObserver(ns, NULL, NULL, screenNumber, ESOWF_EventDisplayed);
    ASSERT_TRUE(err == KErrArgument);
    err = SymbianStreamAddExtendedObserver(ns, NULL, NULL, screenNumber, ESOWF_EventDisplayed);
    ASSERT_TRUE(err == KErrArgument);
    
    // remove the same observer
    err = SymbianStreamRemoveObserver(ns, &screenNumber, ESOWF_EventDisplayed);
    ASSERT_TRUE(err == KErrNotFound);
    err = SymbianStreamRemoveObserver(ns, &screenNumber, ESOWF_EventDisplayed);
    ASSERT_TRUE(err == KErrNotFound);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0132
@SYMTestCaseDesc        Destroy a native stream when a Sym observer is registered
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        
@SYMTestPurpose         Verify that Sym observers are safely removed before native stream destruction
@SYMTestActions 
        Create a surface with 1 buffer
        Add a Sym observer (displayed notification) 
        Destroy the native stream
        Create another native stream using the surface created previously
        Remove the same Sym observer added in step 2
@SYMTestExpectedResults
        KErrNone is returned when the Sym observer is added
        KErrNotFound is returned when the Sym observer is attempting to be removed
 **/ 
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0132L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);

    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 1);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    SymbianStreamType ns;
    err = SymbianStreamAcquire(&surface, &ns);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(ns);
    
    TInt screenNumber = 0;
    err = SymbianStreamAddExtendedObserver(ns, NULL, NULL, screenNumber, ESOWF_EventDisplayed);
    ASSERT_TRUE(err == KErrArgument);
   
    SymbianStreamRemoveReference(ns);
    
    SymbianStreamType ns2;
    err = SymbianStreamAcquire(&surface, &ns2);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(ns2);
    
    err = SymbianStreamRemoveObserver(ns2, &screenNumber, ESOWF_EventDisplayed);
    ASSERT_TRUE(err == KErrNotFound);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
	}

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0133
@SYMTestCaseDesc        Test the construction/destruction of a CContentUpdateProxy
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        
@SYMTestPurpose         Verify that additional screens can be created and destroyed
@SYMTestActions

     1.  Create a new CContentUpdateProxy (screen 1)
     2.  Verify API version and Internal version are correct
     3.  Create a surface
     4.  Create a native stream for the surface
     5.  Register the test class as an observer for ESOWF_EventUpdated to act as a fake composer
     6.  Submit an update to the surface
     7.  The observer function should be invoked and verifies that stream and event parameters.
     8.  Verify that the source-stream updated callback is invoked.
     9.  Destroy the native stream.
     10. Destroy the CContentUpdateProxy (screen 1)

@SYMTestExpectedResults
        KErrNone is returned when screen 1 is created
        API version and Internal version match expected results
        KErrNone is returned when screen 1 is destroyed
 **/ 
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0133L()
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    // Register a new screen
    iScreenNo = 0;    
    iContextUpdatedFlags = 0;
    iStreamUpdatedSerialNumber = 1;
    iSourceStreamUpdatedCalled = 0;
    iImmediateVisible = SYM_CONTENT_VISIBLE;
    
    TSurfaceId surface=iUtility->CreateSurfaceL(TSize(100,100), EUidPixelFormatARGB_8888_PRE, 400, 4);
    ASSERT_FALSE(surface.IsNull());
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    err = SymbianStreamRegisterScreenNotifications(1, 0, KCompositorVersion);
    ASSERT_TRUE(err == KErrNone);   
    

    CExtensionContainer* updateExtension = NULL;
    err = SymbianStreamHasRegisteredScreenNotifications(1,reinterpret_cast<void**>(&updateExtension));
    ASSERT_TRUE(err == KErrNone);
    MCompositionSurfaceUpdate* screen1=updateExtension->GetInterface<MCompositionSurfaceUpdate>();
    ASSERT_NOT_NULL(screen1);

    TVersion expectedVersion(KCompositorVersionMajor,KCompositorVersionMinor,KCompositorVersionRevision);
    
    ASSERT_EQUALS(screen1->ApiVersion(), 3);
    ASSERT_TRUE(screen1->InternalVersion().iBuild == expectedVersion.iBuild &&
                screen1->InternalVersion().iMajor == expectedVersion.iMajor &&
                screen1->InternalVersion().iMinor == expectedVersion.iMinor);
        
    err = SymbianStreamAcquire(&surface, &iNs);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(iNs);
    
    iExpectedSourceStreamUpdatedEventMask = 0;
    err = SymbianStreamAddObserver(iNs, SourceStreamUpdatedCallback, this);
    ASSERT_TRUE(err == KErrNone);
    
    iSourceStreamUpdatedCalled = EFalse;    
    screen1->ContentUpdated(surface,0,NULL,NULL,NULL,NULL,NULL,NULL);
    
    // Verify that the context's callback (screen 1) is invoked when SubmitUpdate is called.
    ASSERT_TRUE(iSourceStreamUpdatedCalled == 1);
    
    SymbianStreamRemoveReference(iNs);
    
    // Destroy the context (screen 1)
    err = SymbianStreamUnregisterScreenNotifications(1);
    ASSERT_TRUE(err == KErrNone);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }              

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0140
@SYMTestCaseDesc        Negative test - Acquire write/read buffers and retrieve the buffer ptr (single threaded test).
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        
@SYMTestPurpose         Verify SymbianStreamAcquireWriteBuffer(), SymbianStreamReleaseWriteBuffer(),
                        SymbianStreamAcquireReadBuffer() and SymbianStreamReleaseReadBuffer() methods
                        work as expected.
                        
@SYMTestActions 
        Create a native stream 
        For each buffer:
        - Acquire the read buffer 
        - Acquire the write buffer 
        - Release the write buffer
        - Acquire the read buffer

        Repeat for each buffer. Finally:
        Acquire the write buffer
        Release the write buffer    
@SYMTestExpectedResults
        For each buffer of the native stream, check:
        - The read buffer should be aquired successfully
        If the number of buffers = 1, check:
        - The write buffer should be aquired successfully
        If the number of buffers > 1, check:
        - The write buffer before the final one should be aquired successfully
        - The final acquire write buffer call should return OWF_INVALID_HANDLE if not single buffered
 **/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0140L(TInt aNumBuffers)
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);

    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    
    ASSERT_TRUE(aNumBuffers > 0);
    
    TSize surfaceSize(TSize(100,100));
    khronos_int32_t width = surfaceSize.iWidth;
    khronos_int32_t height = surfaceSize.iHeight;
    
    OWF_IMAGE_FORMAT pixelFormat =
        {
        OWF_IMAGE_ARGB8888,
        ETrue,
        ETrue,
        aNumBuffers
        };  
    
    SymbianStreamType ns=helperCreateImageStream(width,
                                                    height,
                                                    &pixelFormat,
                                                    aNumBuffers);
    ASSERT_TRUE(ns);
    
    khronos_int32_t writeBuffer = 0;
    khronos_int32_t readBuffer = 0;    
    TInt bufferCount;
    
    if (aNumBuffers == 1)
        {
        bufferCount = aNumBuffers;
        }
    else
        {
        bufferCount = aNumBuffers - 1;
        }
    
    RArray<TInt> bufferHandleArray;

    // Loop through the buffers
    for (TInt count=0; count<bufferCount; count++)
        {
        err = SymbianStreamAcquireReadBuffer(ns, &readBuffer);
        ASSERT_TRUE(err == KErrNone);
        ASSERT_TRUE(readBuffer);
        bufferHandleArray.Append(readBuffer);
        
        // Acquire the write buffer
        err = SymbianStreamAcquireWriteBuffer(ns, &writeBuffer);
        ASSERT_TRUE(err == KErrNone);
        ASSERT_TRUE(writeBuffer);     
        SymbianStreamReleaseWriteBuffer(ns,writeBuffer);        
        }   

    // Acquire final read buffer
    khronos_int32_t finalReadBuffer;
    err = SymbianStreamAcquireReadBuffer(ns, &finalReadBuffer);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(finalReadBuffer);
    bufferHandleArray.Append(finalReadBuffer);        

    khronos_int32_t finalWriteBuffer;
    err = SymbianStreamAcquireWriteBuffer(ns, &finalWriteBuffer);
    ASSERT_TRUE(err != KErrBadHandle);
    
    // Final checks
    if (aNumBuffers == 1)
        {
        ASSERT_TRUE(finalWriteBuffer);
        err =  SymbianStreamReleaseWriteBuffer(ns,finalWriteBuffer);
        ASSERT_TRUE(err == KErrNone);
        }
    else
        {
        ASSERT_FALSE(finalWriteBuffer);        
        }
    
    for (TInt x=0; x < bufferHandleArray.Count(); ++x)
        {
        err = SymbianStreamReleaseReadBuffer(ns,bufferHandleArray[x]);
        ASSERT_TRUE(err == KErrNone);
        }

    err = SymbianStreamAcquireWriteBuffer(ns,&writeBuffer);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(writeBuffer);     
    err = SymbianStreamReleaseWriteBuffer(ns,writeBuffer);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(writeBuffer);     

    bufferHandleArray.Close();
    SymbianStreamRemoveReference(ns);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0141
@SYMTestCaseDesc        Negative test - Acquire multiple read buffers before releasing write buffer (single threaded test).
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            CT
@SYMTestPriority        
@SYMTestPurpose         Verify SymbianStreamAcquireWriteBuffer(), SymbianStreamReleaseWriteBuffer(),
                        SymbianStreamAcquireReadBuffer() and SymbianStreamReleaseReadBuffer() methods
                        work as expected.
                        
@SYMTestActions 
        Create a native stream 
        For each buffer:
        - Acquire the read buffer multiple times
        - Acquire the write buffer 
        - Acquire the read buffer       
        - Release the write buffer
        - Acquire the read buffer

@SYMTestExpectedResults
        For each buffer of the native stream, check:
        - The read buffer index should remain the same before release write bufffer is called
        If the number of buffers = 1, check:
        - The buffer index is always the same
        If the number of buffers > 1, check:
        - The buffer index increments after release write buffer is called
 **/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0141L(TInt aNumBuffers)
    {
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);

    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));
    
    ASSERT_TRUE(aNumBuffers > 0);
    
    TSize surfaceSize(TSize(100,100));
    khronos_int32_t width = surfaceSize.iWidth;
    khronos_int32_t height = surfaceSize.iHeight;
    
    OWF_IMAGE_FORMAT pixelFormat =
        {
        OWF_IMAGE_ARGB8888,
        ETrue,
        ETrue,
        aNumBuffers
        };  
    
    SymbianStreamType ns=helperCreateImageStream(width,
                                                    height,
                                                    &pixelFormat,
                                                    aNumBuffers);
    ASSERT_TRUE(ns);
    
    khronos_int32_t writeBuffer = 0;
    khronos_int32_t readBuffer = 0; 
    khronos_int32_t bufferIndexRead1, bufferIndexRead2, bufferIndexRead3, bufferIndexRead4;
    khronos_int32_t bufferIndexWrite;
    
    err = SymbianStreamAcquireReadBuffer(ns, &readBuffer);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(readBuffer);
    const TSurfaceId* getId = NULL;
    err = SymbianStreamGetBufferId(ns,readBuffer,&bufferIndexRead1,&getId);
    ASSERT_TRUE(err == KErrNone);
    err = SymbianStreamReleaseReadBuffer(ns,readBuffer);
    ASSERT_TRUE(err == KErrNone);
    
    err = SymbianStreamAcquireReadBuffer(ns, &readBuffer);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(readBuffer);
    err = SymbianStreamGetBufferId(ns,readBuffer,&bufferIndexRead2,&getId);
    ASSERT_TRUE(err == KErrNone);
    SymbianStreamReleaseReadBuffer(ns,readBuffer);
    ASSERT_EQUALS(bufferIndexRead1, bufferIndexRead2);

    err = SymbianStreamAcquireWriteBuffer(ns,&writeBuffer);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(writeBuffer);     
    err = SymbianStreamGetBufferId(ns,writeBuffer,&bufferIndexWrite,&getId);
    ASSERT_TRUE(err == KErrNone);
    if (aNumBuffers == 1)
        {
        ASSERT_EQUALS(bufferIndexRead2, bufferIndexWrite);                
        }
    else
        {
        ASSERT_NOT_EQUALS((bufferIndexRead2), bufferIndexWrite);        
        }

    err = SymbianStreamAcquireReadBuffer(ns,&readBuffer);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(readBuffer);
    err = SymbianStreamGetBufferId(ns,readBuffer,&bufferIndexRead3,&getId);
    ASSERT_TRUE(err == KErrNone);
    err = SymbianStreamReleaseReadBuffer(ns,readBuffer);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_EQUALS(bufferIndexRead3, bufferIndexRead2);
    
    err = SymbianStreamReleaseWriteBuffer(ns,writeBuffer);
    ASSERT_TRUE(err == KErrNone);
    
    err = SymbianStreamAcquireReadBuffer(ns,&readBuffer);
    ASSERT_TRUE(err == KErrNone);
    ASSERT_TRUE(readBuffer);
    err = SymbianStreamGetBufferId(ns,readBuffer,&bufferIndexRead4,&getId);
    ASSERT_TRUE(err == KErrNone);
    err = SymbianStreamReleaseReadBuffer(ns,readBuffer);
    ASSERT_TRUE(err == KErrNone);
    if (aNumBuffers == 1)
        {
        ASSERT_EQUALS(bufferIndexRead3, bufferIndexRead4);                
        }
    else
        {
        ASSERT_NOT_EQUALS((bufferIndexRead3), bufferIndexRead4);        
        }

    SymbianStreamRemoveReference(ns);
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/**
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0142
@SYMTestCaseDesc        Special surface ID unit tests
@SYMREQ                 
@SYMPREQ                PREQ2400
@SYMTestType            
@SYMTestPriority        
@SYMTestPurpose         Exercise special surface creation and flip operations
                        
@SYMTestActions 
        Generate a special surface ID with pixel formats of type 
        EUidPixelFormatARGB_8888_PRE, EUidPixelFormatRGB_888, 
        EUidPixelFormatRGB_565, and EUidPixelFormatRGB_332. Then
        - Acquire the read buffer
        - Acquire the write buffer 
        - Check that they are the same
        - Add a value to the read buffer
        - Check that the write buffer has the same value       
        - Release the write buffer
        - Release the read buffer
        Then reset the special surface ID, acquire a stream, and read header data.
        Set the flip flag to true and re-acquire the stream.
        Compare header data of pre-flip and post-flip cases.

@SYMTestExpectedResults
        Read/write buffer pairs are always identical.
        Flipped stream has width and height switched and stride is different
 **/ 
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0142L()
    {    
    _LIT(KDisplayChannelLdd, "display0.ldd");
    
    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));

    err = User::LoadLogicalDevice(KDisplayChannelLdd);
    RDisplayChannel testChannel;
    if (err == KErrNone || err == KErrAlreadyExists)
        {
        User::LeaveIfError(testChannel.Open(0));
        }
    
    TSurfaceId testId;
    SymbianStreamType ns;
    TUidPixelFormat testGuids[] = {EUidPixelFormatARGB_8888_PRE, EUidPixelFormatRGB_888, EUidPixelFormatRGB_565, EUidPixelFormatRGB_332};
    TInt size = sizeof(testGuids) / sizeof(testGuids[0]);
    TUint8 *pWriteBuffer = NULL;
    khronos_int32_t writeBuffer;
    TUint8 *pReadBuffer = NULL;
    khronos_int32_t readBuffer;

    TInt halStride = 0;
    TInt halMode = 0;

    // For header variables before flipping
    TInt32 preFlipWidth = 0;
    TInt32 preFlipHeight = 0;
    TInt32 preFlipStreamStride = 0;
    TInt32 preFlipStreamFormat = EUidPixelFormatUnknown;
    TInt32 preFlipStreamPixelSize = 0;
    
    // For header variables after flipping
    TInt32 width = 0;
    TInt32 height = 0;
    TInt32 streamStride = 0;
    TInt32 streamFormat = EUidPixelFormatUnknown;
    TInt32 streamPixelSize = 0;
    
    for (TInt ii = 0; ii < size; ii++)
        {
        HAL::Get(0, HALData::EDisplayMode, halMode);
        
        testId.iInternal[TSurfaceId::TScreenSurfaceUsage::EScreenField] = 0;      // Screen number
        testId.iInternal[TSurfaceId::TScreenSurfaceUsage::EHalField] = halMode;          // Rotation and hal mode index
        testId.iInternal[TSurfaceId::TScreenSurfaceUsage::ETypeGuidField] = testGuids[ii]; //May be zero for non-GCE modes
        testId.iInternal[TSurfaceId::TScreenSurfaceUsage::ETypeClassField] = ((TUint32)(TSurfaceId::EScreenSurface) << TSurfaceId::TScreenSurfaceUsage::ETypeClassShift);  // Type
                        
        err = SymbianStreamAcquire(&testId, &ns);
        ASSERT_TRUE(err == KErrNone);
        ASSERT_TRUE(ns);
        
        err = SymbianStreamAcquireWriteBuffer(ns,&writeBuffer);
        ASSERT_TRUE(err == KErrNone);
        
        err = SymbianStreamGetBufferPointer(ns,writeBuffer,reinterpret_cast<void**>(&pWriteBuffer));
        ASSERT_TRUE(err == KErrNone);
        ASSERT_NOT_NULL(pWriteBuffer);
        
        err = SymbianStreamReleaseWriteBuffer(ns,writeBuffer);
        ASSERT_TRUE(err == KErrNone);

        err = SymbianStreamAcquireReadBuffer(ns,&readBuffer);
        ASSERT_TRUE(err == KErrNone);
                    
        err = SymbianStreamGetBufferPointer(ns,readBuffer,reinterpret_cast<void**>(&pReadBuffer));
        ASSERT_TRUE(err == KErrNone);
        ASSERT_NOT_NULL(pReadBuffer);

        err = SymbianStreamReleaseReadBuffer(ns,readBuffer);
        ASSERT_TRUE(err == KErrNone);
        
        ASSERT_SAME(pWriteBuffer, pReadBuffer);
        
        *pWriteBuffer = 0xFF;
        ASSERT_TRUE(*pReadBuffer == 0xFF);
        
        halStride = halMode;
        HAL::Get(0, HALData::EDisplayOffsetBetweenLines, halStride);

        SymbianStreamGetHeader(ns, &preFlipWidth, &preFlipHeight, &preFlipStreamStride, &preFlipStreamFormat, &preFlipStreamPixelSize);
        ASSERT_TRUE(preFlipStreamStride == halStride);

        // Now the flipping test
        halMode = halMode^TSurfaceId::TScreenSurfaceUsage::EHalFlippedFlag;
        testId.iInternal[TSurfaceId::TScreenSurfaceUsage::EHalField] = halMode; 
                
        err = SymbianStreamAcquire(&testId, &ns);
        ASSERT_TRUE(err == KErrNone);
        ASSERT_TRUE(ns);

        halStride = halMode;
        HAL::Get(0, HALData::EDisplayOffsetBetweenLines, halStride);
        SymbianStreamGetHeader(ns, &width, &height, &streamStride, &streamFormat, &streamPixelSize);
        
        ASSERT_EQUALS(preFlipWidth, height);
        ASSERT_EQUALS(preFlipHeight, width);
        ASSERT_EQUALS((TInt32)preFlipStreamFormat,(TInt32)streamFormat);
        ASSERT_TRUE(streamStride == halStride);
        ASSERT_EQUALS(preFlipStreamPixelSize, streamPixelSize); 
            
        SymbianStreamGetHeader(ns, NULL, NULL, NULL, NULL, NULL);
        // Clearing the flip flag
        halMode = halMode&~TSurfaceId::TScreenSurfaceUsage::EHalFlippedFlag;
        
        SymbianStreamRemoveReference(ns);     

        *pWriteBuffer = NULL;
        pWriteBuffer = NULL;
        *pReadBuffer = NULL;
        pReadBuffer = NULL;
        }
    
    testChannel.Close();
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
    }

/*
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0143
@SYMTestCaseDesc        Call eglsync helper functions
@SYMREQ                 
@SYMPREQ                
@SYMTestType            
@SYMTestPriority        Low
@SYMTestPurpose         Make calls of eglsync helper functions for coverage results                     
@SYMTestActions 
        Make calls of eglsync helper functions 
@SYMTestExpectedResults
        Test should pass without errors
 */
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0143L()
    {
    // This test is commented out because it won't build for Bullseye.
#ifdef EGLSYNCHELPER_INCLUDED
    INFO_PRINTF1(_L("Test of egl sync helper functions"));

    RHeap* mainHeap = COpenWfcStreamMap::InstanceL().GetMainHeap();
    TRAPD(err, GrowCleanupStackL());
    ASSERT_TRUE(err == KErrNone);
    
    CleanupStack::PushL(TCleanupItem(PopHeap, User::SwitchHeap(mainHeap)));

    EGLDisplay eglDisplay;
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    ASSERT_FALSE(eglDisplay == EGL_NO_DISPLAY);
    ASSERT_FALSE(eglDisplay == EGL_BAD_ALLOC);
    ASSERT_EQUALS(eglGetError(),EGL_SUCCESS);
    eglInitialize(eglDisplay, NULL, NULL);
    ASSERT_EQUALS(eglGetError(),EGL_SUCCESS);

    EGLint attrib_list[1] = {EGL_NONE};
    EGLSyncKHR sync;
    sync = eglCreateSyncKHR(eglDisplay,EGL_SYNC_REUSABLE_KHR, attrib_list);
    ASSERT_NOT_EQUALS(sync,EGL_NO_SYNC_KHR);
    ASSERT_EQUALS(eglGetError(),EGL_SUCCESS);

    eglSignalSyncKHR(eglDisplay, sync, EGL_SIGNALED_KHR);
    eglGetSyncAttribKHR(eglDisplay, sync, NULL, NULL);
    eglClientWaitSyncKHR(eglDisplay, sync, 0, 0);
    
    EGLBoolean eglSyncError = eglDestroySyncKHR(eglDisplay, sync);
    sync = EGL_NO_SYNC_KHR;
    if (eglSyncError != EGL_TRUE)
        {
        INFO_PRINTF2(_L("TearDown: eglSyncError line %d"),__LINE__);
        }
    CleanupStack::PopAndDestroy(); // switch the heap back to current thread's one
#endif
    }

/*
@SYMTestCaseID          GFX_OPENWFC_NATIVESTREAM_0144
@SYMTestCaseDesc        Panic test
@SYMREQ                 
@SYMPREQ                
@SYMTestType            
@SYMTestPriority        Low
@SYMTestPurpose         Test that will cause a panic.               
@SYMTestActions 
        Aquire a symbian stream by passing in a NULL ns
@SYMTestExpectedResults
        Test should panic with the expected panic code 1000008
*/
void CTestNativeStream::GRAPHICS_OPENWFC_NATIVESTREAM_0144L()
    {
    INFO_PRINTF1(_L("Panic test. The expected panic code is 1000008"));
    
    SymbianStreamBuffer bufferHandle;
    // Pass in a NULL ns will cause panic EOwfSymbianStreamBadArgument (1000008)
    SymbianStreamAcquireReadBuffer(NULL, &bufferHandle);
    }
