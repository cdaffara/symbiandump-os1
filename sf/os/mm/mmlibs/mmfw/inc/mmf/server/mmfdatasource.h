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
// include\mmf\Server\mmfdatasource.h
// 
//

#ifndef __MMF_SERVER_DATASOURCE_H__
#define __MMF_SERVER_DATASOURCE_H__

#include <e32base.h>
#include <mmf/server/mmfdatasourcesink.hrh>
#include <mmf/common/mmfutilities.h>
#include <ecom/ecom.h>
#include <mmf/common/mmfbase.h>
#include <mmf/common/mmfcontrollerframework.h>

//
// MDataSource mixim
//

class CMMFBuffer;
class MDataSink;
class MAsyncEventHandler;

/**
@publishedAll
@released

Abstract class representing a data source.
*/
class MDataSource
	{
public:
	static inline MDataSource* NewSourceL( TUid aImplementationUid,  const TDesC8& aInitData ) ;

	/**
	Destructor.
	*/
	virtual ~MDataSource() {REComSession::DestroyedImplementation(iDtor_ID_Key);};

	/**
	Returns the UID identifying the type of data source.

	@return	The UID identifying the type of data source.
	*/
	virtual TUid DataSourceType() const {return iDataSourceType;};

	/**
	Returns the data type as a fourCC code of the data source.

	This is a pure virtual function that each derrived class must implement.

	@param  aMediaId
	        This identifies the type of media eg. audio or video and the stream ID.
	        This parameter is required in cases where the source can supply data
	        of more than one media type and/or multiple streams of data.

	@return The fourCC code identifying the source datatype for the specified aMediaId.
	*/
	virtual TFourCC SourceDataTypeCode(TMediaId aMediaId) = 0;

	inline virtual TInt SetSourceDataTypeCode(TFourCC aSourceFourCC, TMediaId aMediaId);

	/**
	Function called by a MDataSink to request the data source to fill aBuffer with data.

	This is a pure virtual function that each derived class must implement.
	This method is used when a data source is passively waiting for requests from a consumer ie. a data sink
	to fill a buffer. The data source must call the BufferFilledL member on aConsumer when it has filled
	the buffer with data. The data source can either make this callback synchronously or asynchronously.

	The format should read the frame number of the buffer via the buffer's CMMFBuffer::FrameNumber()
	function. From this, the format should be able to determine the actual position of the data on
	the data source. The technique here depends on the nature of the format. For a linear audio
	format, the position can be obtained by a simple calculation of the frame size, the header size
	and where appropriate the datatype.

	For non-linear formats, either an index table of frame number and location will need to be
	created in the NewL() or some form of approximating algorithm will be required. Some formats
	have an index table as part of the format eg. AVI. If no random access is required then no index
	table is required, the format can keep track of the current read position and increment it on
	each access, and reset it if the frame number is reset to 0 or 1. Given that for non-linear
	ie. variable bit rate formats, the size of the data read may vary from frame to frame, then the
	format should either set the request size of the buffer for the required frame or call the
	source's ReadBufferL() (either CMMFClip::ReadBufferL(), CMMFDescriptor ::ReadBufferL() or
	CMMFFile::ReadBufferL()) function that takes the aLength parameter.

	It is the responsibility of the format decode to determine the size and position of the source
	data for each frame. For linear audio formats, the format decode should fill the buffer up to
	its maximum length. For multimedia formats e.g. mp4, AVI etc, the format decode is effectively
	acting as a demultiplexor, and must obtain the appropriate data from the source depending on
	the aMediaId.

	@param  aBuffer
	        The buffer that needs filling with data.
	@param  aConsumer
	        The data sink that consumes the data. The data source needs this to make the BufferFilledL
	        callback on aConsumer when the data source has completed filling the aBuffer.
	@param  aMediaId
	        This identifies the type of media eg. audio or video and the stream ID.
	        This parameter is required in cases where the source can supply data
	        of more than one media type and/or multiple strams of data eg. a multimedia file.
	*/
	virtual void FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer,TMediaId aMediaId)=0;

	/**
	Function called by a data sink to pass back an emptied buffer to the source.

	This is a pure virtual function that each derived class must implement.
	This method is used as the callback when the data source actively requests a consumer ie. a data
	sink to empty a buffer by calling the data sinks EmptyBufferL.
	When the data source gets this callback it knows that the buffer has been emptied and can be
	reused.

	@param  aBuffer
	        The buffer that has been emptied by a data sink and is now available for reuse.
	*/
	virtual void BufferEmptiedL(CMMFBuffer* aBuffer)=0;

	/**
	@deprecated

	Function to indicate whether the data source can create a buffer.

	This is a pure virtual function that each derived class must implement.

	@return	A boolean indicating if the data source can create a buffer. ETrue if it can otherwise
	        EFalse.
	*/
	virtual TBool CanCreateSourceBuffer()=0;

	/**
	@deprecated

	Returns a buffer created by the data source

	This is a pure virtual function that each derived class must implement.

	@param	aMediaId
	        This identifies the type of media eg. audio or video and the stream ID.
	        This parameter is required in cases where the source can supply data
	        of more than one media type and/or multiple strams of data eg a multimedia file.
	@param  aReference
	        This must be written to by the method to indicate whether the created buffer is
	        a 'reference' buffer.  A 'reference' buffer is a buffer that is owned by the source
	        and should be used in preference to the sink buffer provided the sink buffer
	        is also not a reference buffer.

	@return	A pointer to the created buffer.
	*/
	virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, TBool &aReference) = 0;

	inline virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& aSinkBuffer, TBool &aReference);

	inline virtual TInt SourceThreadLogon(MAsyncEventHandler& aEventHandler);

	/**
	Function to 'logoff' the data source from the same thread that source supplies data in.

	This method may be required as the thread that the data source is deleted in may not be
	the same thread that the data transfer took place in.  Therefore any thread specific
	releasing of resources needs to be performed in the SourceThreadLogoff rather than in the
	destructor.

	This is a virtual function that a derrived data source can implement if any thread specific
	releasing of resources is required.
	*/
	virtual void SourceThreadLogoff() {};

	inline virtual void NegotiateSourceL(MDataSink& aDataSink); //called if source setup depends on sink

	/**
	Indicates whether the data source supports sample rate conversion.

	This is a virtual function that a derived class can implement.

	@return	A boolean indicating if the data source can perform a sample rate conversion. ETrue if 
	        it can otherwise EFalse.
	*/
	virtual TBool SourceSampleConvert() {return EFalse;};
	
	/**
	Function to 'prime' the data source.
	
	This is a virtual function that a derrived data source can implement if
	any data source specific 'priming' is required.
	*/
	virtual void SourcePrimeL() {};
	
	/**
	Function to 'play' the data source.

	This is a virtual function that a derrived data source can implement if
	any data source specific action is required prior to 'playing' ie. the start of data transfer.
	*/
	virtual void SourcePlayL() {};

	/**
	Function to 'pause' the data source.

	This is a virtual function that a derrived data source can implement if
	any data source specific action is required to 'pause'
	*/
	virtual void SourcePauseL() {};

	/**
	Function to 'stop' the data source.

	This is a virtual function that a derrived data source can implement if
	any data source specific action is required to 'stop'.
	*/
	virtual void SourceStopL() {};

	inline virtual void SetSourcePrioritySettings(const TMMFPrioritySettings& aPrioritySettings);

	/**
	Function to call a source specific custom command.

	This is a virtual function that a derrived data source can implement if
	it implements any custom commands.

	@param  aMessage
	        The message specifying the custom command.
	*/
	virtual void SourceCustomCommand(TMMFMessage& aMessage) {aMessage.Complete(KErrNotSupported);};
