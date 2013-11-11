// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTSDPAGENTCONSTS_H
#define BTSDPAGENTCONSTS_H

#include <e32base.h>

const static TInt KRetryLimit = 2;
const static TInt KSdpAgentIdleTimeout = 3000000;
const static TInt KSdpAgentConnectTimeout = 10000000;
const static TInt KSdpAgMaxResultCount = 0xffff;
const static TInt KSdpAgMaxByteCount   = 0x400; // .5 K

#endif
