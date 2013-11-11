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

#ifndef CAMERAHISTOGRAM_H
#define CAMERAHISTOGRAM_H

#include <ecam.h>
#include <gdi.h>

#include <ecam/camerasnapshot.h>
#include <ecamcapturecontrol.h>
#include <ecamviewfinder.h>

#include <ecam/ecamcommonuids.hrh>

class MHistogramBuffer;
class MCameraHistogram;
class MCameraV2Histogram;

class MImplementationFactory;

/** 
The current Version of the TDirectHistogramParameters class.
*/
static const TUint KECamDirectHistogramParametersCurrentVersion = 1;

/** 
@deprecated use MCaptureImageObserver, MCaptureVideoObserver, MDirectViewFinderObserver, and MClientViewFinderObserver
which have callback functions which notify the client about availability of histogram data. These are sent when 
CCameraV2Histogram::StartHistogram() is called after CCameraV2Histogram::PrepareClientHistogramL(). If
CCameraV2Histogram::PrepareClientHistogramL() is not called first then the callback returns KErrBadHandle.

Uid used to identify the event that the request for histogram has completed.
A callback to MCameraObserver2::HandleEvent() will occur when the histogram data is ready to be retrieved.
If a call to StartHistogram() is made without a previous successful call to PrepareHistogramL() then the callback returns KErrBadHandle.
*/
static const TUid KUidECamEventCameraHistogram = {KUidECamEventHistogramUidValue};

/** 
This is the UID which is used to obtain the CCameraHistogram interface, via a call to CCamera::CustomInterface().

@see KECamHistogramUidValue
@see CCamera::CCameraHistogram
@deprecated use KECamV2HistogramUid
*/
static const TUid  KECamHistogramUid = {KECamHistogramUidValue};

/**
This class allows the client to get histogram data in a specified format for an image (for example, luminance based histogram,
average colour value histogram, etc).

The data generated can be fed to some exposure based algorithm, or directly displayed in the viewfinder.
It also allows the client to get and set the properties of a histogram.

The client selects histogram parameters calling PrepareHistogramL(). It can then start and stop 
receiving notifications from the ECam implementation by calling StartHistogramL() and StopHistogramL().
The client application using this API should provide MCameraObserver2 interface to be signalled,
with event KUidECamEventCameraHistogram, when histogram data is available to be retrieved from 
the ECam implementation.
The client can then retrieve the histogram data from the ECam implementation calling HistograDataL().

@note This class provides a standardised client interface for the camera histogram. Classes cannot be derived from it.

@note   If the class methods leave, the output type parameter value is not guaranteed to be valid.

@publishedPartner
@deprecated use CCamera::CCameraV2Histogram
*/

class CCamera::CCameraHistogram : public CBase
	{

	friend class CCamera;
	
public:	
	/** Supported histogram types. */
	enum THistogramType
		{
		/** No histogram has been specified. */
		EHistNone			= 0x0000,
		/** Luminance based histogram. */
		ELuminance			= 0x0001,
		/** The histogram is based on the average value of all three RGB colour components. */
		EAverageRGB			= 0x0002,
		/** The histogram is based on the value of the red colour component. */
		ERedComponent		= 0x0004,
		/** The histogram is based on the value of the blue colour component. */
		EBlueComponent		= 0x0008,
		/** The histogram is based on the value of the green colour component. */
		EGreenComponent		= 0x0010
		};
	
public:
	IMPORT_C static CCamera::CCameraHistogram* NewL(CCamera& aCamera);

	IMPORT_C TUint32 SupportedHistograms();
	IMPORT_C TUint PrepareHistogramL(CCamera::CCameraHistogram::THistogramType aType); 
	IMPORT_C TUint PrepareDSAHistogramL(CCamera::CCameraHistogram::THistogramType aType, const TPoint& aPosition, const TSize& aSize, const TRgb& aColor);
    
    IMPORT_C void StartHistogramL(TUint aHistHandle);
    IMPORT_C void StopHistogramL(TUint aHistHandle);
    IMPORT_C void DestroyHistogramL(TUint aHistHandle);
    
    IMPORT_C void GetActiveHistogramsL(RArray<TUint>& aActiveHistograms); 
    IMPORT_C void UpdateDSAHistogramPropertiesL(TUint aHistHandle, const TPoint& aPosition, const TSize& aSize, const TRgb& aColor);
	IMPORT_C void GetDSAHistogramPropertiesL(TUint aHistHandle, TPoint& aPosition, TSize& aSize, TRgb& aColor);
		
	IMPORT_C MHistogramBuffer& HistogramDataL();
	IMPORT_C MHistogramBuffer& HistogramDataL(TUint aHistHandle);
    
	IMPORT_C ~CCameraHistogram();
	    
private:
	CCameraHistogram(CCamera& aOwner);
	void ConstructL();

private:
	CCamera&        iOwner;
    MCameraHistogram* iImpl;  // not owned
 	};
	
