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


#ifndef MIGNORER_H
#define MIGNORER_H

#include <btsdp.h>

class MIgnorer : public MSdpElementBuilder
	{
public:
	virtual MSdpElementBuilder* BuildUnknownL(TUint8 aType, TUint8 aSizeDesc, const TDesC8& aData);
	virtual MSdpElementBuilder* BuildNilL();
	virtual MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
	virtual MSdpElementBuilder* BuildIntL(const TDesC8& aInt);
	virtual MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID);
	virtual MSdpElementBuilder* BuildBooleanL(TBool aBool);
	virtual MSdpElementBuilder* BuildStringL(const TDesC8& aString);
	virtual MSdpElementBuilder* BuildDESL();  // Must not return NULL
	virtual MSdpElementBuilder* BuildDEAL();  // ditto
	virtual MSdpElementBuilder* StartListL(); // ditto
	virtual MSdpElementBuilder* EndListL();
	virtual MSdpElementBuilder* BuildURLL(const TDesC8& aURL);
//	virtual MSdpElementBuilder* BuildEncodedL(const TDesC8& aString);
	};

#endif
