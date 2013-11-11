/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#include "NGAPostProcHwDevice.h"
#include "NGAPostProcSessionManager.h"
#include "NGAPostProcSurfaceHandler.h"

//currently this uid only used if not under WINSCW environment since the only hw to use is bridge
#define EStUidPixelFormatYUV_420MB  0x2001FBC1
// post-processor info
const TUid KUidVideoPostProcHwDevice = {KUidNGAPostProcHwDeviceImplUid};
_LIT(KManufacturer, "Nokia Inc.");
_LIT(KIdentifier, "Nokia S60 Video Post Processor Hardware Device Plugin");

// --- Constants ---
const TInt KMaxVBMBuffers      			= 4;
const TInt KMinVBMInputWidth   			= 32; 
const TInt KMinVBMInputHeight  			= 32;
const TInt KMaxVBMInputWidth   			= 1280; 
const TInt KMaxVBMInputHeight  			= 720;
const TInt KRenderAhead 	     		= 50000;     
const TInt KMaxRenderDelay     			= 50000;
const TInt KPostingOfset       			= 0;    
const TInt KColorConversionBuffers  	= 3;
const TInt KMaxBuffersGceCanHold    	= 3;
const TInt KDefPlayRate					= 100;
const TInt KMaxAllowedSkipInNFrames 	= 40;
#ifdef __cplusplus
extern "C"
{
#endif

int32 gColorConvYUVtoYUV422Int (tBaseVideoFrame *yuv420Frame, tBaseVideoFrame* yuv422Frame,
							   uint8 outClrFmt, int16 stride); 

int32 Emz_VDec_gColorConv_YUVtoRGB ( 
	  tBaseVideoFrame *srcImage, uint8 *dstImage, tWndParam *srcWindow, 
	  tWndParam *dstWindow, uint8 srcImageFormat, uint8 dstImageFormat,
	  uint8 colorConvScheme);
		 	  
#ifdef __cplusplus
}
#endif

//**************************************************

CMMFVideoPostProcHwDevice* CNGAPostProcHwDevice::NewL() 
{ 
   PP_DEBUG(_L("CNGAPostProcHwDevice::NewL() ++"));

    CNGAPostProcHwDevice* self = new (ELeave) CNGAPostProcHwDevice; 
    CleanupStack::PushL(self);
    self->ConstructL(); 
    CleanupStack::Pop();

   PP_DEBUG(_L("CNGAPostProcHwDevice::NewL() --"));
    return (CMMFVideoPostProcHwDevice*)self; 
}

void CNGAPostProcHwDevice::ConstructL() 
{ 
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::ConstructL() ++"), this);
    
    // support for VBM buffer interface
    iVBMBufferOptions.iNumInputBuffers  = KMaxVBMBuffers; 
    iVBMBufferOptions.iBufferSize = TSize(KMaxVBMInputWidth, KMaxVBMInputHeight);
    iPostingTimer = CNGAPostProcTimer::NewL(*this);
    User::LeaveIfError(iWsSession.Connect());
    
    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::ConstructL() --"), this);
}

CNGAPostProcHwDevice::CNGAPostProcHwDevice() 
        :   iProxy(NULL), 
            iInputDecoderDevice(NULL),
            iCurrentPlaybackPosition(TTimeIntervalMicroSeconds(0)),
            iPPState(EInitializing),
            iSurfaceHandler(NULL),
            iSessionManager(NULL),
            iIsInputEnded(EFalse),
            iPostingTimer(NULL),
            iFirstPictureUpdated(EFalse),
            iUsingExternalSurface(EFalse),
            iIsColorConversionNeeded(EFalse),
            iSurfaceCreatedEventPublished(EFalse),
            iOverflowPictureCounter(0),
            iVideoFrameBufSize(0),
            iResourceLost(EFalse),
            iRedrawDone(EFalse),
			iRedrawSurfaceInUse(EFalse),
            iVBMObserver(NULL),
            iVBMEnabled(EFalse),        
            count(0),
            iSurfaceMask(surfaceHints::EAllowAllExternals),
            iSurfaceKey(TUid::Uid(surfaceHints::KSurfaceProtection)),
            iVideoSurfaceObserver(NULL),
            iVPObserver(NULL),
            iPicSize(0,0),
			iAspectRatioNum(1),
			iAspectRatioDenom(1),
            iStepFrameCount(0),
            iPlayRate(KDefPlayRate),
            iKeyFrameMode(EFalse),
            iFPObserver(NULL),
            iIsExternalChunk(EFalse)
{
	iSurfaceId 		 = TSurfaceId::CreateNullId();

#if defined __WINSCW__ 
	iAttributes().iPixelFormat    = EUidPixelFormatYUV_422Interleaved;
#else    
	iAttributes().iPixelFormat    = (TUidPixelFormat) EStUidPixelFormatYUV_420MB;
#endif
}

CNGAPostProcHwDevice::~CNGAPostProcHwDevice()
{
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::~CNGAPostProcHwDevice() ++"), this);
   	if (iSessionManager)
    {
        iSessionManager->CancelNotifiers();
        delete iSessionManager;
        iSessionManager = NULL;
    }
   	
   	while (iSupportedInputFormats.Count()>0)
    {
		iSupportedInputFormats.Remove(0);
    }
    
   	while (iProcessQ.Count()>0)
    {
		iProcessQ.Remove(0);
    }

   	if(iPostingTimer)
    {
    	iPostingTimer->Cancel();
        delete iPostingTimer;
        iPostingTimer = NULL;
    }
    
    while (iVBMBufferReferenceQ.Count()>0)
    {
        TVideoPicture* pic = iVBMBufferReferenceQ[0];
        iVBMBufferReferenceQ.Remove(0);
        if (iColorConversionQ.Count()>0)
    	{
	        iColorConversionQ.Remove(0);
	    }

        if (pic->iHeader) delete pic->iHeader;
        delete pic->iData.iRawData;
        delete pic;
    }
    
    iSupportedInputFormats.Reset();
    iSupportedInputFormats.Close();
    
    iVBMBufferReferenceQ.Reset();
    iVBMBufferReferenceQ.Close();
    
    iColorConversionQ.Reset();
    iColorConversionQ.Close();
    
    iVBMBufferQ.Reset();
    iVBMBufferQ.Close();
    
    iProcessQ.Reset();
    iProcessQ.Close();
        
    iInputQ.Reset();
    iInputQ.Close();
    
    if (iSurfaceHandler)
    {
    	if(!iSurfaceId.IsNull())
    	{
    		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::UnregisterSurface"), this);
			TInt numScreens = iWsSession.NumberOfScreens();
    		for(TInt i=0;i < numScreens;i++)
    		{
    			iWsSession.UnregisterSurface(i, iSurfaceId);
    		}
    		iWsSession.Flush();
        	TInt err = iSurfaceHandler->DestroySurface(iSurfaceId);
    	}
        delete iSurfaceHandler;
        iSurfaceHandler = NULL;
    }
    
    iWsSession.Close();
    if(!iIsExternalChunk)
    {
        iChunk.Close();
    }
    
    RDebug::Printf("------ Statistics of Post Processor ------");
    RDebug::Printf("    Pictures Received : %d", iPictureCounters.iTotalPictures);
    RDebug::Printf("    Pictures Displayed: %d", iPictureCounters.iPicturesDisplayed);
    RDebug::Printf("    Pictures Skipped  : %d", iPictureCounters.iPicturesSkipped);
    RDebug::Printf("    Pictures overflow : %d", iOverflowPictureCounter);
    RDebug::Printf("------------------------------------------");
    
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:~() --"), this);
}

void CNGAPostProcHwDevice::SetInputFormatL(const TUncompressedVideoFormat&  aFormat) 
{ 
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetInputFormatL() Pattern= %x ++"), this, aFormat.iYuvFormat.iPattern);
    if (iPPState != EInitializing)
    {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetInputFormatL() FAILED: Unexpected state"), this);
        User::Leave(KErrNotReady);
    }

		iVideoFormat = aFormat; 
		if( ((iVideoFormat.iYuvFormat.iPattern == EYuv420Chroma1) ||
			(iVideoFormat.iYuvFormat.iPattern == EYuv420Chroma2) ||
    		(iVideoFormat.iYuvFormat.iPattern == EYuv420Chroma3) ))
		{
			iVideoFormat.iYuvFormat.iCoefficients  	     = EYuvBt709Range1;
    		iVideoFormat.iYuvFormat.iPattern       	     = EYuv422Chroma1;
    		iVideoFormat.iYuvFormat.iDataLayout          = EYuvDataInterleavedBE;
			
#if defined __WINSCW__				
				iIsColorConversionNeeded = ETrue; 
#else
				iAttributes().iPixelFormat = (TUidPixelFormat) EStUidPixelFormatYUV_420MB;
#endif
		}	

   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetInputFormatL() WARNING: -- Not Found!"), this);
}


void CNGAPostProcHwDevice::SetInputDevice(CMMFVideoDecodeHwDevice* aDevice) 
{ 
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetInputDevice() ++"), this);

    if (iPPState != EInitializing)
    {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetInputDevice() FAILED: unexpected state"), this);
        return;
	}

    iInputDecoderDevice = aDevice;

   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetInputDevice() --"), this);
}

void CNGAPostProcHwDevice::GetOutputFormatListL(RArray<TUncompressedVideoFormat>& ) 
{ 
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:GetOutputFormatListL() ++"), this);


	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:GetOutputFormatListL() --"), this);
}

void CNGAPostProcHwDevice::SetOutputFormatL(const TUncompressedVideoFormat&  ) 
{
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetOutputFormatL() ++"), this);

	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetOutputFormatL() --"), this);
}

void CNGAPostProcHwDevice::SetClockSource(MMMFClockSource* aClock) 
{ 
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetClockSource() ++"), this);
    
    if (iPPState != EInitializing)
    {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetClockSource() FAILED: Unexpected state"), this);
        return;
	}
    iClockSource = aClock;

   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetClockSource() --"), this);
}

void CNGAPostProcHwDevice::SetVideoDestScreenL(TBool /*aScreen*/) 
{ 
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetVideoDestScreenL() ++"), this);

   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetVideoDestScreenL() --"), this);
}

void CNGAPostProcHwDevice::SetProxy(MMMFDevVideoPlayProxy& aProxy) 
{ 
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetProxy() ++"), this);

    if (iPPState != EInitializing)
    {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetProxy() FAILED: Unexpected state"), this);
        return;
	}

    iProxy = &aProxy;

   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetProxy() --"), this);
}

