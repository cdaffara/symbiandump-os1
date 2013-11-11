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
* Name        : logicalbuffersource.h
* Part of     : ACL Logical Layer
*
*/



#ifndef LOGICALBUFFERSOURCE_H
#define LOGICALBUFFERSOURCE_H

#include <e32base.h>
#include <f32file.h>
#include <a3f/a3f_trace_utils.h>
#include <a3f/mbuffersource.h>
#include <a3f/maudiodatasupplier.h>

#include "audioprocessingunit.h"

#include <a3f/maudioprocessingunitobserver.h>


class MLogicalSettingObserver;

/**

*/
NONSHARABLE_CLASS(CLogicalBufferSource) :	public CAudioProcessingUnit,
											public MMMFBufferSource,
											public MMMFAudioDataSupplier
	{
friend class CAudioContext;
friend class CLogicalAudioStream;
public:

	/**
	Destructor.

	Deletes all objects and releases all resource owned by this instance.
	*/
	virtual ~CLogicalBufferSource();

	// from MMMFBufferSource
	virtual TInt SetDataSupplier(MMMFAudioDataSupplier& aSupplier);
	virtual TInt BufferFilled(CMMFBuffer* aBuffer);
	virtual TInt BuffersDiscarded();

	// from CAudioProcessingUnit
	virtual TBool IsTypeOf(TUid aTypeId) const;
	virtual TAny* Interface(TUid aType);

	// from MMMFDataSupplier
	virtual void BufferToBeFilled(MMMFBufferSource* aSource, CMMFBuffer* aBuffer);
	virtual void DiscardBuffers(MMMFBufferSource* aSource);

	/**
	Create a new instance.

	@return CBufferSource* a pointer to the created instance.
	*/
	static CLogicalBufferSource* NewL(TAny* aParameters);

private:
	void ConstructL();

	CLogicalBufferSource(TAny* aParameters);

protected:
	// The adaptation buffer source
	MMMFBufferSource* iAdaptationBufferSource;

private:
	// The client which provides data
	MMMFAudioDataSupplier* iSupplier;
	};

#endif // LOGICALBUFFERSOURCE_H
