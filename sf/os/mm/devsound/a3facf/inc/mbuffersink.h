// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @released
*/

#ifndef MBUFFERSINK_H
#define MBUFFERSINK_H

#include <a3f/a3fbase.h>
#include <a3f/maudiodataconsumer.h>

class CMMFBuffer;
class MMMFAudioDataConsumer;

/**
 * Buffer sink is used for receiving the audio data in buffers outside the audio process.
 *
 * The type of the actual 'physical' sink can be for example a file or a socket or the
 * data could also be simple analysed and then discarded. Nevertheless, the client
 * (the MAudioDataConsumer) is responsible for writing the data to its final sink
 * while reading it from the buffers received as a parameter with the callbacks in
 * the MAudioDataConsumer interface.
 */
class MMMFBufferSink
	{
public:
	/**
	 * Initializes the sink for using aConsumer as a data consumer.
	 * 
	 * The sink must be initialized before it is attahced to a stream.
	 *
	 * @param aConsumer a reference to the data consumer which will be responsible
	 * for reading the audio data from the buffers provided by this sink.
	 * @return an error code. KErrNone if successful, otherwise one of the system wide error codes.
	 */
	virtual TInt SetDataConsumer(MMMFAudioDataConsumer& aConsumer)=0;

	/**
	 * Indicates that the audio data has been read from a buffer received from this buffer sink.
	 *
	 * @param aBuffer the buffer that has been consumed.
	 * @return an error code. KErrNone if successful, otherwise one of the system wide error codes.
	 */
	virtual TInt BufferEmptied(CMMFBuffer* aBuffer)=0;

	/** 
	 * Called by the client to acknowledgement the processing of DiscardBuffers() to indicate that 
	 * control of any buffer previously passed to the client.
	 * (by BufferToBeEmptied()) returns to the framework proper.
	 * 
	 * @return an error code. KErrNone on success, otherwise one of the system wide error codes.
	 */
	virtual TInt BuffersDiscarded()=0;
	};

#endif // MBUFFERSINK_H
