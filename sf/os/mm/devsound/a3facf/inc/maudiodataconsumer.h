/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
 @publishedPartner 
 @released
*/

#ifndef MAUDIODATACONSUMER_H
#define MAUDIODATACONSUMER_H

#include <a3f/a3fbase.h>

class MMMFBufferSink;
class CMMFBuffer;

/**
 * An interface to a set of callback functions that a buffer sink client must implement.
 */
class MMMFAudioDataConsumer
	{
	public:
	/**
	 * Provides a buffer with audio data to the client.
	 *
	 * After reading the data from the buffer, the client must call MBufferSink::BufferEmptied() passing the buffer as a parameter.
	 * @param aSink a pointer to the sink providing the buffer.
	 * @param aBuffer a pointer to the buffer to empty.
	 */
	virtual void BufferToBeEmptied(MMMFBufferSink* aSink, CMMFBuffer* aBuffer)=0;


	/**
	 * Indicates that any outstanding buffer requests are to be cancelled, so that control of those buffers returns to the framework.
	 * The client must call MMMFBufferSink::BuffersDiscarded() to acknowledge that this has taken place.
	 * @param aSource the source whose previous buffer requests should be ignored.
	 */
	virtual void DiscardBuffers(MMMFBufferSink* aSink)=0;
	};

#endif // MAUDIODATACONSUMER_H