/** 
This is the UID which is used to obtain the CCameraV2Histogram interface, via a call to CCamera::CustomInterface().

@publishedPartner
@prototype
*/
static const TUid  KECamV2HistogramUid = {KECamV2HistogramUidValue};

/**
This class allows the client to get histogram data in a specified format for an image (for example, luminance based histogram,
average colour value histogram, etc).

Histogram may be created for still images, video, viewfinders and snapshot. Further different types of histogram may be 
created for any of the imaging modes.

The data generated can be fed to some exposure based algorithm, or directly displayed on the screen.
It also allows the client to get and set the properties of a histogram.

The client selects histogram parameters calling PrepareClientHistogramL or PrepareDirectHistogramL. It can then start and 
stop receiving notifications from the ECam implementation by calling StartHistogram() and StopHistogram().
The client application using this API shall provide the interfaces: MCaptureImageObserver, MCaptureVideoObserver, 
MDirectViewFinderObserver and MClientViewFinderObserver in order to receive notifications about direct histogram display 
or client histogram data availablility.

Implementation shall use MHistogramV2Buffer in order to provide client histogram data.

@note  This class provides a standardised client interface for the camera histogram. Classes cannot be derived from it.

@note  If the class methods leave, the output type parameter value is not guaranteed to be valid.

@publishedPartner
@prototype
*/

class CCamera::CCameraV2Histogram : public CBase
	{

	friend class CCamera;
	friend CCamera::CCameraV2Histogram* CCamera::CCameraSnapshot::CreateHistogramHandleL() const;
	friend CCamera::CCameraV2Histogram* CCamera::CCameraImageCapture::CreateHistogramHandleL() const;
	friend CCamera::CCameraV2Histogram* CCamera::CCameraVideoCaptureControl::CreateHistogramHandleL() const;
	friend CCamera::CCameraV2Histogram* CCamera::CCameraV2DirectViewFinder::CreateHistogramHandleL() const;
	friend CCamera::CCameraV2Histogram* CCamera::CCameraClientViewFinder::CreateHistogramHandleL() const;
	
public:	
	/** 
	Supported histogram types. 
	The enumeration list may be extended in future.
	*/
	enum THistogramType
		{
		/** No histogram has been specified. */
		EHistNone			= 0x0000,
		/** Luminance based histogram. */
		ELuminance			= 0x0001,
		/** The histogram is based on the average value of all three RGB colour components. */
		EAverageRGB			= 0x0002,
		/** The histogram is based on the value of the red colour component. */
		ERedComponent		= 0x0004,
		/** The histogram is based on the value of the blue colour component. */
		EBlueComponent		= 0x0008,
		/** The histogram is based on the value of the green colour component. */
		EGreenComponent		= 0x0010,
		/** Qualitative Histogram to mark over exposed parts of image. */
		EOverExposure 		= 0x0020,
		/** Qualitative Histogram to mark under exposed parts of image. */
		EUnderExposure 		= 0x0040,
		};
	
	/**
	Specifes the parameters necessary for a direct histogram.
	*/	
	class TDirectHistogramParameters
		{
	public:
		IMPORT_C TDirectHistogramParameters();
		
		IMPORT_C TUint Size() const;
		IMPORT_C TUint Version() const;
		
	private:
		//for future expansion
		TUint iSize:24;
		TUint iVersion:8;
		
		// reserved for future expansion
		TInt iReserved1;
		TInt iReserved2;
		TInt iReserved3;
		
	public:
		/** The type of histogram to be prepared. This must be one of the supported histogram types returned by
		SupportedHistograms(). */
		THistogramType iHistogramType;
		/** The position on the screen (in pixels) where the histogram is to be displayed. */
		TPoint iPosition;
		/** The size of histogram in pixels. */
		TSize iHistogramSize;
		/** The colour and alpha blending with which the histogram will be displayed. */
		TRgb iHistogramColor;
		/** The background color for the direct histogram. */ 
		TRgb iBackgroundColor; 
		};
		
public:
	
	IMPORT_C void GetSupportedHistogramsL(TUint& aSupportedHistogramType) const;
	IMPORT_C void GetDirectHistogramSupportInfoL(TBool& aIsDirectHistogramSupported) const;
	
	IMPORT_C void PrepareClientHistogramL(THistogramType aType); 
	
	IMPORT_C void PrepareDirectHistogramL(const TDirectHistogramParameters& aDirectHistogramParameters);
	
	IMPORT_C void UpdateDirectHistogramPropertiesL(const TDirectHistogramParameters& aDirectHistogramParameters);
	IMPORT_C void GetDirectHistogramPropertiesL(TDirectHistogramParameters& aDirectHistogramParameters) const;
	
	IMPORT_C void StartHistogram();
    IMPORT_C void StopHistogram();

	IMPORT_C void GetHistogramStateL(TBool& aIsHistogramActive) const; 
	
	IMPORT_C ~CCameraV2Histogram();

private:
	IMPORT_C static CCamera::CCameraV2Histogram* CreateL(CCamera& aCamera, MImplementationFactory& aImplFactory);
	
	CCameraV2Histogram(CCamera& aOwner);
	void ConstructL(const MImplementationFactory& aImplFactory);

private:
	CCamera&        iOwner;
    MCameraV2Histogram* iImpl;  // not owned
   	};

	