void CNGAPostProcHwDevice::Initialize() 
{
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Initialize ++"));
	TInt err = KErrNone;

    if (iPPState != EInitializing)
    {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Initialize() FAILED: Unexpected state"), this);
        if (iProxy)
		{
			iProxy->MdvppInitializeComplete(this, KErrNotReady);
		}
		return;
	}
	if (!iSurfaceHandler)
    {
    	TRAP(err, iSurfaceHandler = CNGAPostProcSurfaceHandler::NewL());
    	if (err != KErrNone)
    	{
    	   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Initialize -- failed to create SurfaceHandler."), this);
    	    if (iProxy)
			{
				iProxy->MdvppInitializeComplete(this, err);
			}
			return;
    	}
    }
    if (!iSessionManager)
    {
    	TRAP(err, iSessionManager = CNGAPostProcSessionManager::NewL());
    	if (err != KErrNone)
    	{
    	   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Initialize -- failed to create SessionManager."), this);
    	    if (iProxy)
			{
				iProxy->MdvppInitializeComplete(this, err);
			}
			return;
    	}
    	iSessionManager->SetObserver(*this);
    }

  	if (iInputDecoderDevice)
	{
		MMmfVideoResourceHandler* handler = NULL;
		handler = (MMmfVideoResourceHandler*)iInputDecoderDevice->CustomInterface(KUidMmfVideoResourceManagement);
		if (handler)
		{
			handler->MmvrhSetObserver((MMmfVideoResourceObserver*)this);
		}
		else
		{
			PP_DEBUG(_L("ppHwDev[%x]:Initialize() decoder yet to implement MMmfVideoResourceHandler CI"), this);
		}
		
		MMmfVideoPropertiesNotifier* VPHandler = NULL;
		VPHandler = (MMmfVideoPropertiesNotifier*)iInputDecoderDevice->CustomInterface(KUidMmfVideoPropertiesManagement);
		if (VPHandler)
		{
			PP_DEBUG(_L("ppHwDev[%x]:Initialize() Register for video property changes"), this);
			VPHandler->MmvpnSetObserver((MMmfVideoPropertiesObserver*)this);
		}
		else
		{
			PP_DEBUG(_L("ppHwDev[%x]:Initialize() decoder yet to implement MMmfVideoPropertiesNotifier CI"), this);
		}
	}
		
    // Initialize picture counters
	iPictureCounters.iPicturesSkipped 	= 0;
	iPictureCounters.iPicturesDisplayed = 0;
	iPictureCounters.iTotalPictures = 0;
	iOverflowPictureCounter = 0;
	
	iPPState = EInitialized;
	if(iPostInitializeResponse)
	{
		
		TRAP(err, iPostInitializeResponse->MmpirPostInitializeResponseL());
	}
	
	if(!err)
	{
		TRAP(err, iSessionManager->CreateNotifierL(iInfo().iBuffers));
	}
	else
	{
		iPPState = EInitializing;
	}
	
	if (iProxy)
	{
		iProxy->MdvppInitializeComplete(this, err);
	}
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Initialize --"), this);
}

void CNGAPostProcHwDevice::WritePictureL(TVideoPicture* aPicture) 
{ 
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:WritePicture bufId = %d"), this,GetID(aPicture));
	TVideoPicture* pic;
	if (iPPState==EInitializing || iPPState==EStopped || iIsInputEnded)
    {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:WritePictureL() FAILED: Unexpected state"), this);
        User::Leave(KErrNotReady);
	}

    if(!aPicture)
    {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:WritePictureL() FAILED: Invalid argument"), this);
		User::Leave(KErrArgument);
	}
	pic = aPicture;	
	iPictureCounters.iTotalPictures++;
	if((iPPState != EPlaying) && (iFirstPictureUpdated))
	{
	//If decoder is fast enough, it can happen between Initialize->Start time gap between 
	//DecodeHwDevice and PostProc_HwDevice. OR between Pause->Resume time gap as well.
		AddToQ(pic);
	}
	else if( iInputQ.Count() > 0 )
	{
		AddToQ(pic);
		AttemptToPost();
	}
	else
	{
		TTimeToPost timeToPost = EPostIt;
		TInt64 delta = 0;
	    if(iFirstPictureUpdated)
		{
			timeToPost = (TTimeToPost)IsTimeToPost(pic, delta);
			if(!IsGceReady())
		    {  
				PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:WritePictureL GCE not ready"), this );
				if(timeToPost == EPostIt)
				{
                    timeToPost = EDelayIt;
				}
		    }
		    if (delta > 0x7FFFFFFF)
		    {
		         PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:WritePictureL Too large delta .. skipping"), this ); 
		         timeToPost = ESkipIt;
		    }
		}
		else
		{
			if(!iSurfaceCreatedEventPublished)
                {
                    PublishSurfaceCreated();
                }
			iFirstPictureUpdated = ETrue;
		}
		

		switch(timeToPost)
		{
			case EDelayIt:
			{
				if(AddToQ(pic) != 0)
				{
					break;
				}
				iPostingTimer->Cancel();
				SetTimer(delta);
			}
			break;
			case EPostIt:
			{
		
				if(iIsColorConversionNeeded)
				{
					TVideoPicture* ccPic;				
	    			ccPic = DoColorConvert(pic); // output will be in ccPic
	    			pic = ccPic;			   
				}
						
				#ifdef _DUMP_YUV_FRAMES
				captureYuv(pic);
				#endif
				TInt err = iSessionManager->PostPicture(iSurfaceId, GetID(pic), ETrue); 
				if(err == KErrNone)
				{
				    iProcessQ.Append(pic);
					iCurrentPlaybackPosition = pic->iTimestamp;
				}
				else
				{
					ReleasePicture(pic);
				}
			}
			break;
			case ESkipIt:
			{
				ReleasePicture(pic); 
				PicturesSkipped();
			}
			break;
		}
    }
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:WritePicture --"), this);
}


CPostProcessorInfo* 
CNGAPostProcHwDevice::PostProcessorInfoLC() 
{
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:PostProcessorInfoLC() ++"), this);
    TUncompressedVideoFormat yuvFormat;
    RArray<TUint32>                     SupportedPostProcess;
    TBool                               SupportedHwAcceleration = ETrue;   //Non-Accelerated ETrue, 
    TYuvToRgbCapabilities               SupportedYuvToRgbCapab; 
    TInt32                              SupportedRotations = ERotateNone; // no rotation supported
    
    TBool                               SupportedArbitraryScaling = EFalse; // no scaling supported
    RArray<TScaleFactor>                SupportedScaleFactors;
    TBool                               SupportedAntiAliasing = EFalse;
    
    //default
    yuvFormat.iDataFormat                     = EYuvRawData;
    yuvFormat.iYuvFormat.iYuv2RgbMatrix       = 0;
    yuvFormat.iYuvFormat.iRgb2YuvMatrix       = 0;
    yuvFormat.iYuvFormat.iAspectRatioNum      = 1;
    yuvFormat.iYuvFormat.iAspectRatioDenom    = 1;
    yuvFormat.iYuvFormat.iCoefficients  	  = EYuvBt709Range1;
    yuvFormat.iYuvFormat.iPattern       	  = EYuv422Chroma1;
    yuvFormat.iYuvFormat.iDataLayout          = EYuvDataInterleavedBE;
    
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));
    
    yuvFormat.iYuvFormat.iPattern       	  = EYuv422Chroma2;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));
    
    yuvFormat.iYuvFormat.iCoefficients  	  = EYuvBt709Range0;
    yuvFormat.iYuvFormat.iPattern       	  = EYuv422Chroma1;
    yuvFormat.iYuvFormat.iDataLayout    	  = EYuvDataInterleavedBE;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));
    
    yuvFormat.iYuvFormat.iPattern       	  = EYuv422Chroma2;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));
    
    yuvFormat.iYuvFormat.iCoefficients  	  = EYuvBt709Range0;
    yuvFormat.iYuvFormat.iPattern       	  = EYuv422Chroma1;
    yuvFormat.iYuvFormat.iDataLayout    	  = EYuvDataInterleavedLE;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));
            
	yuvFormat.iYuvFormat.iPattern       	  = EYuv422Chroma2;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));

    yuvFormat.iYuvFormat.iCoefficients  	  = EYuvBt709Range1;
    yuvFormat.iYuvFormat.iPattern       	  = EYuv422Chroma1;
    yuvFormat.iYuvFormat.iDataLayout    	  = EYuvDataInterleavedLE;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));
    
    yuvFormat.iYuvFormat.iPattern       	  = EYuv422Chroma2;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));
            
    yuvFormat.iYuvFormat.iCoefficients  	  = EYuvBt601Range0;
    yuvFormat.iYuvFormat.iPattern       	  = EYuv422Chroma1;
    yuvFormat.iYuvFormat.iDataLayout    	  = EYuvDataInterleavedBE;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));
    
    yuvFormat.iYuvFormat.iPattern       	  = EYuv422Chroma2;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));

    yuvFormat.iYuvFormat.iCoefficients  	  = EYuvBt601Range1;
    yuvFormat.iYuvFormat.iPattern       	  = EYuv422Chroma1;
    yuvFormat.iYuvFormat.iDataLayout    	  = EYuvDataInterleavedBE;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));
    
    yuvFormat.iYuvFormat.iPattern       	  = EYuv422Chroma2;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));
            
    yuvFormat.iYuvFormat.iCoefficients  	  = EYuvBt601Range0;
    yuvFormat.iYuvFormat.iPattern       	  = EYuv422Chroma1;
    yuvFormat.iYuvFormat.iDataLayout    	  = EYuvDataInterleavedLE;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));
    
    yuvFormat.iYuvFormat.iPattern       	  = EYuv422Chroma2;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));

    yuvFormat.iYuvFormat.iCoefficients  	  = EYuvBt601Range1;
    yuvFormat.iYuvFormat.iPattern       	  = EYuv422Chroma1;
    yuvFormat.iYuvFormat.iDataLayout    	  = EYuvDataInterleavedLE;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));
    
    yuvFormat.iYuvFormat.iPattern       	  = EYuv422Chroma2;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));
    
    //YUV 420 planar
    yuvFormat.iYuvFormat.iCoefficients  	  = EYuvBt709Range1;
    yuvFormat.iYuvFormat.iPattern       	  = EYuv420Chroma1;
    yuvFormat.iYuvFormat.iDataLayout    	  = EYuvDataPlanar;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));

    yuvFormat.iYuvFormat.iPattern       	  = EYuv420Chroma2;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));

    yuvFormat.iYuvFormat.iPattern       	  = EYuv420Chroma3;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));

    yuvFormat.iYuvFormat.iCoefficients  	  = EYuvBt709Range0;
    yuvFormat.iYuvFormat.iPattern       	  = EYuv420Chroma1;
    yuvFormat.iYuvFormat.iDataLayout    	  = EYuvDataPlanar;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));

    yuvFormat.iYuvFormat.iPattern       	  = EYuv420Chroma2;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));

    yuvFormat.iYuvFormat.iPattern       	  = EYuv420Chroma3;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));

    yuvFormat.iYuvFormat.iCoefficients  	  = EYuvBt601Range1;
    yuvFormat.iYuvFormat.iPattern       	  = EYuv420Chroma1;
    yuvFormat.iYuvFormat.iDataLayout    	  = EYuvDataPlanar;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));

    yuvFormat.iYuvFormat.iPattern       	  = EYuv420Chroma2;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));

    yuvFormat.iYuvFormat.iPattern       	  = EYuv420Chroma3;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));

    yuvFormat.iYuvFormat.iCoefficients  	  = EYuvBt601Range0;
    yuvFormat.iYuvFormat.iPattern       	  = EYuv420Chroma1;
    yuvFormat.iYuvFormat.iDataLayout    	  = EYuvDataPlanar;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));

    yuvFormat.iYuvFormat.iPattern       	  = EYuv420Chroma2;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));

    yuvFormat.iYuvFormat.iPattern       	  = EYuv420Chroma3;
    User::LeaveIfError(iSupportedInputFormats.Append(yuvFormat));
            
    CPostProcessorInfo* postProcessorInfo = CPostProcessorInfo::NewL( 
                KUidVideoPostProcHwDevice, 
                KManufacturer, 
                KIdentifier, 
                TVersion(1, 0, 0), 
                iSupportedInputFormats.Array(),
                SupportedPostProcess.Array(), 
                SupportedHwAcceleration,   
                ETrue,      //Direct Display
                SupportedYuvToRgbCapab, 
                SupportedRotations, 
                SupportedArbitraryScaling,
                SupportedScaleFactors.Array(), 
                SupportedAntiAliasing);
                
    CleanupStack::PushL(postProcessorInfo);            
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:PostProcessorInfoLC() --"), this);
    return postProcessorInfo;
}

