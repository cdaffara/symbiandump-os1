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

/**
 @file
 @publishedPartner
 @prototype
*/

#include <ecam/mcamerahistogram.h>
#include "ecamversion.h"
#include <ecam/implementationfactoryintf.h>

/**
@deprecated use static CCamera::CCameraV2Histogram* CreateL(CCamera& aCamera, MImplementationFactory& aImplFactory)

Factory function that creates a new camera histogram object on the heap.

@param	aCamera
		A reference to the camera object for which a camera histogram object is to be created.

@leave  KErrNoMemory if out of memory; also any system wide error.

@return A pointer to the newly created camera histogram object.

@note  Clients using MCameraObserver are not recommended to use this extension class since they cannot handle events.
*/
EXPORT_C CCamera::CCameraHistogram* CCamera::CCameraHistogram::NewL(CCamera& aCamera)
	{
	CCamera::CCameraHistogram* self = new (ELeave) CCamera::CCameraHistogram(aCamera);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}
	
/**
@deprecated use void CCamera::CCameraV2Histogram::ConstructL(const MImplementationFactory& aImplFactory)

CCameraHistogram second phase constructor. 

This function used to initialise internal state of the object. 
It uses reference to the camera to retrieve histogram interface pointer.

@leave KErrNotSupported if this functionality is not supported; also any system wide error.
*/ 
void CCamera::CCameraHistogram::ConstructL()
	{
	iImpl = static_cast<MCameraHistogram*>(iOwner.CustomInterface(KECamMCameraHistogramUid));

	if (iImpl == NULL)
		{
		User::Leave(KErrNotSupported);
		}
	}
	
/**
@deprecated use CCamera::CCameraV2Histogram::CCameraV2Histogram(CCamera& aOwner)

Constructor for the CCamera::CCameraHistogram class.

@param aOwner
		A reference to the camera object for which a camera histogram object is to be created.
*/
CCamera::CCameraHistogram::CCameraHistogram(CCamera& aOwner):iOwner(aOwner), iImpl(NULL)
	{
	}

/**
@deprecated use CCamera::CCameraV2Histogram::~CCameraV2Histogram() 

Destructor for the CCamera::CCameraHistogram class.
*/
EXPORT_C CCamera::CCameraHistogram::~CCameraHistogram()
	{
	if (iImpl != NULL)
		{
		iImpl->Release();	
		}
	}

/**
@deprecated use void CCamera::CCameraV2Histogram::GetSupportedHistogramsL(TUint& aSupportedHistogramType)

Gets a list of the types of histograms the ECam implementation supports.

@return	A bit field representing all supported types of histograms. 

@see CCamera::CCameraHistogram::THistogramType
*/
EXPORT_C TUint32 CCamera::CCameraHistogram::SupportedHistograms()
	{
	return iImpl->SupportedHistograms();
	}

/**
@deprecated use void CCamera::CCameraV2Histogram::PrepareClientHistogramL

Request to prepare a non direct histogram.

@note 	A direct histogram is directly embedded into the viewfinder.
		If a non direct histogram is requested the histogram data will be passed to the camera client.

@param	aType		
		The type of histogram to be prepared. This must be one of the supported histogram types returned by
		SupportedHistograms().

@leave	KErrNotSupported if the histogram type supplied in aType is not supported.

@return An integer value which is the handle of the histogram on the ECam implementation.
 		This value must be passed to other API functions when requesting operations 
 		involving this particular histogram.

@see PrepareDSAHistogramL
*/
EXPORT_C TUint CCamera::CCameraHistogram::PrepareHistogramL(THistogramType aType)
	{
	return iImpl->PrepareHistogramL(aType);
	}

/**
@deprecated use void CCamera::CCameraV2Histogram::PrepareDirectHistogramL

Request to prepare a direct histogram.

@note A Direct histogram is directly embedded into the viewfinder.

@param	aType		
		The type of histogram to be prepared. This must be one of the supported histogram types returned by
		SupportedHistograms().
@param	aPosition
		The position on the screen (in pixels) where the histogram is to be displayed.
@param	aSize
		The size of histogram in pixels.
@param	aColor
		The colour and alpha blending with which the histogram will be displayed. 

@leave	KErrNotSupported if the histogram type supplied in aType is not supported.
		
@return An integer value which is the handle of the histogram on the ECam implementation.
 		This value must be passed to other API functions when requesting operations 
 		involving this particular histogram.

@see PrepareHistogramL
*/
EXPORT_C TUint CCamera::CCameraHistogram::PrepareDSAHistogramL(THistogramType aType, const TPoint& aPosition, const TSize& aSize, const TRgb& aColor)
	{
	return iImpl->PrepareDSAHistogramL(aType, aPosition, aSize, aColor);
	}

