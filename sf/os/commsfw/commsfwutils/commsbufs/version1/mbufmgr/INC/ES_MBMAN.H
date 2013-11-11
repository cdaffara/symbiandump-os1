// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Buffer Manager for Protocols
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef __ES_MBMAN_H__
#define __ES_MBMAN_H__

#ifndef __ES_MBUF_H__
#include <es_mbuf.h>
#endif
#include <elements/rworkerlock.h>     // RWorkerThread decleration

class CMBufPool;
class CMBufManager;
class CMBufMemoryAllocator;
class CMBufPoolManager;
class RMBufPoolChain;	// es_mbman.h is exported (eg. for esock), thus all included header files must also be exported
                        // however, RMBufPoolChain is private to mbuf component, hence preferable to use forward decl than include the header file

//
// MBUF MANAGER
//

#include <comms-infras/mbufmanager.h>

#endif