void CNGAPostProcHwDevice::MmvprcGetPlayRateCapabilitiesL(TVideoPlayRateCapabilities& aCap)
{       
    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvprcGetPlayRateCapabilitiesL ++"), this);       
     aCap.iPlayForward = ETrue;       
     aCap.iPlayBackward = ETrue;       
     aCap.iStepForward = ETrue;       
     aCap.iStepBackward = ETrue;       
    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvprcGetPlayRateCapabilitiesL --"), this);       
}       

void CNGAPostProcHwDevice::MmvprcSetPlayRateL(const TInt aRate)
{       
    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvprcSetPlayRateL ++"), this);       
     iPlayRate = aRate;
     if (iPlayRate<0)        
     {       
         iKeyFrameMode = ETrue;    
     }       
     else        
     {       
         iKeyFrameMode = EFalse;   
         ResetCountingBuffer();       
     }       
     //In fast forward go direct to key frame mode if speed >4X =     
    if (iPlayRate>KDefPlayRate*4)
     {       
         if (iFPObserver)        
         {       
             iFPObserver->MmvproKeyFrameModeRequest();       
             iKeyFrameMode=ETrue;       
         }       
     }       
    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvprcSetPlayRateL=%d --"), this, aRate);       
}       

TInt CNGAPostProcHwDevice::MmvprcPlayRateL()
{       
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvprcPlayRateL= ++"), this);       
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvprcPlayRateL= --"), this);       
    return iPlayRate;       
}       

void CNGAPostProcHwDevice::MmvprcStepFrameL(const TInt aStep)
{       
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvprcStepFrameL= ++"), this);       
    iStepFrameCount = aStep;       
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvprcStepFrameL=%d --"), this, aStep);       
}       

void CNGAPostProcHwDevice::MmvprcSetObserver(MMmfVideoPlayRateObserver& aObserver)
{       
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvprcSetObserver ++"), this);       
    iFPObserver  = &aObserver;
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvprcSetObserver --"), this);
} 

void CNGAPostProcHwDevice::MmvsoSetSecureOutputL(TBool aSecure)
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvsoSetSecureOutputL aSecure = %d++"), this, aSecure);     
	TInt err = KErrNone;  
    if(aSecure)
	{
		iSurfaceMask = surfaceHints::EAllowInternalOnly;
	}
	else
	{
		iSurfaceMask = surfaceHints::EAllowAllExternals;
	}
	if(!iSurfaceId.IsNull())
	{
		err = AddHints();
		if(err != KErrNone)
		{
			PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvsoSetSecureOutputL -- leaving err = %d"), this, err);
			User::Leave(err);
		}
	}
    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvsoSetSecureOutputL --"), this);
}

void CNGAPostProcHwDevice::MmavsoSetAllowedOutputL(TUint aAllowedOutputMask)
{
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmavsoSetAllowedOutputL aAllowedOutputMask=0x%08x ++"), this,aAllowedOutputMask);  
   TInt err = KErrNone;
   iSurfaceMask = surfaceHints::EAllowInternalOnly;
    if (aAllowedOutputMask == EVideoAllowAll)
    {
        iSurfaceMask = surfaceHints::EAllowAllExternals;
    }
    else if (aAllowedOutputMask == EVideoAllowInternalOnly)
    {
        iSurfaceMask = surfaceHints::EAllowInternalOnly;
    }
    else 
    {
        // we hope to find some valid output prefs
        if (aAllowedOutputMask & EVideoAllowAnalog)
        {
            iSurfaceMask |= surfaceHints::EAllowAnalog;
        }
        if (aAllowedOutputMask & EVideoAllowMacroVision)
        {
            iSurfaceMask |= surfaceHints::EAllowAnalogProtectionRequired;
        }
        if (aAllowedOutputMask & EVideoAllowHDMI)
        {
            iSurfaceMask |= surfaceHints::EAllowDigital;
        }
        if (aAllowedOutputMask & EVideoAllowHdmiHdcpRequested)
        {
            iSurfaceMask |= surfaceHints::EAllowDigitalProtectionRequested;
        }
        if (aAllowedOutputMask & EVideoAllowHdmiHdcpRequired)
        {
            iSurfaceMask |= surfaceHints::EAllowDigitalProtectionRequired;
        }
    }
    
	if((!iSurfaceId.IsNull()))
	{
		err = AddHints();
		if(err != KErrNone)
		{
			PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmavsoSetAllowedOutputL -- leaving err = %d"), this, err);
			User::Leave(err);
		}
	}
    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmavsoSetAllowedOutputL --"), this);
}	

void CNGAPostProcHwDevice::SetPostProcessTypesL(TUint32 /*aCombination*/) 
{ 
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetPostProcessTypesL ++"), this);
	
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetPostProcessTypesL --"), this);
}

void CNGAPostProcHwDevice::SetInputCropOptionsL(const TRect& /*aRect*/) 
{ 
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetInputCropOptionsL ++"), this);
   
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetInputCropOptionsL --"), this);
}    

void CNGAPostProcHwDevice::SetYuvToRgbOptionsL( const TYuvToRgbOptions&  /*aOptions*/, const TYuvFormat& /*aYuvFormat*/, TRgbFormat /*aRgbFormat*/) 
{ 
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetYuvToRgbOptionsL ++"), this);

	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetYuvToRgbOptionsL --"), this);
}

void CNGAPostProcHwDevice::SetYuvToRgbOptionsL(const TYuvToRgbOptions& /*aOptions*/)
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetYuvToRgbOptionsL ++"), this);

	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetYuvToRgbOptionsL --"), this);
}

void CNGAPostProcHwDevice::SetRotateOptionsL(TRotationType ) 
{ 
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetRotateOptionsL ++"), this);
    
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetRotateOptionsL --"));
}

void CNGAPostProcHwDevice::SetScaleOptionsL(const TSize& /*aTargetSize*/, TBool /*aAntiAliasFiltering*/) 
{ 
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetScaleOptionsL ++"), this);
    
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetScaleOptionsL --"), this);
}

void CNGAPostProcHwDevice::SetOutputCropOptionsL(const TRect& /*aRect*/) 
{ 
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetOutputCropOptionsL ++"), this);
    
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetOutputCropOptionsL --"), this);
}

void CNGAPostProcHwDevice::SetPostProcSpecificOptionsL(const TDesC8& ) 
{ 
    //ignore 
}

void CNGAPostProcHwDevice::CommitL() 
{ 
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:CommitL ++"), this);

   	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:CommitL --"), this);
}

void CNGAPostProcHwDevice::Revert() 
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Revert ++"), this);
    
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Revert --"), this);
}

void CNGAPostProcHwDevice::StartDirectScreenAccessL( const TRect& /*aVideoRect*/, CFbsScreenDevice& /*aScreenDevice*/, const TRegion& /*aClipRegion*/) 
{ 
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:StartDSA ++"), this);
    
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:StartDSA --"), this);
}

void CNGAPostProcHwDevice::AbortDirectScreenAccess() 
{ 
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:AbortDSA ++"), this);

   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:AbortDSA --"), this);
}

void CNGAPostProcHwDevice::SetScreenClipRegion(const TRegion& /*aRegion*/) 
{ 
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetScreenClipRegion ++"), this);
    
    
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetScreenClipRegion --"), this);
}		    

void CNGAPostProcHwDevice::SetPauseOnClipFail(TBool ) 
{ 
    //ignore. Post Processor will always behave as aPause==False. 
}

TBool CNGAPostProcHwDevice::IsPlaying()
{
	if( iPPState == EPlaying)
	{
    	return ETrue; 
    }
    else
    {
    	return EFalse;
    }
}

void CNGAPostProcHwDevice::Redraw() 
{ 
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Redraw ++"), this);
	TInt err = KErrNone;
	if(iRedrawSurfaceInUse && !iRedrawDone)
	{
        err = AddHints();
        if (err != KErrNone)
        {
            PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Redraw -- failed to AddHints %d"), 
                         this, err);
            iProxy->MdvppFatalError(this, err);	
            return;   
        }
        PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Redraw registering the temp surface"), this);
		err = RegisterSurface(iSurfaceId);
		if (err != KErrNone)
		{
		   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Redraw -- failed to Register Surface %d"), 
		   				this, err);
		   	iSurfaceHandler->DestroySurface(iSurfaceId);
	   		iSurfaceId = TSurfaceId::CreateNullId();
			iProxy->MdvppFatalError(this, err);	
			return;   				
		}
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Redraw registering the temp surface done"), this);
        err = iSessionManager->PostPicture(iSurfaceId, 0, EFalse);
		if (err != KErrNone)
		{
			iProxy->MdvppFatalError(this, err);	
			return;
		}
        PublishSurfaceCreated();
        iRedrawDone = ETrue;
    }
    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Redraw --"), this);
}

void CNGAPostProcHwDevice::Start() 
{  
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Start ++"), this);
	iPPState = EPlaying;
	
	//TBC: when buffers given to post proc even before start. 
	//Even the buffers must be available to PostProc but not displayed. 
	//This will happen only when neighbouring decodeHwDevice decodes earlier than Start()
	//call. Need to check if MDF guidelines allow this.
	AttemptToPost();
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Start --"), this);
}

void CNGAPostProcHwDevice::Stop() 
{ 
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Stop ++"), this);
    iPPState = EStopped;
	if (iSessionManager)
	{
	    iSessionManager->CancelNotifiers();
	}
	if (iPostingTimer)
	{
        iPostingTimer->Cancel();
	}
	ReleaseProcessQ();
	ReleaseInputQ();

	//Stop must keep on displaying the last frame. Blank Screen must not be visible
	//to client. No Unregistering of surface should happen here. 
	//This Req is not necessary anymore. Only applicable to Pause.
	
	RDebug::Printf("------ Statistics of Post Processor ------");
    RDebug::Printf("    Pictures Received : %d", iPictureCounters.iTotalPictures);
    RDebug::Printf("    Pictures Displayed: %d", iPictureCounters.iPicturesDisplayed);
    RDebug::Printf("    Pictures Skipped  : %d", iPictureCounters.iPicturesSkipped);
    RDebug::Printf("------------------------------------------");
    
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Stop --"), this);
}

void CNGAPostProcHwDevice::Pause()
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Pause ++"), this);
	iPPState = EPaused;
    iPostingTimer->Cancel();
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Pause --"), this);
}

void CNGAPostProcHwDevice::Resume()
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Resume ++"), this);
	iPPState = EPlaying;
	AttemptToPost();
   	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Resume --"), this);
}

void CNGAPostProcHwDevice::SetPosition(const TTimeIntervalMicroSeconds& aPlaybackPosition) 
{
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetPosition ++"), this);
    
    if (iPPState == EInitializing)
    { 
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetPosition FAILED: Unexpected state"), this);
        return;
    }
    if (iPPState == EPaused)
    {	
        iFirstPictureUpdated = EFalse;
    }
    iCurrentPlaybackPosition = aPlaybackPosition;  
    
    ReleaseInputQ();

   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetPosition --"), this);
}

void CNGAPostProcHwDevice::FreezePicture(const TTimeIntervalMicroSeconds& ) 
{ 
    //TODO:
}

void CNGAPostProcHwDevice::ReleaseFreeze(const TTimeIntervalMicroSeconds&  ) 
{ 
    //TODO:
}

