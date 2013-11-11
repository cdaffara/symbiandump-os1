// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ecam/ecamcapturecontrolintf.h>
#include <ecam/implementationfactoryintf.h>
#include <ecamimageprocessing.h>
#include <ecam/camerahistogram.h>
#include "ecamversion.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamcapturecontrolconst.h>
#endif
const TInt KECamVideoCaptureBit0 = 0x00;
const TInt KECamClientVideoCaptureBit1 = 0x01;
const TInt KECamDirectVideoCaptureBit2 = 0x02;

/**
Factory function for creating the CCameraPreImageCaptureControl object.

@param aCamera 
	   a reference to a CCamera object providing the settings.
	   
@param aPreImageCaptureControlObserver
	   Reference to the pre image capture control observer.

@return a pointer to a fully constructed CCameraPreImageCaptureControl object.

@leave KErrNoMemory Out of memory Or any other system-wide error code.

@leave KErrExtensionNotSupported When NewL/NewDuplicateL used instead of New2L/NewDuplicate2L.
*/
EXPORT_C CCamera::CCameraPreImageCaptureControl* CCamera::CCameraPreImageCaptureControl::NewL(CCamera& aCamera, MPreImageCaptureControlObserver& aPreImageCaptureControlObserver)
	{
 	if(aCamera.CameraVersion() == KCameraDefaultVersion)
 		{
 		User::Leave(KErrExtensionNotSupported);
 		}
 		
	CCamera::CCameraPreImageCaptureControl* self = new (ELeave)CCamera::CCameraPreImageCaptureControl(aCamera); 
	CleanupStack::PushL(self);
	self->ConstructL(aPreImageCaptureControlObserver);
	CleanupStack::Pop(self);
	
	return self; 
	}
	
/**	
CCameraPreImageCaptureControl Constructor.

@param aOwner
       a reference to a CCamera object. 
*/
CCamera::CCameraPreImageCaptureControl::CCameraPreImageCaptureControl(CCamera& aOwner):iOwner(aOwner), iImpl(NULL)
	{
	}

/**
CCameraPreImageCaptureControl second phase constructor. 

Function used to initialise internal state of the object. Uses reference to the camera to retrieve 
PreImageCaptureControl interface pointer.

@param aPreImageCaptureControlObserver
	   Reference to the pre image capture control observer.

@leave KErrNoMemory Out of memory Or any other system-wide error code.
*/ 
void CCamera::CCameraPreImageCaptureControl::ConstructL(MPreImageCaptureControlObserver& aPreImageCaptureControlObserver) 
	{
	iImpl = static_cast<MCameraPreImageCaptureControl*>(iOwner.CustomInterface(KECamMCameraPreImageCaptureControlUid));

	if (iImpl == NULL)
		{
		User::Leave(KErrNotSupported);
		}
		
	iImpl->SetPreImageCaptureControlObserver(aPreImageCaptureControlObserver);
	}

/**
Destructor
*/	
EXPORT_C CCamera::CCameraPreImageCaptureControl::~CCameraPreImageCaptureControl()
	{
	if (iImpl != NULL)
		{
		iImpl->Release();	
		}
	}	
	
/**
Retrieves information regarding the direct snapshot feature support. Direct Snapshot, if supported, can be created
out of version2 direct viewfinder object only.

@param  aDirectSnapshotSupportInfo
		This is a bit field providing supported direct snapshot of type TDirectSnapshotType

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraPreImageCaptureControl::GetDirectSnapshotSupportInfoL(TUint& aDirectSnapshotSupportInfo) const
	{
	iImpl->GetDirectSnapshotSupportInfoL(aDirectSnapshotSupportInfo);	
	}
	
/**
Retrieves the settings supported for embedded still captures. Possibilty exists that not all the settings supported for
normal still image captures are supported for embedded still captures as well.

@param  aSupportedEmbeddedStillCaptureSettings
		Array of TUid which retrieves the supported embedded still captures. Empty list indicated that no settings are
		supported for embedded still captures.
		
@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraPreImageCaptureControl::GetSupportedEmbeddedStillCaptureSettingsL(RArray<TUid>& aSupportedEmbeddedStillCaptureSettings) const
	{
	iImpl->GetSupportedEmbeddedStillCaptureSettingsL(aSupportedEmbeddedStillCaptureSettings);	
	}

/**
Retrieves information regarding the supported direct saving state. If used, still images are saved in files rather than 
providing clients the MCameraImageBuffer. Direct saving to file will continue even if the client application gets closed
for any reasons.

@param  aSupportedDirectSavingType
		Retrieves the enum specifying supported TDirectSavingType.
		If EDirectSavingNotUsed, direct saving not supported. Images will be received in buffer MCameraImageBuffer.
		
		If EDirectSavingHighResolutionFileOnly, direct saving to file is supported. But no cut down version of the image 
		will be saved to file. Callback used is MCaptureImageObserver::ImageDirectSavingCompleted().
		
		If EDirectSavingWithLowerResolutionFile, Direct saving to file is supported. Also, a cut down version of the image 
		will be saved to another specified file. Callbacks used are MCaptureImageObserver::ImageDirectSavingCompleted() for 
		actual image and MCaptureImageObserver::CutDownImageDirectSavingCompleted() for lower resolution image.

@leave  May leave with any error code.	
*/
EXPORT_C void CCamera::CCameraPreImageCaptureControl::GetSupportedDirectSavingTypeL(CCamera::CCameraPreImageCaptureControl
															::TDirectSavingType& aSupportedDirectSavingType) const
	{
	iImpl->GetSupportedDirectSavingTypeL(aSupportedDirectSavingType);	
	}

/**
Provides the base name for file used to save images. If there are sequential images, files will be created by 
implementation with names appearing as base name and appended by increasing integers which would start from 
aStartingSequenceNumber. This is used for the original image only, but not for the cut down version of the original
image.

@param  aFilename
		A const TDesC8&: Base name for files which ECam implementation needs to create in order to collect every sequential
		image.
		
@param  aStartingSequenceNumber
		The starting sequence number which will be appended to the base name 'aFilename'. The sequence number will keep
		on increasing for every individual still image capture.
		
@leave  May leave with any error code.

@note   It is upto the implementation how many digits it uses for sequence number. Accordingly, padding will be done with 
		zeroes.
*/
EXPORT_C void CCamera::CCameraPreImageCaptureControl::SetSequentialImageFilenameL(const TDesC8& aFilename, TInt aStartingSequenceNumber)
	{
	iImpl->SetSequentialImageFilenameL(aFilename, aStartingSequenceNumber);
	}
	
/**
Provides the base name for file used to save cut down version of the original images. If there are sequential images, 
files will be created by implementation with names appearing as base name and appended by increasing integers which would 
start from aStartingSequenceNumber. This is used for the cut down version of the original image.

@param  aLowerResolutionFilename
		A const TDesC8&: Base name for files which ECam implementation needs to create in order to collect the cut down version
		of every sequential image.
		
@param  aStartingSequenceNumber
		The starting sequence number which will be appended to the base name 'aFilename'. The sequence number will keep
		on increasing for every individual still image capture.
		
@leave  May leave with any error code.

@note   It is upto the implementation how many digits it uses for sequence number. Accordingly, padding will be done with 
		zeroes.
*/
EXPORT_C void CCamera::CCameraPreImageCaptureControl::SetLowerResolutionSequentialImageFilenameL(const TDesC8& aLowerResolutionFilename, 
																							TInt aStartingSequenceNumber)
	{
	iImpl->SetLowerResolutionSequentialImageFilenameL(aLowerResolutionFilename, aStartingSequenceNumber);
	}

/**
Retrieves the type of direct saving currently used. This will be represented as a direct saving state as given by 
TDirectSavingType. 

Whether or not the direct saving option is used, client will receive the MCaptureImageObserver::ImageCaptureComplete() 
callback to mark the completion of the image capture operation.

@param  aDirectSavingType
		Current type of the direct saving. 
		
		If EDirectSavingNotUsed, direct saving is not used. Images will be received in buffer MCameraImageBuffer through
		callback MCaptureImageObserver::ImageBufferReady().
		
		If EDirectSavingHighResolutionFileOnly, direct saving to file option is currently used. But no cut down version of
		the image will be saved to file. Callback used is MCaptureImageObserver::ImageDirectSavingCompleted().
		
		If EDirectSavingWithLowerResolutionFile, apart from direct saving to file option for the actual image, a cut down 
		version of the image will be saved to another specified file. Callbacks used are MCaptureImageObserver::
		ImageDirectSavingCompleted() for actual image and MCaptureImageObserver::CutDownImageDirectSavingCompleted() for 
		lower resolution image.
		
@leave  May leave with any error code.

@note   Direct saving to file will continue even if the client application gets closed for any reasons.
*/
EXPORT_C void CCamera::CCameraPreImageCaptureControl::GetDirectSavingTypeL(CCamera::CCameraPreImageCaptureControl::
																			TDirectSavingType& aDirectSavingType) const
	{
	iImpl->GetDirectSavingTypeL(aDirectSavingType);	
	}			