//
// MHistogramBuffer //
//

/**
This class is used to pass histogram data between the camera device and a client that implements MCameraObserver2.

The buffer may contain multiple histograms.

The class allows the client to access the data as a descriptor to a kernel chunk.

@publishedPartner
@deprecated use MHistogramV2Buffer
*/
class MHistogramBuffer
	{
	
public:

	/**
	@deprecated Only one histogram is available per buffer in MHistogramV2Buffer
	
	Returns the number of histograms contained within the buffer.
	 
	This value will not depend upon the number of images for which histograms are requested by the 
	client, but on the number of histograms generated by the ECam implementation. 
	In the case where the driving mode (bracket mode, video mode, etc.) returns more than one image or frame 
	the ECam implementation may not generate histograms for every valid image or frame. This can happen, for example, 
	if there are no changes in the histogram data or if there are performance problems. 

	@return The number of histograms in the buffer. 
	*/
	virtual TInt NumOfHistograms() = 0;

	/** 
	@deprecated use CCamera::CCameraV2Histogram::THistogramType MHistogramV2Buffer::Type()

	Returns the type of histogram data that is available at the specified index within the histogram buffer.
		
	@param  aIndex
	        The index of the required histogram in the buffer. 
	        This must be between 0 and (NumOfHistograms() - 1).
	
	@leave  KErrArgument if aIndex is not between 0 and (NumOfHistograms() - 1).

	@return The type of the histogram in the buffer at the given index.
	*/
	virtual CCamera::CCameraHistogram::THistogramType TypeL(TUint aIndex) = 0;

	/** 
	@deprecated use TInt MHistogramV2Buffer::ElementSize()
	
	Returns the number of bits per histogram element value used to store the histogram data at a 
	specified index within the histogram buffer. 	
	
	The number of bits per histogram element will be decided by the ECam implementation and it may vary
	for each histogram within the histogram buffer.
	
	@see NumOfElementsL()
	
	@param  aIndex
	        The index of the required histogram in the buffer. 
	        This must be between 0 and (NumOfHistograms() - 1).
	
	@leave  KErrArgument if aIndex is not between 0 and (NumOfHistograms() - 1).

	@return The histogram element size in bits.
	*/
	virtual TInt ElementSizeL(TUint aIndex) = 0;

	/** 
	@deprecated use TInt MHistogramV2Buffer::NumOfElements()
	
	Returns the number of elements in the histogram at a specified index within the histogram buffer. 
	
	This value can be used along with the element size obtained by calling ElementSizeL(), to calculate
	the exact size of histogram data ( element-size x number-of-elements = histogram-size ).
	
	@param  aIndex
	        The index of the required histogram in the buffer. 
	        This must be between 0 and (NumOfHistograms() - 1).
	
	@leave  KErrArgument if aIndex is not between 0 and (NumOfHistograms() - 1).

	@return The number of elements in the histogram. 
	*/
	virtual TInt NumOfElementsL(TUint aIndex) = 0;

	/**
	@deprecated use TDesC8* MHistogramV2Buffer::DataL()
	
	Allows access to a given histogram in the histogram buffer via a descriptor.

	@param  aIndex
	        The index of the required histogram in the buffer. 
	        This must be between 0 and (NumOfHistograms() - 1).
	
	@leave  KErrArgument if aIndex is not between 0 and (NumOfHistograms() - 1).

	@return A pointer to a descriptor containing the histogram data.
	*/
	virtual TDesC8* DataL(TUint aIndex) = 0;
	
	/** 
	@deprecated use RChunk& MHistogramV2Buffer::ChunkL() 

	Returns a handle to the chunk that contains the histogram data.
	The RChunk is exposed so that it can potentially be shared between multiple
	processes.
	@note 	The descriptor pointer returned by DataL() is effectively derived from this RChunk.
	
	@leave  KErrNotSupported if the chunk is not available.

	@return A reference to a handle to the chunk containing the histogram data. 
	*/
	virtual RChunk& ChunkL() = 0;
	 
	/** 
	@deprecated use TInt MHistogramV2Buffer::ChunkOffsetL()
	
	Returns the offset in the chunk (returned by ChunkL()) that contains the data for the 
	histogram at a specified index within the histogram buffer. 
	
	@param  aIndex
	        The index of the required histogram in the buffer. 
	        This must be between 0 and (NumOfHistograms() - 1).
	
	@leave  KErrArgument if aIndex is not between 0 and (NumOfHistograms() - 1).

	@return The offset in the chunk that contains the start of the histogram data. 
	*/
	virtual TInt ChunkOffsetL(TUint aIndex) = 0;

	/** 
	@deprecated Only one histogram is available per buffer in MHistogramV2Buffer
	
	Returns the image number or frame index to which the histogram stored at the specified 
	index within the buffer relates.

	@param  aIndex
	        The index of the required histogram in the buffer. 
	        This must be between 0 and (NumOfHistograms() - 1).
	
	@leave  KErrArgument if aIndex is not between 0 and (NumOfHistograms() - 1).

	@return The image number or frame index.
	*/
	virtual TInt ImageNumberL(TUint aIndex) = 0;
	
	/** 
	@deprecated Only one histogram is available per buffer in MHistogramV2Buffer
	
	Returns the handle of a particular histogram stored in the buffer.
	
	@param  aIndex
	        The index of the required histogram in the buffer. 
	        This must be between 0 and (NumOfHistograms() - 1).
	
	@leave  KErrArgument if aIndex is not between 0 and (NumOfHistograms() - 1).
	
	@return The handle of the histogram on the ECam implementation.
	*/
	virtual TUint HandleL(TUint aIndex) = 0;
	
	/** 	   
    @deprecated use void MHistogramV2Buffer::Release()
	
	Releases the buffer. Once the client has processed the histogram data it should
    use this method to signal to the camera that the buffer can be deleted or re-used.
	*/
	virtual void Release() = 0;
	};

