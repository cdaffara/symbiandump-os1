// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMF_SERVER_FORMAT_H__
#define __MMF_SERVER_FORMAT_H__

#include <mmf/server/mmfdatasink.h>
#include <mmf/server/mmfdatasource.h>
#include <ecom/ecom.h>
#include <mmf/plugin/mmfplugininterfaceuids.hrh>

const TUint KMMFFormatDefaultFrameSize = 0x1000;  //4K

/**
@publishedAll
@released

Base class from which source formats can be derived from. The intended usage is for controllers that can support more
than one type of format. The class is an MDataSource as far as the data path is concerned but is an MDataSink to the clip
that is the source of the actual data.

All CMMFFormatDecode plugin DLLs must include interface_uid = KMmfUidPluginInterfaceFormatDecode in their .rss files.
*/
class CMMFFormatDecode : public CBase, public MDataSource, public MDataSink
	{
public:
	// ECOM creation.
	IMPORT_C static CMMFFormatDecode* NewL( TUid aUid, MDataSource* aSource );

	IMPORT_C static CMMFFormatDecode* NewL( const TDesC& aFileName, MDataSource* aSource, const TDesC& aPreferredSupplier ) ;
	IMPORT_C static CMMFFormatDecode* NewL( const TDesC8& aSourceHeader,  MDataSource* aSource, const TDesC& aPreferredSupplier ) ;
	IMPORT_C static CMMFFormatDecode* NewL( MDataSource* aSource, const TDesC& aPreferredSupplier ) ;

	IMPORT_C static CMMFFormatDecode* NewL( const TDesC& aFileName, MDataSource* aSource, const TDesC& aPreferredSupplier, TBool& aSupportsCustomInterfaces ) ;
	IMPORT_C static CMMFFormatDecode* NewL( const TDesC8& aSourceHeader,  MDataSource* aSource, const TDesC& aPreferredSupplier, TBool& aSupportsCustomInterfaces ) ;
	IMPORT_C static CMMFFormatDecode* NewL( MDataSource* aSource, const TDesC& aPreferredSupplier, TBool& aSupportsCustomInterfaces ) ;

	/**
	Destructor.
	*/
	virtual ~CMMFFormatDecode()  {REComSession::DestroyedImplementation(iDtor_ID_Key);};

	/**
	Returns the ECom plugin UID of this format.

	@return The plugin UID.
	*/
	TUid ImplementationUid() const {return iImplementationUid;}

	/**
	Gets the number of streams of the specified media type.

	This is a virtual function that each derived class must implement.

	@param  aMediaType
	        The UID of the media type, for example KUidMediaTypeAudio or KUidMediaTypeVideo.

	@return	The number of streams of multimedia data in the format for the specified media type.
	        For example, for a WAV or mp3 audio file this procedure would return 1 for a media
	        type of audio and 0 for a media type of video. More complex multimedia formats (for
	        example AVI and mp4) can support multiple streams of both video and audio.
	*/
	virtual TUint Streams(TUid aMediaType) const = 0;

	/**
	Returns the time interval for one frame for the specified media type.

	This is a virtual function that each derived class must implement.

	In the case of video, a frame would usually be one frame of video. In the case of
	audio, a frame may correspond to a frame of audio, if the particular audio data type
	is framed eg mp3. There are two definitions of a frame. A format frame, which may
	consist of several frames of a particular framed audio data type. This may be the case,
	for example for GSM610 (a low quality audio data type used in telephony) where a frame
	is only 65 bytes large. In this case a format frame consists of several GSM610 data type
	frames as passing the data out 65 bytes at a time would be inefficient. In the case of
	non-framed data such as pcm, a frame can be an arbitrary size determined by the format plugin.

	@param  aMediaType
	        The media type id.

	@return	The frame time interval
	*/
	virtual TTimeIntervalMicroSeconds FrameTimeInterval(TMediaId aMediaType) const = 0;


	/**
	Returns the duration of the clip for the specified media type.

	This is a virtual function that each derived class must implement.

	@param  aMediaType
	        The media type ID.

	@return The duration of the clip.
	*/
	virtual TTimeIntervalMicroSeconds Duration(TMediaId aMediaType) const = 0;

	/**
	Request from CMMFDataPath to fill the specified buffer.
	The CMMFFormat needs to break this down into one or more reads from the clip
	(from MDataSource - CMMFFormatDecode is a MDataSource to a CMMFDataPath consumer).

	This is a virtual function that each derived class must implement.

	This method is the means by which data is obtained from the data source. aBuffer is
	the buffer that needs filling from the source data stream as identified by aMediaId. The
	format should read the frame number of the buffer via the buffer's CMMFBuffer::FrameNumber()
	method. From this the format should be able to determine the actual position of the data on
	the data source. The technique here depends on the nature of the format. For a linear audio
	format, the position can be obtained by a simple calculation of the frame size, the header size
	and where appropriate the datatype.

	For non-linear formats either an index table of frame number and location will need to be
	created in the NewL() or some form of approximating algorithm will be required. Some formats have
	an index table as part of the format e.g. AVI. If no random access is required then no index table
	is required, the format can keep track of the current read position and increment it on each access,
	and reset it if the frame number is reset to 0 or 1. Given that for non-linear i.e. variable bit rate
	formats, the size of the data read may vary from frame to frame, then the format should either set
	the request size of the buffer for the required frame or call the source's ReadBufferL() (either
	CMMFClip::ReadBufferL(), CMMFDescriptor ::ReadBufferL() or CMMFFile::ReadBufferL()) function that
	takes the aLength parameter.

	It is the responsibility of the format decode to determine the size and position of the source data
	for each frame. For linear audio formats, the format decode should fill the buffer up to its maximum
	length. For multimedia formats e.g. mp4, AVI etc, the format decode is effectively acting as a demultiplexor,
	and must obtain the appropriate data from the source depending on the aMediaId.

	@param  aBuffer
	        The buffer to fill.
	@param  aConsumer
	        The consumer.
	@param  aMediaId
	        The media type ID.
	*/
	virtual void FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId aMediaId)=0;

	/**
	@internalAll

	Indicates data has been added to the file.

	@param  aBuffer
	        The emptied buffer.
    */
	inline virtual void BufferEmptiedL(CMMFBuffer* aBuffer);

	/**
	Tests whether a source buffer can be created.

	The format knows what type of source buffer it requires so default returns ETrue.
	It doesn't usually need to set the size of this buffer.

	@return	A boolean indicating whether a buffer can be created. ETrue if the buffer can be created,
	        EFalse otherwise.
	*/
	virtual TBool CanCreateSourceBuffer() {return ETrue;}

	/**
	Creates a source buffer.

	This is a virtual function that each derived class must implement.

	This function should create a buffer of length 0, the maximum length should be equal to the maximum
	possible frame size. In the case of non framed data it should be set to an arbitrary size, which is
	effectively a trade off between being too small which will affect performance as more source reads
	are required, and being too large which will give very coarse positioning granularity. For pcm data,
	a buffer size of 4K is a good compromise. The same compromise also applies when deciding to put multiple
	audio frames into one format frame. The sink buffer size may also effect the format buffer size and
	the controller may use this to suggest a buffer size to the format by calling the format's SuggestSourceBufferSize()
	method. Alternatively the MDataSource::CreateSourceBufferL() function that takes the additional aSinkBuffer
	parameter may also be used when deciding the source buffer maximum size of the created source buffer.

	@param  aMediaId
	        The media type ID.
	@param  aReference
	        If ETrue the MDataSource owns the buffer. If EFalse, then the caller owns the buffer. This
	        should normally be set to EFalse as format plugins do not create the reference buffer.

	@return The created source buffer.
	*/
	virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, TBool &aReference)=0;

	/**
	Returns the source data type code for the specified media type ID.

	Used by the CMMFDataPath for codec matching.

	This is a virtual function that each derived class must implement.

	@param  aMediaId
	        The media type ID.

	@return The source data type code.
	*/
	virtual TFourCC SourceDataTypeCode(TMediaId aMediaId)=0;

	/**
	@internalAll

    Adds a buffer to a clip.

	@param  aBuffer
	        The buffer to which the clip is added.
	@param  aSupplier
	        The data source.
	@param  aMediaId
	        The Media ID.
	*/
	inline virtual void EmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId aMediaId);

	/**
	Indicates the data source has filled the buffer.

	Called by the CMMFFormat's MDataSource when it has filled the buffer.
	The default implementation below would need overriding in cases where multiple
	clip reads were required to fill the buffer from the data path.

	@param  aBuffer
	        The buffer to which the clip is added.
	*/
	virtual void BufferFilledL(CMMFBuffer* aBuffer) {iDataPath->BufferFilledL(aBuffer);}

	/**
	Tests whether a sink buffer can be created.

	Format would normally pass its own buffer onto the CMMFClip, so
	this may not be required. The default returns EFalse.

	@return A boolean indicating if the sink buffer can be created. ETrue if buffer can be created, EFalse otherwise.
	*/
	virtual TBool CanCreateSinkBuffer() {return EFalse;}

	/**
	Creates a sink buffer for the specified media ID. The default version returns NULL.

	@param  aMediaId
	        The media type ID.
	@param  aReference
	        If ETrue the MDataSink owns the buffer.
	        If EFalse, then the caller owns the buffer.
	@return The CMMFBuffer sink buffer.
	*/
	inline virtual CMMFBuffer* CreateSinkBufferL(TMediaId aMediaId, TBool &aReference);

	/**
	Returns the sink data type code for the specified media type ID.
	This would be the same as the source data type four CC although
	the clip is not going to need this info.

	@param  aMediaId
	        The media type id.

	@return The sink data type code.
	*/
	inline virtual TFourCC SinkDataTypeCode(TMediaId aMediaId);

	/**
	Gets the number of meta data entries.

	Meta Data support. The decode format is only capable of reading meta data entries from the clip.

	This is an optional method, used to return the number of meta data entries present in the format.
	A meta data entry is a format-specific field such as authorship, copyright, security details etc.
	The function is optional as many formats do not provide support for such additional meta data fields.

	The default leaves with KErrNotSupported.

	@param  aNumberOfEntries
	        A reference to the number of meta data entries supported by the format. On return, contains
	        the number of meta data entries.
	*/
	inline virtual void GetNumberOfMetaDataEntriesL(TInt& aNumberOfEntries);

	/**
	Returns the specified meta data entry.

	This method is optional as many formats do not provide support for such additional meta data fields.

	The default leaves with KErrNotSupported.

	@param  aIndex
	        The zero based meta data entry index to retrieve.

	@return The meta data entry.
	*/
	inline virtual CMMFMetaDataEntry* MetaDataEntryL(TInt aIndex);

	//audio format methods

	/**
	Sets the number of channels.

	The default returns KErrNotSupported.

	There would normally be no need to override this function as the format decode plugin can normally
	determine for itself the number of channels. It is only necessary to override this in cases where the
	format decode plugin cannot determine for itself the number of channels.  This function should not be
	used if the audio clip already exists; that is, in the "Open and Append" scenario, when the function's
	behaviour is undefined.

	@param  aChannels
	        The number of channels.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
            another of the system-wide error codes.
	*/
	inline virtual TInt SetNumChannels(TUint aChannels);

	/**
	Sets the sample rate.

	The default implementation returns KErrNotSupported.

	There would normally be no need to override this function as the format decode can normally determine
	the sample rate for itself. It is only necessary to override this in cases where the format decode plugin
	cannot determine for itself the sample rate.

	@param  aSampleRate
	        The sample rate.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	inline virtual TInt SetSampleRate(TUint aSampleRate);

	/**
	Sets the bit rate.

	The default implementation returns KErrNotSupported.

	This method is mainly provided for variable bit rate formats, such as mp3, where the bit rate can
	not be directly calculated from the sample rate. There would normally be no need to override this
	function as the format decode can normally determine the sample rate for itself. It is only necessary
	to override this in cases where the format decode plugin cannot determine for itself the sample rate.

	@param aBitRate
	       The bit rate in bits per second.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	inline virtual TInt SetBitRate(TUint aBitRate);

	/**
	Returns the number of channels.

	The default implementation returns 0.

	@return	The number of channels.
	*/
	virtual TUint NumChannels() {return 0;}

	/**
	Gets the sample rate.

	The default implementation returns 0.

	@return The sample rate.
	*/
	virtual TUint SampleRate() {return 0;}

	/**
	Gets the bit rate.

	The default implementation returns 0.

	This method is mainly provided for variable bit rate formats, such as mp3, where the bit rate 
	can not be directly calculated from the sample rate. This function needs overriding for any format 
	decode that supports audio.

	@return The bit rate.
	*/
	virtual TUint BitRate() {return 0;}

	/**
	Gets the supported sample rates.

	The default implementation leaves with KErrNotSupported.

	This should return an array of sample rates supported by the format where applicable. Note 
	that this refers to the possible sample rates supported by the format, not the actual sample rate,
	which will be one of the supported sample rates. The implementation of this function is optional.

	@param  aSampleRates
	        Reference to an array of supported sample rates.
	*/
	inline virtual void GetSupportedSampleRatesL(RArray<TUint>& aSampleRates);

	/**
	Gets the supported bit rates.

	The default leaves with KErrNotSupported.

	This should return an array of bit rates supported by the format where applicable. Note that this
	refers to the possible bit rates supported by the format, not the actual bit rate, which will be
	one of the supported bit rates. The implementation of this function is optional.

	@param  aBitRates
	        Reference to an array of supported bit rates.
	*/
	inline virtual void GetSupportedBitRatesL(RArray<TUint>& aBitRates);

	/**
	Gets the supported number of channels.

	The default leaves with KErrNotSupported.

	The implementation of this procedure should return an array of channels supported by the format
	where applicable. Note that this refers to the possible number of channels supported by the format,
	not the actual number of channels, which will be one of the supported channels. For example, for a
	format decode plugin capable of supporting mono and stereo the procedure would return an array of
	length two the first array member containing the value 1 and the second containing the value 2. The
	implementation of this function is optional.

	@param  aNumChannels
	        A reference to an array of supported number of channels.
	*/
	inline virtual void GetSupportedNumChannelsL(RArray<TUint>& aNumChannels);

	/**
	Gets the supported data types for the given media type id.

	The default leaves with KErrNotSupported.

    The implementation of this procedure should return an array of data types supported by the format where
	applicable. Note that this refers to the possible data types that may be supported by the format, not the
	actual data type of the format, which will be one of the supported data types. For example, for a format
	decode plugin capable of supporting pcm16 and GSM610 the procedure would return an array of length two the
	first array member containing the fourCC code ' P16' and the second containing the value GSM6. The
	implementation of this function is optional.

	@param  aMediaId
	        The media type id.
	@param  aDataTypes
	        A reference to an array of supported data types.
	*/
	inline virtual void GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes);

	/**
	Used by the sink to suggest a source buffer size.

	This is an optional function provided so that a controller can suggest a buffer
	size for the format. The controller should not assume that the format will accept
	the suggested buffer size and there is no obligation on behalf of the format to
	use the suggested buffer size.

	@param  aSuggestedBufferSize
	        A recommended buffer size that the format should create.
	*/
	inline virtual void SuggestSourceBufferSize(TUint aSuggestedBufferSize);

	/**
	Used to set the format's position.

	Subsequent data reads should ignore the FrameNumber in the CMMFBuffer and use this
	setting to determine what data to provide.

	The actual position the format sets itself may vary from this setting to ensure
	that it is aligned to the sample boundaries ensuring consistent data output.

	If not supported, positional information should be extracted from the FrameNumber in CMMFBuffer

	@param  aPosition
	        The position the format should use.
	*/
    inline virtual void SetPositionL(const TTimeIntervalMicroSeconds& aPosition);


	/**
	Supplies the current position.

	Subsequent data reads will commence from this position.

	@return The current position in microseconds.
	*/
	virtual TTimeIntervalMicroSeconds PositionL() {User::Leave(KErrNotSupported);return TTimeIntervalMicroSeconds(0);}
	
