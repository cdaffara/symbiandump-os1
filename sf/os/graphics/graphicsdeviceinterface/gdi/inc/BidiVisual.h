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

#ifndef BIDIVISUAL_H_
#define BIDIVISUAL_H_

#include <bidi.h>

class TBidiLogicalToVisual
/** 
This class is a low-level class for bi-directionally reordering text. It 
allows the re-ordering of text using no leaving functions at all, and provides 
a finer degree of control than TBidiText. However, it should not be used 
without understanding the basics of the Unicode Bi-directional Reordering 
Algorithm. 
@publishedAll
@released
*/
	{
public:
	enum {KMinCharAvailable = 4};

	IMPORT_C TBidiLogicalToVisual(const TDesC& aText, TBool aRightToLeft,
		TBidirectionalState::TRunInfo* aRunInfoArray, TInt aRunInfoLength);
	IMPORT_C TBidiLogicalToVisual(const TDesC& aText,
		TBidirectionalState::TRunInfo* aRunInfoArray, TInt aRunInfoLength);
	IMPORT_C TInt Reorder();
	IMPORT_C void GetVisualLine(TDes& aLine, TInt aStart, TInt aEnd,
		TChar aTruncationChar);
private:
	const TDesC& iText;
	TBool iRightToLeft;
	TBidirectionalState::TRunInfo* iRunInfoArray;
	TInt iRunInfoArrayLength;
	TInt iRuns;
	TUint32 iSpare2;
	};

#endif
