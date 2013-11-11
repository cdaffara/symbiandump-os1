// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BIDICOPY_H_
#define BIDICOPY_H_

#include <e32def.h>

class BidiCopy
/** Text copying and reversal routines.
@internalTechnology */
	{
public:
	static TInt Mirror(TInt a);
	static void ReverseCodes(TText* aText, TInt aLength);
	static TText* CopyBackwards(TText* aDestination,
		const TText* aSource, TInt aLength);
	static void DeleteUnreversedSurrogates(TText* aText, TInt aLength);
	static void CorrectSurrogatePairs(TText* aText, TInt aLength);
	static void CorrectGroups(TText* aText, TInt aLength);
	 static void SubstituteMirrorImages(TText* aText, TInt aLength);
	static TText* CopyMirror(TText* aDestination, const TText* aSource, TInt length);
	static TText* OutputTChar(TText* aDestination, TInt aToOutput);
	static TText* CopyBackwardsWithMirroring(TText* aDestination,
		const TText* aSource, TInt aLength);
	static TText* CopyGroupsBackwards(TText* aDestination,
		const TText* aSource, TInt aLength);
	static TBool ImplicitDirectionalityIsRightToLeft(
		const TText* aText, TInt aLength, TBool* aFound);
	};

#endif
