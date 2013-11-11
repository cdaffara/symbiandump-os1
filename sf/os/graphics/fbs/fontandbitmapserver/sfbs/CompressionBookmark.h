// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CompressionBookmark
// 
//

#ifndef __COMPRESSIONBOOKMARK_H_
#define __COMPRESSIONBOOKMARK_H_

class TCompressionBookMark
	{
public:
	TCompressionBookMark();

public:
	inline void CalculateCheckSum();
	inline TBool IsCheckSumOk() const;
	TInt iSrcDataOffset;
	TInt iCursorPos;
	TUint32 iCheckSum;
	TInt iCheckSum2;

	};

inline TCompressionBookMark::TCompressionBookMark() :
	iSrcDataOffset(0), iCursorPos(0)
	{
	CalculateCheckSum();
	}

inline void TCompressionBookMark::CalculateCheckSum()
	{
	iCheckSum=((TUint32) iSrcDataOffset)^((TUint32) iCursorPos);
	iCheckSum2=(1-iSrcDataOffset)+(1-iCursorPos);
	}

inline TBool TCompressionBookMark::IsCheckSumOk() const
	{
	TBool check1=(iCheckSum==(((TUint32) iSrcDataOffset)^((TUint32) iCursorPos)));
	TBool check2=(iCheckSum2==((1-iSrcDataOffset)+(1-iCursorPos)));
	return (check1 && check2);
	}


#endif//__COMPRESSIONBOOKMARK_H_
