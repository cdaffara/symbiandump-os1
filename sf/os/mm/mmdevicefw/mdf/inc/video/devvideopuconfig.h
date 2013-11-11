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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef DEVVIDEOPUCONFIG_H
#define DEVVIDEOPUCONFIG_H

#include <mdf/mdfpuconfig.h>
#include <mmf/devvideo/devvideobase.h>

const TInt KUidDevVideoPlayConfig = 0x102737F0;
const TInt KUidDevVideoHeaderConfig = 0x102737F2;
const TInt KUidDevVideoPictureHeaderConfig = 0x102737F3;
const TInt KUidDevVideoTimestamp =  0x10273824;

/**
Used to configure the CMdfProcessingUnit plugin.
*/
class TDevVideoPlayPuConfig
	{
public:
	/**
	The image format.
	*/
	TUncompressedVideoFormat iImageFormat;
	/**
	The input buffer size.
	*/
	TUint iInputBufferSize;
	};
		
class TDevVideoHeaderPuConfig
	{
public:
	/** 
	A pointer to a buffer with header data
	*/
	TPtrC8 iHeaderData;
	/**
	Type of data in header
	*/
	TVideoDataUnitType iDataUnitType; 
	/**
	The type of encapsulation of the stream
	*/
	TVideoDataUnitEncapsulation iDataUnitEncapsulation;
	};
	
typedef TPuConfigParam<KUidDevVideoPlayConfig, TDevVideoPlayPuConfig> TPuConfigVideoPlayback;
typedef TPuConfigParam<KUidDevVideoHeaderConfig, TDevVideoHeaderPuConfig> TPuConfigVideoHeader;
typedef TPuConfigParam<KUidDevVideoPictureHeaderConfig, TVideoPictureHeader> TPuConfigVideoPictureHeader;	
typedef TPuConfigParam<KUidDevVideoTimestamp, TTimeIntervalMicroSeconds> TPuConfigTimestamp;		
	
/**
Enum for decoder status returns. Decoder device is expected to map
its decoder status to these values.
*/
enum TVideoPlayDecoderStatus
	{
	EVideoPlayDecoderStatusNone,
	EVideoPlayDecoderStatusHeader,
	EVideoPlayDecoderStatusFrame
	};
		
const TInt KUidDevVideoRecordConfig = 0x102737F1;

/**
Used by the SetConfig() method to configure the frame size, image format 
and frame rate of a CMMFVideoRecordHwDevice plugin. 
The configuration of HwDevices is device specific.
*/
class TDevVideoRecordPuConfig
	{
public:
	/**
	The frame size.
    */
	TSize iFrameSize;
	/**
	The image format.
	*/
	TUncompressedVideoFormat iImageFormat;
	/**
	The frame rate.
	*/
	TUint iFrameRate;
	};
	
typedef TPuConfigParam<KUidDevVideoRecordConfig, TDevVideoRecordPuConfig> TPuConfigDevVideoRecord;

#endif // DEVVIDEOPUCONFIG_H