/**
Instructs the ECam implementation to use the desired type of direct saving option as specifed by its state.

Whether or not the direct saving option is used, client will receive the MCaptureImageObserver::ImageCaptureComplete() 
callback to mark the completion of the image capture operation.

@param  aDirectSavingType
		The desired type of the direct saving. 
		
		If EDirectSavingNotUsed, direct saving will not be used. Images will be received in buffer MCameraImageBuffer 
		through	callback MCaptureImageObserver::ImageBufferReady().
		
		If EDirectSavingHighResolutionFileOnly, direct saving to file option will be used. But no cut down version of
		the image will be saved to file. Callback to be used is MCaptureImageObserver::ImageDirectSavingCompleted().
		
		If EDirectSavingWithLowerResolutionFile, apart from direct saving to file option for the actual image, a cut down 
		version of the image will be saved to another specified file. Callbacks to be used are MCaptureImageObserver::
		ImageDirectSavingCompleted() for actual image and MCaptureImageObserver::CutDownImageDirectSavingCompleted() for 
		lower resolution image.
		
@leave  May leave with any error code.

@note   Clients need to provide the filename before capturing still images under direct saving option. Callback 
		MCaptureImageObserver::ImageDirectSavingCompleted() may provide error KErrNotReady if filenames are not 
		provided before images are captured for direct saving. Similarly, if cut down version of image is also to be saved
		to file, MCaptureImageObserver::CutDownImageDirectSavingCompleted() may provide error KErrNotReady if filenames are 
		not provided before hand. 
				
@note   Direct saving to file will continue even if the client application gets closed for any reasons.
*/
EXPORT_C void CCamera::CCameraPreImageCaptureControl::SetDirectSavingTypeL(CCamera::CCameraPreImageCaptureControl::
															TDirectSavingType aDirectSavingType)
	{
	iImpl->SetDirectSavingTypeL(aDirectSavingType);
	}

/**
Retrieves whether the camera device is capable of providing capture event notification to the client. Client may take
the responsibility of playing the capture sound after receiving the notification. 

@param  aSupportedDriveModes
		A reference to bit-field of TUint which indicates the drive modes in which the capture event notification is
		supported. If aSupportedDriveModes =0, capture event notification is not supported.
		
@note   If capture event notification is supported, ECam implementation will use KUidECamEventImageCaptureEvent to notify 
		clients that the image has been exposed to the camera sensor. Clients may play capture sound if they desire to do so.

@leave  May leave with any error code.	

@see    CCamera::CCameraAdvancedSettings::TDriveMode
*/
EXPORT_C void CCamera::CCameraPreImageCaptureControl::GetCaptureEventSupportInfoL(TUint& aSupportedDriveModes) const
	{
	iImpl->GetCaptureEventSupportInfoL(aSupportedDriveModes);	
	}

/**
Retrieves the supported image formats for a given resolution.

@param  aImageFormatsSupported
		A bit field which retrieves the supported image formats for a given resolution.
		Formats have been defined as CCamera::TFormat
		
@param  aSize
		The resolution (or size) for which the total number of supported image formats have to be retrieved.

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraPreImageCaptureControl::GetImageFormatsSupportedL(TUint& aImageFormatsSupported, const TSize& aSize) const
	{
	iImpl->GetImageFormatsSupportedL(aImageFormatsSupported, aSize);		
	}

/**
Retrieves the supported pixel aspect ratio for a given resolution in case of still image.

@param  aPixelAspectsSupported
		A bit field which retrieves the supported pixel aspect ratio for a given resolution.
		Pixel aspect ratio have been defined as CCamera::CCameraAdvancedSettings::TPixelAspectRatio
		
@param  aImageFormat
		The image format for which the supported pixel aspect ratio have to be retrieved.

@param  aSize
		The resolution (or size) for which the supported pixel aspect ratio have to be retrieved.

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraPreImageCaptureControl::GetPixelAspectsSupportedL(TUint& aPixelAspectsSupported, CCamera::TFormat aImageFormat, const TSize& aSize) const
	{
	iImpl->GetPixelAspectsSupportedL(aPixelAspectsSupported, aImageFormat, aSize);		
	}

/**
Performs setup and allocation of memory for image capture operation. Implementation creates a new CCameraImageCapture*
object which reflects the prepare image parameters passed by the client. The callback MPreImageCaptureControlObserver::
PrepareImageComplete() passes the ownership of the CCameraImageCapture* object to the client.

This needs to be called every time client wishes to change prepare image parameters or if the client wishes to create
a new CCameraImageCapture* object. 

@param  aPrepareImageParameters 
        The desired image parameters to be used for capturing still images using the CCameraImageCapture instance
        which would be passed by the implementation.
        
@param  aCaptureImageObserver
		The Capture image observer which is needed by the implementation in order to pass it to the CCameraImageCapture 
		while creating it.

@note 	Next PrepareImageCapture can be called only after receiving the notification KUidECamEventReadyForNextPrepare.

@note	If drive mode is EDriveModeTimeNudgeCapture, when the client initiates image capture the total amount of images
		requested by the client	(specified in TDriveModeDependentAttributes) will be returned to the client
		(via MCaptureImageObserver::ImageBufferReady())	or saved to file. No other instance of CCameraImageCapture can be
		created whilst using this drive mode until the first instance is destroyed.

@see 	CCamera::PrepareImageCaptureL(TFormat aImageFormat,TInt aSizeIndex)
*/
EXPORT_C void CCamera::CCameraPreImageCaptureControl::PrepareImageCapture(const CCamera::CCameraPreImageCaptureControl::
			TPrepareImageParameters& aPrepareImageParameters, MCaptureImageObserver& aCaptureImageObserver)
	{
	// MCameraPreImageCaptureControl concrete implementation will call CCameraImageCapture::CreateL(CCamera& aCamera, 
	// MCaptureImageObserver& aCaptureImageObserver) method and pass the CCameraImageCapture* to the client through 
	// the MPreImageCaptureControlObserver::PrepareImageComplete() callback.
	iImpl->PrepareImageCapture(aPrepareImageParameters, aCaptureImageObserver);	
	}

/**
Informs whether or not the setting of maximum memory size when encoding to the current format is supported.

@param  aIsImageMaxMemorySizeSettingSupported
		ETrue indicates that setting of maximum memory size is supported.
		EFalse indicates that setting of maximum memory size is not supported.

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraPreImageCaptureControl::GetImageMaxMemorySizeSettingSupportInfoL(TBool& aIsImageMaxMemorySizeSettingSupported) const
	{
	iImpl->GetImageMaxMemorySizeSettingSupportInfoL(aIsImageMaxMemorySizeSettingSupported);	
	}

/**
Get the maximum memory size in kilo bytes when encoding to the current format.

@param  aMemorySize
		Retrieves the maximum memory size in kilo bytes.

@note   In case of JPEG, the maximum memory size will take preference over JPEG quality if the maximum memory size is 
		not sufficient to achieve the desired quality. Refer CCamera::JpegQuality()
		
@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraPreImageCaptureControl::GetImageMaxMemorySizeL(TUint& aMemorySize) const
	{
	iImpl->GetImageMaxMemorySizeL(aMemorySize);
	}

/**
Retrieves the supported processing options.

@param  aEcamProcessingOptionsSupported
		Bitfield containing the available processing options.

@leave May leave with any error code.
*/
EXPORT_C void CCamera::CCameraPreImageCaptureControl::GetSupportedProcessingOptionsL(TUint& aECamProcessingOptionsSupported) const
	{
	iImpl->GetSupportedProcessingOptionsL(aECamProcessingOptionsSupported);
	}

/**
Constructor for the TPrepareImageParameters class.
Sets the size and version of this class.
*/
EXPORT_C CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters::TPrepareImageParameters()
	{
	iSize = sizeof(CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters);
	iVersion = KECamPrepareImageParametersCurrentVersion;
	
	iImageMaxMemorySize = KECamNoSpecificMaxMemorySize;
	iImageProcessingOptions = 0;
	}

/**	
Returns the size of the class. Used for extensibility by deriving from this base class and adding new member variables.
Intended to be used for implementation of methods where this class reference is passed as function arguments. 
Implementation of such methods can find out the whether the actual class passed is base or the derived one. So, if a new application 
is made to run on an old implementation, an error may occur in such cases after the old implementation detects this by getting 
the size information of the T class passed. Also, if old application is made to run on a new implementation, this could be 
properly handled if the derived class variables handling is done in a proper 'if-else' statement.

@return The size of this class.

@note The size will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters::Size() const
	{
	return iSize;
	}
	
/**	
Returns the version of the class. Used for extensibility specially when the class members are not added but the Reserved 
members get used at a later stage.

@return The version of this class.

@note The version will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters::Version() const
	{
	return iVersion;
	}

/**
Sets the image processing options to be used during image capture.

@param  aImageProcessingOptions
	     Bitfield of image processing options to be used during image capture.
*/
EXPORT_C void CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters::SetImageProcessingOptions(TUint aImageProcessingOptions)
	{
	iImageProcessingOptions = aImageProcessingOptions;
	}

/**
Gets the current image processing options to be used during image capture.

@param  aImageProcessingOptions
	     Bitfield of current image processing options to be used during image capture.
*/
EXPORT_C void CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters::GetImageProcessingOptions(TUint& aImageProcessingOptions) const
	{
	aImageProcessingOptions = iImageProcessingOptions;
	}

/**
Constructor for the TDriveModeDependentAttributes class.
Sets the size and version of this class.
*/
EXPORT_C CCamera::CCameraPreImageCaptureControl::TDriveModeDependentAttributes::TDriveModeDependentAttributes()
	{
	iSize = sizeof(CCamera::CCameraPreImageCaptureControl::TDriveModeDependentAttributes);
	iVersion = KECamDriveModeDependentAttributesCurrentVersion;
	iParam1 = -1;
	iParam2 = -1;
	}