protected:

	/**
	Performs any source construction dependant on the source construction
	initialisation data aInitData.

	This is a pure virtual function that a derrived data source must implement

	@param  aInitData
	        The source specific initialisation data required for source construction.
	*/
	virtual void ConstructSourceL(  const TDesC8& aInitData ) = 0 ;

	/**
	Protected constructor.
	*/
	MDataSource(TUid aType): iDataSourceType(aType) {}
private:
	TUid iDataSourceType;
	TUid iDtor_ID_Key;

	};

/**
Instantiates a data source.

@param  aImplementationUid
        The UID identifying the data source to be instantiated.
@param  aInitData
        The source specific initialisation data required for source construction.

@return	The instantiated data source.
*/
inline MDataSource* MDataSource::NewSourceL( TUid aImplementationUid, const TDesC8& aInitData )
	{
	MDataSource* retPtr = REINTERPRET_CAST( MDataSource*, REComSession::CreateImplementationL( aImplementationUid,
																			_FOFF(MDataSource, iDtor_ID_Key) ) ) ;
	CleanupDeletePushL(retPtr);
	retPtr->ConstructSourceL( aInitData ) ;

	CleanupStack::Pop(retPtr);
	return retPtr ;
	}

/**
Sets the data type as a fourCC code for the data source.

This is a virtual function that each derived class can implement if the data source supports
the ability to have its data type set.

@param  aSourceFourCC
        This specifies the data type as a fourCC code to set the source to.
@param  aMediaId
        This identifies the type of media eg. audio or video and the stream ID.
        This parameter is required in cases where the source can supply data
        of more than one media type and/or multiple strams of data eg a multimedia file.

@return	KErrNone if successful, KErrNotSupported if the source does not support having
        it's data type set, otherwise a system wide error code.
*/
inline TInt MDataSource::SetSourceDataTypeCode(TFourCC /*aSourceFourCC*/, TMediaId /*aMediaId*/)
{
	return KErrNotSupported;
}

