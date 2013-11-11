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

#ifndef MAUDIODATASUPPLIER_H
#define MAUDIODATASUPPLIER_H

#include <a3f/a3fbase.h>

class MMMFBufferSource;
class CMMFBuffer;

/**
 * An interface to a set of callback functions that a buffer source client must implement.
 */
class MMMFAudioDataSupplier
	{
	public:
	/**
	 * Request a buffer to be filled with audio data.
	 *
	 * After filling the buufer, the client must call MBufferSource::BufferFilled() passing the filled buffer as a parameter.
	 * 
	 * @param aSource a pointer to the source requesting the buffer.
	 * @param aBuffer a pointer to the buffer to fill.
	 */
	virtual void BufferToBeFilled(MMMFBufferSource* aSource, CMMFBuffer* aBuffer)=0;

	/**
	 * Indicates that all buffers originating from this source have been removed from the audio path.
	 * Also, all pending buffers previously requested by this source have become invalid and must not be used.
	 *
	 * @param aSource the source whose previous buffer requests should be ignored.
	 */
	virtual void DiscardBuffers(MMMFBufferSource* aSource)=0;
	};

#endif // MAUDIODATASUPPLIER_H