/**	
Returns the size of the class. Used for extensibility by deriving from this base class and adding new member variables.
Intended to be used for implementation of methods where this class reference is passed as function arguments. 
Implementation of such methods can find out the whether the actual class passed is base or the derived one. So, if a new application 
is made to run on an old implementation, an error may occur in such cases after the old implementation detects this by getting 
the size information of the T class passed. Also, if old application is made to run on a new implementation, this could be 
properly handled if the derived class variables handling is done in a proper 'if-else' statement.

@return The size of this class.

@note The size will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraPreImageCaptureControl::TDriveModeDependentAttributes::Size() const
	{
	return iSize;
	}

/**	
Returns the version of the class. Used for extensibility specially when the class members are not added but the Reserved 
members get used at a later stage.

@return The version of this class.

@note The version will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraPreImageCaptureControl::TDriveModeDependentAttributes::Version() const
	{
	return iVersion;
	}

/**
Constructor for the TImageBufferInfo class.
Sets the size and version of this class.
*/
EXPORT_C CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::TImageBufferInfo()
	{
	iSize = sizeof(CCamera::CCameraPreImageCaptureControl::TImageBufferInfo);
	iVersion = KECamImageBufferInfoCurrentVersion;
	}

/**	
Returns the size of the class. Used for extensibility by deriving from this base class and adding new member variables.
Intended to be used for implementation of methods where this class reference is passed as function arguments. 
Implementation of such methods can find out the whether the actual class passed is base or the derived one. So, if a new application 
is made to run on an old implementation, an error may occur in such cases after the old implementation detects this by getting 
the size information of the T class passed. Also, if old application is made to run on a new implementation, this could be 
properly handled if the derived class variables handling is done in a proper 'if-else' statement.

@return The size of this class.

@note The size will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::Size() const
	{
	return iSize;
	}
	
/**	
Returns the version of the class. Used for extensibility specially when the class members are not added but the Reserved 
members get used at a later stage.

@return The version of this class.

@note The version will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::Version() const
	{
	return iVersion;
	}
	
/** 
Indicates whether the streamed image scheme or sub-frame scheme is being used.
		
@return Whether the sub-frame scheme is being used or not.

@see CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::iIsSubFrameUsed
*/
EXPORT_C TBool CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::IsSubFrameUsed() const
	{
	if (iIsSubFrameUsed)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}
		
/** 
Sets the state to inform whether the streamed image scheme or sub-frame scheme is being used.

@param  aIsSubFrameUsed
		ETrue implies sub-frame scheme is being used.
		EFalse implies sub-frame scheme is not being used. 
		
@note   The set method will be used by the ECAM implementation in order to provide necessary information to the
		clients when they receive the image buffers.
		
@see CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::iIsSubFrameUsed
*/
EXPORT_C void CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::SetSubFrameState(TBool aIsSubFrameUsed)
	{
	iIsSubFrameUsed = static_cast<TUint>(aIsSubFrameUsed);
	}
		
/**
Indicates whether the parallel buffering is being used by the implementation in order to speed up the streamed 
image operation, as a whole.

@return Whether the parallel buffer option is being used by the implementation under the sub-frame scheme.

@note   Parallel buffering indicates that implementation is using more than one buffer to handle the various 
		sub-frames; hence speeding up the operation.
		
@see CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::iIsParallelBufferUsed
*/
EXPORT_C TBool CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::IsParallelStreamedBufferUsed() const
	{
	if (iIsParallelBufferUsed)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}
		
/** 
Sets the state to inform whether the parallel buffering is being used by the implementation in order to speed up 
the streamed image operation, as a whole.

@param  aParallelStreamedBufferUsed
		ETrue implies sub-frame scheme is being used.
		EFalse implies sub-frame scheme is not being used. 
		
@note   The set method will be used by the ECAM implementation in order to provide necessary information to the
		clients when they receive the image buffers.
		
@see CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::iIsParallelBufferUsed
*/
EXPORT_C void CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::SetParallelStreamedBufferState(TBool aParallelStreamedBufferUsed)
	{
	iIsParallelBufferUsed = static_cast<TUint>(aParallelStreamedBufferUsed);
	}
		
/**
Sequence number of the sub-frame.

@return Sequence number of the sub-frame.

@see CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::iSubFrameSequenceNumber
*/
EXPORT_C TUint CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::SubFrameSequenceNumber() const
	{
	return iSubFrameSequenceNumber;
	}
		
/**
Sets the sequence number of the sub-frame.

@param aSubFrameSequenceNumber
	   The sequence number of the sub-frame. 
	   
@return  The error code

@note 	9 bits used for sequence no. assuming that KECamMaxTotalSubFrames sub-frames would be used at max. Sequence number for sub-frames 
        lies between 0 to KECamMaxTotalSubFrames-1 inclusive.
	   
@note   The set method will be used by the ECAM implementation in order to provide necessary information to the
		clients when they receive the image buffers.

@see CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::iSubFrameSequenceNumber
*/
EXPORT_C TInt CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::SetSubFrameSequenceNumber(TUint aSubFrameSequenceNumber)
	{
	if (aSubFrameSequenceNumber > (KECamMaxTotalSubFrames-1) )
		{
		return KErrOverflow;	
		}
	else 
		{
		iSubFrameSequenceNumber = aSubFrameSequenceNumber;
		return KErrNone;
		}
	}
		
/**
Total number of sub-frames needed in order to properly reconstruct the actual image.

@return Total number of sub-frames which can reconstruct the actual image.

@note 	It does not give the number of outstanding sub-frames needed to reconstruct the image. 
		This value will be same for every sub-frames needed to re-construct the actual image. 
		Maximum no. of total sub-frames is KECamMaxTotalSubFrames.

@see CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::iTotalSubFrames
*/
EXPORT_C TUint CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::TotalSubFrames() const
	{
	return iTotalSubFrames;
	}
		
/**
Sets the total number of sub-frames needed in order to properly reconstruct the actual image.

@param  aTotalSubFrames
		Total number of sub-frames which can reconstruct the actual image.
		
@return error code

@note 	It does not give the number of outstanding sub-frames needed to reconstruct the image. 
		This value will be same for every sub-frames needed to re-construct the actual image. 
		Maximum no. of total sub-frames is KECamMaxTotalSubFrames.
		
@note   The set method will be used by the ECAM implementation in order to provide necessary information to the
		clients when they receive the image buffers.
		
@see CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::iTotalSubFrames
*/
EXPORT_C TInt CCamera::CCameraPreImageCaptureControl::TImageBufferInfo::SetTotalSubFrames(TUint aTotalSubFrames)
	{
	if (aTotalSubFrames > KECamMaxTotalSubFrames)
		{
		return KErrOverflow;	
		}
	else 
		{
		iTotalSubFrames = aTotalSubFrames;
		return KErrNone;
		}
	}

/**
Retrieves the maximum number of spots present from where the camera hardware collects the focussing information.

@param  aFocusMode
		The focus mode for which the number of spots have to be retrieved.

@param  aMaximumSpots
		Retrieves the maximum number of spots for the given focus mode.
		
@leave  May leave with any error code.
		
@note   The maximum number of spots is supposed to be less than or equal to KMaxNumberOfFocusSpots.

@see    CCameraViewFinder::GetSpotsPositionL
*/
void CCamera::CCameraPreImageCaptureControl::GetMaximumSpotsL(CCamera::CCameraAdvancedSettings::TFocusMode /*aFocusMode*/, TInt& /*aMaximumSpots*/) const
	{
	User::Leave(KErrNotSupported);
	}

/**
Retrieves the supported spot combinations for the given focus mode.

@param  aFocusMode
		The focus mode for which the supported spot combinations have to be retrieved.

@param  aPossibleSpotCombinations
		Each member of this array is of type TUint and is a bit field where every bit represents presence or absence of
		a particular spot.
		
@leave  May leave with any error code.
	
@note   Since maximum number of spots can go up to KMaxNumberOfFocusSpots, it can be easily known which spots are present
		in a particular array entry (every array entry is a bit field and every bit represents presence or absence of a 
		particular spot). For example,if KMaxNumberOfFocusSpots is 32 and if a particular array entry is 0x07001000. This 
		means that spot number 13,25,26 and 27 are being represented. 
		
@see    CCameraViewFinder::GetSpotsPositionL
*/
void CCamera::CCameraPreImageCaptureControl::GetSupportedSpotsCombinationL(CCamera::CCameraAdvancedSettings::TFocusMode /*aFocusMode*/, RArray<TUint>& /*aPossibleSpotCombinations*/) const
	{
	User::Leave(KErrNotSupported);	
	}

/**
Retrieves the focussing spot combination which is used to provide focussing feedback in a particular focussing mode.

@param  aFocusMode
		The focussing mode in which the spot combination has to be used for retrieving feedback.
			
@param  aSpotsCombination
		The retrieved spot combination. It is a bitfield marking the presence or absence of spots.
		This is dependent on the focusssing mode.
	
@leave  May leave with any error code. 

@see    CCameraViewFinder::GetSpotsPositionL
*/
void CCamera::CCameraPreImageCaptureControl::GetSpotsCombinationL(CCamera::CCameraAdvancedSettings::TFocusMode /*aFocusMode*/, TUint& /*aSpotsCombination*/) const
	{
	User::Leave(KErrNotSupported);
	}

/**
Sets a particular spot combination which can be used to provide focussing feedback in a particular focussing mode.

@param  aFocusMode
		The focussing mode in which the spot combination has to be used for retrieving feedback.
		
@param  aSpotsCombination
		The spot combination to be set. It is a bitfield marking the presence or absence of spots.
		
@note   Event KUidECamEvent2ImageCaptureControlSpotCombination is used to notify the requesting client about setting of a particular
		spot combination.

@note   Event KUidECamEvent2ImageCaptureControlFocussingInformation is used to provide the focussing feedback, whenever applicable.

@see    CCameraViewFinder::GetSpotsPositionL
*/
void CCamera::CCameraPreImageCaptureControl::SetSpotsCombination(CCamera::CCameraAdvancedSettings::TFocusMode /*aFocusMode*/, TUint /*aSpotsCombination*/)
	{
	return;	
	}

