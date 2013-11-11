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

#ifndef MBUFFERSOURCE_H
#define MBUFFERSOURCE_H

#include <a3f/a3fbase.h>
#include <a3f/maudiodatasupplier.h>

class CMMFBuffer;

/**
 * Buffer source is used for providing audio data from outside the audio process.
 *
 * The type of the actual 'physical' source can be for example a file or a socket or the
 * data could also be generated. Nevertheless, the client (the MAudioDataSupplier) is responsible
 * for reading the data and providing it to the audio process by filling the buffers received
 * as a parameter with the callbacks in the MAudioDataSupplier interface.
 */
class MMMFBufferSource
	{
	public:
	/**
	 * Initializes the source for using aSupplier as a data supplier.
	 * 
	 * The source must be initialized before it is attached to a stream.
	 *
	 * @param aSupplier a reference to the data supplier which will provide the audio data by
	 * filling the buffers received with the callbacks.
	 * @return an error code. KErrNone if successful, otherwise one of the system wide error codes.
	 */
	virtual TInt SetDataSupplier(MMMFAudioDataSupplier& aSupplier)=0;

	/**
	 * Indicates that a buffer received from this buffer source has
	 * been filled with audio data.
	 *
	 * @param aBuffer the buffer that has been filled.
	 * @return an error code. KErrNone if successful, otherwise one of the system wide error codes.
	 */
	virtual TInt BufferFilled(CMMFBuffer* aBuffer)=0;

	/**
	 * Acknowledges the call to MMMFAudioDataSupplier::DiscardBuffers().
	 *
	 * A DiscardBuffers() callback will be issued to MMMFAudioDataSupplier to request that any requested buffers have been discarded.
	 * This call acknowledges that the data supplier has stopped using the buffers, and returns their control to the framework proper
	 * 
	 * @return an error code. KErrNone on success, otherwise one of the system wide error codes.
	 */
	virtual TInt BuffersDiscarded()=0;

	};

#endif // MBUFFERSOURCE_H
