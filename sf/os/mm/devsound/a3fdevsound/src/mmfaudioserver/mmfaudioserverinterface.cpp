// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// INCLUDE FILES
#include <e32std.h>
#include "mmfaudioserver.h"
#include <a3f/mmfaudioserverinterface.h>
#include "mmfaudioserverinterfaceimpl.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAudioSvrLoader::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioSvrLoader* CAudioSvrLoader::NewL()
	{
	CAudioSvrLoaderImpl* self = new(ELeave) CAudioSvrLoaderImpl();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CAudioSvrLoader::CAudioSvrLoader
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAudioSvrLoader::CAudioSvrLoader()
	{
	}


// -----------------------------------------------------------------------------
// CAudioSvrLoader::~CAudioSvrLoader
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 CAudioSvrLoader::~CAudioSvrLoader()
	{
	}



// -----------------------------------------------------------------------------
// CAudioSvrLoaderImpl::CAudioSvrLoaderImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAudioSvrLoaderImpl::CAudioSvrLoaderImpl()
	{
	}


// -----------------------------------------------------------------------------
// CAudioSvrLoaderImpl::~CAudioSvrLoaderImpl
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 CAudioSvrLoaderImpl::~CAudioSvrLoaderImpl()
	{
	delete iServer;
	}

// -----------------------------------------------------------------------------
// CAudioSvrLoaderImpl::~CAudioSvrLoaderImpl
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 void CAudioSvrLoaderImpl::ConstructL()
	{
	iServer = CMMFAudioServer::NewL();
	}


// End of file