/**
Retrieves whether the streamed image is supported or not. Sub-frames are used if streamed image output is used.

@param  aIsStreamedImageSupported
		ETrue indicates that streamed image is supported.
		EFalse indicates that streamed image is not supported.

@leave  May leave with any error code.	
*/
void CCamera::CCameraPreImageCaptureControl::GetStreamedImageSupportInfoL(TBool& /*aIsStreamedImageSupported*/) const
	{
	User::Leave(KErrNotSupported);	
	}

/**
Enables the streamed image output and hence the use of sub-frames to collect image data. MCameraImageBuffer will be 
used in place of MCameraBuffer in order to provide sub-frames details. Every MCaptureImageObserver::ImageBufferReady() 
will retrieve only one sub-frame. Full image will be reconstructed after receiving all the sub-frames.

@leave  May leave with any error code.

@note   The corresponding image capture call is considered to be complete only when every sub-frame has been received.
*/
void CCamera::CCameraPreImageCaptureControl::EnableSubFramesL()
	{
	User::Leave(KErrNotSupported);	
	}

/**
Disable the streamed image output and hence the use of sub-frames to collect image data. Every 
MCaptureImageObserver::ImageBufferReady() will retrieve one full still image.

@leave  May leave with any error code.
*/
void CCamera::CCameraPreImageCaptureControl::DisableSubFramesL()
	{
	User::Leave(KErrNotSupported);
	}

/**
Retrieves the current state about streamed image option or sub-frame usage.

@param  iIsSubFrameEnabled
		ETrue indicates that streamed image option is currently being used. 
		EFalse indicates that streamed image option is currently not being used.
		
@leave  May leave with any error code.
*/	
void CCamera::CCameraPreImageCaptureControl::GetSubFramesStateL(TBool& /*aIsSubFrameEnabled*/) const
	{
	User::Leave(KErrNotSupported);
	}

/**
Retrieves the list of transformations (image processing) available in case streamed image is used.

@param  aStreamedImageSupportedTransformations
		Retrieves an array of uids which represents those image processing attributes which are supported for streamed 
		image.
		
@leave  May leave with any error code.
*/
void CCamera::CCameraPreImageCaptureControl::GetStreamedImageSupportedTransformationsL(RArray<TUid>& /*aStreamedImageSupportedTransformations*/) const
	{
	User::Leave(KErrNotSupported);	
	}
	
/**
Retrieves the panorama mode support.

@param  aIsPanoModeSupported
		Informs whether the pano mode is supported or not.

@param  aSupportedStitchingOption
		Retrieves the supported image stitching option available with the ECAM implementation for the images captured
		under panorama mode. It is a TInt represepesting bitfield for supported TStitchingOption 
		If pano mode is not supported, this has to be 0.
		
@leave  May leave with any error code.			
*/
void CCamera::CCameraPreImageCaptureControl::GetPanoModeSupportInfoL(TBool& /*aIsPanoModeSupported*/, TInt& /*aSupportedStitchingOption*/) const
	{
	User::Leave(KErrNotSupported);	
	}

/**
Starts panning the viewfinder. Any image captured under pano mode gets overlaid on top of viewfinder and in a 
particular direction.

This method starts panorama mode for every viewfinder screen.

@param  aStitchingOption
		Enum value EStitchingOptionEnable instructs implementation to perform stitching of images captured under pano mode.
		So only one completion feedback will be given to the client regarding image being ready.(either through  
		MCaptureImageObserver::ImageBufferReady() or through MCaptureImageObserver::ImageDirectSavingCompleted())
		
		Enum value EStitchingOptionDisable instructs implemenation to provide the images unstitched.
		
		Enum value EStitchingOptionNone should be passed only when stitching option is not supported by the 
		implementation.
		
@note   Event KUidECamEventImageCaptureControlStartPanoMode is used to notify clients about the start of panorama mode.
		
@note   Calling the StartPanoMode twice and without stopping it will result in an error.
*/
void CCamera::CCameraPreImageCaptureControl::StartPanoMode(CCamera::CCameraPreImageCaptureControl::TStitchingOption /*aStitchingOption*/)
	{
	return;	
	}

/**
Starts panning the viewfinder. Any image captured under pano mode gets overlaid on top of viewfinder and in a 
particular direction.

This method starts panorama mode only on few specified viewfinder screens.

@param  aStitchingOption
		Enum value EStitchingOptionEnable instructs implementation to perform stitching of images captured under pano mode. So 
		only one completion feedback will be given to the client regarding image being ready.(either through  
		MCaptureImageObserver::ImageBufferReady() or through MCaptureImageObserver::ImageDirectSavingCompleted())
		
		Enum value EStitchingOptionDisable instructs implemenation to provide the images unstitched.
		
		Enum value EStitchingOptionNone should be passed only when stitching option is not supported by the 
		implementation.
		
@param  aVFHandles
		The array of viewfinder handles on which the panorama will run. Every member of this array is a valid viewfinder 
		handle.

@note   Event KUidECamEventImageCaptureControlStartPanoMode is used to notify clients about the start of panorama mode.		

@note   Calling the StartPanoMode twice and without stopping it will result in an error.		
*/
void CCamera::CCameraPreImageCaptureControl::StartPanoMode(CCamera::CCameraPreImageCaptureControl::TStitchingOption /*aStitchingOption*/, const RArray<TInt>& /*aVFHandles*/)
	{
	return;
	}
	
/**
Retrieves the direction of panning.

@param  aPanoDirection
		The currently used panning direction.

@leave  May leave with any error code.

@note   When the image is captured, the cropped version of the captured image covering a full rectangle is overlaid at 
		the left hand side of the display screen (if TPanoDirection is EPanoRight) and viewfinder runs below that. 	
		
@note   If the method leaves, the reference to CCamera::CCameraPreImageCaptureControl::TPanoDirection i.e. aPanoDirection is 
		not guaranteed to be valid.	
*/
void CCamera::CCameraPreImageCaptureControl::GetPanoDirectionL(CCamera::CCameraPreImageCaptureControl::TPanoDirection& /*aPanoDirection*/) const
	{
	User::Leave(KErrNotSupported);	
	}

/**
Sets the direction of panning

@param  aPanoDirection
		The desired panning direction. 
		
@note   Event KUidECamEventImageCaptureControlPanoDirection is used to notify the requesting client about setting of the 
		panning direction.
*/
void CCamera::CCameraPreImageCaptureControl::SetPanoDirection(CCamera::CCameraPreImageCaptureControl::TPanoDirection /*aPanoDirection*/)
	{
	return;	
	}

/**
Stop the panorama mode.

@param  aStitchedImageRetrieval
		Enum value EStitchedImageRetrieve indicates that images has to be stitched if initially image stitching was 
		enabled.
		
		Enum value EStitchedImageDiscard indicates that images should not be stitched (discarded) if initially image 
		stitching was enabled. No image ready notification returned to the client in this case. This may happen when 
		client is not happy with the panned images captured so far. 

@leave  May leave with any error code. 
				
@note   If image stitching option was not enabled while starting the pano mode, the 'aStitchedImageRetrieval' parameter is 
		ignored by the implementation.
*/
void CCamera::CCameraPreImageCaptureControl::StopPanoModeL(CCamera::CCameraPreImageCaptureControl::TStitchedImageRetrieval /*aStitchedImageRetrieval*/)
	{
	User::Leave(KErrNotSupported);
	}
		
/**
Retrieves the supported color space.

@param  aSupportedColorSpace
		A bitfield representing the supported TColorSpace
		
@leave  May leave with any error code.
*/
void CCamera::CCameraPreImageCaptureControl::GetSupportedColorSpaceL(TUint& /*aSupportedColorSpace*/) const
	{
	User::Leave(KErrNotSupported);	
	}

/**
Get the current color space being used.

@param  aColorSpace
		Currently used color space.
					
@leave  May leave with any error code. 
*/
void CCamera::CCameraPreImageCaptureControl::GetColorSpaceL(CCamera::CCameraPreImageCaptureControl::TColorSpace& /*aColorSpace*/) const
	{
	User::Leave(KErrNotSupported);	
	}

/**
Set a particular color space.

@param  aColorSpace
		The color space value to be set.
		
@note   Event KUidECamEventImageCaptureControlColorSpace is used to notify clients about setting the color space.
*/
void CCamera::CCameraPreImageCaptureControl::SetColorSpace(CCamera::CCameraPreImageCaptureControl::TColorSpace /*aColorSpace*/)
	{
	return;	
	}
	
/**
Synchronous marker method in order to provide selective settings for embedded still captures only (not to video capture)

@note   Any feature setting issued after calling this method gets saved on the implementation side and the actual 
		setting performed before still images (embedded still captures) are taken.

@note   Any getter method issued after calling this method gives feature value which would be used for still images 
	    (embedded still captures). For example, if WhiteBalance set for 'Auto' for EmbeddedStillCapture but 'Cloudy' otherwise, then
	    under the marker method, white balance will show 'Cloudy'.

@note   Event KUidECamEventFailedEmbeddedStillCaptureSetting notified to the client if in case any embedded still capture
		setting gets failed during the time they are actually set. After this notification, client can issue 
		GetFailedEmbeddedStillCaptureSettingsL to retrieve the list of failed settings.

@leave  May leave with any error code.
*/	
void CCamera::CCameraPreImageCaptureControl::StartEmbeddedStillCaptureSettingsL()
	{
	User::Leave(KErrNotSupported);	
	}

/**
Marker method to mark end of providing selective settings for embedded still captures only (not to video capture)

@note   Any feature setting issued after calling this method is treated as a normal setting operation. 	

@note   Any getter method issued after calling this method gives feature value which would be used normally.

@leave  May leave with any error code.
*/
void CCamera::CCameraPreImageCaptureControl::EndEmbeddedStillCaptureSettingsL()
	{
	User::Leave(KErrNotSupported);
	}