protected:
	//ConstructSourceL should never be called. The CMMFFormatDecode NewL functions should
	//always be used to instantiate a CMMFFormatDecode object (not MDataSource::NewL)
	/**
	@internalAll
	*/
	virtual void ConstructSourceL( const TDesC8& /*aInitData*/ ) {User::Leave(KErrNotSupported);}

	//ConstructSinkL should never be called. The CMMFFormatDecode NewL functions should
	//always be used to instantiate a CMMFFormatDecode object (not MDataSink::NewL)
	/**
	@internalAll
	*/
	virtual void ConstructSinkL( const TDesC8& /*aInitData*/ ) {User::Leave(KErrNotSupported);}

	/**
	Default constructor
	*/
	CMMFFormatDecode() : MDataSource(KUidMmfFormatDecode), MDataSink(KUidMmfFormatDecode) {};

	// Creates and initialises format plugin.
	static CMMFFormatDecode* CreateFormatL(TUid aImplementationUid, MDataSource* aSource);
protected:

	/**
	The clip is the source for the decode format.
	*/
	MDataSource* iClip;

	/** 
	The data path is the sink for the decode format.
	*/
	MDataSink* iDataPath;
private:
	TUid iDtor_ID_Key; 			// do not move - referenced inline in ~CMMFFormatDecode()
	TUid iImplementationUid;	// do not move - referenced inline in ImplementationUid()
	};


