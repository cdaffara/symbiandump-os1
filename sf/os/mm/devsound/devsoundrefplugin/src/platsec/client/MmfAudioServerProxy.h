// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMFAUDIOSERVERPROXY_H__
#define __MMFAUDIOSERVERPROXY_H__

#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include "MmfAudioClientServer.h"


class RMMFAudioServerProxy; // declared here;

NONSHARABLE_CLASS( RMMFAudioServerProxy ): public RMmfSessionBase
/**
*@internalTechnology
*/
	{
public:

	IMPORT_C TInt Open();
	IMPORT_C TInt SetDevSoundInfo();
	IMPORT_C TInt GetDevSoundSessionHandle();
	};

#endif
