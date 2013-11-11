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
// Implements the inline methods for CSmsProtocol and CSmsProvider.
// Includes
// 
//

/**
 @file
 @internalAll
*/

#include "smsustrm.h"

//
// CSmsProtocol inline definitions
//

inline TBool CSmsProtocol::ObserverIsPresent(const MSmsMessageObserver& aObserver) const
	{
	return ObserverIndex(aObserver)!=iSmsMessageObserverList.Count();
	}

inline TBool CSmsProtocol::ClosingDown() const
	{
	return iFlags&ESmsProtocolClosingDown;
	}

inline void CSmsProtocol::SetClosingDown(TBool aClosingDown)
	{
	iFlags=aClosingDown? iFlags|ESmsProtocolClosingDown: iFlags&(~ESmsProtocolClosingDown);
	}

inline RFs& CSmsProtocol::FileSession()
	{
	return iFs;
	}


//
// CSmsProvider inline definitions
//

inline TBool CSmsProvider::ObserverAddedToProtocol() const
	{
	return iFlags&ESmsProviderObserverAddedToProtocol;
	}

inline void CSmsProvider::SetObserverAddedToProtocol(TBool aAdded)
	{
	iFlags=aAdded? iFlags|ESmsProviderObserverAddedToProtocol: iFlags&(~ESmsProviderObserverAddedToProtocol);
	}

inline TBool CSmsProvider::IoctlOutstanding() const
	{
	return iFlags&ESmsProviderIoctlOutstanding;
	}

inline void CSmsProvider::SetIoctlOutstanding(TBool aOutstanding)
	{
	iFlags=aOutstanding? iFlags|ESmsProviderIoctlOutstanding: iFlags&(~ESmsProviderIoctlOutstanding);
	}

inline TInt CSmsProvider::NumSegments(TInt aSize)
	{
	return (aSize+(KSmsMaxSegmentLength-1))/KSmsMaxSegmentLength;
	}
