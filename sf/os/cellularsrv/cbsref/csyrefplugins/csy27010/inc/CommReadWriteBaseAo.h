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
// * Description: This file declares the base class CCommReadWriteBaseAo. 
//                Class CCommReadWriteBaseAo contains the common elements for derived 
//                Frame Reader and Frame Writer classes using a serial comm port.
//

// CommReadWriteBaseAo.h

/** @file CommReadWriteBaseAo.h
 *
 * The file declares the base class CCommReadWriteBaseAo. This class
 * holds the common elements for derived Frame Reader and Frame Writer
 * classes using a serial comm port.
 */

#ifndef _CCOMMREADWRITEBASEAO_H_
#define _CCOMMREADWRITEBASEAO_H_

#include "cs_port.h"
#include <d32comm.h>
#include <c32comm.h>
#include <e32hal.h>


class CPortFactory;
class CMux0710Protocol;

class CCommReadWriteBaseAo : public CActive
    {
public:
    CCommReadWriteBaseAo(CPortFactory* aParent, CMux0710Protocol* aMux0710Protocol,
		TInt aPriority = EPriorityStandard);
    ~CCommReadWriteBaseAo();

    TInt SetServerConfig(TCommServerConfig& aConfig);
    void GetServerConfig(TCommServerConfig& aConfig) const;
    void FreeMemory();

    inline void SetRole(TCommRole aRole){iRole=aRole;};

protected:
    void SetBuffersL();

	// attributes
    HBufC8*				iBuffer;	// pointer to the Tx/Rx buffer
    TPtr8*				iBuf;		// pointer to a TPtr8 that points to the current buffer

    TUint				iBufFlags;	// contains buffer flags e.g for partial read/write
    TInt				iBufSize;	// size of the Tx/Rx buffer

    TCommRole			iRole;		// DTE or DCE role for this port unit

    CPortFactory*		iParent;	// pointer to the CPortC32Interface object
	CMux0710Protocol*	iMux0710Protocol;
	RBusDevComm*		iCommPort;  // pointer to LDD comm port

    };

#endif  // _CCOMMREADWRITEBASEAO_H_