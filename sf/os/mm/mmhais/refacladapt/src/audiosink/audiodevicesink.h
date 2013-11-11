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


#ifndef AUDIODEVICESINK_H
#define AUDIODEVICESINK_H

#include <e32base.h>
#include <f32file.h>
#include <a3f/a3f_trace_utils.h>

#include "mmrcclient.h"


/////
 // This is the implementation of CAudioSink API.
 //
 // @see CAudioComponent
 ///
NONSHARABLE_CLASS(CAudioDeviceSink) : public CBase
	{
public:
	/////
	// Create a new instance.
	//
	// @return CAudioDeviceSink* a pointer to the created instance.
	///
	IMPORT_C static CAudioDeviceSink* NewL();

	/////
	// Destructor.
	//
	// Deletes all objects and releases all resource owned by this instance.
	///
	virtual ~CAudioDeviceSink();

private:

	void ConstructL();
	CAudioDeviceSink();
	};

#endif // AUDIODEVICESINK_H
