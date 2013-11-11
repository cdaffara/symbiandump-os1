/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __MMF_HW_DEVICE_H__
#define __MMF_HW_DEVICE_H__

#include <e32base.h>
#include <ecom/implementationinformation.h>
#include <mmf/common/mmfutilities.h> //TFourCC
#include <mmf/server/mmfhwdevicecmds.h>
#include <mmf/server/taskconfig.h>

/**
@publishedPartner
@released

The connection type defines what the stream will connect to,
either gpp, dsp node, or dsp device.
*/
typedef enum TConnectionType_tag
	{
	/** Gpp
	*/
    EConnectionGpp,
    /** dsp node
	*/
    EConnectionTask,
    /** dsp device
	*/
    EConnectionDevice
	} TConnectionType ;

/**
@publishedPartner
@released

Connection Type
*/
typedef struct TStreamConnection_tag
	{
	/** connection id
	*/
    TInt               iId ;
    /** connection type
    */
    TConnectionType    iType ;
	} TStreamConnection;

const TUint KMaxDeviceNameLength = 128;

enum TSamplingFreq { ESampFreq8k, ESampFreq11025k, ESampFreq16k, ESampFreq2205k, ESampFreq32k, ESampFreq441k, ESampFreq48k, ESampFreq12k, ESampFreq24k, ESampFreq64k};

/**
@publishedPartner
@released

Type class for stream connection parameters.
This structure defines the connection parameters that are required to
be filled in by the HwDevice observer.  The connection type defines
what the stream will connect to, either gpp, dsp node, or dsp device.
*/
class TConnection
	{
	public:
	/** Connection type.
	*/
    TStreamConnection           iConnection;
	/** Name of the device if needed.
	*/
    TBuf8<KMaxDeviceNameLength> iDeviceName;
	};

// Forward reference
class MMMFHwDeviceObserver;

/**
@publishedPartner
@released

Type class for Hardware initialization parameters.
The implementation specific initialization attributes are passed in this
structure. Elements common to all HwDevices are defined here. These are
the pointer to the observer and stream connection identification for the
HwDevice.

These stream connections are defined as follows:
- SrcStream is the source data stream to the HwDevice for a decoder/player.
- OutStream is the counterpart stream of data out of the HwDevice of
processed source data.
- InStream is the input stream of data to the HwDevice for an
encoder/recorder. 
- DestStream is the destination data stream of
this InStream after it has been processed.

The last element is a pointer to a buffer of the rest of the initialization
data for the HwDevice. This data will vary for different HwDevices.
However, the first element is always the number of internal connections (TUint8)
required by the HwDevice. Following this, is an element (TUint8) for each of
the required connection ID's. The elements following this are only known by
the HwDevice.
*/
class THwDeviceInitParams
	{
	public:
		THwDeviceInitParams() : iHwDeviceInitArgsPtr(0,0){};
		/** Pointer to an observer.
		*/
		MMMFHwDeviceObserver* iHwDeviceObserver;
		/** Source stream attributes.
		*/
		TConnection           iSrcStream;
		/** Destination stream attributes.
		*/
		TConnection           iDestStream;
		/** Input stream attributes.
		*/
		TConnection           iInStream;
		/** Output stream attributes.
		*/
		TConnection           iOutStream;
		/** Pointer to a buffer.
		*/
		TPtr8                 iHwDeviceInitArgsPtr;
	};


/**
@publishedPartner
@released

The observer class interface for a hardware device listener class.

MMMFHwDeviceObserver provides a generic interface which should be implemented
by the classes that are interested in receiving events from the CMMFHwDevice
implementation class.
*/
class MMMFHwDeviceObserver
	{
public:

	/**
	Handles CMMFHwDevice object’s data request event.

	This function is called by the CMMFHwDevice implementation class when it
	needs data for decoding.

	@param  aHwDataBuffer
	        The buffer to fill the data.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt FillThisHwBuffer(CMMFBuffer& aHwDataBuffer) = 0;

	/**
	Handles CMMFHwDevice object’s data request event.
	
	This function is called by the CMMFHwDevice implementation class when it
	fills encoded data into a buffer.

	@param  aHwDataBuffer
	        The buffer with encoded data.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt EmptyThisHwBuffer(CMMFBuffer& aHwDataBuffer) = 0;

	/**
	Handles the CMMFHwDevice object’s message event.

	This function is called by the CMMFHwDevice implementation class when a
	message is generated and passed back to the observer.

	@param  aMessageType
	        The type of message. Defines the contents of aMsg.
	@param  aMsg
	        The message packed in a descriptor.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt MsgFromHwDevice(TUid aMessageType, const TDesC8& aMsg)=0;

	/**
	Called by a CMMFHwDevice derived class when it stops a decoding/encoding task.
	*/
	virtual void Stopped() = 0;

	/**
	Called by a CMMFHwDevice derived class when it stops a decoding/encoding task.
	*/
	virtual void Error(TInt aError=KErrNone) = 0;
	};

