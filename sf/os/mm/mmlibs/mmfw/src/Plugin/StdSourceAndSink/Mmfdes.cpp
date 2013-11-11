// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <f32file.h>
#include <e32std.h>
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmfpaniccodes.h>

void Panic(TMMFDescriptorPanicCode aPanicCode)
	{
	_LIT(KMMFDescriptorPanicCategory, "MMFDescriptor");
	User::Panic(KMMFDescriptorPanicCategory, aPanicCode);
	}

/**
Protected constructor.

Sets the offset to zero.
*/
CMMFDescriptor::CMMFDescriptor( ) : CMMFClip( KUidMmfDescriptorSource, KUidMmfDescriptorSink ) 
	{
	}

/**
Destructor.

The default implementation closes the descriptor thread.
*/
CMMFDescriptor::~CMMFDescriptor()
	{
	iDesThread.Close() ;
	}

/**
Constructs a CMMFDescriptor MDataSource.

@return A pointer to a new CMMFDescriptor.
*/
MDataSource* CMMFDescriptor::NewSourceL( )
	{
	CMMFDescriptor* self = new (ELeave) CMMFDescriptor( ) ;
	return STATIC_CAST( MDataSource*, self ) ;
	}

/**
Constructs a CMMFDescriptor MDataSink.

@return A pointer to a new CMMFDescriptor.
*/
MDataSink* CMMFDescriptor::NewSinkL( )
	{
	CMMFDescriptor* self = new (ELeave) CMMFDescriptor( ) ;
	return STATIC_CAST( MDataSink*, self ) ;
	}

/**
Performs source construction dependant on the source construction
initialisation data aInitData.

@param  aInitData
        The TPckgC<TMMFDescriptorParams> descriptor package containing the descriptor and the thread 
        ID for the descriptor.
*/
void CMMFDescriptor::ConstructSourceL( const TDesC8& aInitData )
	{
	ConstructL( aInitData ) ;
	}


/***
Sets how much of the underlying descriptor will be used, up	to the underlying descriptor's maximum
length.

@param  aSize
        The size of the descriptor.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
*/
TInt CMMFDescriptor::SetSize( TInt aSize )
	{
	//[ precondition aSize >= 0
	// precondition sSize < MaxSize()
	// iDes is not null]
	 if(!iDes )
		 return KErrNotReady;

     if( aSize > MaxLength() )
		 return KErrOverflow;

	 if( aSize < 0 )
		 return KErrUnderflow;

	 // [ actually do the work ]
	 iDes->SetLength( aSize );

	 //[ assert the post condition
	 // aSize == Length()
	 // descriptor is still ok]
     ASSERT( aSize == iDes->Length());
	 ASSERT( iDes );

	 return KErrNone;
	}


/**
Performs sink construction dependant on the sink construction initialisation data aInitData.

@param  aInitData
        The TPckgC<TMMFDescriptorParams> descriptor package containing
        the descriptor and the thread ID for the descriptor.
*/
void CMMFDescriptor::ConstructSinkL( const TDesC8& aInitData )
	{
	ConstructL( aInitData ) ;
	}

void CMMFDescriptor::ConstructL( const TDesC8& aInitData )
	{
	TMMFDescriptorParams params;
	TPckgC<TMMFDescriptorParams> config(params);
	if (aInitData.Length() < config.Length())
		User::Leave(KErrGeneral);
	config.Set(aInitData);
	iDes = STATIC_CAST( TDes8*, config().iDes);
	User::LeaveIfError( iDesThread.Open( config().iDesThreadId ) );
	}


