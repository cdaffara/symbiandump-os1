// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalComponent
*/

#include "mmcameraservershutdown.h"

CMMCameraServerShutdown* CMMCameraServerShutdown::NewL()
	{
	CMMCameraServerShutdown* self = new(ELeave) CMMCameraServerShutdown();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CMMCameraServerShutdown::RunL()
	{
	CActiveScheduler::Stop();
	}

CMMCameraServerShutdown::~CMMCameraServerShutdown()
	{
	}

CMMCameraServerShutdown::CMMCameraServerShutdown()
	: CTimer(CActive::EPriorityLow)
	{
	CActiveScheduler::Add(this);
	}

void CMMCameraServerShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}

void CMMCameraServerShutdown::Start()
	{
	After(KMMServerShutdownDelay);
	}
