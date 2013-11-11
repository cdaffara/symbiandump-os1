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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef BEARERPARAMS_H
#define BEARERPARAMS_H

#include <e32base.h>

class MRemConBearerObserver;

/**
Struct for initialisation parameters for concrete bearer plugins. The derived 
(concrete) plugin will receive an instance of this in its NewL. It should pass 
it to the CRemConBearerPlugin base class constructor.
*/
class TBearerParams
	{
public:
	/**
	Constructor.
	@param aImplementationUid The implementation UID of the bearer.
	@param aObserver Observer.
	*/
	IMPORT_C TBearerParams(TUid aImplementationUid, MRemConBearerObserver& aObserver);

	/** Destructor. */
	IMPORT_C ~TBearerParams();

public:
	/** 
	Accessor for the implementation UID.
	@return Implementation UID.
	*/
	IMPORT_C TUid ImplementationUid() const;

	/**
	Accessor for the observer.
	@return Observer.
	*/
	IMPORT_C MRemConBearerObserver& Observer() const;

private: // owned
	const TUid iImplementationUid;

	/** 
	Pad for BC-friendly future change.
	*/
	TUint32 iPad;

private: // unowned
	MRemConBearerObserver& iObserver;
	};

#endif // BEARERPARAMS_H