/**
Retrieves the list of failed embedded still capture settings.

@param  aFailedEmbeddedStillCaptureSettings
		Array of uids which reflects the failed embedded still capture settings. 
		
@note   This method may be issued after receiving the notification KUidECamEventFailedEmbeddedStillCaptureSetting.
*/	
void CCamera::CCameraPreImageCaptureControl::GetFailedEmbeddedStillCaptureSettingsL(RArray<TUid>& /*aFailedEmbeddedStillCaptureSettings*/) const
	{
	User::Leave(KErrNotSupported);
	}

/**
@publishedPartner
Factory function for creating the CCameraImageCapture object. This will be called only by the concrete implementation
of MCameraPreImageCaptureControl::PrepareImageCapture(const TPrepareImageParameters& aPrepareImageParameters, 
MCaptureImageObserver& aCaptureImageObserver).

@param aCamera 
	   a reference to a CCamera object providing the settings.

@param aPrepareImageParameters
	   TPrepareImageParameters tied with this image capture class object.
	   
@param aCaptureImageObserver
	   Reference to the capture image observer.
	   
@return a pointer to a fully constructed CCameraImageCapture object.

@leave KErrNoMemory Out of memory Or any other system-wide error code.
*/
EXPORT_C CCamera::CCameraImageCapture* CCamera::CCameraImageCapture::CreateL(CCamera& aCamera, const CCamera::CCameraPreImageCaptureControl
			::TPrepareImageParameters& aPrepareImageParameters, MCaptureImageObserver& aCaptureImageObserver)
	{
	CCamera::CCameraImageCapture* self = new (ELeave)CCamera::CCameraImageCapture(aCamera); 
	CleanupStack::PushL(self);
	self->ConstructL(aPrepareImageParameters, aCaptureImageObserver);
	CleanupStack::Pop(self);
	
	return self; 
	}
	
/**	
CCameraImageCapture Constructor.

@param aOwner
       a reference to a CCamera object. 
*/
CCamera::CCameraImageCapture::CCameraImageCapture(CCamera& aOwner):
				iOwner(aOwner), 
				iImpl(NULL)
	{
	}
	
/**
CCameraImageCapture second phase constructor. 

Function used to initialise internal state of the object. Uses reference to the camera to retrieve 
CameraImageCapture interface pointer.

@param aPrepareImageParameters
	   TPrepareImageParameters tied with this image capture class object.
	   
@param aCaptureImageObserver
	   Reference to the capture image observer.
	   
@leave KErrNoMemory Out of memory or any other system-wide error code
*/ 
void CCamera::CCameraImageCapture::ConstructL(const CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters& 
												aPrepareImageParameters, MCaptureImageObserver& aCaptureImageObserver) 
	{
	iImpl = static_cast<MCameraImageCapture*>(iOwner.CustomInterface(KECamMCameraImageCaptureUid));

	if (iImpl == NULL)
		{
		User::Leave(KErrNotSupported);
		}
		
	iImpl->SetPrepareImageParameters(aPrepareImageParameters);
	iImpl->SetCaptureImageObserver(aCaptureImageObserver);	
	}
	
/**
Returns the MCameraImageCapture* which will be used by the CCameraPostImageCaptureControl for the concrete 
implementation. Both CCameraImageCapture and CCameraPostImageCaptureControl are made to use the same interface
for implementation.

@return  Pointer to the MCameraImageCapture class.
*/
MCameraImageCapture* CCamera::CCameraImageCapture::Impl() const
	{
	return iImpl;
	}

/**
Destructor
Destruction of this class is equivalent to releasing the resources owned in order to prepare and allocate memory for 
capturing images.

@note  The child objects created out of this image capture class object shall be delete beforehand. Various child objects 
	   are snapshot, image processing and histograms.
*/	
EXPORT_C CCamera::CCameraImageCapture::~CCameraImageCapture()
	{
	if (iImpl != NULL)
		{
		//cancel any outstanding capture image operation at implementation level.
		//destory every CCameraPostImageCaptureControl object created by the implementation.
		iImpl->Release(this);	
		}
	}	

/**
Retrieve pointer to histogram API in order to use it specifically for a specific still image capture.

@return Pointer to use histogram API specifically for the given still image capture.
		
@leave  May leave with any error code.

@note   Different types of histogram may be used for a specific still image capture. Every time this method will be called
		on the CCameraImageCapture class object, a new type of histogram will be created.
*/	
EXPORT_C CCamera::CCameraV2Histogram* CCamera::CCameraImageCapture::CreateHistogramHandleL() const
	{
	MImplementationFactory* implFactory = NULL;
	
	iImpl->CreateHistogramImplFactoryL(implFactory);
	
	CleanupReleasePushL(*implFactory);
	CCamera::CCameraV2Histogram* histogram = CCamera::CCameraV2Histogram::CreateL(iOwner, *implFactory);
	CleanupStack::Pop(implFactory);
	
	implFactory->Release();
	return histogram;	
	}
	
/**
Retrieve pointer to snapshot API in order to use it specifically for a specific still image capture.

@param aClientViewFinderId
	   The client viewfinder on which this client snapshot will be displayed.

@return Pointer to use snapshot API specifically for the given still image capture.
		
@leave  May leave with any error code.
*/
EXPORT_C CCamera::CCameraSnapshot* CCamera::CCameraImageCapture::GetSnapshotHandleL(TInt aClientViewFinderId) const
	{
	MImplementationFactory* implFactory = NULL;
	
	iImpl->GetSnapshotImplFactoryL(implFactory);
	
	CleanupReleasePushL(*implFactory);
	CCamera::CCameraSnapshot* snapshot = CCamera::CCameraSnapshot::CreateL(iOwner, *implFactory, aClientViewFinderId);
	CleanupStack::Pop(implFactory);
	
	implFactory->Release();
	
	return snapshot;	
	}

/**
Retrieve pointer to image processing API in order to use it specifically for tha specific still image capture.

@return Pointer to use image processing API specifically for the given still image capture.
		
@leave  May leave with any error code.
*/
EXPORT_C CCamera::CCameraImageProcessing* CCamera::CCameraImageCapture::GetTransformationHandleL() const
	{
	MImplementationFactory* implFactory = NULL;
	
	iImpl->GetImageProcessingImplFactoryL(implFactory);
	
	CleanupReleasePushL(*implFactory);
	CCamera::CCameraImageProcessing* imgProc = CCamera::CCameraImageProcessing::CreateL(iOwner, *implFactory);
	CleanupStack::Pop(implFactory);
	
	implFactory->Release();
	return imgProc;	
	}

/**
Retrieves the prepare image parameters tied with this image capture class object.

@param aPrepareImageParameters
	   TPrepareImageParameters tied with this image capture class object.
	   
@leave May leave with any error code
*/
EXPORT_C void CCamera::CCameraImageCapture::GetPrepareImageParametersL(CCamera::CCameraPreImageCaptureControl::TPrepareImageParameters& aPrepareImageParameters) const
	{
	iImpl->GetPrepareImageParametersL(aPrepareImageParameters);
	}

/**
Performant image capture. This postpones the processing options involved with current image captured in order to 
capture/prepare for next images. 

Previously created CCameraPostImageCaptureControl objects for this CCameraImageCapture object will become unavailable 
after this call.

@note   Further images (either still or video) can be captured only after receiving the notification 
		KUidECamEventReadyForNextCapture. Current image may be outstanding in order to undergo any processing options.

@note   Callback MCaptureImageObserver::ImageCaptureComplete() informs the client that the image capture operation has 
		been completed. The CCameraImageCapture& can be further used for next image captures.
		In case of continuous drive modes, this callback will be received by the client only once in order to provide the 
		result of the whole image capture operation. 
		
@note   Callback MCaptureImageObserver::ImageBufferReady() provides client the link to CCameraPostImageCaptureControl& 
		which helps retrieving the image buffer. In case of continuous drive modes, this callback will be received by the
		client for every individual image. In case of single shots, this will be received by the client only once in 
		order to retrieve the image buffer.
*/	
EXPORT_C void CCamera::CCameraImageCapture::CaptureImage()
	{
	// MCameraImageCapture concrete implementation will use CCameraImageCapture& to pass in callbacks and to create 
	// CCameraPostImageCaptureControl object for every image and pass it to the client.
	// Previously created CCameraPostImageCaptureControl objects will become unavailable after this call.
 	iImpl->CaptureImage(this);
	}

/**
Cancels the outstanding Capture Image operation. This will also cancel any outstanding processing options associated with 
the concerned Capture Image operation. This method is synchronous. Hence, no callback shall be received with respect
to concerned capture image operation.
*/	
EXPORT_C void CCamera::CCameraImageCapture::CancelCaptureImage()
	{
	iImpl->CancelCaptureImage();
	}

/**
Retrieves the number of images exposed to sensor with respect to a specific image capture command. Any processing options
associated with these images may be pending.

@param  aNumImagesExposed
		Retrieves the number of images exposed to sensor till now. For single shot type of drive modes, this value will be
		one after the image gets exposed to sensor. For multiple shot type of drive modes, this value will be the number of 
		images exposed till now.
		
@note   May leave with any error code.
*/
EXPORT_C void CCamera::CCameraImageCapture::GetNumImagesExposedL(TUint& aNumImagesExposed) const
	{
	iImpl->GetNumImagesExposedL(aNumImagesExposed);	
	}

/**
Retrieves the total number of images which is supposed to be captured with respect to a specific image capture command. 

@param  aNumTotalImages
		Retrieves the total number of images supposed to be captured. For single shot type of drive modes, this value will be
		always be one. For multiple shot type of drive modes, this value will be greater than one.
		
@note   May leave with any error code.
*/
EXPORT_C void CCamera::CCameraImageCapture::GetNumTotalImagesL(TUint& aNumTotalImages) const
	{
	iImpl->GetNumTotalImagesL(aNumTotalImages);	
	}