/**
@deprecated use void CCamera::CCameraV2Histogram::StartHistogram()

Request to start getting histogram notifications.

@param	aHistHandle	
		The handle identifying the histogram on the ECam implementation.

@leave	KErrArgument if aHistHandle is out of range; also any system wide error.

@see KUidECamEventCameraHistogram
*/
EXPORT_C void CCamera::CCameraHistogram::StartHistogramL(TUint aHistHandle)
	{
	iImpl->StartHistogramL(aHistHandle);
	}

/**
@deprecated use void CCamera::CCameraV2Histogram::StopHistogram()

Request to stop getting histogram notifications.

@param	aHistHandle	
		The handle identifying the histogram on the ECam implementation.
		
@leave	KErrArgument if aHistHandle is out of range; also any system wide error.
*/
EXPORT_C void CCamera::CCameraHistogram::StopHistogramL(TUint aHistHandle)
	{
	iImpl->StopHistogramL(aHistHandle);
	}

/**
@deprecated only one histogram is available per buffer with the usage of MHistogramV2Buffer and CCameraV2Histogram.

Destroys a histogram on the ECam implementation and releases its handle.

@param	aHistHandle	
		The handle identifying the histogram on the ECam implementation.

@leave	KErrArgument if aHistHandle is out of range; also any system wide error.
*/
EXPORT_C void CCamera::CCameraHistogram::DestroyHistogramL(TUint aHistHandle)
	{
	iImpl->DestroyHistogramL(aHistHandle);
	}

/**
@deprecated use void CCamera::CCameraV2Histogram::GetHistogramStateL(TBool& aIsHistogramActive)

Gets a list of all histograms that are active on the ECam implementation. 
A histogram is in an active state if StartHistogramL() has been called on it.

@param	aActiveHistograms
		Returned list of histogram handles for which StartHistogramL() has been called. 

@leave	KErrNoMemory if the ECam implementation cannot add more histogram handles due to low memory; also any system wide error.  
*/
EXPORT_C void CCamera::CCameraHistogram::GetActiveHistogramsL(RArray<TUint>& aActiveHistograms)
	{
	iImpl->GetActiveHistogramsL(aActiveHistograms);
	}

/**
@deprecated use void CCamera::CCameraV2Histogram::UpdateDirectHistogramPropertiesL

Updates the properties of a direct histogram.

@param	aHistHandle	
		The handle identifying the histogram on the ECam implementation.
@param	aPosition	
		The new position on the screen (in pixels) where the histogram is to be displayed.
@param	aSize	
		The new size of histogram in pixels.
@param	aColor		
		The new colour and alpha blending with which the histogram will be displayed. 

@leave	KErrArgument if aHistHandle is out of range; also any system wide error.
*/
EXPORT_C void CCamera::CCameraHistogram::UpdateDSAHistogramPropertiesL(TUint aHistHandle, const TPoint& aPosition, const TSize& aSize, const TRgb& aColor)
	{
	iImpl->UpdateDSAHistogramPropertiesL(aHistHandle, aPosition, aSize, aColor);
	}
	
/**
@deprecated use void CCamera::CCameraV2Histogram::GetDirectHistogramPropertiesL

Gets the properties of a direct histogram.

@param 	aHistHandle		
		The handle on the ECam implementation of the direct histogram whose properties are to be retrieved.
@param 	aPosition
		A reference to a TPoint object that will receive the position (in pixels) of the histogram on the screen.
@param 	aSize	
		A reference to a TSize object that will receive the size of the histogram in pixels.
@param 	aColor
		A reference to a TRgb object that will receive the colour and alpha blending of the histogram.

@leave	KErrArgument if aHistHandle is out of range; also any system wide error.

@note   If the method leaves, the reference arguments are not guaranteed to be valid.
*/
EXPORT_C void CCamera::CCameraHistogram::GetDSAHistogramPropertiesL(TUint aHistHandle, TPoint& aPosition, TSize& aSize, TRgb& aColor)
	{
	iImpl->GetDSAHistogramPropertiesL(aHistHandle, aPosition, aSize, aColor);
	}


/**
@deprecated use MCaptureImageObserver, MCaptureVideoObserver, MDirectViewFinderObserver, and MClientViewFinderObserver
which have callback functions which notify the client about availability of histogram data. These are sent when 
CCameraV2Histogram::StartHistogram() is called after CCameraV2Histogram::PrepareClientHistogramL(). If
CCameraV2Histogram::PrepareClientHistogramL() is not called first then the callback returns KErrBadHandle.
	
Returns to the client the histogram data for all the histograms generated by the ECam implementation.

@note The client application using this API should provide MCameraObserver2 interface to be 
signalled when histogram data is available to be retrieved from the ECAM implementation.

@see 	KUidECamEventCameraHistogram
	
@leave	KErrNoMemory if the ECam implementation has not been able to create the histogram buffer;
		also any system wide error.

@return A reference to a histogram buffer which will contain the returned histogram data.
*/	
EXPORT_C MHistogramBuffer& CCamera::CCameraHistogram::HistogramDataL()
	{
	return iImpl->HistogramDataL();
	}