/** 
Loads aBuffer from iDes.

File read is asynchronous.  CReadRequest is created to respond to completion.

@param  aBuffer
        The buffer to be filled from the descriptor.
@param  aConsumer
        The data sink consumer of the buffer.
@param  aMediaId
        Not used.
*/
void CMMFDescriptor::FillBufferL( CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId /*aMediaId*/  ) 
	{
	// Current position in Descriptor is iOffset.

	// Read from iDes in iDesThread into Des in aBuffer.

	// Assume that the amount to be read is the size of the buffer descriptor
	// Should check that there is sufficient data in the source buffer
	// If there is not enough to fill the target then copy what there is
	// How should the function deal with no data in the source buffer?

	// Use of a single iOffset will preclude use by more than one client (use ReadBufferL())
	if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		TDes8& bufferDes = STATIC_CAST(CMMFDataBuffer*, aBuffer)->Data();

		//if request size is set, use it, else use max length of buffer
		TInt targetMaxLength = aBuffer->RequestSize() ? aBuffer->RequestSize() : bufferDes.MaxLength();

		//ensure RequestSize was within bounds
		if(targetMaxLength > bufferDes.MaxLength())
			targetMaxLength = bufferDes.MaxLength();

		TInt sourceLengthRemaining = iDes->Length() - iOffset;
		if ( ( sourceLengthRemaining - targetMaxLength ) > 0 )
			{
			bufferDes = iDes->Mid(iOffset,targetMaxLength);
			iOffset += targetMaxLength;
			}
		else if (sourceLengthRemaining > 0)
			{
			bufferDes = iDes->Mid(iOffset,sourceLengthRemaining);
			iOffset += sourceLengthRemaining;
			aBuffer->SetLastBuffer(ETrue);
			}
		else
			{
			bufferDes.SetLength(0);
			aBuffer->SetLastBuffer(ETrue);
			}

		aConsumer->BufferFilledL( aBuffer ) ;
		}
	else
		User::Leave(KErrNotSupported);
	}

/**
Empties aBuffer into iDes.

@param  aBuffer
        The buffer to be written to the descriptor.
@param  aSupplier
        The data source supplier of the buffer.
@param  aMediaId
        The Media ID.
*/
void CMMFDescriptor::EmptyBufferL( CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId /*aMediaId*/ )
	{
	// Does the buffer type need to be checked?

	// Assume that the amount to be read is the size of the buffer descriptor
	// Should check that there is sufficient data in the source buffer
	// If there is not enough to fill the target then copy what there is
	// How should the function deal with no data in the source buffer?
	if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		TDes8& bufferDes = STATIC_CAST(CMMFDataBuffer*, aBuffer)->Data();

		TInt sourceLength = bufferDes.Length() ;	
		TInt targetLength = iDes->MaxLength() - iDes->Length();
		if ( targetLength>0 )
			{
			if (sourceLength>targetLength)
				{
				sourceLength = targetLength;
				bufferDes.SetLength(targetLength);
				}

			iDes->Append(bufferDes) ;
			}
		else
			bufferDes.SetLength(0);

		aSupplier->BufferEmptiedL( aBuffer ) ;
		}
	else
		User::Leave(KErrNotSupported);
	}

/** 
Loads aLength number of bytes into aBuffer from specified point in iDes.

@param  aLength
        The number of bytes to be read into buffer.
@param  aBuffer
        The buffer to be filled from the descriptor.
@param  aPosition
        The offset into the descriptor at which to start reading.
@param  aConsumer
        The data sink consumer of the buffer
*/
void CMMFDescriptor::ReadBufferL(TInt aLength, CMMFBuffer* aBuffer, TInt aPosition, MDataSink* aConsumer)
	{
	if (!aBuffer)
		User::Leave(KErrArgument);

	if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		TDes8& bufferDes = STATIC_CAST(CMMFDataBuffer*, aBuffer)->Data();

		if (aLength>bufferDes.MaxLength())
			User::Leave(KErrOverflow);

		if ((aLength<0) || (aPosition<0))
			User::Leave(KErrArgument);

		TInt sourceLength = iDes->Length() ;

		//ensure not trying to read more than is available
		if(aPosition + aLength > sourceLength)
			aLength = sourceLength - aPosition;

		if (aLength>0)
			{
			TPtrC8 srcPtr(iDes->Mid(aPosition,aLength));
			bufferDes.Copy(srcPtr.Ptr(),aLength);
			}
		else
			bufferDes.SetLength(0);

		//have we read all the available data
		if(aPosition + aLength >= sourceLength)
			aBuffer->SetLastBuffer(ETrue);

		if (aConsumer)
			aConsumer->BufferFilledL(aBuffer);
		}
	else
		User::Leave(KErrNotSupported);
	}