/**
Retrieves the post capture control handle based on the ID passed to it. Client may use this handle for post capture 
operations of the image identified by the ID. The ID is received by the clients through MCaptureImageObserver callbacks.

@param  aPostCaptureControlHandle
		Retrieves pointer to the post capture control class object.
		
@param  aPostCaptureControlId
		The ID used to retrieve the post capture control handle of the captured image.
		
@note   May leave with any error code.
*/	
EXPORT_C void CCamera::CCameraImageCapture::GetPostCaptureControlHandleL(CCamera::CCameraPostImageCaptureControl*& 
															aPostCaptureControlHandle, TPostCaptureControlId aPostCaptureControlId) const
	{
	iImpl->GetPostCaptureControlHandleL(aPostCaptureControlHandle, aPostCaptureControlId);	
	}													

/**
Prioritises the Capture Image operation. This implies that any processing options associated with the concerned Capture
Image operation will be prioritised over other pending processing options.

@param  aCaptureImagePriority
		The desired level of priority.

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraImageCapture::SetCaptureImagePriorityL(TECamImagePriority aCaptureImagePriority)
	{
	iImpl->SetCaptureImagePriorityL(aCaptureImagePriority);
	}
	
/**
Retrieves the priority of the Capture Image operation.

@param  aCaptureImagePriority
		Retrieves the current level of priority.

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraImageCapture::GetCaptureImagePriorityL(TECamImagePriority& aCaptureImagePriority) const
	{
	iImpl->GetCaptureImagePriorityL(aCaptureImagePriority);
	}

/**
Pauses processing options associated with the concerned Capture Image operation.

@param  aProcessingTypes
		The processing options which need to be paused.
*/	
EXPORT_C void CCamera::CCameraImageCapture::PauseProcessing(TUint aProcessingTypes)
	{
	iImpl->PauseProcessing(aProcessingTypes);
	}

/**
Resumes processing options associated with the concerned Capture Image operation.

@param  aProcessingTypes
		The processing options which need to be resumed.

@leave  May leave with any error code.
*/	
EXPORT_C void CCamera::CCameraImageCapture::ResumeProcessingL(TUint aProcessingTypes)
	{
	iImpl->ResumeProcessingL(aProcessingTypes);
	}

/**
@publishedPartner

Factory function for creating the CCameraPostImageCaptureControl object for every individual image to be captured.
This method is supposed to be called only by concrete implementation of MCameraImageCapture. It will be the client's 
responsibility to destroy it whenever it does not need it any more. (Possible milestone for deletion: Individual image cancellation completed, 
individual image saved). Previously created CCameraPostImageCaptureControl objects for the CCameraImageCapture object will
become unavailable if CaptureImage() is called while previous call on the same object is still outstanding.

@param aCameraImageCapture 
	   A pointer to the CCameraImageCapture which was used to issue the Capture Image call.
	   
@param aPostCaptureControlId
	   The ID used for identification of the post capture control class object by the clients. The ID is passed by the
	   implementation while creating this object.

@return a pointer to a fully constructed CCameraPostImageCaptureControl object.

@leave KErrNoMemory Out of memory Or any other system-wide error code.
*/	
EXPORT_C CCamera::CCameraPostImageCaptureControl* CCamera::CCameraPostImageCaptureControl::CreateL(CCameraImageCapture* aCameraImageCapture, TPostCaptureControlId aPostCaptureControlId)
	{
	CCamera::CCameraPostImageCaptureControl* self = new (ELeave)CCamera::CCameraPostImageCaptureControl(aCameraImageCapture, aPostCaptureControlId); 
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	return self;
	}

/**	
CCameraPostImageCaptureControl Constructor.

@param aOwner
       a reference to a CCamera object. 

@param aPostCaptureControlId
	   The ID used for identification of the post capture control class object by the clients. The ID is passed by the
	   implementation while creating this object.
*/
CCamera::CCameraPostImageCaptureControl::CCameraPostImageCaptureControl(CCamera::CCameraImageCapture* aCameraImageCapture, TInt aPostCaptureControlId):
 																		iPostCaptureControlId(aPostCaptureControlId),
																		iCameraImageCapture(aCameraImageCapture),
																		iImpl(NULL)
	{
	}
	
/**
CCameraPostImageCaptureControl second phase constructor. 

Function used to initialise internal state of the object. Uses CCameraImageCapture* to retrieve 
CCameraPostImageCaptureControl interface pointer. This interface pointer is provided by 'CCameraImageCapture interface'.

@leave KErrNoMemory Out of memory or any other system-wide error code
*/ 
void CCamera::CCameraPostImageCaptureControl::ConstructL() 
	{
	iImpl = static_cast<MCameraPostImageCaptureControl*>(iCameraImageCapture->Impl()->CreatePostImageCaptureControlImpl(KECamMCameraPostImageCaptureControlUid, iPostCaptureControlId));

	if (iImpl == NULL)
		{
		User::Leave(KErrNotSupported);
		}
	}

/**
Destructor
*/		
EXPORT_C CCamera::CCameraPostImageCaptureControl::~CCameraPostImageCaptureControl()
	{
	iCameraImageCapture = NULL;
	if(iImpl != NULL)
		{
		//cancel any outstanding image at implementation level.
		iImpl->Release(iPostCaptureControlId);
		}
	}

/**
Retrieves the ID which this class object represents. The ID is unique for this class object and is used for identification
purpose.

@param  aPostCaptureControlId
		Reference to the post capture control ID.
*/	
EXPORT_C void CCamera::CCameraPostImageCaptureControl::GetPostCaptureControlId(TPostCaptureControlId& aPostCaptureControlId) const
	{
	aPostCaptureControlId = iPostCaptureControlId;
	}
	
/**
Retrieves the CCameraImageCapture* associated with the object of this class. This implies that the individual image, which
the object of this class is referring to, belongs to the CaptureImage operation issued by CCameraImageCapture* object.

@return Pointer to CCameraImageCapture associated with the object of this class.
*/
EXPORT_C CCamera::CCameraImageCapture* CCamera::CCameraPostImageCaptureControl::ImageCaptureHandle() const
	{
	return iCameraImageCapture;
	}

/**
Retrieves the sequence number of the image being represented by this post capture control object. The sequence number
specifies the index of the individual image if, in case, the multiple shot type of drive mode is being used. The first 
individual image which is exposed to the sensor has a sequence number of zero. In case of single shot type of drive mode,
the sequence number will be zero.

@param  aSequenceNumber
		Retrieves the sequence number of the image.

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraPostImageCaptureControl::GetImageSequenceNumberL(TUint& aSequenceNumber) const
	{
	iImpl->GetImageSequenceNumberL(aSequenceNumber);
	}

/**
Cancels the outstanding individual image which the object of this class is referring to. This will also cancel any outstanding 
processing options associated with this individual image. This method is synchronous. Hence, no callback shall be received
for this individual image.
*/
EXPORT_C void CCamera::CCameraPostImageCaptureControl::CancelImage()
	{
	iImpl->CancelImage();
	}

/**
Prioritises the individual image which the object of this class is referring to. This implies that any processing
options associated with this individual image will be prioritised over other pending processing options.

@param  aImagePriority
		The desired level of priority.
		
@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraPostImageCaptureControl::SetImagePriorityL(TECamImagePriority aImagePriority)
	{
	iImpl->SetImagePriorityL(aImagePriority);	
	}
	
/**
Retrieves the priority of the individual image which the object of this class is referring to.

@param  aImagePriority
		The current level of priority.
		
@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraPostImageCaptureControl::GetImagePriorityL(TECamImagePriority& aImagePriority) const
	{
	iImpl->GetImagePriorityL(aImagePriority);	
	}
	
/**
Pauses processing options associated with the individual image which the object of this class is referring to.

@param  aProcessingTypes
		The processing options which need to be paused.
*/	
EXPORT_C void CCamera::CCameraPostImageCaptureControl::PauseProcessing(TUint aProcessingTypes)
	{
	iImpl->PauseProcessing(aProcessingTypes);
	}

/**
Resumes processing options associated with the individual image which the object of this class is referring to.

@param  aProcessingTypes
		The processing options which need to be resumed.

@leave  May leave with any error code.
*/	
EXPORT_C void CCamera::CCameraPostImageCaptureControl::ResumeProcessingL(TUint aProcessingTypes)
	{
	iImpl->ResumeProcessingL(aProcessingTypes);
	}

/**
Retrieves the image buffer which contains the individual image captured. This method should be called by the client as a 
result of receiving the callback MCaptureImageObserver::ImageBufferReady(), if the error information received is 
satisfactory.

@param  aCameraImageBuffer
		A reference to an MCameraImageBuffer if successful, or NULL if not successful.

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraPostImageCaptureControl::GetImageBufferL(MCameraImageBuffer& aCameraImageBuffer) const
	{
	iImpl->GetImageBufferL(aCameraImageBuffer);
	}

/**
Retrieves the state of the individual image which the object of this class is referring to.

@param  aImageState
		Retrieves the current state of the individual image.
		
@leave  May leave with any error code.
*/	
EXPORT_C void CCamera::CCameraPostImageCaptureControl::GetImageStateL(TImageState& aImageState) const
	{
	iImpl->GetImageStateL(aImageState);		
	}
	
/**
Retrieves the state of the individual image buffer.

@param  aBufferState
		Retrieves the current state of the individual image buffer.
		
@leave  May leave with any error code.
*/	
EXPORT_C void CCamera::CCameraPostImageCaptureControl::GetBufferStateL(TBufferState& aBufferState) const
	{
	iImpl->GetBufferStateL(aBufferState);		
	}
	