/**
@deprecated use MCaptureImageObserver, MCaptureVideoObserver, MDirectViewFinderObserver, and MClientViewFinderObserver
which have callback functions which notify the client about availability of histogram data. These are sent when 
CCameraV2Histogram::StartHistogram() is called after CCameraV2Histogram::PrepareClientHistogramL(). If
CCameraV2Histogram::PrepareClientHistogramL() is not called first then the callback returns KErrBadHandle.
	
Returns the data for a single histogram from the ECam implementation to the client.

@note The client application using this API should provide MCameraObserver2 interface to be 
signalled when histogram data is available to be retrieved from the ECAM implementation.

@see 	KUidECamEventCameraHistogram

@param 	aHistHandle		
		The handle on the ECam implementation of the histogram whose data is to be retrieved.
			
@leave	KErrNoMemory if the ECam implementation has not been able to create the histogram buffer;
		also any system wide error.

@return A reference to a histogram buffer which will contain the returned histogram data.
*/
EXPORT_C MHistogramBuffer& CCamera::CCameraHistogram::HistogramDataL(TUint aHistHandle)
	{
	return iImpl->HistogramDataL(aHistHandle);
	}
 
	
//
//			    	CCameraV2Histogram                                              //
//	
/**
@internalComponent

Factory function that creates a new camera histogram object specifically for a camera mode, for example, still image,
video, snapshot and specific viewfinder.

@param	aCamera
		A reference to the camera object for which a camera histogram object is to be created.
		
@param aImplFactory 
	   A reference to the MImplementationFactory derived object.

@leave KErrNoMemory if out of memory; also any system wide error.

@leave KErrExtensionNotSupported When NewL/NewDuplicateL used instead of New2L/NewDuplicate2L.

@return A pointer to a fully constructed camera histogram object.

@note  This method is supposed to be used by internal ECAM components only.
*/	
EXPORT_C CCamera::CCameraV2Histogram* CCamera::CCameraV2Histogram::CreateL(CCamera& aCamera, MImplementationFactory& aImplFactory)
	{
	if(aCamera.CameraVersion() == KCameraDefaultVersion)
 		{
 		User::Leave(KErrExtensionNotSupported);
 		}
 		
	CCamera::CCameraV2Histogram* self = new (ELeave) CCamera::CCameraV2Histogram(aCamera);
	CleanupStack::PushL(self);
	self->ConstructL(aImplFactory);
	CleanupStack::Pop(self);
	return self; 
	}
	
/**
@internalComponent

CCameraV2Histogram second phase constructor 

Function used to initialise internal state of the object specifically for a camera mode, for example, still image,
video, snapshot and specific viewfinder.

This may be used in other possible cases as well.

@param aImplFactory 
	   A constant reference to the MImplementationFactory derived object.

@leave KErrNoMemory Out of memory; or any other error code as well.

@note This method is supposed to be used by this class only.
*/ 
void CCamera::CCameraV2Histogram::ConstructL(const MImplementationFactory& aImplFactory) 
	{
	TInt err = KErrNone;
	TAny* implPtr = NULL;
	
	err = aImplFactory.GetImpl(implPtr, KECamMCameraV2HistogramUid);
	if (err != KErrNone)
		{
		User::Leave(err);
		}
	iImpl = static_cast<MCameraV2Histogram*>(implPtr);
	
	iImpl->SetHistogramHandle(this);
	}
	
/**
Constructor for the CCamera::CCameraV2Histogram class.

@param aOwner
	   A reference to the camera object for which a camera histogram object is to be created.
*/
CCamera::CCameraV2Histogram::CCameraV2Histogram(CCamera& aOwner):iOwner(aOwner), iImpl(NULL)
	{
	}

/**
Destructor for the CCamera::CCameraV2Histogram class.
*/
EXPORT_C CCamera::CCameraV2Histogram::~CCameraV2Histogram()
	{
	if (iImpl != NULL)
		{
		iImpl->Release(this);	
		}
	}
	
/**
Retrieves a list of the supported types of histograms.

@param 	aSupportedHistogramType
		A reference to bit field representing the supported types of histograms as given by CCamera::
		CCameraV2Histogram::THistogramType

@leave  May leave with any error code. 

@see CCamera::CCameraV2Histogram::THistogramType
*/
EXPORT_C void CCamera::CCameraV2Histogram::GetSupportedHistogramsL(TUint& aSupportedHistogramType) const
	{
	return iImpl->GetSupportedHistogramsL(aSupportedHistogramType);
	} 

