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


#ifndef BUFFERSOURCE_H
#define BUFFERSOURCE_H

#include <e32base.h>
#include <f32file.h>
#include <a3f/mbuffersource.h>

#include "moutputport.h"
#include "mstreambuffercontrol.h"

#include <a3f/a3f_trace_utils.h>


/////
 // This is the implementation of CAudioSource API.
 //
 // @see CAudioComponent.
 ///
NONSHARABLE_CLASS(CBufferSource) : 	public CActive,
									public MMMFBufferSource,		// A3F interface
									public MOutputPort			// Adaptation interface

	{
public:

	IMPORT_C static CBufferSource* NewL();
	/////
	// Destructor.
	//
	// Deletes all objects and releases all resource owned by this instance.
	///
	virtual ~CBufferSource();

	// from MMMFBufferSource
	TInt SetDataSupplier(MMMFAudioDataSupplier& aSupplier);
	TInt BufferFilled(CMMFBuffer* aBuffer);
	TInt BuffersDiscarded();

	// from class MOutputPort
	TInt GetOutputPort(MOutputPort*& aOutputPort);
	TInt FillBuffer(CMMFBuffer* aBuffer, MInputPort* aConsumer);
	TInt BufferEmptied(CMMFBuffer* aBuffer);
	TInt SetInput(MInputPort* aInput);
	TInt RemoveInput(MInputPort* aInput);
	TInt FlushBuffer(MFlushHandlerObserver* aFlushObserver);
private:

	void ConstructL();

	CBufferSource();

	void DoCancel();

	void RunL();

private:
	MFlushHandlerObserver* iObserver;

	MMMFAudioDataSupplier* iSupplier;

	MInputPort* iInput;

	CMMFBuffer* iBuffer;
	};

#endif // BUFFERSOURCE_H
