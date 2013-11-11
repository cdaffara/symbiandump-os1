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

#ifndef __MMFCLIP_H_
#define __MMFCLIP_H_

#include <mmf/server/mmfdatasource.h>
#include <mmf/server/mmfdatasink.h>
#include <mmf/server/mmfbuffer.h>
#include <mmf/common/mmfutilities.h>

/**
@publishedAll
@released

Abstract class to represent a source or sink that contains a multimedia clip (i.e. not a stream or hardware device).

Typical examples are a file or an area of memory (descriptor).
*/
class CMMFClip : public CBase, public MDataSource, public MDataSink
	{
public :
	//asynchronous Read/WriteBufferLs

	/**
	Reads aLength number of bytes of data from the offset, aPosition into the buffer, aBuffer.
	Intended for asynchronous usage.

	This is a virtual function that each derived class must implement.

	@param  aLength
	        The number of bytes to read.
	@param  aBuffer
	        The buffer to read the data into.
	@param  aPosition
	        The offset from which to start reading.
	@param  aConsumer
	        The sink of tha data read from the clip. Will be informed of read if not NULL.
	*/
	virtual void ReadBufferL( TInt aLength, CMMFBuffer* aBuffer, TInt aPosition, MDataSink* aConsumer) = 0 ;

	/**
	Writes aLength number of bytes of data from the offset, aPosition from the buffer, aBuffer.
	Intended for asynchronous usage.

	This is a virtual function that each derived class must implement.

	@param  aLength
	        The number of bytes to write.
	@param  aBuffer
	        The buffer to write the data into.
	@param  aPosition
	        The offset from which to start writing.
	@param  aSupplier
	        The source of the data writen to the clip. Will be informed of write if not NULL.
	*/
	virtual void WriteBufferL( TInt aLength, CMMFBuffer* aBuffer, TInt aPosition, MDataSource* aSupplier) = 0 ;

	/**
	Reads the maximum number of bytes of data from the offset, aPosition into the buffer, aBuffer.
	Intended for asynchronous usage.

	This is a virtual function that each derived class must implement.

	@param  aBuffer
	        The buffer to read the data into.
	@param  aPosition
	        The offset from which to start reading.
	@param  aConsumer
	        The sink of tha data read from the clip. Will be informed of read if not NULL.
	*/
	virtual void ReadBufferL( CMMFBuffer* aBuffer, TInt aPosition, MDataSink* aConsumer) = 0 ;

	/**
	Writes the maximum number of bytes of data from the offset, aPosition from the buffer, aBuffer.
	Intended for asynchronous usage.

	This is a virtual function that each derived class must implement.

	@param  aBuffer
	        The buffer to write the data into.
	@param  aPosition
	        The offset from which to start writing.
	@param  aSupplier
	        The source of the data writen to the clip. Will be informed of write if not NULL.
	*/
	virtual void WriteBufferL( CMMFBuffer* aBuffer, TInt aPosition, MDataSource* aSupplier) = 0 ;

	//synchronous Read/WriteBufferLs

	/**
	Reads the maximum number of bytes of data from the offset, aPosition into the buffer, aBuffer.
	Intended for synchronous usage.

	This is a virtual function that each derived class must implement.

	@param  aBuffer
	        The buffer to read the data into.
	@param  aPosition
	        The offset from which to start reading.
	*/
	virtual void ReadBufferL( CMMFBuffer* aBuffer, TInt aPosition) = 0 ;

	/**
	Writes the maximum number of bytes of data from the offset, aPosition from the buffer, aBuffer.
	Intended for synchronous usage.

	This is a virtual function that each derived class must implement.

	@param  aBuffer
	        The buffer to write the data into.
	@param  aPosition
	        The offset from which to start writing.
	*/
	virtual void WriteBufferL( CMMFBuffer* aBuffer, TInt aPosition) = 0 ;

	/**
	Returns the amount of space available for the clip.

	This is a virtual function that each derived class must implement.

	@return The amount of space available.
	*/
	virtual TInt64 BytesFree() = 0 ;

	/**
	Returns the size of the clip in bytes.

	This is a virtual function that each derived class must implement.

	@return	The size of the clip.
	*/
	virtual TInt Size() = 0 ;

	/**
	Deletes the clip. 

	This should be overriden in the derived	classes, the default version returns KErrNotSupported.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	virtual TInt Delete()	{return KErrNotSupported;}

	/**
	Sets the size of the clip. This should be overriden in the derived
	classes, the default version returns KErrNotSupported.

	@param  aSize
	        The size of the clip.

	@return An error code indicating if the function call was successful. KErrNone on success, otherwise
	        another of the system-wide error codes.
	*/
	inline virtual TInt SetSize(TInt aSize);

protected :
	/**
	Protected constructor.

	@param  aSourceType
	        The source type.
	@param  aSinkType
	        The sink type.
	*/
	CMMFClip( TUid aSourceType, TUid aSinkType ) : MDataSource( aSourceType ), MDataSink( aSinkType ) {}
	} ;

inline TInt CMMFClip::SetSize(TInt /*aSize*/)
{
	return KErrNotSupported;
}


#endif