/**
Informs whether or not the direct histogram is supported.

@param  aIsDirectHistogramSupported
		ETrue implies that direct histogram is supported.
		EFalse implies that direct histogram is not supported.

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraV2Histogram::GetDirectHistogramSupportInfoL(TBool& aIsDirectHistogramSupported) const
	{
	return iImpl->GetDirectHistogramSupportInfoL(aIsDirectHistogramSupported);	
	}
	
/**
Request to prepare a client based histogram.

@note 	A direct histogram is directly embedded into the viewfinder.
		If a non direct histogram is requested the histogram data will be passed to the camera client.

@param	aType		
		The type of histogram to be prepared. This must be one of the supported histogram types given by
		GetSupportedHistogramsL.

@leave  May leave with any error code.

@note   Each histogram object will either prepare direct histogram or client histogram. Preparing multiple histograms
		per object is an invalid case.
		
@see PrepareDSAHistogramL
*/
EXPORT_C void CCamera::CCameraV2Histogram::PrepareClientHistogramL(CCamera::CCameraV2Histogram::THistogramType aType) 
	{
	iImpl->PrepareClientHistogramL(aType);
	}
	
/**
Prepares the direct histogram.

@param  aDirectHistogramParameters
		The parameters required to prepare the direct histogram.

@leave  May leave with any error code.

@note   Each histogram object will either prepare direct histogram or client histogram. Preparing multiple histograms
		per object is an invalid case.

@see PrepareClientHistogramL
*/
EXPORT_C void CCamera::CCameraV2Histogram::PrepareDirectHistogramL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters)
	{
	iImpl->PrepareDirectHistogramL(aDirectHistogramParameters);
	}

/**
Update the parameters for the direct histogram.

@param  aDirectHistogramParameters
		The desired parameters which have to be used to update the direct histogram.

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraV2Histogram::UpdateDirectHistogramPropertiesL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters)
	{
	iImpl->UpdateDirectHistogramPropertiesL(aDirectHistogramParameters);
	}
	
/**
Retrieves the currently used parameters for the direct histogram.

@param  aDirectHistogramParameters
		Retrieves the parameters currently being used for the direct histogram.

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraV2Histogram::GetDirectHistogramPropertiesL(CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters) const
	{
	iImpl->GetDirectHistogramPropertiesL(aDirectHistogramParameters);
	}
	
/**
Request to start getting histogram notifications.

@note   Histogram should have been prepared before calling this method.

@note   Histogram notifications are sent to the client by using relevant callbacks for image capture, video capture and 
		viewfinders.

@see    MCaptureImageObserver
@see    MCaptureVideoObserver
@see    MDirectViewFinderObserver
@see    MClientViewFinderObserver
*/
EXPORT_C void CCamera::CCameraV2Histogram::StartHistogram()
    {
    iImpl->StartHistogram();	
    }

/**
Request to stop generating histogram.
*/
EXPORT_C void CCamera::CCameraV2Histogram::StopHistogram()
	{
	iImpl->StopHistogram();
	}
    
/**
Informs whether the histogram is currently active or not.  
A histogram is in an active state if StartHistogram() has been called on it and has not been yet stopped.

@param	aIsHistogramActive
		ETrue indicates that the histogram is active.
		EFalse indicates that the histogram is inactive. 

@leave  May leave with any error code.
*/
EXPORT_C void CCamera::CCameraV2Histogram::GetHistogramStateL(TBool& aIsHistogramActive) const 
	{
	iImpl->GetHistogramStateL(aIsHistogramActive);
	}
    
/**
Constructor for the TDirectHistogramParameters class.
Sets the size and version of this class.
*/
EXPORT_C CCamera::CCameraV2Histogram::TDirectHistogramParameters::TDirectHistogramParameters()
	{
	iSize = sizeof(CCamera::CCameraV2Histogram::TDirectHistogramParameters);
	iVersion = KECamDirectHistogramParametersCurrentVersion;
	}

/** 
Returns the size of the class. Used for extensibility by deriving from this base class and adding new member variables.
Intended to be used for implementation of methods where this class reference is passed as function arguments. 
Implementation of such methods can find out the whether the actual class passed is base or the derived one. So, if a new application 
is made to run on an old implementation, an error may occur once the old implementation detects this by getting 
the size information of the T class passed. Also, if an old application is made to run on a new implementation, this can be 
handled correctly if the derived class variables handling is done in a proper 'if-else' statement. 

@return The size of the class.

@note The size will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraV2Histogram::TDirectHistogramParameters::Size() const
	{
	return iSize;
	}
	
/**	
Returns the version of the class. Used for extensibility specially when the class members are not added but the Reserved 
members get used at a later stage.

@return The version of the class.

@note The version will be modified when the T-class gets updated.
*/
EXPORT_C TUint CCamera::CCameraV2Histogram::TDirectHistogramParameters::Version() const
	{
	return iVersion;
	}
