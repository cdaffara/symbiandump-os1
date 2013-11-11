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
// Description:
//

#ifndef BTSYNCBANDWIDTH_H
#define BTSYNCBANDWIDTH_H

#include <e32base.h>

/**
@file
@publishedAll
@released
*/

/**
A pair of transmit and receive bandwidths for use on synchronous Bluetooth links
*/
NONSHARABLE_CLASS(TBTSyncBandwidth)
	{
	public:
		IMPORT_C TBTSyncBandwidth();
		IMPORT_C TBTSyncBandwidth(TUint aBandwidth);
		
		TUint32 iTransmit;
		TUint32 iReceive;
	
	private:
		// This data padding has been added to help prevent future binary compatibility breaks	
		// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
		TUint32     iPadding1; 
		TUint32     iPadding2; 		
	};
#endif // BTSYNCBANDWIDTH_H