/**
Extension class to allow derived classes to support custom interfaces

@publishedAll
@released
*/

class CMMFFormatDecode2 : public CMMFFormatDecode
	{
public:
	/**
	Gets a custom interface.

	@param aInterfaceId
			The Uid of the particular interface required.
 
	@return Custom interface pointer. NULL if the requested interface is not supported.
	*/
	virtual TAny* CustomInterface(TUid aInterfaceId)=0;
	};


/**
@publishedAll
@released

Base class from which sink formats can be derived from.   

The intended usage is for controllers that can support more than one type of format.
The class is an MDataSink as far as the data path is concerned but is an MDataSource to the clip
that is the sink of the actual data. All CMMFFormatEncode plugin DLLs must include 
interface_uid = KMmfUidPluginInterfaceFormatEncode in their .rss files.
*/
class CMMFFormatEncode : public CBase, public MDataSource, public MDataSink
	{
public:
	// ECOM creation.
	IMPORT_C static CMMFFormatEncode* NewL( TUid aUid, MDataSink* aSink );
	IMPORT_C static CMMFFormatEncode* NewL( const TDesC& aFileName, MDataSink* aSink, const TDesC& aPreferredSupplier ) ;
	IMPORT_C static CMMFFormatEncode* NewL( const TDesC8& aSourceHeader,  MDataSink* aSink, const TDesC& aPreferredSupplier ) ;
	IMPORT_C static CMMFFormatEncode* NewL( MDataSink* aSink, const TDesC& aPreferredSupplier ) ;

	/**
	Destructor.
	*/
	virtual ~CMMFFormatEncode()  {REComSession::DestroyedImplementation(iDtor_ID_Key);}

	// returns ECOM plugin uid of this format
	/**
	Gets the ECom plugin UID of this format.
	
	@return The plugin UID.
	*/
	TUid ImplementationUid() const {return iImplementationUid;}

	/**
	Returns the time interval for one frame for the specified media type.

	This is a virtual function that each derived class must implement.

	@param  aMediaType
	        The media type ID.
	@return The frame time interval in microseconds.
	*/
	virtual TTimeIntervalMicroSeconds FrameTimeInterval(TMediaId aMediaType) const = 0;

	/**
	Returns the duration of the sink clip for the specified media type.

	This is a virtual function that each derived class must implement.

	@param  aMediaType
	        The media type ID.
	
	@return The duration of the sink clip.
	*/
	virtual TTimeIntervalMicroSeconds Duration(TMediaId aMediaType) const = 0;

	/**
	@internalAll
	
	Request from CMMFDataPath to fill the specified buffer.

	@param  aBuffer
	        The buffer to fill.
	@param  aConsumer
	        The consumer.
	@param  aMediaId
	        The media ID.
	*/
	inline virtual void FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId aMediaId);

	/**
	Called by the clip when it has added the data to the file.

	@param  aBuffer
	        The emptied buffer.
	*/
	virtual void BufferEmptiedL(CMMFBuffer* aBuffer) {iDataPath->BufferEmptiedL(aBuffer);}

	/**
	Tests whether a source buffer can be created.

	The default implementation returns EFalse.

	@return	A boolean indicating if the buffer can be created. ETrue if buffer can be created, EFalse otherwise.
	*/
	virtual TBool CanCreateSourceBuffer() {return EFalse;}

	/**
	Creates a source buffer. The default returns NULL.

	@param  aMediaId
	        The media type id.
	@param  aReference
	        If ETrue the MDataSource owns the buffer.
	        If EFalse, then the caller owns the buffer.

	@return	Source buffer.
	*/
	inline virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, TBool &aReference);

	/**
	Returns the source data type code for the specified media type ID.

	This is a virtual function that each derived class must implement.

	@param  aMediaId
	        The media type id.

	@return The source data type code.
	*/
	inline virtual TFourCC SourceDataTypeCode(TMediaId aMediaId);

	/**
	Adds a buffer to a clip.

	Called by CMMFDataPath.
	(from MDataSink - CMMFFormatEncode is a MDataSink to a CMMFDataPath)

	This is a virtual function that each derived class must implement.

	@param  aBuffer
	        The buffer to which the clip is added.
	@param  aSupplier
	        The data source.
	@param	aMediaId
	        The media type ID.
	*/
	virtual void EmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId aMediaId)=0;

	/**
	@internalAll

	Called by the CMMFDataPath's MDataSource when it has filled the buffer.

	@param  aBuffer
	        The buffer that has been filled.
	*/
	inline virtual void BufferFilledL(CMMFBuffer* aBuffer);

	/**
	Tests whether a sink buffer can be created.

	Format would normally pass its own buffer onto the CMMFClip, so
	this may not be required. The default returns ETrue.

	@return	A boolean indicating if the buffer can be created. ETrue if buffer can be created, EFalse otherwise.
	*/
	virtual TBool CanCreateSinkBuffer() {return ETrue;}

	/**
	Creates a sink buffer for the specified media ID.

	This is a virtual function that each derived class must implement.

	@param  aMediaId
	        The media type ID.
	@param  aReference
	        If ETrue then MDataSink owns the buffer.
	        If EFalse, then the caller owns the buffer.

	@return A pointer to the CMMFBuffer sink buffer.
	*/
	virtual CMMFBuffer* CreateSinkBufferL(TMediaId aMediaId, TBool &aReference)=0;

	/**
	Returns the sink data type code for the specified media type ID.

	This would be the same as the source data type four CC although the clip
	is not going to need this info.

	This is a virtual function that each derived class must implement.

	@param  aMediaId
	        The media type ID.

	@return The sink data type code.
	*/
	virtual TFourCC SinkDataTypeCode(TMediaId aMediaId) = 0;

	/**
	This function is used to truncate the sink ie. a CMMFClip,

	If aToEnd = ETrue the sink is cropped from the aPosition to the	end of the clip.
	If aToEnd = EFalse then the sink is cropped from the start of the clip to aPosition.

	This function would be called by the CMMFController. The default implementation leaves
	with KErrNotSupported.

	@param  aPosition
	        The position within the clip.
	@param  aToEnd
	        Flag to determine which part of the clip to delete.
	*/
	inline virtual void CropL(TTimeIntervalMicroSeconds aPosition, TBool aToEnd = ETrue);


	/**
	Gets the number of meta data entries.

	The encode format is capable of reading and writing meta data to the clip.

	The default implementation leaves with KErrNotSupported.

	@param  aNumberOfEntries
	        On return, contains the number of meta data entries.
	*/
	inline virtual void GetNumberOfMetaDataEntriesL(TInt& aNumberOfEntries);

	/**
	Returns the specified meta data entry.

	The default implementation leaves with KErrNotSupported.

	@param  aIndex
	        The zero based meta data entry index to retrieve.

	@return The meta data entry.
	*/
	inline virtual CMMFMetaDataEntry* MetaDataEntryL(TInt aIndex);

	/**
	Adds the specified meta data entry to the clip.

	The default implementation leaves with KErrNotSupported.

	@param  aNewEntry
	        The meta data entry to add.
	*/
	inline virtual void AddMetaDataEntryL(const CMMFMetaDataEntry& aNewEntry);

	/**
	Removes the specified meta data entry.

	The default implementation returns KErrNotSupported.

	@param  aIndex
	        The zero based meta data entry index to remove.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	 */
	inline virtual TInt RemoveMetaDataEntry(TInt aIndex);

	/**
	Replaces the specified meta data entry with the entry supplied in aNewEntry.

	The default implementation leaves with KErrNotSupported.

	@param  aIndex
	        The zero based meta data entry index to replace.
	@param  aNewEntry
	        The meta data entry to replace.
	*/
	inline virtual void ReplaceMetaDataEntryL(TInt aIndex, const CMMFMetaDataEntry& aNewEntry);

	//audio format methods

	/**
	Sets the number of channels.

	The default implementation returns KErrNotSupported.

	@param  aChannels
	        The number of channels.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	inline virtual TInt SetNumChannels(TUint aChannels);

	/**
	Sets the sample rate.

	The default implementation returns KErrNotSupported.

	@param  aSampleRate
	        The sample rate.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	inline virtual TInt SetSampleRate(TUint aSampleRate);

	/**
	Sets the bit rate.

	The default implementation returns KErrNotSupported.

	@param  aBitRate
	        The bit rate.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	inline virtual TInt SetBitRate(TUint aBitRate);

	/**
	Returns the number of channels.

	The default implementation returns 0.

	@return The number of channels.
	*/
	virtual TUint NumChannels() {return 0;}

	/**
	Returns the sample rate.

	The default implementation returns 0.

	@return The sample rate.
	*/
	virtual TUint SampleRate() {return 0;}

	/**
	Returns the default sample rate.

	The default returns 0.

	@return The default sample rate.
	*/
	virtual TUint GetDefaultSampleRate() {return 0;}

	/**
	Returns the bit rate.

	The default returns 0.

	@return The bit rate.
	*/
	virtual TUint BitRate() {return 0;}

	/**
	Returns the bytes per second.

	The default returns 0.

	@return The bytes per second.
	*/
	virtual TInt64 BytesPerSecond() {return 0;}

	/**
	Gets the supported sample rates.

	The default implementation leaves with KErrNotSupported.

	@param  aSampleRates
	        A reference to an array of supported sample rates.
	*/
	inline virtual void GetSupportedSampleRatesL(RArray<TUint>& aSampleRates);

	/**
	Gets the supported bit rates.

	The default implementation leaves with KErrNotSupported.

	@param  aBitRates
	        A reference to an array of supported bit rates.
	*/
	inline virtual void GetSupportedBitRatesL(RArray<TUint>& aBitRates);

	/**
	Gets the supported number of channels.

	The default implementation leaves with KErrNotSupported.

	@param  aNumChannels
	        A reference to an array of supported number of channels.
	*/
	inline virtual void GetSupportedNumChannelsL(RArray<TUint>& aNumChannels);

	/**
	Gets the supported data types for the given media type ID.

	The default implementation leaves with KErrNotSupported.

	@param  aMediaId
	        The media type id.
	@param  aDataTypes
	        A reference to an array of supported data types.
	*/
	inline virtual void GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes);

	/**
	Sets the maximum clip size.

	The default implementation leaves with KErrNotSupported.

	@param  aBytes
	        The maximum clip size.
	*/
	inline virtual void SetMaximumClipSizeL(TInt aBytes);

	/**
	Returns the maximum clip size.

	The default returns 0.

	@return The maximum clip size.
	*/
	virtual TInt MaximumClipSize() { return 0;}

	/**
	Used to set the format's position.

	Subsequent data reads should ignore the FrameNumber in the CMMFBuffer and use this
	setting to determine what data to provide.

	The actual position the format sets itself may vary to this setting to ensure
	that it is aligned to the sample boundaries ensuring consistent data output.

	If not supported, positional information should be extracted from the FrameNumber in CMMFBuffer

	@param  aPosition
	        The position the format should use.
	*/
    inline virtual void SetPositionL(const TTimeIntervalMicroSeconds& aPosition);


	/**
	Supplies the current position.

	Subsequent data reads will commence from this position.

	@return The current position in microseconds.
	*/
	virtual TTimeIntervalMicroSeconds PositionL() {User::Leave(KErrNotSupported);return TTimeIntervalMicroSeconds(0);}


