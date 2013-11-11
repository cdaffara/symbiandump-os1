// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// include\mmf\common\mmfcodec.h
// 
//

#ifndef MMFCODEC_H
#define MMFCODEC_H


#include <e32base.h>
#include <ecom/ecom.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/plugin/mmfplugininterfaceuids.hrh>

/*
 * This UID is the INTERFACE_UID for CMMFCodec.  It is used to search for codec plugins in plugin DLLs.
 * All codec plugin DLLs must include interface_uid = KMmfUidPluginInterfaceCodec in their .rss files.
 */

/**
@publishedAll
@released

Indicates the result of processing data from the source buffer to a destination buffer
and provides functions to compare the result code.
*/
class TCodecProcessResult
	{
public:
	/**
	Flag to track the codec's processing status.
	*/
	enum TCodecProcessResultStatus
	{
	/** 
	The codec successfully has completed its processing.

	A codec should return this code when it has fully processed the source buffer, and is finished
	with the destination buffer. The codec should finish with the destination buffer when it has 
	been filled. The EProcessComplete return code indicates that the codec has finished
	with the destination buffer. This informs the data path that the destination buffer may now be 
	passed onto the data sink. If the codec returns EProcessComplete this means that the codec is
	expecting a new source buffer and a new destination buffer the next time it's ProcessL() method
	is called.
	*/
	EProcessComplete,
	/** 
	Could not empty the source buffer because the destination buffer became full.

	A codec should return this code when it has not been able to fully process the source buffer. 
	This would usually be the case if the codec has filled the destination buffer (or the remaining 
	space available in the destination buffer is insufficient to perform any further useful 
	processing) before the source buffer has been processed. The EProcessIncomplete return code 
	indicates that the codec has finished with the destination buffer. This informs the data path 
	that the destination buffer may now be passed onto the data sink. If the codec returns 
	EProcessIncomplete this means that the codec is expecting the same source buffer but a new 
	destination buffer the next time it's ProcessL() method is called.
	*/
	EProcessIncomplete,

	/**
	Codec came across an end of data.

	This can be returned if a codec is able to determine that there is no more source data. It is
	not necessary for the codec to return this however as in most cases the codec will not be able
	to determine when the end of data has been reached.
	*/
	EEndOfData,

	/** 
	Could not fill the destination buffer because the source buffer has been emptied 

	A codec should return this code when is has fully processed the source buffer and there is still
	sufficient space available in the destination buffer for the codec to continue using the same
	destination buffer. The EDstNotFilled return code indicates that the codec has not finished with
	the destination buffer. If the codec returns EDstNotFilled this means that the codec is
	expecting a new source buffer but the same destination buffer the next time its ProcessL()
	method is called.
	*/
	EDstNotFilled,

	/** 
	An error occured.
	
	This is no longer required as if an error occurs in the codec's ProcessL()function, it should 
	leave. When used with a datapath, returning EProcessError has the same effect as leaving with 
	KErrCorrupt.
	*/
	EProcessError,

	/**
	@deprecated

	As 'EFrameIncomplete' but also requests a call to GetNewDataPosition.
	*/
	EProcessIncompleteRepositionRequest,

	/**
	@deprecated

	As 'EFrameComplete' but also requests a call to GetNewDataPosition.
	*/
	EProcessCompleteRepositionRequest
	};

	/** 
	Overloaded operator to test equality.
	
	@param  aStatus
	        The status to compare the result of the process function.

	@return A boolean indicating if the two status codes are the same. ETrue if they are, EFalse
	        otherwise.
	*/
	TBool operator==(const TCodecProcessResultStatus aStatus) const {return (iStatus == aStatus);}

	/** 
	Overloaded operator to test inequality.

	@param  aStatus
	        The status to compare the result of the process function.
	@return A boolean indicating if the two status codes are not the same. ETrue if they are not, 
	        EFalse otherwise.
	*/
	TBool operator!=(const TCodecProcessResultStatus aStatus) const {return (iStatus != aStatus);}

	/**
	The codec's processing status.

	@see enum TCodecProcessResultStatus
	*/
	TCodecProcessResultStatus iStatus;

	/** 
	The number of source bytes processed.

	The number of bytes of source data that have been processed.

	A codec should set this, and iDstBytesAdded, to indicate the source bytes processed for a 
	particular call (i.e. not the total number of source bytes processed or destination bytes 
	added). The codec should also ensure that the length of the destination buffer matches the 
	length of the processed data in the destination buffer. Note that the codec should not 
	reallocate the maximum length of the destination buffer.
	*/
	TUint iSrcBytesProcessed;

	/** 
	The number of bytes added to the destination buffer.

	A codec should set this, and iSrcBytesProcessed, to indicate the source bytes processed for a 
	particular call (i.e. not the total number of source bytes processed or destination bytes 
	added). The codec should also ensure that the length of the destination buffer matches the 
	length of the processed data in the destination buffer. Note that the codec should not 
	reallocate the maximum length of the destination buffer.
	*/
	TUint iDstBytesAdded;
public:

	/**
	Default constructor.
	*/
	TCodecProcessResult()
		:iStatus(EProcessError), iSrcBytesProcessed(0), iDstBytesAdded(0) {};
private:
	/**
	This member is internal and not intended for use.
	*/
	TInt iReserved1;
	TInt iReserved2;
	TInt iReserved3;
	};


class TFourCC; //forward reference
class CMMFBuffer; //forward reference