TTimeIntervalMicroSeconds 
CNGAPostProcHwDevice::PlaybackPosition() 
{ 
	if (iPPState == EInitializing)
    {
        return TTimeIntervalMicroSeconds(0);
    }
    
    return iCurrentPlaybackPosition; 
}

TUint CNGAPostProcHwDevice::PictureBufferBytes() 
{   //TODO 
    return 0; 
}

void CNGAPostProcHwDevice::GetPictureCounters( CMMFDevVideoPlay::TPictureCounters&  aCounters) 
{ 
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:GetPictureCounters ++"), this);
	
	if (iPPState == EInitializing)
		return;
	aCounters = iPictureCounters;
	
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:GetPictureCounters --"), this);
}

void CNGAPostProcHwDevice::SetComplexityLevel(TUint ) 
{ 
    //not required 
}

TUint CNGAPostProcHwDevice::NumComplexityLevels() 
{ 
    //not required 
    return 1; 
}

void CNGAPostProcHwDevice::GetComplexityLevelInfo(TUint , CMMFDevVideoPlay::TComplexityLevelInfo& ) 
{ 
    //not required 
}

void CNGAPostProcHwDevice::ReturnPicture(TVideoPicture* ) 
{ 
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:ReturnPicture +-"), this);
    //not required for direct rendering 
}

TBool CNGAPostProcHwDevice::GetSnapshotL(TPictureData& aPictureData, const TUncompressedVideoFormat& /*aFormat*/)
{ 
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:GetSnapshotL %d %d ++"), this, iVBMEnabled, iProcessQ.Count());
	TVideoPicture* 		pic = NULL;
	TInt 				err = KErrNone;
	TBool				frameAvailable =EFalse;
	tWndParam			inputCropWindow;
	tWndParam			outputCropWindow;
	tBaseVideoFrame		inputFrame;
	inputFrame.lum 		= NULL; 
	
	if(aPictureData.iDataFormat == ERgbFbsBitmap)
	{	
		if(iProcessQ.Count())
		{
			pic = iProcessQ[0]; //frame already submitted for display
		}
		else if(iInputQ.Count())
		{
			pic = iInputQ[0]; //frame yet to be displayed
		}
		if(pic) 
		{
			if (iVBMEnabled)
		    {
				inputFrame.lum	= (TUint8*)pic->iData.iRawData->Ptr();
			}
			else
			{
				if (iInputDecoderDevice)
				{
					MMmfVideoFetchFrame* VFHandler = NULL;
					VFHandler = (MMmfVideoFetchFrame*)iInputDecoderDevice->CustomInterface(KUidMMFVideoFetchFrame);
					if (VFHandler)
					{
						PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:GetSnapshotL() fetch frame"), this);
						inputFrame.lum = (TUint8*)VFHandler->MmvffGetFrame(GetID(pic));
					}
					else
					{
						PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:GetSnapshotL() decoder yet to implement MMmfVideoFetchFrame CI"), this);
					}
				}
			}
		}
		if(inputFrame.lum)
		{
			inputFrame.cb	= inputFrame.lum + iPicSize.iWidth * iPicSize.iHeight;
			
			if( ((iVideoFormat.iYuvFormat.iPattern == EYuv420Chroma1) ||
				(iVideoFormat.iYuvFormat.iPattern == EYuv420Chroma2) ||
	    		(iVideoFormat.iYuvFormat.iPattern == EYuv420Chroma3) ))						
			{
				inputFrame.cr = inputFrame.lum + (iPicSize.iWidth * iPicSize.iHeight*5)/4;
			}
			else
			{
				inputFrame.cr = inputFrame.lum + (iPicSize.iWidth * iPicSize.iHeight*3)/2;
			}
			
			inputFrame.width	= (unsigned short)iPicSize.iWidth;
			inputFrame.height	= (unsigned short)iPicSize.iHeight;
			
			outputCropWindow.wndHeight  = iPicSize.iHeight;	
			outputCropWindow.wndWidth	= iPicSize.iWidth; 	
			outputCropWindow.xOffset	= 0;
			outputCropWindow.yOffset	= 0;
			
			inputCropWindow.wndHeight  = iPicSize.iHeight;	
			inputCropWindow.wndWidth	= iPicSize.iWidth; 	
			inputCropWindow.xOffset	= 0;
			inputCropWindow.yOffset	= 0;
			
			RFbsSession fbs;
			User::LeaveIfError(fbs.Connect());
			CFbsBitmap* iOutBitmap = aPictureData.iRgbBitmap;
			TInt status = iOutBitmap->Resize(iPicSize);
			if (status == KErrNone)
			{
				// Lock the heap to prevent the FBS server from invalidating the address
		        iOutBitmap->LockHeap();
		        TUint8* dataAddress = (TUint8*)iOutBitmap->DataAddress();
				err = ColorConvert(&inputFrame, dataAddress, &inputCropWindow, &outputCropWindow);
				iOutBitmap->UnlockHeap();
				frameAvailable = ETrue;
			}
			fbs.Disconnect();
		}
	}
	else
	{
		err = KErrNotSupported;
	}
	if(err != KErrNone)
	{
		User::Leave(err);
	}
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:GetSnapshotL --"), this);
	return(frameAvailable);
}

void CNGAPostProcHwDevice::InputEnd() 
{ 
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:InputEnd ++"), this);
    
	if (iPPState!=EPlaying && iPPState!=EPaused)
    {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:InputEnd FAILED: Unexpected state"), this);
        return;
	}
    iIsInputEnded = ETrue;
    
    if( (iProcessQ.Count() <= 1) && (iInputQ.Count() == 0))
        {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:InputEnd() Stream end"), this);
		iProxy->MdvppStreamEnd();
	}
    
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:InputEnd --"), this);
}

TAny* CNGAPostProcHwDevice::CustomInterface(TUid aInterface)
{
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:CustomInterface UID = %d ++"), this, aInterface.iUid);
	
	if (aInterface == KUidMmfVideoBufferManagement)
    {
      return (MMmfVideoBufferManagement *)this;
    }
	if (aInterface == KUidMMFVideoSurfaceSupport)
	{
      return (MMMFVideoSurfaceSupport *)this;
    }
	if (aInterface == KUidMMFVideoSurfaceHandleControl)
	{
    	return (MMmfVideoSurfaceHandleControl *)this;
  }    
    if (aInterface == KUidMmfVideoPlayRateControl)
    {
      return (MMmfVideoPlayRateControl *)this;
    } 
    if (aInterface == KMmfVideoAdvancedSecureOutputUid)
    {
      return (MMmfAdvancedVideoSecureOutput *)this;
    }
    if (aInterface == KUidMmfVideoResourceManagement)
    {
      return (MMmfVideoResourceObserver *)this;
    } 
    if (aInterface == KUidMmfPostInitializeRequest)
    {
      return (MMmfPostInitializeRequest *)this;
    }
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:CustomInterface --"), this);
    return NULL;
}

void CNGAPostProcHwDevice::BufferAvailable(TInt aBufId, TInt aStatus)
{ 
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:CNGAPostProcHwDevice::BufferAvailable aStatus = %d aBufId = %d++"), this, aStatus, aBufId);
    TVideoPicture* pic = NULL;
    if((aStatus != KErrNone) && (aStatus != KErrOverflow) && (aStatus != KErrNotVisible))
	{
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:BufferAvailable FAILED: aStatus = %d"), this, aStatus);
		iProxy->MdvppFatalError(this, aStatus);
	}

	if(aStatus == KErrOverflow)
	{
		iOverflowPictureCounter++;
		PicturesSkipped();
	}
    	
    if (iVBMEnabled)
    {
		for(TInt i=0; i < iProcessQ.Count(); i++)
		{
			if(iVBMBufferReferenceQ[aBufId] == iProcessQ[i])
			{
				pic = iProcessQ[i];
				iProcessQ.Remove(i);				
				ReturnPicToDecoder(pic);
				if (iIsColorConversionNeeded)
				{
				    AddPictureToColorConversionQ(pic);
				}
				else
				{
				    AddPictureToVBMQ(pic);
				}
				break;
			}
		} 
	}
	else
	{
	    for(TInt i=0; i < iProcessQ.Count(); i++)
		{
			TInt bufId;
			if (iUsingExternalSurface)
			{
				bufId = GetExternalBufferID(iProcessQ[i]);
			}
			else
			{
				bufId = GetID(iProcessQ[i]);
			}
			
			if (aBufId == bufId)
			{
				pic = iProcessQ[i];
				iProcessQ.Remove(i);
				ReturnPicToDecoder(pic);
				break;
			}
		} 
	}
	
	if(aStatus == KErrNone)
	{
		if (!iKeyFrameMode && iPlayRate>KDefPlayRate)     
		{   
		 	if (iSkippedFramesCountingBuffer[iCurrentPosInFramesCountingBuffer]==1)        
		 	{       
		 		iSkippedFramesCountingBuffer[iCurrentPosInFramesCountingBuffer] = 0;       
		 		iSkippedFramesInLast64Frames--;       
		 	}       
		 	iCurrentPosInFramesCountingBuffer = ++iCurrentPosInFramesCountingBuffer%64;       
		} 
		iPictureCounters.iPicturesDisplayed++;
		if (iStepFrameCount != 0)
        {       
        	iStepFrameCount > 0 ? iStepFrameCount-- : iStepFrameCount++;		            	      
            if (iStepFrameCount==0 && iFPObserver)        
            {       
            	iFPObserver->MmvproStepFrameComplete(pic->iTimestamp);       
            }       
        }
	}
	
	if(iPPState == EPlaying)
	{
		AttemptToPost();
	}

	if( iIsInputEnded && (iProcessQ.Count() <= 1)  && (iInputQ.Count() == 0))
	{
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:BufferAvailable() Stream end"), this);
		iProxy->MdvppStreamEnd();
	}
	
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:CNGAPostProcHwDevice::BufferAvailable --"), this);
}

//=== MMmfVideoBufferManagement ===
void CNGAPostProcHwDevice::MmvbmSetObserver(MMmfVideoBufferManagementObserver* aObserver)
{
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmSetObserver() ++"), this);

    if (iPPState != EInitializing)
    {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmSetObserver FAILED: Unexpected state"), this);
        iProxy->MdvppFatalError(this, KErrNotReady);
	}

    iVBMObserver  = aObserver;

   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmSetObserver() --"), this);
}
    

void CNGAPostProcHwDevice::MmvbmEnable(TBool aEnable)
{
    if (iPPState != EInitializing)
    {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmEnable FAILED: Unexpected state"), this);
        iProxy->MdvppFatalError(this, KErrNotReady);
	}

    iVBMEnabled = aEnable;
}
    
void CNGAPostProcHwDevice::MmvbmSetBufferOptionsL(const TBufferOptions& aOptions)
{
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmSetBufferOptionsL ++"), this);

    if (iPPState != EInitializing)
    {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmSetBufferOptionsL FAILED: Unexpected state"), this);
        User::Leave(KErrNotReady);
	}

    // why limiting the number of buffers? any particular reason for this?
    //if (aOptions.iNumInputBuffers > KMaxVBMBuffers || aOptions.iNumInputBuffers <= 1)          //at least two buffers
    if (aOptions.iNumInputBuffers <= 1)          //at least two buffers
    {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmSetBufferOptionsL FAILED: Input buffer count limit"), this);
        User::Leave(KErrNotSupported);
	}

    if (aOptions.iNumInputBuffers == 0 
        || aOptions.iBufferSize.iWidth <= KMinVBMInputWidth 
        || aOptions.iBufferSize.iHeight <= KMinVBMInputHeight  
        || aOptions.iBufferSize.iWidth > KMaxVBMInputWidth 
        || aOptions.iBufferSize.iHeight > KMaxVBMInputHeight)
    {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmSetBufferOptionsL FAILED: Unexpected buffer size"), this);
        User::Leave(KErrArgument);
	}

    iVBMBufferOptions.iNumInputBuffers  = aOptions.iNumInputBuffers;
    iVBMBufferOptions.iBufferSize       = aOptions.iBufferSize;

   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmSetBufferOptionsL --"), this);
}