protected:
	//ConstructSourceL should never be called. The CMMFFormatEncode NewL functions should
	//always be used to instantiate a CMMFFormatEncode object (not MDataSource::NewL)
	/**
	@internalAll
	*/
	virtual void ConstructSourceL( const TDesC8& /*aInitData*/ ) {User::Leave(KErrNotSupported);}
	//ConstructSinkL should never be called. The CMMFFormatEncode NewL functions should
	//always be used to instantiate a CMMFFormatEncode object (not MDataSink::NewL)
	/**
	@internalAll
	*/
	virtual void ConstructSinkL(  const TDesC8& /*aInitData*/ ) {User::Leave(KErrNotSupported);}

	/**
	Default constructor
	*/
	CMMFFormatEncode() : MDataSource(KUidMmfFormatEncode), MDataSink(KUidMmfFormatEncode) {};

	// Creates and initialises format plugin.
	static CMMFFormatEncode* CreateFormatL(TUid aImplementationUid, MDataSink* aSink);
protected:

	/** 
	The Data path is the source for the encode format.
	*/
	MDataSource* iDataPath;
	
	/** 
	The clip is the sink for the encode format.
	*/
	MDataSink* iClip;
private:
	TUid iDtor_ID_Key;				// do not move - referenced inline in ~CMMFFormatDecode()
	TUid iImplementationUid;		// do not move - referenced inline in ImplementationUid()
	};


class CMMFFormatPluginSelectionParameters;

/**
@publishedAll
@released

Provides an internal utility function to choose a format plugin from ECom.
*/
class MMFFormatEcomUtilities
	{
public:
	//Internal utility function to choose a format plugin from ECOM
	static TUid SelectFormatPluginL(const CMMFFormatPluginSelectionParameters& aSelectParams);

	// Internal utility function to instantiate each format decode plugin in turn
	// until we find one that works
	static CMMFFormatDecode* SelectFormatDecodePluginL(const CMMFFormatPluginSelectionParameters& aSelectParams, MDataSource* aSource);

	static CMMFFormatDecode* SelectFormatDecodePluginL(const CMMFFormatPluginSelectionParameters& aSelectParams, MDataSource* aSource, TBool& aSupportsCustomInterfaces);
	};

#include <mmf/server/mmfformat.inl>


#endif