/**
@publishedAll
@released

ECom plugin class for a codec that processes source data in a certain fourCC coding type and
converts it to a destination buffer of another fourCC coding type.
The function is synchronous as it is expected that the codec will be operating in its own thread
of execution - usually via a CMMFDataPath or CMMFDataPathProxy

The codec can be instantiated in 3 different ways:

1.	NewL(const TFourCC& aSrcDatatype, const TFourCC& aDstDataType).

This instantiate a codec that can convert the aSrcDatatype to a aDstDataType.

2.	NewL(const TFourCC& aSrcDatatype, const TFourCC& aDstDataType, const TDesC& aPreferredSupplier).

This is similar to the above but is used where there may be multiple codecs that
perform the same conversion - the idea is that a 3rd party deveoper may develop there own
controller and codecs and can ensure the controller uses their codecs by setting the preferredSupplier
to themselves.

3.	NewL(TUid aUid).

This is used to explicitly instantiated a codec where the uid is known.  This might be used by
controlers that only support one codec type.

The processing of the data is handled by the codecs ProcessL() member.
The intention is that the source buffer for conversion is converted to the appropriate coding type
in the destination buffer.  The buffers can be of any size.  Since the buffers can be of any size there is no
guarantee that all the source buffer can be processed to fill the destination buffer or that the
all the source buffer may be processed before the destination is full.  Therefore the
ProcessL needs to return a TCodecProcessResult returing the number of source bytes processed
and the number of destination bytes processed along with a process result code defined thus:
- EProcessComplete: the codec processed all the source data into the sink buffer
- EProcessIncomplete: the codec filled sink buffer before all the source buffer was processed
- EDstNotFilled: the codec processed the source buffer but the sink buffer was not filled
- EEndOfData: the codec detected the end data - all source data in processed but sink may not be full
- EProcessError: the codec process error condition

The ProcessL should start processing the source buffer from the iPosition data member of the source data
and start filling the destination buffer from its iPosition.
*/
class CMMFCodec  : public CBase
{
public:
	//The function which instantiates an object of this type
	// using the TFourCC codes as the resolver parameters.
	IMPORT_C static CMMFCodec* NewL(const TFourCC& aSrcDatatype, const TFourCC& aDstDataType);
	IMPORT_C static CMMFCodec* NewL(const TFourCC& aSrcDatatype, const TFourCC& aDstDataType, const TDesC& aPreferredSupplier);
	IMPORT_C static CMMFCodec* NewL(TUid aUid);

	static void SelectByPreference( RImplInfoPtrArray& aPlugInArray, const TDesC& aPreferredSupplier ) ;

	inline virtual void ConfigureL(TUid aConfigType, const TDesC8& aConfigData);

	/**
	Codec reset.

	Used to flush out status information when a reposition occurs. This is a virtual function that 
	is provided should the codec require resetting prior to use.
	*/
	virtual void ResetL() {}
	//Standardised destructor.
	inline virtual ~CMMFCodec();


	/**
	Processes the data in the specified source buffer and writes the processed data to the specified 
	destination buffer.

	This function is synchronous, when the function returns the data has been processed. The source 
	buffer is converted to the appropriate coding type in the destination buffer. The buffers can be 
	of any size, therefore there is no guarantee that all the source buffer can be processed to fill 
	the destination buffer or that all the source buffer may be processed before the destination is 
	full. This function therefore returns the number of source, and destination, bytes processed
	along with a process result code indicating completion status.

	The aSource and aSink buffers passed in are derived from CMMFBuffer. The buffer type (e.g. a 
	CMMFDataBuffer) passed in should be supported by the codec otherwise this function should leave 
	with KErrNotSupported. The position of the source buffer should be checked by calling the source 
	buffer's Position() member which indicates the current source read position in bytes. The codec 
	should start processing from the current source buffer read position. The position of the
	destination buffer should be checked by calling the destination buffer's Position() method which 
	indicates the current destination write position in bytes. The codec should start writing to the 
	destination buffer at the current destination buffer write position.

	This is a virtual function that each derived class must implement.

	@see enum TCodecProcessResult

	@param  aSource
	        The source buffer containing data to encode or decode.
	@param  aDestination
	        The destination buffer to hold the data after encoding or decoding.

	@return The result of the processing.
	*/
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDestination) = 0;

	/**
	@internalComponent

	Gets a pointer to the extension specified by an identifier. The extension can be either an
	interface or function. If the extension is not supported NULL value is given and returns
	KErrExtensionNotSupported.

	@param	aExtensionId
		Extension identifier.
	@param	aExtPtr
		Pointer to get the extension.
	@return	If successful returns KErrNone otherwise KErrExtensionNotSupported.		
	*/
	TInt ExtensionInterface(TUint aExtensionId, TAny*& aExtPtr);

private:
	TUid iDtor_ID_Key;
};

/**
Destructor.

Destroys any variables then informs ECom that this specific instance of the interface has been
destroyed.
*/
inline CMMFCodec::~CMMFCodec()
	{
	// Destroy any instance variables and then
	// inform ecom that this specific 
	// instance of the interface has been destroyed.
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

/**
Sets codec configuration.

This is a virtual function which does not need to be implemented
by a codec, but may be if required. This function provides additional configuration
information for the codec. The configuration is passed in as a descriptor.
The default version leaves with KErrNotSupported.

@param  aConfigType
        The UID of the configuration data.
@param  aConfigData
        The configuration information.
*/
inline void CMMFCodec::ConfigureL(TUid /*aConfigType*/, const TDesC8& /*aConfigData*/)
{
	User::Leave(KErrNotSupported);
}

#endif