/**
Loads aBuffer from specified point in iDes

@param  aBuffer
        The buffer to be filled from the descriptor.
@param  aPosition
        The offset into the descriptor at which to start reading.
@param  aConsumer
        The data sink consumer of the buffer.
*/
void CMMFDescriptor::ReadBufferL(CMMFBuffer* aBuffer, TInt aPosition, MDataSink* aConsumer)
	{
	if (!aBuffer)
		User::Leave(KErrArgument);

	if (aPosition<0)
		User::Leave(KErrArgument);

	if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		TDes8& bufferDes = STATIC_CAST(CMMFDataBuffer*, aBuffer)->Data();
		//if request size is set, use it, else use max length of buffer
		TUint copyLength = aBuffer->RequestSize() ? aBuffer->RequestSize() : bufferDes.MaxLength();

		ReadBufferL(copyLength, aBuffer, aPosition, aConsumer);
		}
	else
		User::Leave(KErrNotSupported);
	}

 /** 
Loads aBuffer from specified point in iDes.  Note that this is a synchronous read.

@param  aBuffer
        The buffer to be filled from the descriptor.
@param  aPosition
        The offset into the descriptor at which to start reading.
*/
void CMMFDescriptor::ReadBufferL(CMMFBuffer* aBuffer, TInt aPosition)
	{
	ReadBufferL(aBuffer, aPosition, NULL);
	}

/**
Empties aBuffer into iDes at specified location.

@param  aBuffer
        The data buffer containing bytes to be written.
@param  aPosition
        The offset into the descriptor at which to start writing.
@param  aSupplier
        The data source to be notified when the write has been completed.
*/
void CMMFDescriptor::WriteBufferL(CMMFBuffer* aBuffer, TInt aPosition, MDataSource* aSupplier) 
	{
	if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		TDes8& bufferDes = STATIC_CAST(CMMFDataBuffer*, aBuffer)->Data();

		WriteBufferL(bufferDes.Length(), aBuffer, aPosition, aSupplier);
		}
	else
		User::Leave(KErrNotSupported);
	}

/**
Empties aLength bytes from aBuffer into iDes at specified location.

@param  aLength
        The number of bytes to be emptied from buffer.
@param  aBuffer
        The data buffer containing bytes to be written.
@param  aPosition
        The offset into the descriptor at which to start writing.
@param  aSupplier
        The data source to be notified when the write has been completed.

@leave  KErrNotReady
        If SinkPrimeL() and SinkThreadLogon() have not been called.
@leave  KErrArgument
        If aLength<0 or aPosition<0 or aSupplier is NULL.
@leave  KErrNotSupported 
        If aBuffer is not of type KMMFDataBuffer.
*/
void CMMFDescriptor::WriteBufferL(TInt aLength, CMMFBuffer* aBuffer, TInt aPosition, MDataSource* aSupplier)
	{
	if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		TDes8& bufferDes = STATIC_CAST(CMMFDataBuffer*, aBuffer)->Data();

		if (aLength>bufferDes.Length() || (aLength<0) || (aPosition<0))
			User::Leave(KErrArgument);

		TInt sourceLength = aLength;
		TPtr8 bufferPtr(((sourceLength) ? &bufferDes[0] : NULL), sourceLength, sourceLength);
		TInt targetLength = iDes->MaxLength() - aPosition;
		if (targetLength>0 && sourceLength > 0)
			{
			if (sourceLength>targetLength)
				User::Leave(KErrOverflow);

			if ((iDes->Length() - aPosition) > 0)
				{
				TInt bytesToReplace = iDes->Length() - aPosition;
				if (sourceLength > bytesToReplace) 
					{
					TPtrC8 replaceBuf = bufferPtr.Left(bytesToReplace);
					TPtrC8 appendBuf = bufferPtr.Right(sourceLength-bytesToReplace);
					iDes->Replace(aPosition, bytesToReplace, replaceBuf);
					iDes->Append(appendBuf);
					} 
				else
					iDes->Replace(aPosition, sourceLength, bufferPtr);

				} 
			else
				iDes->Append(bufferPtr.Ptr(),sourceLength);
			}
		else if (targetLength<0)
			User::Leave(KErrArgument);
		else if (aLength != 0)
			User::Leave(KErrOverflow);

		if (aSupplier)
			aSupplier->BufferEmptiedL(aBuffer);
		}
	else
		User::Leave(KErrNotSupported);
	}

