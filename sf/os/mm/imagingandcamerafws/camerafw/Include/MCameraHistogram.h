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

#ifndef MCAMERAHISTOGRAM_H
#define MCAMERAHISTOGRAM_H

#include <ecam/camerahistogram.h>
#include <ecam/ecamcommonuidif.hrh>

/** 
This is the UID which is used to obtain the MCameraHistogram interface, 
via a call to CCamera::CustomInterface(), which provides implementation of the M-class interface. 

@see KECamMCameraHistogramUidValue
@deprecated use KECamMCameraV2HistogramUid
*/   
static const TUid KECamMCameraHistogramUid =  {KECamMCameraHistogramUidValue};

/** 
This is the UID which is used to obtain the MCameraV2Histogram interface, 
via a call to CCamera::CustomInterface(), which provides implementation of the M-class interface. 

@see KECamMCameraV2HistogramUidValue
*/   
static const TUid KECamMCameraV2HistogramUid =  {KECamMCameraV2HistogramUidValue};

/** 
This is a mixin class to be implemented by providers of the extension API for the camera histogram.
	
Histograms can be sent to the client or directly displayed on the viewfinder. 

@note	This class is intended for sub classing by licensees only.

@see CCamera::CCameraHistogram

@publishedPartner
@deprecated use MCameraV2Histogram
*/
class MCameraHistogram
	{
	
public:
	/**
	@deprecated use void MCameraV2Histogram::GetSupportedHistogramsL(TUint& aSupportedHistogramType)
	
	Gets a list of the types of histograms the ECam implementation supports.

	@return	A bit field representing all supported types of histograms. 

	@see CCamera::CCameraHistogram::THistogramType
	*/
    virtual TUint32 SupportedHistograms()=0;
    
	/**
	@deprecated use void MCameraV2Histogram::PrepareClientHistogramL
	
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
	virtual TUint PrepareHistogramL(CCamera::CCameraHistogram::THistogramType aType)=0; 

	/**
	@deprecated use void MCameraV2Histogram::PrepareDirectHistogramL
	
	Request to prepare a direct histogram.

	@note A direct histogram is directly embedded into the viewfinder.

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
    virtual TUint PrepareDSAHistogramL(CCamera::CCameraHistogram::THistogramType aType, const TPoint& aPosition, const TSize& aSize, const TRgb& aColor)=0; 
    
	/**
	@deprecated use void MCameraV2Histogram::StartHistogram()
	
	Request to start getting histogram notifications.

	@param	aHistHandle	
			The handle identifying the histogram on the ECam implementation.

	@leave	KErrArgument if aHistHandle is out of range; also any system wide error.
	*/
    virtual void StartHistogramL(TUint aHistHandle)=0;

	/**
	@deprecated use void MCameraV2Histogram::StopHistogram()
	
	Request to stop getting histogram notifications.

	@param	aHistHandle	
			The handle identifying the histogram on the ECam implementation.
		
	@leave	KErrArgument if aHistHandle is out of range; also any system wide error.
	*/
    virtual void StopHistogramL(TUint aHistHandle)=0;

	/**
	@deprecated only one histogram is available per buffer with the usage of MHistogramV2Buffer and MCameraV2Histogram.
	
	Destroys a histogram on the ECam implementation and releases its handle.

	@param	aHistHandle	
			The handle identifying the histogram on the ECam implementation.

	@leave	KErrArgument if aHistHandle is out of range; also any system wide error.
	*/
    virtual void DestroyHistogramL(TUint aHistHandle)=0;

	/**
	@deprecated use void MCameraV2Histogram::Release(CCamera::CCameraV2Histogram *aHistogramHandle)
	
	Releases all the histogram handles created by this object and destroys them on the ECam implementation.
	This function is called from the destructor of CCamera::CCameraHistogram.
	*/
    virtual void Release()=0;
    
	/**
	@deprecated use void MCameraV2Histogram::GetHistogramStateL(TBool& aIsHistogramActive)
	
	Gets a list of all histograms that are active on the ECam implementation. 
	A histogram is in an active state if StartHistogramL() has been called on it.

	@param	aActiveHistograms
			Returned list of histogram handles for which StartHistogramL() has been called. 

	@leave	KErrNoMemory if the ECam implementation cannot add more histogram handles due to low memory; also any system wide error.  
	*/
    virtual void GetActiveHistogramsL(RArray<TUint>& aActiveHistograms)=0; 

	/**
	@deprecated use void MCameraV2Histogram::UpdateDirectHistogramPropertiesL
	
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
	virtual void UpdateDSAHistogramPropertiesL(TUint aHistHandle, const TPoint& aPosition, const TSize& aSize, const TRgb& aColor)=0;
		
	/**
	@deprecated use void MCameraV2Histogram::GetDirectHistogramPropertiesL
	
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
	*/
	virtual void GetDSAHistogramPropertiesL(TUint aHistHandle, TPoint& aPosition, TSize& aSize, TRgb& aColor)=0;
		
	/**
	@deprecated use MCaptureImageObserver, MCaptureVideoObserver, MDirectViewFinderObserver, and MClientViewFinderObserver
	which have callback functions which notify the client about availability of histogram data. These are sent when 
	CCameraV2Histogram::StartHistogram() is called after CCameraV2Histogram::PrepareClientHistogramL(). If
	CCameraV2Histogram::PrepareClientHistogramL() is not called first then the callback returns KErrBadHandle.
	
	Returns to the client the histogram data for all the histograms generated by the ECam implementation.
			
	@leave	KErrNoMemory if the ECam implementation has not been able to create the histogram buffer;
			also any system wide error.

	@return A reference to a histogram buffer which will contain the returned histogram data.
	*/	
	virtual MHistogramBuffer& HistogramDataL()=0;
	
	/**
	@deprecated use MCaptureImageObserver, MCaptureVideoObserver, MDirectViewFinderObserver, and MClientViewFinderObserver
	which have callback functions which notify the client about availability of histogram data. These are sent when 
	CCameraV2Histogram::StartHistogram() is called after CCameraV2Histogram::PrepareClientHistogramL(). If
	CCameraV2Histogram::PrepareClientHistogramL() is not called first then the callback returns KErrBadHandle.
	
	Returns the data for a single histogram from the ECam implementation to the client.

	@param 	aHistHandle		
			The handle on the ECam implementation of the histogram whose data is to be retrieved.
			
	@leave	KErrNoMemory if the ECam implementation has not been able to create the histogram buffer;
			also any system wide error.

	@return A reference to a histogram buffer which will contain the returned histogram data.
	*/
	virtual MHistogramBuffer& HistogramDataL(TUint aHistHandle)=0;
	};
	