void CNGAPostProcHwDevice::MmvbmGetBufferOptions(TBufferOptions& aOptions)
{
    if (iPPState == EInitializing)
    {
    	aOptions = iVBMBufferOptions;
    }
    else
    {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmGetBufferOptions FAILED: Unexpected state"), this);
        iProxy->MdvppFatalError(this, KErrNotReady);
	}
}
    
TVideoPicture* CNGAPostProcHwDevice::MmvbmGetBufferL(const TSize& aSize)
    {
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmGetBufferL() ++"), this);
    
    TInt err = KErrNone;
    TVideoPicture* lPic = NULL;

    if (iPPState == EInitializing)
    {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmGetBufferL() FAILED: Unexpected state"), this);
        User::Leave(KErrNotReady);
	  }

    if (aSize.iWidth < KMinVBMInputWidth 
        || aSize.iHeight < KMinVBMInputHeight
        || aSize.iWidth > iVBMBufferOptions.iBufferSize.iWidth 
        || aSize.iHeight > iVBMBufferOptions.iBufferSize.iHeight)
  	{
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmGetBufferL() FAILED: Unexpected buffer size w=%d h=%d "), this,aSize.iWidth,aSize.iHeight );
        User::Leave(KErrNotSupported);
		}
		
		if(iVBMBufferReferenceQ.Count() == 0)
		{
			iPicSize = aSize;
			err = SetupSurface();
			if(err)
			{
					PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmGetBufferL() Surface Setup Failed %d"), this, err);
					User::Leave(err);
			}
		}
		
    if(!iVBMBufferQ.Count())
    {
       PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmGetBufferL() WARNING: Queue buffer count zero"), this);
        return NULL;
    }

    lPic    = iVBMBufferQ[0];
    iVBMBufferQ.Remove(0);

   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmGetBufferL() -- %d"), this, lPic);
    return lPic;
}
    
void CNGAPostProcHwDevice::MmvbmReleaseBuffer(TVideoPicture* aBuffer)
{
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmReleaseBuffer() ++"), this);

    if(!aBuffer)
    {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmReleaseBuffer() FAILED: Invalid buffer ptr"), this);
    	iProxy->MdvppFatalError(this, KErrArgument);
	}

    TInt err = iVBMBufferQ.Append(aBuffer);
    if (err)
    {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmReleaseBuffer() FAILED: Failed to append"), this);
		iProxy->MdvppFatalError(this, err);
	}

   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvbmReleaseBuffer() --"), this);
}

//=== MMMFVideoSurfaceSupport ===

void CNGAPostProcHwDevice::MmvssUseSurfaces()
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvssUseSurfaces() ++"), this);
	// do nothing
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvssUseSurfaces() --"), this);
}

TInt CNGAPostProcHwDevice::MmvshcCreateSurface(const RSurfaceManager::TSurfaceCreationAttributes& aAttributes, TInt aHandle, TSurfaceId& aSurfaceId)
    {
    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvshcCreateSurface() ++"), this);
    TInt err(KErrNone);
    
    if(!iSurfaceId.IsNull())
    {
        PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvssCreateSurface Cleaning Surface"), this);
        
		if (iVideoSurfaceObserver && iSurfaceCreatedEventPublished)
		{
			PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvshcCreateSurface - Telling client to remove old surface"), this);
			iVideoSurfaceObserver->MmvsoRemoveSurface();
			iSurfaceCreatedEventPublished = EFalse;
		}
		else
		{
			// We never told the client about the surface, so we must destroy it ourselves
			PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetupExternalSurface - Destroying old surface"), this);
			TInt numScreens = iWsSession.NumberOfScreens();
    		for(TInt i=0;i < numScreens;i++)
    		{
    			iWsSession.UnregisterSurface(i, iSurfaceId);
    		}
   			iWsSession.Flush();
			iSurfaceHandler->DestroySurface(iSurfaceId);
		}
		//remove any handle to chunk. not needed perhaps
		iChunk.Close();
		
    }
        
    // Create the surface handler if it doesn't exist.
    if (!iSurfaceHandler)
    {
        TRAP(err, iSurfaceHandler = CNGAPostProcSurfaceHandler::NewL());
        if (err != KErrNone)
        {
            PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvssCreateSurface -- failed to create SurfaceHandler."), this);
            return err;
        }
    }
         
    iChunk.SetHandle(aHandle);
    err = iSurfaceHandler->CreateSurface(aAttributes, aSurfaceId, iChunk);
    if (err != KErrNone)
    {
       PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvssCreateSurface -- failed to create surface %d"), this, err);
       return err;
    }
    iSurfaceId = aSurfaceId;
    iIsExternalChunk = ETrue;

    err = RegisterSurface(iSurfaceId);
    if (err != KErrNone)
    {
       PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvssCreateSurface -- failed RegisterSurface %d"), this, err);
       iSurfaceHandler->DestroySurface(iSurfaceId);
       iSurfaceId = TSurfaceId::CreateNullId();
       return err;
    }
    
    err = iSurfaceHandler->SurfaceInfo(iSurfaceId, iInfo);
    if (err != KErrNone)
    {
        PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvssCreateSurface -- failed to get Surface info %d"), this, err);
        return err;
    }

    if(iAttributes().iPixelFormat == EUidPixelFormatYUV_422Interleaved) 
        {
            iVideoFrameBufSize          = iInfo().iSize.iWidth * iInfo().iSize.iHeight * 2;
        }
        else
        {//EStUidPixelFormatYUV_420MB
        // EUidPixelFormatYUV_420Planar            
            iVideoFrameBufSize          =  iInfo().iSize.iWidth * iInfo().iSize.iHeight * 3/2;
        }
    
     
    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvshcCreateSurface err=%d"), this, err);
    return err;
    }

void CNGAPostProcHwDevice::MmvssSetObserver(MMMFVideoSurfaceObserver& aObserver)
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvssSetObserver() ++"), this);
	iVideoSurfaceObserver = &aObserver;
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvssSetObserver() --"), this);
}

void CNGAPostProcHwDevice::MmvssGetSurfaceParametersL(TSurfaceId& aSurfaceId, 
						TRect& aCropRect, TVideoAspectRatio& aPixelAspectRatio)
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvssGetSurfaceParametersL() ++"), this);

	iSurfaceHandler->SurfaceInfo(iSurfaceId, iInfo);
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvssGetSurfaceParametersL() \
		surfaceWidth = %d surfaceHeight = %d SurfaceId = 0x%x --"), 
		this, iInfo().iSize.iWidth, iInfo().iSize.iHeight, iSurfaceId);
	aSurfaceId = iSurfaceId;
		aCropRect = TRect(0, 0, iInfo().iSize.iWidth, iInfo().iSize.iHeight);
		if((iPicSize.iWidth > 0) && (iPicSize.iHeight > 0))
		{
			aCropRect.Intersection( iPicSize);
		}
	aPixelAspectRatio = TVideoAspectRatio(iAspectRatioNum,iAspectRatioDenom);
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvssGetSurfaceParametersL()  \
		cropRectWidth = %d cropRectHeight = %d"), this, aCropRect.Width(), aCropRect.Height());
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvssGetSurfaceParametersL()  \
		PAR Num = %d PAR Denom = %d"), this, aPixelAspectRatio.iNumerator, aPixelAspectRatio.iDenominator);
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvssGetSurfaceParametersL() --"), this);
}

void CNGAPostProcHwDevice::MmvssSurfaceRemovedL(const TSurfaceId& aSurfaceId)
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvssSurfaceRemovedL()++"), this);
	if(!aSurfaceId.IsNull())
	{
	    if(iSurfaceId == aSurfaceId)
        {//closing down top surface....current surface.
            if(iSessionManager)
            {
                iSessionManager->CancelNotifiers();
            }
	    }
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvssSurfaceRemovedL(): UnregisterSurface ID = 0x%x"), this, aSurfaceId );
		TInt numScreens = iWsSession.NumberOfScreens();
		for(TInt i=0;i < numScreens;i++)
		{
			iWsSession.UnregisterSurface(i, aSurfaceId);
		}
		iWsSession.Flush();
		iSurfaceHandler->DestroySurface(aSurfaceId);
		if(iSurfaceId == aSurfaceId)
		{
			iSurfaceCreatedEventPublished = EFalse;
			iSurfaceId = TSurfaceId::CreateNullId();
			 if(!iIsExternalChunk)
			 {
				iChunk.Close();
			 }
		}
	}

	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvssSurfaceRemovedL() --"), this);
}

// === MMmfVideoPropertiesObserver ===
    
void CNGAPostProcHwDevice::MmvpoUpdateVideoProperties(const TYuvFormat& aYuvFormat, const TSize& aPictureSize)
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvpoUpdateVideoProperties ++"), this);
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvpoUpdateVideoProperties PAR \
		iAspectRatioNum = %d, iAspectRatioDenom = %d"), this,
					 aYuvFormat.iAspectRatioNum,aYuvFormat.iAspectRatioDenom);
	iPicSize = aPictureSize;
	iAspectRatioNum = aYuvFormat.iAspectRatioNum;
	iAspectRatioDenom = aYuvFormat.iAspectRatioDenom;
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvpoUpdateVideoProperties Picture Size \
		iWidth = %d, iHeight = %d, iSurfaceCreatedEventPublished = %d"), 
		this, iPicSize.iWidth,iPicSize.iHeight, iSurfaceCreatedEventPublished?1:0);
			 
	if(iVPObserver)
	{
		iVPObserver->MmvpoUpdateVideoProperties(aYuvFormat, aPictureSize);
	} 
	if(iVideoSurfaceObserver && iSurfaceCreatedEventPublished)
	{
    	iVideoSurfaceObserver->MmvsoSurfaceParametersChanged(); 
    }
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvpoUpdateVideoProperties --"), this);
}

// === MMmfVideoResourceObserver ===

void CNGAPostProcHwDevice::MmvroResourcesLost(TUid )
{
    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvroResourcesLost ++"), this);
    if(!iResourceLost)
    {
		iResourceLost = ETrue;
		iRedrawDone = EFalse;
		Pause();
		ReleaseInputQ();
		iSessionManager->CancelNotifiers();
		ReleaseProcessQ();
		if(iVideoSurfaceObserver && iSurfaceCreatedEventPublished)
		{
			PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvroResourcesLost - Telling client to remove surface"), this);
			iVideoSurfaceObserver->MmvsoRemoveSurface();
			iSurfaceCreatedEventPublished = EFalse;
		}
	}
	else if(iResourceLost && iRedrawDone)
	{
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvroResourcesLost ResourceLost happening \
					while Postprocessor is already in ResourceLoss state"), 
	   				this);
		iProxy->MdvppFatalError(this, KErrHardwareNotAvailable);	   				
	    return;		
	}
	else
	{
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvroResourcesLost Ignoring the \
					duplicate ResourceLoss call"), 
	   				this);
	}
    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvroResourcesLost --"), this);
}