/**
Factory function for creating the CCameraVideoCaptureControl object.

@param aCamera 
	   A reference to a CCamera object providing the settings.
	   
@param aCaptureVideoObserver
	   Reference to the capture video observer.

@return a pointer to a fully constructed CCameraVideoCaptureControl object.

@leave KErrNoMemory Out of memory Or any other system-wide error code.

@leave KErrExtensionNotSupported When NewL/NewDuplicateL used instead of New2L/NewDuplicate2L.
*/
EXPORT_C CCamera::CCameraVideoCaptureControl* CCamera::CCameraVideoCaptureControl::NewL(CCamera& aCamera, MCaptureVideoObserver& aCaptureVideoObserver)
	{
 	if(aCamera.CameraVersion() == KCameraDefaultVersion)
 		{
 		User::Leave(KErrExtensionNotSupported);
 		}
 		
	CCamera::CCameraVideoCaptureControl* self = new (ELeave)CCamera::CCameraVideoCaptureControl(aCamera); 
	CleanupStack::PushL(self);
	self->ConstructL(aCaptureVideoObserver);
	CleanupStack::Pop(self);
	
	return self; 
	}
	
/**	
CCameraVideoCaptureControl Constructor.

@param aOwner
       a reference to a CCamera object. 
*/
CCamera::CCameraVideoCaptureControl::CCameraVideoCaptureControl(CCamera& aOwner):iOwner(aOwner), iImpl(NULL)
	{
	}

/**
CCameraVideoCaptureControl second phase constructor. 

Function used to initialise internal state of the object. Uses reference to the camera to retrieve 
Camera Video Capture Control interface pointer.

@param aCaptureVideoObserver
	   Reference to the capture video observer.

@leave KErrNoMemory Out of memory. 
*/ 
void CCamera::CCameraVideoCaptureControl::ConstructL(MCaptureVideoObserver& aCaptureVideoObserver) 
	{
	iImpl = static_cast<MCameraVideoCaptureControl*>(iOwner.CustomInterface(KECamMCameraVideoCaptureControlUid));

	if (iImpl == NULL)
		{
		User::Leave(KErrNotSupported);
		}
		
	iImpl->SetCaptureVideoObserver(aCaptureVideoObserver);	
	}

/**
Destructor

@note  The child objects created out of this video capture control class object shall be delete beforehand. Various child 
	   objects are snapshot, image processing and histograms.
*/	
EXPORT_C CCamera::CCameraVideoCaptureControl::~CCameraVideoCaptureControl()
	{
	if (iImpl != NULL)
		{
		iImpl->Release();	
		}
	}	

/**
Retrieve pointer to histogram API in order to use it specifically for the video capture.

@return Pointer to use histogram API specifically for the video capture.
		
@leave  May leave with any error code.

@note   Different types of histogram may be used for video capture. Every time this method will be called
		on the CCameraVideoCaptureControl class object, a new type of histogram will be created.
*/	
EXPORT_C CCamera::CCameraV2Histogram* CCamera::CCameraVideoCaptureControl::CreateHistogramHandleL() const
	{
	MImplementationFactory* implFactory = NULL;
	
	iImpl->CreateHistogramImplFactoryL(implFactory);
	
	CleanupReleasePushL(*implFactory);
	CCamera::CCameraV2Histogram* histogram = CCamera::CCameraV2Histogram::CreateL(iOwner, *implFactory);
	CleanupStack::Pop(implFactory);
	
	implFactory->Release();
	return histogram;	
	}


/**
Retrieve pointer to snapshot API in order to use it specifically for the video capture.

@param aClientViewFinderId
	   The client viewfinder on which this client snapshot will be displayed.

@return Pointer to use snapshot API specifically for the video capture.
		
@leave  May leave with any error code.
*/
EXPORT_C CCamera::CCameraSnapshot* CCamera::CCameraVideoCaptureControl::GetSnapshotHandleL(TInt aClientViewFinderId) const
	{
	MImplementationFactory* implFactory = NULL;
	
	iImpl->GetSnapshotImplFactoryL(implFactory);
	
	CleanupReleasePushL(*implFactory);
	CCamera::CCameraSnapshot* snapshot = CCamera::CCameraSnapshot::CreateL(iOwner, *implFactory, aClientViewFinderId);
	CleanupStack::Pop(implFactory);
	
	implFactory->Release();

	return snapshot;	
	}

/**
Retrieve pointer to image processing API in order to use it specifically for the video capture.

@return Pointer to use image processing API specifically for the video capture.
		
@leave  May leave with any error code.
*/
EXPORT_C CCamera::CCameraImageProcessing* CCamera::CCameraVideoCaptureControl::GetTransformationHandleL() const
	{
	MImplementationFactory* implFactory = NULL;
	
	iImpl->GetImageProcessingImplFactoryL(implFactory);
	
	CleanupReleasePushL(*implFactory);
	CCamera::CCameraImageProcessing* imgProc = CCamera::CCameraImageProcessing::CreateL(iOwner, *implFactory);
	CleanupStack::Pop(implFactory);
	
	implFactory->Release();
	return imgProc;	
	}
	
/**
Retrieves the supported video formats for a given resolution.

@param  aVideoFormatsSupported
		A bit field which retrieves the supported video formats for a given resolution.
		Formats have been defined as CCamera::TFormat
		
@param  aSize
		The resolution (or size) for which the total number of supported video formats have to be retrieved.

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraVideoCaptureControl::GetVideoFormatsSupportedL(TUint& aVideoFormatsSupported, const TSize& aSize) const
	{
	iImpl->GetVideoFormatsSupportedL(aVideoFormatsSupported, aSize);
	}

/**
Retrieves the supported pixel aspect ratio for a given resolution in case of video.

@param  aPixelAspectsSupported
		A bit field which retrieves the supported pixel aspect ratio for a given resolution.
		Pixel aspect ratio have been defined as CCamera::CCameraAdvancedSettings::TPixelAspectRatio
		
@param	aVideoFormat
		The video format for which the supported pixel aspect ratio have to be retrieved.

@param  aSize
		The resolution (or size) for which the supported pixel aspect ratio have to be retrieved.

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraVideoCaptureControl::GetPixelAspectsSupportedL(TUint& aPixelAspectsSupported, CCamera::TFormat aVideoFormat, const TSize& aSize) const
	{
	iImpl->GetPixelAspectsSupportedL(aPixelAspectsSupported, aVideoFormat, aSize);
	}

/**
Informs whether or not the 'embedded still capture' feature is supported. Allowing still image capture in between on-going
video capture is referred to as embedded still capture.

@param  aSupportedEmbeddedStillCaptureTypes
		Retrieves the supported embedded still capture. The TInt is retrieved as a bit field of supported TEmbeddedStillCaptureTypes.
		
@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraVideoCaptureControl::GetEmbeddedStillCaptureSupportInfoL(TInt& aSupportedEmbeddedStillCaptureTypes) const
	{
	iImpl->GetEmbeddedStillCaptureSupportInfoL(aSupportedEmbeddedStillCaptureTypes);	
	}

/**
Asynchronous method to prepare for video capture.

Performs setup and allocation of memory prior to calling StartVideoCapture() to keep the latency of that function to a 
minimum.

@param  aPrepareVideoParameters 
 		Parameters necessary to prepare for video capture.
 		
@note   Event KUidECamEventVideoCaptureControlPrepareComplete is used to notify clients about completing the preparation
		for video capture.
		
@note 	Next PrepareVideoCapture can be called only after receiving the notification KUidECamEventReadyForNextPrepare.
		
@note   If some camera settings get affected because of desired video settings such as frame rate/frame size, 
		specific notifications will be sent to the client about camera settings being affected.
		
@note	Event KUidECamEventVideoCaptureControlSettingsRangeChanged: informs that range of certain camera settings have been changed.
		Client may call GetRangeAffectedSettingsL(RArray<TUid>& aRangeAffectedSettings) const to get the list of affected camera settings.

@note	Event KUidECamEventVideoCaptureControlSettingsValueChanged: informs that value of certain camera settings have been changed. 
		Client may call GetValueAffectedSettingsL(RArray<TUid>& aValueAffectedSettings) const to get the list of affected camera settings.

@note	Event KUidECamEventVideoCaptureControlSettingsDisabled: informs that certain camera settings have been disabled. 
		Client may call GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const to get the list of affected camera settings.

@see 	CCamera::PrepareVideoCaptureL(TFormat aFormat,TInt aSizeIndex,TInt aRateIndex,TInt aBuffersToUse,TInt aFramesPerBuffer)

@see    ReleaseVideoResource()
*/
EXPORT_C void CCamera::CCameraVideoCaptureControl::PrepareVideoCapture(const CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters& aPrepareVideoParameters)
	{
	iImpl->PrepareVideoCapture(aPrepareVideoParameters);
	}

/**
Retrieves the camera settings whose range got affected once the desired video settings (frame rate/frame size) are in place. 
This method may be called by the client after receiving the notification KUidECamEventVideoCaptureControlSettingsRangeChanged.

@param  aRangeAffectedSettings
		Retrieves the list of range affected settings
		
@leave  May leave with any error code.  

@see 	PrepareVideoCapture(const TPrepareVideoParameters& aPrepareVideoParameters)
*/
EXPORT_C void CCamera::CCameraVideoCaptureControl::GetRangeAffectedSettingsL(RArray<TUid>& aRangeAffectedSettings) const
	{
	iImpl->GetRangeAffectedSettingsL(aRangeAffectedSettings);	
	}
	
