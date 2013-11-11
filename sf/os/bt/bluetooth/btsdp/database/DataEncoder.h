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


#ifndef DATAENCODER_H
#define DATAENCODER_H

#include <e32base.h>
#include <btsdp.h>
#include "sdpconsts.h"


NONSHARABLE_CLASS(TElementEncoder)
	{
public:
	IMPORT_C TElementEncoder(TDes8& aWriteBuffer);
	IMPORT_C TInt WriteNil();
	IMPORT_C TInt WriteUint(const TDesC8& aUint);
	IMPORT_C TInt WriteUint(TUint aVal, TInt aNumberOfBytes=4);
	IMPORT_C TInt WriteInt(const TDesC8& aInt);
	IMPORT_C TInt WriteInt(TInt aVal, TInt aNumberOfBytes=4);
	IMPORT_C TInt WriteUUID(const TUUID& aUUID);
	IMPORT_C TInt WriteString(const TDesC8& aString);
	IMPORT_C TInt WriteBoolean(TBool aBoolean);
	IMPORT_C TInt WriteDES(TUint aDataSize);
	IMPORT_C TInt WriteDEA(TUint aDataSize);
	IMPORT_C TInt WriteURL(const TDesC8& aUrl);

	IMPORT_C TInt WriteElement(TSdpElementType aType, const TDesC8& aData);
	IMPORT_C TInt WriteHeader(TSdpElementType aType, TUint aDataSize);
	IMPORT_C TInt WriteDesc(const TDesC8& aDesc);

	// Static Util functions
	IMPORT_C static void PutUint(TUint8* aPtr, TUint aUint, TInt aNumberOfBytes);
	IMPORT_C static TSdpElementSizeDesc SizeDescriptor(TSdpElementType aType, TUint aDataSize);
	IMPORT_C static TInt HeaderSize(TSdpElementType aType, TUint aDataSize);
	IMPORT_C static TInt EncodedSize(TSdpElementType aType, TUint aDataSize);

private:
	TDes8& iWriteBuf;
	};


#endif