// === MMmfVideoPropertiesNotifier ===
    
void CNGAPostProcHwDevice::MmvpnSetObserver(MMmfVideoPropertiesObserver* aObserver)
{
	PP_DEBUG(_L("ppHwDev[%x]::MmvpnSetObserver ++"), this);
	iVPObserver = aObserver;
	PP_DEBUG(_L("ppHwDev[%x]::MmvpnSetObserver --"), this);
}

void CNGAPostProcHwDevice::MmvroResourcesRestored(TUid )
{
   	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvroResourcesRestored ++"), this);
	iFirstPictureUpdated = EFalse;
	iResourceLost = EFalse;
   	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvroResourcesRestored state=%d --"), 
   			this, iPPState);
}

void CNGAPostProcHwDevice::MmvshcSetSurfaceHandle(const TSurfaceId &aSurfaceID)
{
    
    SetupExternalSurface(aSurfaceID);
    
}

void CNGAPostProcHwDevice::MmvshcRedrawBufferToSurface(TPtrC8& aRedrawBuffer)
{
    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvshcRedrawBufferToSurface ++"), this);
	
    TUint8*         lPtr;
    TInt 			offset;

    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvshcRedrawBufferToSurface -- Creating %d x %d surface"), this, iPicSize.iWidth, iPicSize.iHeight);

   	TInt err = KErrNone;
	SetSurfaceAttributes(iPicSize, 1); 
	
  	err = iSurfaceHandler->CreateSurface(iAttributes, iSurfaceId);
  	if (err != KErrNone)
	{
	   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvshcRedrawBufferToSurface -- failed to create Surface %d"), 
	   				this, err);
		iProxy->MdvppFatalError(this, err);	   				
	    return;
	}

	err = iSurfaceHandler->SurfaceInfo(iSurfaceId, iInfo);
	if (err != KErrNone)
	{
	   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvshcRedrawBufferToSurface -- failed to get Surface info %d"), 
	   				this, err);
	   	iSurfaceHandler->DestroySurface(iSurfaceId);
	   	iSurfaceId = TSurfaceId::CreateNullId();
		iProxy->MdvppFatalError(this, err);	   				
	    return;
	}

	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvshcRedrawBufferToSurface() \
		surfaceWidth = %d surfaceHeight = %d surfaceStride = %d"), this, iInfo().iSize.iWidth, iInfo().iSize.iHeight, iInfo().iStride);

	TInt redrawBufferSize = aRedrawBuffer.Size();
	TInt surfaceSize = iInfo().iStride * iInfo().iSize.iHeight;

    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvshcRedrawBufferToSurface RedrawBuffer size= %d Surface size = %d"), this, redrawBufferSize, surfaceSize);

	// Check whether redraw buffer will fit onto the surface.
	// If this check fails then we won't raise a fatal error - We just won't create the redraw surface
	if (redrawBufferSize > surfaceSize)
	{
    	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvshcRedrawBufferToSurface Redraw buffer size larger than surface size"), this);
    	iSurfaceHandler->DestroySurface(iSurfaceId);
	   	iSurfaceId = TSurfaceId::CreateNullId();
    	return;
	}

	err = iSurfaceHandler->MapSurface(iSurfaceId, iChunk);
	if (err != KErrNone)
	{
	   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:MmvshcRedrawBufferToSurface -- failed to map Surface %d"), 
	   				this, err);
	   	iSurfaceHandler->DestroySurface(iSurfaceId);
	   	iSurfaceId = TSurfaceId::CreateNullId();
		iProxy->MdvppFatalError(this, err);	   				
	    return;
	}
	iIsExternalChunk = EFalse;
    if((err = iSurfaceHandler->GetBufferOffset(iSurfaceId, 0, offset)) != KErrNone)
    {
    	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvshcRedrawBufferToSurface offset query failed %d"), this, err);
    	iSurfaceHandler->DestroySurface(iSurfaceId);
	   	iSurfaceId = TSurfaceId::CreateNullId();
		iChunk.Close();
    	iProxy->MdvppFatalError(this, err);
    	return;
    }

    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvshcRedrawBufferToSurface offset = %d"), this, offset);

	lPtr = reinterpret_cast<TUint8*>(iChunk.Base() + offset);
	memcpy((TAny *)lPtr, (TAny *)aRedrawBuffer.Ptr(), redrawBufferSize);

	iRedrawSurfaceInUse = ETrue;

	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvshcRedrawBufferToSurface(): New surface = 0x%x"), this, iSurfaceId);

    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::MmvshcRedrawBufferToSurface error = %d --"), this, err);
}

TInt CNGAPostProcHwDevice::SetupExternalSurface(const TSurfaceId &aSurfaceID)
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetupExternalSurface(): aSurfaceID = 0x%x"), this, aSurfaceID );

    TInt err = KErrNone;
    
    if(!iSurfaceId.IsNull())
    {
		if (iVideoSurfaceObserver && iSurfaceCreatedEventPublished)
		{
			PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetupExternalSurface - Telling client to remove old surface"), this);
			iVideoSurfaceObserver->MmvsoRemoveSurface();
			iSurfaceCreatedEventPublished = EFalse;
		}
		else
		{
			// We never told the client about the surface, so we must destroy it ourselves
			PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetupExternalSurface - Destroying old surface"), this);
			TInt numScreens = iWsSession.NumberOfScreens();
    		for(TInt i=0;i < numScreens;i++)
    		{
    			iWsSession.UnregisterSurface(i, iSurfaceId);
    		}
   			iWsSession.Flush();
			iSurfaceHandler->DestroySurface(iSurfaceId);
		}

		iChunk.Close();
	}
    
    iSurfaceId            = aSurfaceID;
    iUsingExternalSurface = ETrue;
    iRedrawSurfaceInUse = EFalse;

    // Create the surface handler if it doesn't exist.
    if (!iSurfaceHandler)
    {
        TRAP(err, iSurfaceHandler = CNGAPostProcSurfaceHandler::NewL());
        if (err != KErrNone)
        {
           PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetupExternalSurface -- failed to create SurfaceHandler."), this);
            return err;
        }
    }
    
    err = iSurfaceHandler->OpenSurface(iSurfaceId);
    if (err != KErrNone)
	{
	   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetupExternalSurface -- failed OpenSurface %d"), 
	   				this, err);
	    return err;
	}
    err = AddHints();
    if (err != KErrNone)
    {
        PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetupExternalSurface -- failed to AddHints %d"), 
                    this, err);
        return err;
    }
	err = RegisterSurface(iSurfaceId);
	if (err != KErrNone)
	{
	   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetupExternalSurface -- failed RegisterSurface %d"), 
	   				this, err);
	   	iSurfaceHandler->DestroySurface(iSurfaceId);
	   	iSurfaceId = TSurfaceId::CreateNullId();
	    return err;
	}

    err = iSurfaceHandler->SurfaceInfo(iSurfaceId, iInfo);
    if (err != KErrNone)
    {
        PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetupExternalSurface -- failed to get Surface info %d"), 
                     this, err);
        return err;
    }
 
  PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetupExternalSurface err=%d"), this, err);
   return err;
}

//=== Internal ===
TVideoPicture* CNGAPostProcHwDevice::CreateBuffersL(TInt aBufId)
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::CreateBuffersL ++"), this);
	
	TVideoPicture*          lVideoPicture = NULL;
    TUint8*                 lPtr;
    TPtr8*                  lTemp;
    TInt 					offset;
    
	lVideoPicture = new (ELeave) TVideoPicture;
    CleanupStack::PushL(lVideoPicture);
    if(TInt err = iSurfaceHandler->GetBufferOffset(iSurfaceId, aBufId, offset) != KErrNone)
    {
    	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::CreateBuffersL offset query failed %d"), this, err);
    }
    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::CreateBuffersL offset = %d id =%d --"), this, offset, aBufId);
    
	lPtr = reinterpret_cast<TUint8*>(iChunk.Base() + offset);

    lTemp = new (ELeave) TPtr8(lPtr, 0, (iVideoFrameBufSize ));
    CleanupStack::PushL(lTemp);

    lVideoPicture->iData.iRawData   = lTemp;
    lVideoPicture->iHeader = NULL ;
    lVideoPicture->iLayerBitRates = NULL ;
    
    CleanupStack::Pop(2);
    
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::CreateVBMBuffersL --"), this);
    return lVideoPicture;
}

void CNGAPostProcHwDevice::CreateVBMBuffersL()
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::CreateVBMBuffersL ++"), this);
    
    TInt err = KErrNone;
    TVideoPicture* pic = NULL;
    iVBMBufferReferenceQ.Reset();
    iVBMBufferQ.Reset();
    iColorConversionQ.Reset();

    for(TInt i = 0; i < iVBMBufferOptions.iNumInputBuffers; i++)
    {
        TRAP(err, pic = CreateBuffersL(i));
	    	if (err != KErrNone)
	    	{
	    	   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Initialize -- failed to create VBM buffer %d"), this, err);
	    	    User::Leave(err);
	    	}

        // This will hold the references which will be used in destructor
        User::LeaveIfError(iVBMBufferReferenceQ.Append(pic));
        User::LeaveIfError(iVBMBufferQ.Append(pic));
    }
    if(iIsColorConversionNeeded)
    {
		    for(TInt i = iVBMBufferOptions.iNumInputBuffers ; 
		    				 i < (iVBMBufferOptions.iNumInputBuffers + KColorConversionBuffers ); i++)
		    {
		        TRAP(err, pic = CreateBuffersL(i));
			    	if (err != KErrNone)
			    	{
			    	   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Initialize -- failed to create VBM buffer %d"), this, err);
			    	    User::Leave(err);
			    	}
		
		        // This will hold the references which will be used in destructor
		        User::LeaveIfError(iVBMBufferReferenceQ.Append(pic));
		        User::LeaveIfError(iColorConversionQ.Append(pic));
		    }
		}
	    
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]::CreateVBMBuffersL --"), this);
}

void CNGAPostProcHwDevice::ReturnPicToDecoder(TVideoPicture* aPic)
{
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:ReturnPicToDecoder ++"), this);
	if (aPic == NULL)
	{
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:ReleaseInputPicture FAILED: Invalid pic ptr."), this);
		return;
	}
	
   	if (iInputDecoderDevice)
    {
       PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:ReleaseInputPicture .. before return picture. 2"), this);
        iInputDecoderDevice->ReturnPicture(aPic);
    }
	
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:ReturnPicToDecoder --"), this);
}

