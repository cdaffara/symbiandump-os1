// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MDFVIDEODECODEHWDEVICEADAPTER_H
#define MDFVIDEODECODEHWDEVICEADAPTER_H

#include <fbs.h>
#include <mmf/devvideo/videoplayhwdevice.h>
#include "mdfvideodecodehwdeviceadapter.hrh"
#include <mmf/server/mmfdatabuffer.h>

#include <mdf/mdfinputport.h>
#include <mdf/mdfoutputport.h>
#include <mdf/mdfprocessingunit.h>
#include <mdf/mdfpuloader.h>
#include <mdf/codecapivideoresolverutils.h>
#include <mmf/devvideo/devvideopuconfig.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/devvideo/devvideohwdeviceadaptersetup.h>
#endif

// From hwdevicevideoutils.dll
#include "mdfvideodecoderbuffermanager.h"
#include "mdfplayerengine.h"

const TUid KUidDevVideoDecodeHwDevice = {KUidDevVideoDecoderHwDeviceDefine};

// decoder info
_LIT(KVideoDecoderInfoManufacturer, "Symbian Ltd.");
_LIT8(KVideoDecoderInfoCSInfo, "Coded by Symbian");
_LIT8(KVideoDecoderInfoISInfo, "Implemented by Symbian");
const TInt KVideoDecoderInfoVersionMaj = 0;
const TInt KVideoDecoderInfoVersionMin = 1;
const TInt KVideoDecoderInfoVersionBuild = 1;

/**
Video decoder hardware device. 
This is the DevVideo decoder plugin component
@internalComponent
*/
class CMdfVideoDecodeHwDeviceAdapter :	public CMMFVideoDecodeHwDevice,
				       					public MMdfVideoPlayerHwDeviceObserver,
				       					public MMdfInputPortObserver,
				       					public MMdfOutputPortObserver,
				       					public MMdfProcessingUnitObserver,
				       					public MDevVideoHwDeviceAdapterSetup		       					

