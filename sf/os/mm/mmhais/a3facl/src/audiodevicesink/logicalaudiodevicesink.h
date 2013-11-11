/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CLOGICALAUDIODEVICESINK_H
#define CLOGICALAUDIODEVICESINK_H

#include <e32base.h>
#include <f32file.h>
#include <a3f/a3f_trace_utils.h>
#include "audioprocessingunit.h"

class MLogicalSettingObserver;

/**
This is the implementation of CAudioSink API.

@see CAudioComponent
*/
NONSHARABLE_CLASS(CLogicalAudioDeviceSink) :	public CAudioProcessingUnit
	{
friend class CAudioContext;
friend class CLogicalAudioStream;
public:

	/**
	* Destructor.
	*
	* Deletes all objects and releases all resource owned by this instance.
	*/
	virtual ~CLogicalAudioDeviceSink();

	// from base class CAudioProcessingUnit
	virtual TBool IsTypeOf(TUid aTypeId) const;

	/**
	* Create a new instance.
	*
	* @return CAudioDeviceSink* a pointer to the created instance.
	*/
	static CLogicalAudioDeviceSink* NewL(TAny* aParam);

private:

	void ConstructL();

	CLogicalAudioDeviceSink(TAny* aParam);
	};

#endif // C_LOGICALAUDIODEVICESINK_H
