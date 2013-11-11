// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef IRDAEXTENSIONS_H
#define IRDAEXTENSIONS_H

NONSHARABLE_CLASS(RIrdaDiscoveryResponseSuppressor)
/** This class allows the suppression of discovery responses.

@publishedPartner
@released
*/
	{
public:
	IMPORT_C TInt Open();
	IMPORT_C void Close();

private:
	TInt DefineAndSetKey(TBool aSuppressDiscoveryResponses);
	};

#endif // IRDAEXTENSIONS_H