//
// MHistogramV2Buffer  //
//
	
/**
This class is used to pass histogram data between the camera device and a client that uses CCameraV2Histogram and 
implements MCaptureImageObserver, MCaptureVideoObserver, MDirectViewFinderObserver and MClientViewFinderObserver.

Only one histogram data is available per buffer. ECam implementation will issue another callback once a new histogram
data is ready. Till that time, client shall use the histogram data available from the histogram buffer currently retrieved.

The class allows the client to access the data as a descriptor to a kernel chunk.

@see CCameraV2Histogram

@publishedPartner
@prototype
*/
class MHistogramV2Buffer
	{
	
public:
	/** 
	Returns the type of histogram data that is available within the histogram buffer.
		
	@return The type of the histogram in the buffer at the given index.
	*/
	virtual CCamera::CCameraV2Histogram::THistogramType Type()=0;

	/** 
	Returns the number of bits per histogram element value used to store the histogram data within the histogram buffer. 
	
	The number of bits per histogram element will be decided by the ECam implementation.
	
	@see NumOfElementsL()
	
	@return Positive value means the histogram element size in bits. Negative value means error case.
	*/
	virtual TInt ElementSize()=0;

	/** 
	Returns the number of elements in the histogram within the histogram buffer. 
	
	This value can be used along with the element size obtained by calling ElementSizeL(), to calculate
	the exact size of histogram data ( element-size x number-of-elements = histogram-size ).
	
	@return Positive value means the number of elements in the histogram. Negative value means error case.
	*/
	virtual TInt NumOfElements()=0;

	/**
	Allows access to the histogram in the histogram buffer via a descriptor.

	@return A pointer to a descriptor containing the histogram data.
	
	@leave  May Leave with any error code.
	*/
	virtual TDesC8* DataL()=0;
	
	/** 
	Returns a handle to the chunk that contains the histogram data.
	The RChunk is exposed so that it can potentially be shared between multiple
	processes.
	@note 	The descriptor pointer returned by DataL() is effectively derived from this RChunk.
	
	@leave  KErrNotSupported if the chunk is not available. May Leave with any other error code as well.

	@return A reference to a handle to the chunk containing the histogram data. 
	*/
	virtual RChunk& ChunkL()=0;
	 
	/** 
	Returns the offset in the chunk (returned by ChunkL()) that contains the data for the 
	histogram within the histogram buffer. 
	
	@leave  KErrNotSupported if the chunk is not available. May Leave with any other error code as well.

	@return The offset in the chunk that contains the start of the histogram data. 
	*/
	virtual TInt ChunkOffsetL()=0;

	/** 
	Releases the buffer. Once the client has processed the histogram data it should
    use this method to signal to the camera that the buffer can be deleted or re-used.
	*/
	virtual void Release()=0;
	};

#endif // CAMERAHISTOGRAM_H	
