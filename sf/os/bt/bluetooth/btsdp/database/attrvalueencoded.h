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

#ifndef ATTRVALUEENCODED_H 
#define ATTRVALUEENCODED_H

#include <btsdp.h>

NONSHARABLE_CLASS(CSdpAttrValueEncoded) : public CSdpAttrValue
	{
public:
	IMPORT_C static CSdpAttrValueEncoded* NewEncodedL(const TDesC8& aString);
	virtual ~CSdpAttrValueEncoded();

	IMPORT_C void SetEncodedValue(const TDesC8& aValue);
	virtual TSdpElementType Type() const;
	virtual const TPtrC8 Des() const;
	virtual TUint DataSize() const;
private:
	CSdpAttrValueEncoded();
	void ConstructL(const TDesC8& aString);
private:
	HBufC8 *iBuffer;
	};


#endif

