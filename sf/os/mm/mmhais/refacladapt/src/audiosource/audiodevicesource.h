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


#ifndef AUDIODEVICESOURCE_H
#define AUDIODEVICESOURCE_H

#include <e32base.h>
#include <f32file.h>
#include <a3f/a3f_trace_utils.h>

#include "mmrcclient.h"

/////
 //This is the implementation of CAudioSource API.
 //@see CAudioComponent
///
NONSHARABLE_CLASS(CAudioDeviceSource) : public CBase
	{
public:

	/////
	// Create a new instance.
	//
	// @return CAudioDeviceSource* a pointer to the created instance.
	///
	IMPORT_C static CAudioDeviceSource* NewL();
	/////
	// Destructor.
	//
	// Deletes all objects and releases all resource owned by this instance.
	///
	virtual ~CAudioDeviceSource();

private:

	void ConstructL();

	CAudioDeviceSource();
	};

#endif // AUDIODEVICESOURCE_H