TInt CNGAPostProcHwDevice::AttemptToPost()
{
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:AttemptToPost ++ Q:%d"), this, iInputQ.Count());
   if (iPPState == EPaused)
   {
        return KErrNone;
   }

    TInt err = KErrNotReady;
    TInt count = iInputQ.Count();
    TBool bDone = EFalse;
    TVideoPicture* pic = PeekQ();   		
  	while(pic && !bDone)
  	{
	    if(!IsGceReady())
		{  
			PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:AttemptToPost GCE not ready"), this );
			return err; //no need to catch this error
		}
		TInt64 delta = 0;
		TTimeToPost	timeToPost = (TTimeToPost)IsTimeToPost(pic, delta);
		switch(timeToPost)
		{
			case EDelayIt:
			{
				iPostingTimer->Cancel();
				SetTimer(delta);
				bDone = ETrue;
			}
			break;
			case EPostIt:
			{
				RemoveFromQ(); // remove the pic that was returned by PeekQ				
				if(iIsColorConversionNeeded)
				{
				    TVideoPicture* ccPic;
    				ccPic = DoColorConvert(pic); // output will be in ccPic
    				pic = ccPic;
			    }
				
                #ifdef _DUMP_YUV_FRAMES
                    captureYuv(pic);
                #endif
				TInt err = iSessionManager->PostPicture(iSurfaceId, GetID(pic), ETrue); 
				if(err == KErrNone)
				{
				    iProcessQ.Append(pic);
					iCurrentPlaybackPosition = pic->iTimestamp;
					if(!iFirstPictureUpdated)
					{
						iFirstPictureUpdated = ETrue;
						if(!iSurfaceCreatedEventPublished)
                    	{
                        	PublishSurfaceCreated();
                    	}
					}
				}
				else
				{
					ReleasePicture(pic);
				}
										 					
				
			}	// end of postit
			break;
			case ESkipIt: 
			{
				RemoveFromQ();
				ReleasePicture(pic);
				PicturesSkipped();				
			}
			break;
		} // end of switch
		
		// get the next picture
		pic = PeekQ();	
    } // end of while
    
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:AttemptToPost --"), this);
 	return err;
}

TInt CNGAPostProcHwDevice::IsTimeToPost(TVideoPicture* frame, TInt64& delta)
{
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:IsTimeToPost ++"), this);

    if (!frame)
    {
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:IsTimeToPost FAILED: Invalid frame ptr."), this);
		return KErrGeneral;
	}

    TInt resp = EPostIt;
    // Frame presentation time
    TInt64 uPresTime = frame->iTimestamp.Int64();
      
    // Check if this is an out of order frame in case of forward playback
    if((iCurrentPlaybackPosition.Int64() > uPresTime) && (iPlayRate > 0))    
    {      
         PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:IsTimeToPost : Out of order frame (forward playback) Tfm=%d"), this,(TInt)uPresTime);
         resp = ESkipIt;  //drop      
    }      // Check if this is an out of order frame in case of backward playback
    else if((iCurrentPlaybackPosition.Int64() < uPresTime) && (iPlayRate < 0))    
    {      
        PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:IsTimeToPost : Out of order frame (backward playback) Tfm=%d"), this,(TInt)uPresTime);
        resp = ESkipIt;  //drop      
    }
    else if (iClockSource)
    {
        // The time to sync with.
        TInt64 uSyncTime = iClockSource->Time().Int64();
        
        delta = uPresTime - uSyncTime;
        if (( delta > KRenderAhead ) &&  (iPlayRate > 0))	// Delay condition not checked for 
        {													// backward playback
        	resp = EDelayIt;  //wait
        }
        else if ( (delta < (-KMaxRenderDelay) && (iPlayRate > 0))
          		||  ((delta > KMaxRenderDelay) && (iPlayRate < 0)))
        {
            resp = ESkipIt;  //drop
        }
       PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:IsTimeToPost .. Tfm=%d, Tcs=%d, delta=%d"), this, (TInt)uPresTime, (TInt)uSyncTime, (TInt)delta);
    }       
   
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:IsTimeToPost -- %d BufID = %d"), this, resp, GetID(frame));
    return resp;
}

void CNGAPostProcHwDevice::ReleaseInputQ()
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:ReleaseInputQ ++ Q = %d"), this, iInputQ.Count());
    while (iInputQ.Count()>0)
    {
    	ReleasePicture(iInputQ[0]);
        iInputQ.Remove(0);
    }
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:ReleaseInputQ --"), this);
}

void CNGAPostProcHwDevice::ReleaseProcessQ()
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:ReleaseProcessQ ++ Q = %d"), this, iProcessQ.Count() );
	TVideoPicture* pic = NULL;
	
    while (iProcessQ.Count()>0)
    {
		pic = iProcessQ[0];
		iProcessQ.Remove(0);
		ReturnPicToDecoder(pic);
    }
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:ReleaseProcessQ --"), this);
}

void CNGAPostProcHwDevice::ReleasePicture(TVideoPicture *pic)
{
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:ReleasePicture ++"), this);
	if (pic == NULL)
	{
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:ReleaseInputPicture FAILED: Invalid pic ptr."), this);
		return;
	}
	
   	if (iInputDecoderDevice)
    {
       PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:ReleaseInputPicture .. before return picture. 2"), this);
        iInputDecoderDevice->ReturnPicture(pic);
    }
	if (iVBMEnabled)
    {
        iVBMBufferQ.Append(pic);

        if ( !iIsInputEnded && iPPState != EStopped )
        {
            iVBMObserver->MmvbmoNewBuffers();
        }
	}
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:ReleasePicture --"), this);
}

void CNGAPostProcHwDevice::PublishSurfaceCreated()
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:PublishSurfaceCreated ++"), this);   
	if(iVideoSurfaceObserver)
	{
		iVideoSurfaceObserver->MmvsoSurfaceCreated();
		iSurfaceCreatedEventPublished = ETrue;
	}
    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:PublishSurfaceCreated --"), this);
}

TInt CNGAPostProcHwDevice::SetupSurface()
{
	TInt err = KErrNone;
	if(iVBMEnabled && iVBMObserver)
    {
    	SetSurfaceAttributes(iVBMBufferOptions.iBufferSize, iVBMBufferOptions.iNumInputBuffers);
    	
	  	err = iSurfaceHandler->CreateSurface(iAttributes, iSurfaceId);
	  	if (err != KErrNone)
    	{
    	   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Initialize -- failed to create Surface %d"), this, err);
    	    return err;
    	}
    	err = iSurfaceHandler->MapSurface(iSurfaceId, iChunk);
    	if (err != KErrNone)
    	{
    	   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Initialize -- failed to map Surface %d"), this, err);
    	  	iSurfaceHandler->DestroySurface(iSurfaceId);
	   		iSurfaceId = TSurfaceId::CreateNullId();
    	    return err;
    	}
    	err = iSurfaceHandler->SurfaceInfo(iSurfaceId, iInfo);
    	if (err != KErrNone)
    	{
    	   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Initialize -- failed to get Surface info %d"), this, err);
    	   	iSurfaceHandler->DestroySurface(iSurfaceId);
	   		iSurfaceId = TSurfaceId::CreateNullId();
    	    return err;
    	}
    	TRAP(err, CreateVBMBuffersL());
    	if (err != KErrNone)
    	{
    	   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Initialize -- failed to create VBM buffer %d"), this, err);
    	   	iSurfaceHandler->DestroySurface(iSurfaceId);
	   		iSurfaceId = TSurfaceId::CreateNullId();
    	    return err;
    	}
        err = AddHints();
        if (err != KErrNone)
        {
            PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Initialize -- failed to AddHints %d"), this, err);
            return err;
        }
        err = RegisterSurface(iSurfaceId);
        if (err != KErrNone)
        {
            PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Initialize -- failed to RegisterSurface %d"), this, err);
            iSurfaceHandler->DestroySurface(iSurfaceId);
            iSurfaceId = TSurfaceId::CreateNullId();
            return err;
        }
        
    }
    return err;
} 

void CNGAPostProcHwDevice::SetSurfaceAttributes(const TSize& aSize, TInt aNumBuf)
{
	iAttributes().iSize                   = aSize; // size of the video frame
    iAttributes().iBuffers            	  = aNumBuf;
    
    /* The stride needs to be calculated for the surface manager to know
    how much memory to allocate */
    
    if(iAttributes().iPixelFormat == EUidPixelFormatYUV_420Planar)
    	{
    		iAttributes().iStride       = aSize.iWidth * 3/2;
    		iVideoFrameBufSize			= aSize.iWidth * aSize.iHeight * 3/2;
    	}
    	else
    	{
    		iAttributes().iStride       = aSize.iWidth * 2;
    		iVideoFrameBufSize			= aSize.iWidth * aSize.iHeight * 2;
    	}
    		
    if(iIsColorConversionNeeded)
    {
    	iAttributes().iBuffers            = aNumBuf + KColorConversionBuffers;
    }
    else
    {
    	iAttributes().iBuffers            = aNumBuf;	
    }
    
    iAttributes().iOffsetToFirstBuffer    = 0;
#if defined __WINSCW__ 
    iAttributes().iAlignment              = 4;
#else //on hw, its always better to have page aligned chunks
    iAttributes().iAlignment              = -1;
#endif
    iAttributes().iContiguous             = ETrue;
    iAttributes().iHintCount              = 0;
    iAttributes().iMappable               = ETrue;
}

TInt CNGAPostProcHwDevice::GetID(TVideoPicture *aPicture)
{
    if (iUsingExternalSurface)
    {
	    return GetExternalBufferID(aPicture);
    }
    else
    {
        TUint8* aPtr = (TUint8*) aPicture->iData.iRawData->Ptr();
        return( (TInt) ((aPtr - iChunk.Base() - iAttributes().iOffsetToFirstBuffer) / 
			(iVideoFrameBufSize )));
    }
}

TInt CNGAPostProcHwDevice::GetExternalBufferID(TVideoPicture *aPicture)
{
    // currently type cast the pointer as buffer ID.
    // FIXME once the new data structure is available.
    return( (TInt) aPicture->iData.iRawData->Ptr());
}

TInt CNGAPostProcHwDevice::RegisterSurface(const TSurfaceId& aSurfaceId)
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:RegisterSurface(): RegisterSurface ID = 0x%x"), this, aSurfaceId);
	TInt err = KErrNone;
	TInt numScreens = iWsSession.NumberOfScreens();
	for(TInt i=0; (i < numScreens && err == KErrNone); i++)
	{
		err = iWsSession.RegisterSurface(i, aSurfaceId);
	}	
	return(err);
}

TInt CNGAPostProcHwDevice::IsGceReady()
{
    if(iProcessQ.Count() >= KMaxBuffersGceCanHold)
    {
    		return EFalse;
    }
    return ETrue;
}

void CNGAPostProcHwDevice::SetTimer(TInt64 aDelta)
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetTimer .. aDelta=%d"), this, (TInt)aDelta);
	if(aDelta <= KRenderAhead)
	{
		if(aDelta < 0)
		{
			iPostingTimer->After(aDelta * -1);
		}
		else
		{
			iPostingTimer->After((aDelta - KRenderAhead) * -1);
		}
	}
	else
	{
		iPostingTimer->After(aDelta - KRenderAhead - KPostingOfset);
	}
}
//
// Convert YUV420 to YUV422InterLeaved.
//
TInt CNGAPostProcHwDevice::ConvertPostProcBuffer(TVideoPicture* pSrc, TVideoPicture* pDest)
{
    PP_DEBUG(_L("CMdfPostingSurfaceProxy::ConvertPostProcBuffer ++"));
    TInt err = KErrNone;
    if (!pDest && !pSrc)
	{
		PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:ConvertPostProcBuffer FAILED: Invalid pic pSrc %x pDest %x."), this, pSrc, pDest);
		return KErrArgument;
	}
    
    // --- Prepare wrappers ---
    tBaseVideoFrame tFrame420, tFrame422;
    TInt    frameSize = pSrc->iData.iDataSize.iWidth * pSrc->iData.iDataSize.iHeight;

   PP_DEBUG(_L("CMdfPostingSurfaceProxy::ConvertPostProcBuffer .. w=%d, h=%d"), pSrc->iData.iDataSize.iWidth, pSrc->iData.iDataSize.iHeight);

    tFrame420.width = pSrc->iData.iDataSize.iWidth;
    tFrame420.height= pSrc->iData.iDataSize.iHeight;
    tFrame420.lum   = (TUint8*)pSrc->iData.iRawData->Ptr();
    tFrame420.cb    = (TUint8*)tFrame420.lum + frameSize;
    tFrame420.cr    = (TUint8*)tFrame420.lum + (frameSize*5)/4;
    
    tFrame422.width = pSrc->iData.iDataSize.iWidth;
    tFrame422.height= pSrc->iData.iDataSize.iHeight;
    tFrame422.lum   = (TUint8*)pDest->iData.iRawData->Ptr();
    tFrame422.cb    = 0;
    tFrame422.cr    = 0;
    
    // --- Convertion to posting buffer ---
    TInt stride     = pSrc->iData.iDataSize.iWidth * 2;
    EBufferLayout422 layout = YUV422INT_BE;
        
    err = gColorConvYUVtoYUV422Int(&tFrame420, &tFrame422, layout, stride);
    if(err != KErrNone)
    {
    		PP_DEBUG(_L("CNGAPostProcHwDevice::ConvertPostProcBuffer .. err= %d."), err);
    }
	return err;
}   