/**
Retrieves the camera settings whose value got affected once the desired video settings (frame rate/frame size) are in place. 
This method may be called by the client after receiving the notification KUidECamEventVideoCaptureControlSettingsValueChanged.

@param  aValueAffectedSettings
		Retrieves the list of value affected settings

@leave  May leave with any error code.

@see 	PrepareVideoCapture(const TPrepareVideoParameters& aPrepareVideoParameters)
*/
EXPORT_C void CCamera::CCameraVideoCaptureControl::GetValueAffectedSettingsL(RArray<TUid>& aValueAffectedSettings) const
	{
	iImpl->GetValueAffectedSettingsL(aValueAffectedSettings);	
	}
	
/**
Retrieves the camera settings whose value got affected once the desired video settings (frame rate/frame size) are in place. 
This method may be called by the client after receiving the notification KUidECamEventVideoCaptureControlSettingsDisabled.

@param  aDisabledSettings
		Retrieves the list of disabled settings

@leave  May leave with any error code.

@see 	PrepareVideoCapture(const TPrepareVideoParameters& aPrepareVideoParameters)
*/
EXPORT_C void CCamera::CCameraVideoCaptureControl::GetDisabledSettingsL(RArray<TUid>& aDisabledSettings) const
	{
	iImpl->GetDisabledSettingsL(aDisabledSettings);	
	}

/**
Frees the video resources which were set up as a result of CCameraVideoCaptureControl::PrepareVideoCapture call.
If this methid is called while PrepareVideoCapture call is outstanding, then this will be equivalent to cancelling the
PrepareVideoCapture call and release any allocated resources.

@see 	PrepareVideoCapture(const TPrepareVideoParameters& aPrepareVideoParameters)
*/
EXPORT_C void CCamera::CCameraVideoCaptureControl::ReleaseVideoResource()
	{
	iImpl->ReleaseVideoResource();
	}

/**
Starts the video capture. This operation gives priority to the low latency aspects.

Video frames are send to client via MCaptureVideoObserver::VideoBufferReady().

@leave  May leave with any error code.

@note	This method is recommended to be used rather than CCamera::StartVideoCaptureL().
*/												
EXPORT_C void CCamera::CCameraVideoCaptureControl::StartVideoCaptureL()
	{
	iImpl->StartVideoCaptureL();	
	}

/**
Stops the video capture. This operation gives priority to the low latency aspects.

@note	This method is recommended to be used rather than CCamera::StopVideoCapture().
*/												
EXPORT_C void CCamera::CCameraVideoCaptureControl::StopVideoCapture()
	{
	iImpl->StopVideoCapture();	
	}

/**
Pauses the on-going video capture. MCaptureVideoObserver::VideoBufferReady() 
callback will not be received by the client until the video capture is resumed.
*/
EXPORT_C void CCamera::CCameraVideoCaptureControl::PauseVideoCapture()
	{
	iImpl->PauseVideoCapture();	
	}
	
/**
Resumes the on-going video capture. MCaptureVideoObserver::VideoBufferReady() 
callback will again be received by the client.

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraVideoCaptureControl::ResumeVideoCaptureL()
	{
	iImpl->ResumeVideoCaptureL();	
	}

/**
Retrieves the fading effect state for video capture.

@param  aFadingEffectState
		Retrieves the current fading effect state for video capture.

@leave  May leave with any error code.

@internalTechnology
*/
EXPORT_C void CCamera::CCameraVideoCaptureControl::GetFadingEffectStateL(CCamera::CCameraVideoCaptureControl::TFadingEffectState& aFadingEffectState) const
	{
	iImpl->GetFadingEffectStateL(aFadingEffectState);	
	}

/**
Sets the fading effect state for video capture.

@param  aFadingEffectState
		The desired fading effect state for video capture.
		
@note   Triggers a KUidECamEventVideoCaptureControlFadingEffect event notification.

@internalTechnology
*/	
EXPORT_C void CCamera::CCameraVideoCaptureControl::SetFadingEffectState(CCamera::CCameraVideoCaptureControl::TFadingEffectState aFadingEffectState)
	{
	iImpl->SetFadingEffectState(aFadingEffectState);
	}

/**
Retrieves the current video capture state.

@param aVideoCaptureState
	   Retrieves the current video capture state.

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraVideoCaptureControl::GetVideoCaptureStateL(CCamera::CCameraVideoCaptureControl::TVideoCaptureState& aVideoCaptureState) const
	{
	iImpl->GetVideoCaptureStateL(aVideoCaptureState);		
	}

/**
Retrieves the various types of video capture supported. 
@param  aSupportedVideoCaptureTypes
		Retrieves the supported video capture type. The TInt is retrieved as a bit field of supported 
		TVideoCaptureType.
		
@leave  May leave with any error code.
*/	
EXPORT_C void CCamera::CCameraVideoCaptureControl::GetVideoCaptureSupportInfoL(TInt& aSupportedVideoCaptureTypes) const
	{
	iImpl->GetVideoCaptureSupportInfoL(aSupportedVideoCaptureTypes);	
	}
	
/*
Retrieves the current prepare video parameters.

@param aPrepareVideoParameters
	   Retrieves the current prepare video parameters.
	   	
@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraVideoCaptureControl::GetPrepareVideoParametersL(CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters& aPrepareVideoParameters) const
	{
	iImpl->GetPrepareVideoParametersL(aPrepareVideoParameters);	
	}
	
/**
Constructor for the TPrepareVideoParameters class.
Sets the size and version of this class.
*/
EXPORT_C CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters::TPrepareVideoParameters()
	{
	iSize = sizeof(CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters);
	iVersion = KECamPrepareVideoParametersCurrentVersion;
	}

/**	
Returns the size of the class. Used for extensibility by deriving from this base class and adding new member variables.
Intended to be used for implementation of methods where this class reference is passed as function arguments. 
Implementation of such methods can find out the whether the actual class passed is base or the derived one. So, if a new application 
is made to run on an old implementation, an error may occur in such cases after the old implementation detects this by getting 
the size information of the T class passed. Also, if old application is made to run on a new implementation, this could be 
properly handled if the derived class variables handling is done in a proper 'if-else' statement.

@return The size of the class.

@note The size will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters::Size() const
	{
	return iSize;	
	}

/**	
Returns the version of the class. Used for extensibility specially when the class members are not added but the Reserved 
members get used at a later stage.

@return The version of this class.

@note The version will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters::Version() const
	{
	return iVersion;	
	}

/** 
Indicates whether or not the embedded still image capture state is enabled.
		
@return Whether or not the embedded still image capture state is enabled.

@see CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters::iIsEmbeddedStillCaptureEnabled
*/		
EXPORT_C TBool CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters::IsEmbeddedStillCaptureEnabled() const
	{
	if (iIsEmbeddedStillCaptureEnabled)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}	
	}
	
/** 
Sets the state to inform whether or not the embedded still image capture state is enabled.

@param  aIsEmbeddedStillCaptureEnabled
		ETrue implies embedded still capture state is enabled.
		EFalse implies embedded still capture state is disabled.
		
@see CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters::iIsEmbeddedStillCaptureEnabled
*/
EXPORT_C void CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters::SetEmbeddedStillCaptureState(TBool aIsEmbeddedStillCaptureEnabled)
	{
	iIsEmbeddedStillCaptureEnabled 	= static_cast<TUint>(aIsEmbeddedStillCaptureEnabled);
	}

/**
Retrieves the current video capture type used.

@param  aVideoCaptureType
		Retrieves the current video capture type.
		
@see CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters::iVideoCaptureType
*/
EXPORT_C CCamera::CCameraVideoCaptureControl::TVideoCaptureType CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters::VideoCaptureType() const
	{
	switch(iVideoCaptureType)	
		{
		case KECamVideoCaptureBit0:
			{
			return CCamera::CCameraVideoCaptureControl::EVideoCaptureNotSupported;
			}
		case KECamClientVideoCaptureBit1:
			{
			return CCamera::CCameraVideoCaptureControl::EClientVideoCapture;
			}
		case KECamDirectVideoCaptureBit2:
			{
			return CCamera::CCameraVideoCaptureControl::EDirectVideoCapture;
			}			
		default:
			{
			return CCamera::CCameraVideoCaptureControl::EVideoCaptureNotSupported;	
			}
		}
	}

/**
Sets the desired video capture type.

@param  aVideoCaptureType
		The desired video capture type.
		
@see CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters::iVideoCaptureType
*/
EXPORT_C void CCamera::CCameraVideoCaptureControl::TPrepareVideoParameters::SetVideoCaptureType(CCamera::CCameraVideoCaptureControl::TVideoCaptureType aVideoCaptureType)
	{
	iVideoCaptureType = static_cast<TUint>(aVideoCaptureType);
	}

/**
Retrieves the supported YUV conversion coefficient in case of video.

@param  aSupportedConversionCoefficients
		A bit field which retrieves the supported YUV conversion coefficient.
		YUV conversion coefficients have been defined as TYuvCoefficients

@leave  May leave with any error code.
*/
void CCamera::CCameraVideoCaptureControl::GetSupportedConversionCoefficientsL(TUint& /*aSupportedConversionCoefficients*/) const
	{
	User::Leave(KErrNotSupported);
	}

/**
Retrieves the currently used YUV conversion coefficients.

@param  aConversionCoefficients
		Currently used TYuvCoefficients
		
@leave  May leave with any error code.
*/
void CCamera::CCameraVideoCaptureControl::GetConversionCoefficientL(TYuvCoefficients& /*aConversionCoefficients*/) const
	{
	User::Leave(KErrNotSupported);		
	}

/**
Asynchronous method which sets the desired YUV conversion coefficients.

@param  aConversionCoefficients
		The desired TYuvCoefficients
		
@note   Event KUidECamEventVideoCaptureControlConversionCoefficient is used to notify clients about setting of desired YUV 
		conversion coefficients.
*/
void CCamera::CCameraVideoCaptureControl::SetConversionCoefficient(TYuvCoefficients /*aConversionCoefficients*/)
	{
	return;	
	}
