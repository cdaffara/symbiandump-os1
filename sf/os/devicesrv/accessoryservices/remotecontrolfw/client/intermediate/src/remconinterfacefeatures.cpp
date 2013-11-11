// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <remcon/remconinterfacefeatures.h>

// Default size chosen based on number of operations in core api
const TInt KDefaultOperationBufLength = 62;
const TInt KIncrementReallocGranularity = 62;

EXPORT_C TInt RRemConInterfaceFeatures::Open()
	{
	return iOperations.Create(KDefaultOperationBufLength);
	}

EXPORT_C void RRemConInterfaceFeatures::Close()
	{
	iOperations.Close();
	}

EXPORT_C void RRemConInterfaceFeatures::AddOperationL(TUint aOperationId)
	{
	if(iOperations.Length() + sizeof(TUint) > iOperations.MaxLength())
		{
		// Need more space!
		iOperations.ReAllocL(iOperations.Length() + KIncrementReallocGranularity);
		}
	
	TPckgBuf<TUint> opBuf(aOperationId);
	iOperations.Append(opBuf);
	}

const TDesC8& RRemConInterfaceFeatures::SupportedOperations() const
	{
	return iOperations;
	}
