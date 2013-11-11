/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef BUFFERSINK_H
#define BUFFERSINK_H

#include <e32base.h>
#include <f32file.h>

#include "minputport.h"

#include <a3f/mbuffersink.h>
#include <a3f/a3f_trace_utils.h>


/////
//  This is the implementation of CAudioSink API.
//
//  @see CAudioComponent
///
NONSHARABLE_CLASS(CBufferSink) : public CBase,
								 public MInputPort,
								 public MMMFBufferSink
	{
public:

	/////
	// Create a new instance.
	//
	// @return CBufferSink* a pointer to the created instance.
	///
	IMPORT_C static CBufferSink* NewL();

	/////
	// Destructor.
	//
	// Deletes all objects and releases all resource owned by this instance.
	///
	virtual ~CBufferSink();

	// from class MInputPort
	TInt GetInputPort(MInputPort*& aInputPort);
	virtual TInt EmptyBuffer(CMMFBuffer* aBuffer, MOutputPort* aSupplier);
	virtual TInt BufferFilled(CMMFBuffer* aBuffer);
	virtual TInt SetOutput(MOutputPort* aOutput);
	virtual TInt RemoveOutput(MOutputPort* aOutput);

	// from class MMFBufferSink
	virtual TInt SetDataConsumer(MMMFAudioDataConsumer& aConsumer);
	virtual TInt BufferEmptied(CMMFBuffer* aBuffer);
	virtual TInt BuffersDiscarded();

private:

	void ConstructL();

	CBufferSink();

	MMMFAudioDataConsumer*	iConsumer;

	MOutputPort*			iOutput;

	CMMFBuffer*				iBuffer;
	};

#endif // BUFFERSINK_H
