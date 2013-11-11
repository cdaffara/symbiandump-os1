/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : logicalbuffersink.h
* Part of     : ACL Logical Layer
*
*/



#ifndef LOGICALBUFFERSINK_H
#define LOGICALBUFFERSINK_H

#include <e32base.h>
#include <f32file.h>
#include <a3f/a3f_trace_utils.h>
#include <a3f/mbuffersink.h>
#include <a3f/maudiodataconsumer.h>

#include "audioprocessingunit.h"

#include <a3f/maudioprocessingunitobserver.h>


class MLogicalSettingObserver;

/**
This is the implementation of CAudioSink API.

@see CAudioComponent
*/
NONSHARABLE_CLASS(CLogicalBufferSink) : public CAudioProcessingUnit,
										public MMMFBufferSink,
										public MMMFAudioDataConsumer
	{
friend class CAudioContext;
friend class CLogicalAudioStream;

public:

	/**
	Destructor.

	Deletes all objects and releases all resource owned by this instance.
	*/
	virtual ~CLogicalBufferSink();

	// from class MBufferSink
	TInt SetDataConsumer(MMMFAudioDataConsumer& aConsumer);
	TInt BufferEmptied(CMMFBuffer* aBuffer);
	TInt BuffersDiscarded();

	// from class CAudioProcessingUnit
	virtual TBool IsTypeOf(TUid aTypeId) const;
	virtual TAny* Interface(TUid aType);

	// from MMMFAudioDataConsumer
	void BufferToBeEmptied(MMMFBufferSink* aSink, CMMFBuffer* aBuffer);
	void DiscardBuffers(MMMFBufferSink* aSink);
	/**
	Create a new instance.

	@return CBufferSink* a pointer to the created instance.
	*/
	static CLogicalBufferSink* NewL(TAny* aParameters);

private:

	void ConstructL();

	CLogicalBufferSink(TAny* aParameters);

protected:
	// The adaptation buffer sink
	MMMFBufferSink* iAdaptationBufferSink;

private:
	// The client which requests data
	MMMFAudioDataConsumer *iConsumer;

	CMMFBuffer* iBuffer;
	};

#endif // LOGICALBUFFERSINK_H
