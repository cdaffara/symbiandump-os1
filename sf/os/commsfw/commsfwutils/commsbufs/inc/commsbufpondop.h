// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: Operator to create the pond and the accessor of the pond 
// from TLS
//

#ifndef __COMMSBUFPONDINTERNALOP_H__ 
#define __COMMSBUFPONDINTERNALOP_H__ 

#include <e32base.h>
#include <comms-infras/commsbufpond.h>


/**
Pool creation information. Typically read from the c32start.ini file

@publishedPartner
*/
struct TCommsBufPoolCreateInfo
	{
	TInt	iBufSize;			// Buffer size for the pool
	TInt	iInitialBufs;		// No. of buffers initially allocater buffers
	TInt	iGrowByBufs;		// The minimum number of buffers that the pool will grow by
	TInt	iMinFreeBufs;		// Minimum no. of free bufs. If the value falls below this number
								// the buffer manager system will grow.
	TInt	iCeiling;			// Pool ceiling. Maximum no. of buffers the pool can have.
	TInt	iAlignment;			// Alignment
	};

/**
Operator to initialize the pond with the given pool info and destruct the
pond.

@internalTechnology
*/
class RCommsBufPondOp : public RCommsBufPond
	{
	public:
	IMPORT_C RCommsBufPondOp ();
	IMPORT_C TInt Open(RArray<TCommsBufPoolCreateInfo>& aPoolInfo, TInt aMaxMBufSize =0);
	IMPORT_C void Close();	
	};

/**
Accessor to set and get the comms buf pond from the TLS

@internalTechnology 
 */
class TCommsBufPondTLSOp
	{
	public:
	inline TCommsBufPondTLSOp(RCommsBufPond& aPond);
	IMPORT_C void Set();
	IMPORT_C static RCommsBufPond Get(); 
	private:
	RCommsBufPond& iPond;
	};

#include <comms-infras/commsbufpondop.inl>
#endif // __COMMSBUFPONDOP_H__