/** 
This is a mixin class to be implemented by providers of the extension API for the camera v2 histogram.

Histograms can be sent to the client or directly displayed on the screen. 

@note	This class is intended for sub classing by licensees only.

@see CCamera::CCameraV2Histogram

@publishedPartner
@prototype
*/
class MCameraV2Histogram
	{

public:

	/** 
	Releases the interface. 
	
	@param  aHistogramHandle
			The pointer to the histogram class object which would be destroyed by the client.
	*/
	virtual void Release(CCamera::CCameraV2Histogram* aHistogramHandle)=0;
	
	/**
	The pointer to the V2 Histogram class used to pass back to the client via observer callbacks when direct histogram
	gets displayed. Implementation is not supposed to not destroy this object.
	
	@param  aHistogramHandle
			The pointer to the histogram class object.
			
	@see    MCaptureImageObserver
	@see	MDirectViewFinderObserver
	@see    MClientViewFinderObserver
 	*/
	virtual void SetHistogramHandle(CCamera::CCameraV2Histogram* aHistogramHandle)=0;
	
	/**
	Retrieves a list of the supported types of histograms.

	@param 	aSupportedHistogramType
			A reference to bit field representing the supported types of histograms as given by CCamera::
			CCameraV2Histogram::THistogramType
	
	@leave  May leave with any error code. 

	@see CCamera::CCameraV2Histogram::THistogramType
	*/
    virtual void GetSupportedHistogramsL(TUint& aSupportedHistogramType) const =0;
    
    /**
	Informs whether or not the direct histogram is supported.

	@param  aIsDirectHistogramSupported
			ETrue implies that direct histogram is supported.
			EFalse implies that direct histogram is not supported.

	@leave  May leave with any error code.
	*/
	virtual void GetDirectHistogramSupportInfoL(TBool& aIsDirectHistogramSupported) const=0;
		
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
	virtual void PrepareClientHistogramL(CCamera::CCameraV2Histogram::THistogramType aType)=0; 
	
	/**
	Prepares the direct histogram.
	
	@param  aDirectHistogramParameters
			The parameters required to prepare the direct histogram.
	
	@leave  May leave with any error code.
	
	@note   Each histogram object will either prepare direct histogram or client histogram. Preparing multiple histograms
			per object is an invalid case.
	
	@see PrepareClientHistogramL
	*/
	virtual void PrepareDirectHistogramL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters)=0;

	/**
	Update the parameters for the direct histogram.
	
	@param  aDirectHistogramParameters
			The desired parameters which have to be used to update the direct histogram.
	
	@leave  May leave with any error code.
	*/
	virtual void UpdateDirectHistogramPropertiesL(const CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters)=0;
	
	/**
	Retrieves the currently used parameters for the direct histogram.
	
	@param  aDirectHistogramParameters
			Retrieves the parameters currently being used for the direct histogram.
	
	@leave  May leave with any error code.
	*/
	virtual void GetDirectHistogramPropertiesL(CCamera::CCameraV2Histogram::TDirectHistogramParameters& aDirectHistogramParameters) const=0;
	
	/**
	Request to start getting histogram notifications.

	@note   Histogram should have been prepared before calling this method.
	
	@note   Histogram notifications are sent to the client by using relevant callbacks for image capture, video capture 
			and viewfinders.

	@see    MCaptureImageObserver
	@see    MCaptureVideoObserver
	@see    MDirectViewFinderObserver
	@see    MClientViewFinderObserver
	*/
    virtual void StartHistogram()=0;

	/**
	Request to stop generating histogram.
	*/
    virtual void StopHistogram()=0;
    
    /**
	Informs whether the histogram is currently active or not.  
	A histogram is in an active state if StartHistogram() has been called on it and has not been yet stopped.

	@param	aIsHistogramActive
			ETrue indicates that the histogram is active.
			EFalse indicates that the histogram is inactive. 

	@leave  May leave with any error code.
	*/
    virtual void GetHistogramStateL(TBool& aIsHistogramActive) const=0; 
    };
	
#endif // MCAMERAHISTOGRAM_H