{

public:
		
	/**
	The current state of the Hardware Device Adapter.
	*/
	enum THwDevAdapterState
		{
		/*
		The PULoader has been loaded.
		*/
		EProcessingUnitLoaderLoaded,
		/*
		The Processing Units have been loaded.
		*/
		EProcessingUnitLoaded,
		/*
		The Processing Units are currently being initialised.
		*/
		EProcessingUnitInitializing,
		/*
		The Processing Units are currently in the idle state.
		*/
		EProcessingUnitIdle,
		/*
		The Processing Units are currently in the executing state.
		*/
		EProcessingUnitExecuting,
		/*
		The Processing Units are currently in the paused state.
		*/
		EProcessingUnitPaused		
		};	


	static CMdfVideoDecodeHwDeviceAdapter* NewL();
	~CMdfVideoDecodeHwDeviceAdapter();		
	TBool FrameBufferAvailable() const;

	// from CMMFVideoHwDevice
    TAny* CustomInterface(TUid aInterface);

	// from CMMFVideoPlayHwDevice
	CPostProcessorInfo* PostProcessorInfoLC();
	void GetOutputFormatListL(RArray<TUncompressedVideoFormat>& aFormats);
	void SetOutputFormatL(const TUncompressedVideoFormat &aFormat);
	void SetPostProcessTypesL(TUint32 aPostProcCombination);
	void SetInputCropOptionsL(const TRect& aRect);
	void SetYuvToRgbOptionsL(const TYuvToRgbOptions& aOptions, const TYuvFormat& aYuvFormat, TRgbFormat aRgbFormat);
	void SetYuvToRgbOptionsL(const TYuvToRgbOptions& aOptions);
	void SetRotateOptionsL(TRotationType aRotationType);
	void SetScaleOptionsL(const TSize& aTargetSize, TBool aAntiAliasFiltering);
	void SetOutputCropOptionsL(const TRect& aRect);
	void SetPostProcSpecificOptionsL(const TDesC8& aOptions);
	void SetClockSource(MMMFClockSource* aClock);
	void SetVideoDestScreenL(TBool aScreen);
	void Initialize();
	void StartDirectScreenAccessL(const TRect& aVideoRect, CFbsScreenDevice& aScreenDevice, const TRegion& aClipRegion);
	void SetScreenClipRegion(const TRegion& aRegion);
	void SetPauseOnClipFail(TBool aPause);
	void AbortDirectScreenAccess();
	TBool IsPlaying();
	void Redraw();
	void Start();
	void Stop();
	void Pause();
	void Resume();
	void SetPosition(const TTimeIntervalMicroSeconds& aPlaybackPosition);
	void FreezePicture(const TTimeIntervalMicroSeconds& aTimestamp);
	void ReleaseFreeze(const TTimeIntervalMicroSeconds& aTimestamp);
	TTimeIntervalMicroSeconds PlaybackPosition();
	TUint PictureBufferBytes();
	void GetPictureCounters(CMMFDevVideoPlay::TPictureCounters& aCounters);
	void SetComplexityLevel(TUint aLevel);
	TUint NumComplexityLevels();
	void GetComplexityLevelInfo(TUint aLevel, CMMFDevVideoPlay::TComplexityLevelInfo& aInfo);
	void ReturnPicture(TVideoPicture* aPicture);
	TBool GetSnapshotL(TPictureData& aPictureData, const TUncompressedVideoFormat& aFormat);
	void GetTimedSnapshotL(TPictureData* aPictureData, const TUncompressedVideoFormat& aFormat, const TTimeIntervalMicroSeconds& aPresentationTimestamp);
	void GetTimedSnapshotL(TPictureData* aPictureData, const TUncompressedVideoFormat& aFormat, const TPictureId& aPictureId);
	void CancelTimedSnapshot();
	void GetSupportedSnapshotFormatsL(RArray<TUncompressedVideoFormat>& aFormats);
	void InputEnd();
	void CommitL();
	void Revert();

	// from CMMFVideoDecodeHwDevice
    CVideoDecoderInfo* VideoDecoderInfoLC();
	TVideoPictureHeader* GetHeaderInformationL(TVideoDataUnitType aDataUnitType, TVideoDataUnitEncapsulation aEncapsulation, TVideoInputBuffer* aDataUnit);
    void ReturnHeader(TVideoPictureHeader* aHeader);
    void SetInputFormatL(const CCompressedVideoFormat& aFormat, TVideoDataUnitType aDataUnitType, TVideoDataUnitEncapsulation aEncapsulation, TBool aDataInOrder);
    void SynchronizeDecoding(TBool aSynchronize);
    void SetBufferOptionsL(const CMMFDevVideoPlay::TBufferOptions& aOptions);
    void GetBufferOptions(CMMFDevVideoPlay::TBufferOptions& aOptions);
    void SetHrdVbvSpec(THrdVbvSpecification aHrdVbvSpec, const TDesC8& aHrdVbvParams);
    void SetOutputDevice(CMMFVideoPostProcHwDevice* aDevice);
    TTimeIntervalMicroSeconds DecodingPosition();
    TUint PreDecoderBufferBytes();
    void GetBitstreamCounters(CMMFDevVideoPlay::TBitstreamCounters& aCounters);
    TUint NumFreeBuffers();
    TVideoInputBuffer* GetBufferL(TUint aBufferSize);
    void WriteCodedDataL(TVideoInputBuffer* aBuffer);
	void SetProxy(MMMFDevVideoPlayProxy& aProxy);
	void ConfigureDecoderL(const TVideoPictureHeader& aVideoPictureHeader);
	
	// from MMdfVideoPlayerHwDeviceObserver
	TUint Time() const;
	TBool FrameAvailable() const;
	void DisplayFrame();
	void DiscardFrame();
	
	// from MMdfInputPortObserver
	void MipoWriteDataComplete(const MMdfInputPort* aInputPort, CMMFBuffer* aBuffer,
		TInt aErrorCode);
	void MipoDisconnectTunnelComplete(const MMdfInputPort* aInputPort, TInt aErrorCode);
	void MipoRestartTunnelComplete(const MMdfInputPort* aInputPort,	TInt aErrorCode);
	
	// from MMdfOutputPortObserver
	void MopoReadDataComplete(const MMdfOutputPort* aOutputPort, CMMFBuffer* aBuffer,
		TInt aErrorCode);
	void MopoDisconnectTunnelComplete(const MMdfOutputPort* aOutputPort, TInt aErrorCode);
	void MopoRestartTunnelComplete(const MMdfOutputPort* aOutputPort, TInt aErrorCode);
		
	// from MMdfProcessingUnitObserver
	void InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	void ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	// from MDevVideoHwDeviceAdapterSetup
	void LoadProcessingUnitL(const CImplementationInformation& aImplInfo);

private:
	CMdfVideoDecodeHwDeviceAdapter();
	void ConstructL();
	
	void InitializeL();
	void DoPostPictureNotify();
	void DisplayFrameDirect(TVideoPicture* aPicture); 
	void DisplayLastFrameDirect(); 
	
	void SendLastBuffer();
	
private:
	// minimum decoder buffer useful data size.
	// to work around a problem with the decoder when too 
	// small a buffer passed to it will cause a KERN-EXEC:3;
	// the alternative is putting bounds-checking into the C code
	// throughout reading an encoded header, which would be inefficient
	static const TInt KVideoDecoderMinUsefulDataSize = 256;

	// minimum decoder data buffer size.
	// the decoder will always return a buffer of at least this size.
	static const TInt KVideoDecoderMinDataBufferSize = 0x2000; // 8k

	// maximum decoder data buffer size.
	// this is the largest buffer the decoder is able to return.
	static const TInt KVideoDecoderMaxDataBufferSize = 0x40000; // 512k
	
	MMMFDevVideoPlayProxy* iProxy;

	// the decoder itself
	CMdfProcessingUnit* iDecoderPU;
	
	// the buffer manager
	CMdfVideoDecoderBufferManager* iBufferManager;
	
	// the player engine
	CMdfVideoPlayerEngine* iPlayerEngine;
	
	// the Processing Unit Loader plugin
	CMdfPuLoader* iPuLoader;
	
	// video format arrays (for VideoDecoderInfoLC())
	RPointerArray<CCompressedVideoFormat> iInputVidFormats;
	RArray<TUncompressedVideoFormat> iOutputVidFormats;
	RArray<TPictureRateAndSize> iPictureRates;
	
	// Processing Unit Ports
	MMdfInputPort* iDecoderPUInputPort; 
	MMdfOutputPort* iDecoderPUOutputPort;

	// decoder settings
	TUncompressedVideoFormat iFormat;
	TReal32 iFrameRate;
	CMMFDevVideoPlay::TPictureCounters iPictureCounters;
	CMMFDevVideoPlay::TBitstreamCounters iBitstreamCounters;
	CMMFDevVideoPlay::TComplexityLevelInfo iComplexityLevelInfo;
	CMMFDevVideoPlay::TBufferOptions iBufferOptions;
	
	// clock settings
	MMMFClockSource* iClockSource;
	TBool iSynchronize;
	TInt64 iSystemClock;	// system clock without HAL
	
	// header info
	TVideoPictureHeader* iPictureHeader;
			
	// input data buffer pointers 
	CVideoDataBuffer* iInputBuffer; // not owned
	CMMFBuffer* iPUInputBuffer; // not owned
	
	// parameters for decoding loop
	TInt iInDataSize;
	TInt iTotalBytesUsed;
	
	// player settings
	TBool iPrimed;
	TBool iInputBufferWaiting;
	TBool iInputEnd;
	TSize iFrameSize;
	
	TBool iDisplayPictureAvailable;
	
	// post-processor output device, if any
	CMMFVideoPostProcHwDevice* iPostProcOutputDevice;
	
	// direct screen access
	TBool iDSAEnabled;
	TBool iDSAStarted;
	CFbsScreenDevice* iScreenDevice; // not owned
	CFbsBitGc* iScreenDeviceGc;
	TRect iScreenDeviceRect;
	
	TUid iPuLoaderDtorKey;	
	THwDevAdapterState iState;	
	TBool iPUInitialized;
	
	CCodecApiVideoOpaqueData* iPuData;	
	HBufC* iManufacturer;
	TUid iPuUid;
	TBool iLastFrameBufferReceived;
	TSize iMaxPictureSize;
	TBool iWriteRequestOutstanding;
	};

#endif // MDFVIDEODECODEHWDEVICEADAPTER_H
