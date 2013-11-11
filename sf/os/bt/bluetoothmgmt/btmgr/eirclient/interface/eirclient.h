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
// REirMan Client side header
// Implements the Symbian OS EIR Management API REirMan
// 
//

/**
 @file
 @internalComponent
*/

#ifndef _EIRMAN_H
#define _EIRMAN_H

#include <e32std.h>
#include <bluetooth/eirmanshared.h>

NONSHARABLE_CLASS(REirMan) : public RSessionBase
/**
The REirMan class implements the Symbian OS EIR Management API REirMan
*/
	{
public:
	REirMan();
	void Connect(TRequestStatus& aStatus);
	void SpaceAvailableNotification(TUint& aBytesAvailable, TRequestStatus& aStatus);
	void CancelSpaceAvailableNotification();
	void SetData(const TDesC8& aData, TEirDataMode aDataMode, TRequestStatus& aStatus); 
	void RegisterTag(TEirTag aEirTag, TRequestStatus& aStatus);
	void NewData(TInt aRequiredLength, TRequestStatus& aStatus);

private:
	TVersion Version() const;
	TInt RegisterTag(TEirTag aEirTag);

private:

	TPckg<TUint32> iBytesAvailablePckg;
	
private:
	// This data padding has been added to help prevent future binary compatibility breaks
	// iPadding hasn't been zero'd because it is currently not used
	TUint32 iPadding;
	};

#endif // _EIRMAN_H