/**
@deprecated

Returns a buffer created by the data source.

This is a virtual function that a derived class can implement.
This can be used in preference to the above CreateSourceBufferL method in cases where
the source buffer creation has a dependancy on the sink buffer.

@param  aMediaId
        This identifies the type of media eg. audio or video and the stream ID.
        This parameter is required in cases where the source can supply data
        of more than one media type and/or multiple strams of data eg a multimedia file
@param  aSinkBuffer
        The sink buffer the nature of which may influence the creation of the source buffer.
@param  aReference
        This must be written to by the method to indicate whether the created buffer is
        a 'reference' buffer.  A 'reference' buffer is a buffer that is owned by the source
        and should be used in preference to the sink buffer provided the sink buffer is not a
        reference buffer.

@return A pointer to the created buffer.
*/
inline CMMFBuffer* MDataSource::CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& /*aSinkBuffer*/, TBool &aReference)
{
	return CreateSourceBufferL(aMediaId, aReference);
}

/**
Function to 'logon' the data source to the same thread that source will be supplying data in.

This method may be required as the thread that the data source was created in is not always
the same thread that the data transfer will take place in.  Therefore any thread specific
initialisation needs to be performed in the SourceThreadLogon rather than in the creation
of the data source.

This is a virtual function that a derrived data source can implement if any thread specific
initialisation is required and/or the data source can create any asynchronous events.

@param  aEventHandler
        This is an MAsyncEventHandler to handle asynchronous events that occur during the
        transfer of multimedia data.  The event handler must be in the same thread as the data transfer
        thread. Hence the reason it is passed in the SourceThreadLogon as opposed to say the constructor.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
inline TInt MDataSource::SourceThreadLogon(MAsyncEventHandler& /*aEventHandler*/)
{
	return KErrNone;
}

/**
Function to allow the data source to negotiate with a data sink

This method is required in cases where the settings of data source are dependant on those of a
data sink. This is a virtual function that a derrived data source can implement.

@param  aDataSink
        The data sink whose settings can affect the data source.
*/
inline void MDataSource::NegotiateSourceL(MDataSink& /*aDataSink*/)
{
}

/**
Function to set the source priority settings.

This is a virtual function that a derrived data source can implement if
a priority mechanism is required to arbitrate between multiple clients
trying to access the same resource.

@param  aPrioritySettings
        The source priority settings.

*/
inline void MDataSource::SetSourcePrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/)
{
}

/**
This function is used by TCleanupItem objects to perform
a SourceStopL() when leaving functions exist, ususally between SourcePrimeL-SourceStopL pairs.

@param  aSource
        The data source to be stopped.
*/
inline static void DoDataSourceStop(TAny* aSource)
	{
	MDataSource* source = STATIC_CAST(MDataSource*, aSource);
	// we don't want this function to leave because no leaving functions are supposed
	// to be used as argument to the TCleanupItem objects. Hence we catch the error but
	// we do nothing with it.
	TRAP_IGNORE(source->SourceStopL());
	}

/**
This method is used by TCleanupItem objects to perform a SourceThreadLogoff().

@param  aSource
        The data source to be logged off.
*/
inline static void DoDataSourceThreadLogoff(TAny* aSource)
	{
	MDataSource* source = STATIC_CAST(MDataSource*, aSource);
	source->SourceThreadLogoff();
	}

#endif
