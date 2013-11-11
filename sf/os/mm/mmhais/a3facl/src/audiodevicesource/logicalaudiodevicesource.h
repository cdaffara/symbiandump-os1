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
*
*/



#ifndef CLOGICALAUDIODEVICESOURCE_H
#define CLOGICALAUDIODEVICESOURCE_H

#include <e32base.h>
#include <f32file.h>
#include <a3f/a3f_trace_utils.h>
#include <a3f/a3fbase.h>
#include "audioprocessingunit.h"

class MLogicalSettingObserver;

/**

*/
NONSHARABLE_CLASS(CLogicalAudioDeviceSource) :	public CAudioProcessingUnit
	{
friend class CAudioContext;
friend class CLogicalAudioStream;
public:

	/**
	Destructor.

	Deletes all objects and releases all resource owned by this instance.
	*/
	virtual ~CLogicalAudioDeviceSource();

	// from CAudioProcessingUnit
	virtual TBool IsTypeOf(TUid aTypeId) const;

	/**
	Create a new instance.

	@return CBufferSource* a pointer to the created instance.
	*/
	static CLogicalAudioDeviceSource* NewL(TAny* aParam);
private:
	void ConstructL();

	CLogicalAudioDeviceSource(TAny* aParam);
	};

#endif // C_LOGICAL_AUDIOSOURCE_H