/** 
Empties aBuffer into iFile at specified location.  Note that this is a synchronous write

@param  aBuffer
        The data buffer containing bytes to be written.
@param  aPosition
        The offset into file at which to start writing.
*/
void CMMFDescriptor::WriteBufferL( CMMFBuffer* aBuffer, TInt aPosition)
	{
	WriteBufferL( aBuffer, aPosition, NULL );
	}

/**
Returns the amount of space available for the clip.

@return The space available in descriptor (difference between length and maxlength).
*/
TInt64 CMMFDescriptor::BytesFree() 
	{
	TInt64 length = iDes->Length() ;
	TInt64 maxLength =  iDes->MaxLength() ;
	return( maxLength - length ) ;
	}

/**
Returns the length of the clip.

@return The length (not max length) of the descriptor.
*/
TInt CMMFDescriptor::Size() 
	{
	TInt length = iDes->Length();
	return(length);
	}

/**
Returns the data type as a fourCC code for the CMMFDescriptor data source.

@param  aMediaId
        The ID of the media for which the codec is obtained.

@return The data type fourCC code.
*/
TFourCC CMMFDescriptor::SourceDataTypeCode(TMediaId /*aMediaId*/) 
	{
	return iSourceFourCC ;
	}

/**
Returns the data type as a fourCC code of the CMMFDescriptor data sink.

Used by MDataSource and MDataSink.

@return The data type fourCC code.
*/
TFourCC CMMFDescriptor::SinkDataTypeCode(TMediaId /*aMediaId*/) 
	{
	return iSinkFourCC ;
	}

/**
CMMFDescriptor as a source is always passive so this function is not supported.

@param  aBuffer
        The emptied buffer.
*/
void CMMFDescriptor::BufferEmptiedL( CMMFBuffer* /*aBuffer*/ )
	{
	Panic(EMMFDescriptorPanicBufferEmptiedLNotSupported);
	}

/**
Tests whether a source buffer can be created.

@return	A boolean indicating if the buffer can be created. EFalse if a CMMFDescriptor cannot create 
        it's own buffer
*/
TBool CMMFDescriptor::CanCreateSourceBuffer()
	{
	return EFalse ;
	}

/**
Creates a source buffer.

@param  aMediaId
        The Media ID.
@param  aReference
        A boolean indicating if MDataSource owns the buffer. ETrue if MDataSource owns the buffer,
        EFalse if the caller owns the buffer.

@return	NULL as a CMMFFile cannot create it's own buffer
*/
CMMFBuffer* CMMFDescriptor::CreateSourceBufferL(  TMediaId /*aMediaId*/, TBool& /*aReference*/ )
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

/**
CMMFDescriptor as a sink is always passive so this function is not supported.

@param  aBuffer
        The filled buffer.
*/
void CMMFDescriptor::BufferFilledL( CMMFBuffer* /*aBuffer*/ )
	{
	Panic(EMMFDescriptorPanicBufferFilledLNotSupported);
	}

/**
Tests whether a sink buffer can be created.

@return A boolean indicating if the sink buffer can be created. EFalse if a CMMFDescriptor cannot 
        create it's own buffer.
*/
TBool CMMFDescriptor::CanCreateSinkBuffer()
	{
	return EFalse ;
	}

/**
Creates a sink buffer.

@param  aMediaId
        The Media ID.
@param  aReference
        A boolean indicating if MDataSource owns the buffer. ETrue if MDataSource owns the buffer,
        EFalse if the caller owns the buffer.

@return	NULL as a CMMFDescriptor cannot create it's own buffer
 */
CMMFBuffer* CMMFDescriptor::CreateSinkBufferL( TMediaId /*aMediaId*/ , TBool& /*aReference*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}