/*
 *
 *	KMmfUidPluginInterfaceHwDevice
 *
 *	This UID is the INTERFACE_UID for CMMFHwDevice.  It is used to search for hardware device plugins in plugin DLLs.
 *	All hardware device plugin DLLs must include interface_uid = KMmfUidPluginInterfaceHwDevice in their .rss files.
 *
 */

/**
@publishedPartner
@released

ECom plugin class for a hardware device that decodes (plays) source data in a certain
FourCC coding type to another FourCC coding type or encodes (records) data from a certain
FourCC coding type to a another FourCC coding type.

The hardware device can be instantiated in 3 different ways:

1.	NewL(const TFourCC& aSrcDatatype, const TFourCC& aDstDataType)

This instantiate a hardware device that can encode or decode the aSrcDatatype to a aDstDataType.

2.	NewL(const TFourCC& aSrcDatatype, const TFourCC& aDstDataType, const TDesC& aPreferredSupplier)

This is similar to the above but is used where there may be multiple hardware devices that
perform the same conversion. Third party developers may use their own
hardware device implementations and can ensure the controller uses their implementation by setting
the preferred supplier to themselves.

3.	NewL(TUid aUid)

This is used to explicitly instantiate a hardware device where the UID is known.

4.	NewL(TFileName aFileName)

This is used to create an instance of a HwDevice implementation by loading the library

CMMFHwDevice provides a generic interface that every Hardware Device that decodes and/or encodes has to implement.
Based on parameters specified in Start, the device implementation will start decoding or encoding.
The source and destination of the decoding/encoding operations should be set up before starting the task by sending
THwDeviceInitParams to the Init() function.
*/
class CMMFHwDevice  : public CBase
	{
public:
	static CMMFHwDevice* NewL(const TFourCC& aSrcDatatype, const TFourCC& aDstDataType);
	static CMMFHwDevice* NewL(const TFourCC& aSrcDatatype, const TFourCC& aDstDataType, const TDesC& aPreferredSupplier);
	inline static CMMFHwDevice* NewL(TUid aUid);
	static CMMFHwDevice* NewL(TFileName aFileName);

	/**
	Starts Encoding or Decoding task(s) based on the parameter specified.

	@param  aFuncCmd
	        The device function specifying the requested service i.e. decode or encode.
	@param  aFlowCmd
	        The device flow directions for the requested service.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt Start(TDeviceFunc aFuncCmd, TDeviceFlow aFlowCmd) = 0;

	/**
	Stops the current on-going task.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt Stop() = 0;

	/**
	Temporarily suspends the current task of decoding or encoding.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt Pause() = 0;

	/**
	Initializes the hardware device tasks.

	@param  aDevInfo
	        The device initialization parameters.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt Init(THwDeviceInitParams& aDevInfo) = 0;

	/**
	Retrieves a custom interface to the device.

	@param  aInterfaceId
	        The interface UID, defined with the custom interface.

	@return A pointer to the interface implementation, or NULL if the device does not
	        implement the interface requested. The return value must be cast to the
	        correct type by the user.
	*/
	virtual TAny* CustomInterface(TUid aInterfaceId) = 0;

	/**
	Call this function to notify hardware device implementation that
	data is available in aFillBufferPtr for decoding.

	@param  aFillBufferPtr
	        The data buffer filled by the observer.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt ThisHwBufferFilled(CMMFBuffer& aFillBufferPtr) = 0;

	/**
	Call this function to notify hardware device implementation that
	data in aEmptyBufferPtr from encoding is processed.

	@param  aEmptyBufferPtr
	        The data buffer processed by the observer.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt ThisHwBufferEmptied(CMMFBuffer& aEmptyBufferPtr) = 0;

	/**
    Used to configure the sample rate and stereo mode of a CMMFHwDevice plugin.

	The configuration of HwDevices is device specific and is not used in any of the reference
	devices that return KErrNotSupported.

	@param  aConfig
	        The device configuration.
	*/
	virtual TInt SetConfig(TTaskConfig& aConfig) = 0;

	/**
	Call this function to stop and then delete a codec. This is used to allow resources to be freed 
	when using a DSP or similar hardware.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt StopAndDeleteCodec() = 0;

	/**
	Call this function to delete a codec. 

	This is used to allow resources to be freed when using a DSP or similar hardware.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt DeleteCodec() = 0;

	/**
	Destructor.

	The destructor is called by ECom framework allowing derived classes
	to clean up the implementation specific resources.
	*/
	inline virtual ~CMMFHwDevice();

private:
	static void SelectByPreference( RImplInfoPtrArray& aPlugInArray, const TDesC& aPreferredSupplier ) ;

protected:
	/**
	A pointer to the hardware device observer.
	*/
	MMMFHwDeviceObserver* iHwDeviceObserver;

//Attributes
private:
	TUid iDtor_ID_Key;
	};
	
#include <mmf/server/mmfhwdevice.inl>

#endif

