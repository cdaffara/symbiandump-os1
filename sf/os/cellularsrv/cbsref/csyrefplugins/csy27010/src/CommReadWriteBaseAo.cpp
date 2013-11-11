//
// * Copyright 2004 Neusoft America Inc.
// * All rights reserved.
// * This component and the accompanying materials are made available
// * under the terms of the Eclipse Public License v1.0
// * which accompanies this distribution, and is available
// * at the URL "http://www.eclipse.org/legal/epl-v10.html".
// *
// * Contributors:
// * Keith Collins (Neusoft America Inc.)  original software development and additional code and modifications.
// * Thomas Gahagen (Neusoft America Inc.)  additional code and modifications.
// * Zhen Yuan (Neusoft America Inc.)  additional code and modifications.
// *
// * Description:  This file has the implementation for the class MsgToBPCmdQueue
// *               for adding and removing the BasePort Command messages from the queue.
//

// CommReadWriteBaseAo.cpp

/** @file CommReadWriteBaseAo.cpp
 *
 * 
 */

#include "CommReadWriteBaseAo.h"
#include "Portfactory.h"
#include "CsyDebugLogger.h"
#include "CsyGlobals.h"

CCommReadWriteBaseAo::CCommReadWriteBaseAo(CPortFactory* aParent,
										   CMux0710Protocol* aMux0710Protocol,
										   TInt aPriority)
/**
 * Constructor.
 * @param aParent - Pointer to the parent
 * @param aMux0710Protocol - Pointer to the mux protocol
 * @param aPriority - Priority to set for active object
 */
 : CActive(aPriority),
   iBufSize(KDefaultTxRxBufSize),
   iRole(ECommRoleDTE),
   iParent(aParent),
   iMux0710Protocol(aMux0710Protocol)
	{
	iCommPort = aParent->DTEPort();
	CActiveScheduler::Add(this);
	}


CCommReadWriteBaseAo::~CCommReadWriteBaseAo()
/**
 * Destructor. Delete the memory allocated by this object.
 */
    {
    delete iBuffer;
    delete iBuf;
    }

TInt CCommReadWriteBaseAo::SetServerConfig(TCommServerConfig& aConfig)
/**
 * Set the port to use partial reads/writes or the bungee buffer.
 *
 * @param aConfig Reference to the new Comm server configuration
 * @return TInt error code. KErrNone for sucess
 */
    {
    _LOG_L4C1(">>CCommReadWriteBaseAo::SetServerConfig");

    TCommServerConfigV01& c = aConfig();
    TInt res = KErrNone;

    if (c.iBufFlags & KCommBufferPartial)
        {
        TInt bufSave = iBufSize;
        iBufSize = c.iBufSize;
        TRAP(res, SetBuffersL();)
        if (res==KErrNone)
            iBufFlags = c.iBufFlags;
        else
            iBufSize = bufSave;
        }

	_LOG_L4C2("<<CCommReadWriteBaseAo::SetServerConfig [res=%d]",res);
	return res;
    }

void CCommReadWriteBaseAo::GetServerConfig(TCommServerConfig& aConfig) const
/**
 * This method reads the comm server buffer configuration.
 *
 * @param aConfig Reference to location to write the Comm server configuration
 */
    {
    _LOG_L4C1("CCommReadWriteBaseAo::GetServerConfig");

    aConfig().iBufFlags = iBufFlags;
    aConfig().iBufSize  = iBufSize;
    }

void CCommReadWriteBaseAo::FreeMemory()
/**
 * Reduce memory allocation levels by order of the comm server.
 */
    {
    _LOG_L4C1("CCommReadWriteBaseAo::FreeMemory");

    TRAP_IGNORE(SetBuffersL();)
	// MAF the result is ignored?
    }

void CCommReadWriteBaseAo::SetBuffersL()
/**
 * This method frees the buffer memory and then tries to allocate
 * a smaller piece of memory. Note that this function will leave if
 * the memory allocation fails.
 */
    {
    _LOG_L4C1(">>CCommReadWriteBaseAo::SetBuffersL");

    if (!IsActive())
        {
		_LOG_L3C1("Not active - allow alloc to happen");
        TInt allocLen = Align4(iBufSize);
   
        delete iBuffer;
        delete iBuf;
        iBuf = NULL;   // set to NULL, in case new leaves
        iBuffer = NULL;

        iBuffer = HBufC8::NewMaxL(allocLen);
        iBuf = new (ELeave) TPtr8((TText8*)iBuffer->Des().Ptr(), allocLen, allocLen);
        }

	_LOG_L4C1("<<CCommReadWriteBaseAo::SetBuffersL");
	}