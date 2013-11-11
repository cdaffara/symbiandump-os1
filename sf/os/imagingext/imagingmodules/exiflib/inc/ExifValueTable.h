/*
* Copyright (c) 2003, 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines Exif-specific numbers (i.e. tag IDs) used by internal 
*                service classes.
*
*/


#ifndef EXIFTABLE_H
#define EXIFTABLE_H

// CONSTANTS

// Constant Exif data
const TUint32 KExifVersion = 0x30323230;
const TUint32 KFlashPixVersion = 0x30303130;
const TUint32 KGpsVersion = 0x00000202;
const TUint16 KCompressed = 0x0006;

/* Exif Tag IDs */
// ___________________________________________________________

/* Non-modifiable tags */
const TUint16 KIdExifIfdPointer = 0x8769;
const TUint16 KIdGpsIfdPointer = 0x8825;
const TUint16 KIdExifVersion = 0x9000;
const TUint16 KIdFlashPixVersion = 0xa000;
const TUint16 KIdCompression = 0x0103;
const TUint16 KIdJpegInterchangeFormat = 0x0201; 
const TUint16 KIdJpegInterchangeFormatLength = 0x0202;
const TUint16 KIdIntOpIfdPointer = 0xa005;
const TUint16 KIdGpsVersion = 0x0000;

/* Mandatory Ifd0 tags */
const TUint16 KIdXResolution = 0x011A;
const TUint16 KIdYResolution = 0x011B;
const TUint16 KIdResolutionUnit = 0x0128;
const TUint16 KIdYCbCrPositioning = 0x0213;
// The following tag ID is also mandatory Ifd0 tag, but it is 
// already defined above.
// const TUint16 KIdExifIfdPointer = 0x8769;

/* Mandatory Exif Ifd tags */
const TUint16 KIdComponentsConfiguration = 0x9101;
const TUint16 KIdColorSpace = 0xA001;
const TUint16 KIdPixelXDimension = 0xA002;
const TUint16 KIdPixelYDimension = 0xA003;
// The following two tag IDs are also mandatory Exif Ifd tags, but they are 
// already defined above.
//const TUint16 KIdExifVersion = 0x9000; 
//const TUint16 KIdFlashPixVersion = 0xa000; 


/* Mandatary Ifd1 tags: */
// The following six tag IDs are mandatory Ifd1 tags, but they are 
// already defined above.
//const TUint16 KIdCompression = 0x0103; 
//const TUint16 KIdXResolution = 0x011A; 
//const TUint16 KIdYResolution = 0x011B; 
//const TUint16 KIdResolutionUnit = 0x0128; 
//const TUint16 KIdJpegInterchangeFormat = 0x0201; // already defined!
//const TUint16 KIdJpegInterchangeFormatLength = 0x0202; // already defined!

/* Others: */
const TUint16 KIdImageDescription = 0x010E;
const TUint16 KIdMake = 0x010F;
const TUint16 KIdModel = 0x0110;
const TUint16 KIdOrientation = 0x0112;
const TUint16 KIdTransferFunction = 0x012D;
const TUint16 KIdDateTime = 0x0132;
const TUint16 KIdSoftware = 0x0131;
const TUint16 KIdArtist = 0x013B;
const TUint16 KIdCopyright = 0x8298;
const TUint16 KIdExposureTime = 0x829A;
const TUint16 KIdFlash = 0x9209;
const TUint16 KIdExposureMode = 0xA402;
const TUint16 KIdWhiteBalance = 0xA403;
const TUint16 KIdSceneCaptureType = 0xA406;
const TUint16 KIdExposureProgram = 0x8822;
const TUint16 KIdIsoSpeedRatings = 0x8827;
const TUint16 KIdDateTimeOriginal = 0x9003;
const TUint16 KIdDateTimeDigitized = 0x9004;
const TUint16 KIdApertureValue = 0x9202;
const TUint16 KIdExposureBiasValue = 0x9204;
const TUint16 KIdMeteringMode = 0x9207;
const TUint16 KIdLightSource = 0x9208;
const TUint16 KIdMakerNote = 0x927C;
const TUint16 KIdUserComment = 0x9286;
const TUint16 KIdRelatedSoundFile = 0xA004;
const TUint16 KIdFileSource = 0xA300;
const TUint16 KIdDigitalZoomRatio = 0xA404;
const TUint16 KIdContrast = 0xA408;
const TUint16 KIdSaturation = 0xA409;
const TUint16 KIdSharpness = 0xA40A;
const TUint16 KIdShutterSpeedValue = 0x9201;
const TUint16 KIdBrightnessValue = 0x9203;
const TUint16 KIdCustomRendered = 0xA401;
const TUint16 KIdGainControl = 0xA407;

#endif // EXIFTABLE_H