void CNGAPostProcHwDevice::AddPictureToVBMQ(TVideoPicture *pic)
{
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:AddPictureToVBMQ ++"), this);
    iVBMBufferQ.Append(pic);

    if ( !iIsInputEnded && iPPState != EStopped )
    {
        iVBMObserver->MmvbmoNewBuffers();
    }
	
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:AddPictureToVBMQ --"), this);
}

void CNGAPostProcHwDevice::AddPictureToColorConversionQ(TVideoPicture *pic)
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:AddPictureToColorConversionQ ++"), this);
    iColorConversionQ.Append(pic);
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:AddPictureToColorConversionQ --"), this);
}

#ifdef _DUMP_YUV_FRAMES
void CNGAPostProcHwDevice::captureYuv(TVideoPicture* aPicture)
{
	char buf[128];              
	sprintf(buf, "c:\\fb%d.yuv", count++);          
	FILE *fp = ::fopen(buf, "w");
	TInt size = aPicture->iData.iRawData->Size();
	//{FILE* f1 = fopen(MY_LOG_FILE_NAME, "a+"));if(f1){fprintf(f1, "Size  %d \n"), size );fclose(f1); }}

	::fwrite(aPicture->iData.iRawData->Ptr(), 1, size, fp);
	::fclose(fp);    	
}
#endif

void CNGAPostProcHwDevice::ResetCountingBuffer()
{       
	memset(iSkippedFramesCountingBuffer,0,sizeof(iSkippedFramesCountingBuffer));
    iSkippedFramesInLast64Frames = 0;       
    iCurrentPosInFramesCountingBuffer = 0;       
} 

void CNGAPostProcHwDevice::PicturesSkipped()
{       
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:PicturesSkipped ++"), this);
	iPictureCounters.iPicturesSkipped++;
	if (!iKeyFrameMode && iPlayRate>KDefPlayRate)
    {       
    	if (iSkippedFramesCountingBuffer[iCurrentPosInFramesCountingBuffer]==0)        
        {       
        	iSkippedFramesCountingBuffer[iCurrentPosInFramesCountingBuffer] = 1;       
            iSkippedFramesInLast64Frames++;       
            if (iSkippedFramesInLast64Frames>KMaxAllowedSkipInNFrames && iFPObserver )       
            {       
            	iFPObserver->MmvproKeyFrameModeRequest();       
                iKeyFrameMode=ETrue;       
                ResetCountingBuffer();       
            }       
        }       
        iCurrentPosInFramesCountingBuffer = ++iCurrentPosInFramesCountingBuffer%64;       
    }
    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:PicturesSkipped --"), this);   
}

TVideoPicture* CNGAPostProcHwDevice::DoColorConvert(TVideoPicture* aPicture)
{
    TVideoPicture *pOutPicture  = aPicture;
    					    		
	if(iColorConversionQ.Count())
    {
	    pOutPicture    = iColorConversionQ[0];
	    iColorConversionQ.Remove(0);
	    ConvertPostProcBuffer(aPicture, pOutPicture);
	   	pOutPicture->iTimestamp = aPicture->iTimestamp;
	    ReleasePicture(aPicture);    	    
    }				    
    else
    {
       PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:WritePictureL() FAILED: color conversion"), this);
    }

	return pOutPicture;
}

TInt CNGAPostProcHwDevice::AddToQ(TVideoPicture* aPicture)
{
	TVideoPicture* pic = aPicture;
	TInt pos = -1;
	if(iInputQ.Count() == 0)
	{
		iInputQ.Append(pic);
	}
	else
	{
		pos = iInputQ.Count()-1;
		for(; pos >= 0; pos--)
		{
			if(pic->iTimestamp.Int64() > iInputQ[pos]->iTimestamp.Int64())
			{
				break;
			}
		} 
		if(iInputQ.Count() == pos+1)
		{
			iInputQ.Append(pic);
		}
		else
		{
			iInputQ.Insert(pic, pos+1);
		}
	}
	return pos+1;
}

void CNGAPostProcHwDevice::RemoveFromQ()
{
	if(iInputQ.Count())
	{
		if(iPlayRate > 0)
		{
			iInputQ.Remove(0);
		}
		else
		{
			iInputQ.Remove(iInputQ.Count()-1);
		}
	}
}

TVideoPicture* CNGAPostProcHwDevice::PeekQ()
{	
	TVideoPicture *pic = NULL;
	if(iInputQ.Count())
	{
		if(iPlayRate > 0)
		{			
			pic = iInputQ[0];
		}
		else
		{			
			pic = iInputQ[iInputQ.Count()-1];
		}	
	}
	return pic;
}

TInt CNGAPostProcHwDevice::AddHints()
{
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:AddHints iSurfaceMask 0x%08x ++"), this, iSurfaceMask);
   TInt err = KErrNone;
   iHint.Set(iSurfaceKey,iSurfaceMask,ETrue);
   err = iSurfaceHandler->AddSurfaceHint(iSurfaceId,iHint);
   if(err == KErrAlreadyExists)
   {
		err = KErrNone;
		err = iSurfaceHandler->SetSurfaceHint(iSurfaceId,iHint);
   }
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:AddHints. err = %d --"), this,err);
   iHint.iKey.iUid = surfaceHints::KSurfaceContent;
   iHint.iValue = surfaceHints::EVideoPlayback;
   iHint.iMutable = ETrue;
   err = iSurfaceHandler->AddSurfaceHint(iSurfaceId,iHint);
   if(err == KErrAlreadyExists)
   {
		err = KErrNone;
		err = iSurfaceHandler->SetSurfaceHint(iSurfaceId,iHint);
   }
   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:AddHints. err = %d --"), this,err);
   return err;
}

TInt CNGAPostProcHwDevice::ColorConvert(tBaseVideoFrame* aInputFrame, TUint8* aDestPtr, tWndParam* aInputCropWindow, tWndParam* aOutputCropWindow)
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:ColorConvert ++"), this);
	__ASSERT_ALWAYS(aDestPtr, User::Invariant());
	TInt				lError = E_SUCCESS;
	TInt				err = KErrNone;
	
	err = SetSourceFormat();
	if(err == KErrNone)
	{
    	err = SetSourceRange();
    	if(err == KErrNone)
    	{
						
			lError = Emz_VDec_gColorConv_YUVtoRGB(aInputFrame,aDestPtr, 
						aInputCropWindow, aOutputCropWindow, iSourceFormat,
						EBitmapColor16MU, iSourceRange);

			if(lError)
			{
				if(lError == E_OUT_OF_MEMORY)
					{
					err = KErrNoMemory;
					}
				else if(lError == E_FAILURE)
					{
					err = KErrNotSupported;
					}
				else
					{
					err = KErrGeneral;
					}
			}
		}
	}
	
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:ColorConvert --"), this);
	return err;
}

TInt CNGAPostProcHwDevice::SetSourceFormat()
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetSourceFormatL ++"), this);
	TInt err = KErrNone;
	switch (iVideoFormat.iYuvFormat.iPattern)
	{
	    case EYuv420Chroma1:
    		iSourceFormat = EYuv420Chroma1_Planar;
    		break;
        case EYuv420Chroma2:
    		iSourceFormat = EYuv420Chroma2_Planar;
    		break;
        case EYuv420Chroma3:
    		iSourceFormat = EYuv420Chroma3_Planar;
    		break;
	    case EYuv422Chroma1:
			if( iVideoFormat.iYuvFormat.iDataLayout == EYuvDataInterleavedLE)
    			iSourceFormat = EYuv422Chroma1_LE;
	    	else if( iVideoFormat.iYuvFormat.iDataLayout == EYuvDataInterleavedBE )
				iSourceFormat = EYuv422Chroma1_BE;
			else
			    err = KErrArgument;
			break;
    	case EYuv422Chroma2:
    		if( iVideoFormat.iYuvFormat.iDataLayout == EYuvDataInterleavedLE)
	    		iSourceFormat = EYuv422Chroma2_LE;
    		else if( iVideoFormat.iYuvFormat.iDataLayout == EYuvDataInterleavedBE )
    			iSourceFormat = EYuv422Chroma2_BE;
			else
			    err = KErrArgument;
			break;
      default:
    		err = KErrNotSupported;
	}
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetSourceFormatL --"), this);
	return err;
}


TInt CNGAPostProcHwDevice::SetSourceRange()
{
	PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetSourceRangeL ++"), this);
	TInt err = KErrNone;
	switch (iVideoFormat.iYuvFormat.iCoefficients)
	{
	    case EYuvBt601Range0:
			iSourceRange = EITU601_5_REDUCEDRANGE;
            break;
        case EYuvBt601Range1:
			iSourceRange = EITU601_5_FULLRANGE;
			break;
        case EYuvBt709Range0:
			iSourceRange = EB709_REDUCEDRANGE;
			break;
        case EYuvBt709Range1:
			iSourceRange = EB709_FULLRANGE;
            break;
	    default:
		    err = KErrNotSupported;
    }
    PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:SetSourceRangeL --"), this);
    return err;
}

CNGAPostProcTimer::CNGAPostProcTimer( CNGAPostProcHwDevice& aParent )
:CTimer(EPriorityHigh),iParent(aParent)
{
	CActiveScheduler::Add(this);
}

CNGAPostProcTimer::~CNGAPostProcTimer()
{
	PP_DEBUG(_L("CNGAPostProcTimer[%x]:~CNGAPostProcTimer ++"), this);
	Cancel();
	PP_DEBUG(_L("CNGAPostProcTimer[%x]:~CNGAPostProcTimer --"), this);
}

CNGAPostProcTimer* CNGAPostProcTimer::NewL( CNGAPostProcHwDevice& aParent )
{
	CNGAPostProcTimer* self = new (ELeave)CNGAPostProcTimer(aParent);
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
}

void CNGAPostProcTimer::ConstructL()
{
	CTimer::ConstructL();
}

void CNGAPostProcTimer::RunL()
{
	PP_DEBUG(_L("CNGAPostProcTimer[%x]:RunL ++"), this);
	if (iStatus ==KErrCancel)
	{
		PP_DEBUG(_L("CNGAPostProcNotifier[%x]:CNGAPostProcNotifier:RunL State canceled"), this);
		return;
	}
	iParent.AttemptToPost();
	PP_DEBUG(_L("CNGAPostProcTimer[%x]:RunL --"), this);
}

void CNGAPostProcHwDevice::MmpirPostInitializeRequest(MMmfPostInitializeResponse& aResponse)
	{
	iPostInitializeResponse = &aResponse;
	}
