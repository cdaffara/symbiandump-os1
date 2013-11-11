// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__GDSFCOMP_H__)
#define __GDSFCOMP_H__
#if !defined(__FONTCOMP_H__)
#include "FONTCOMP.H"
#endif
/**
@internalComponent
*/
const char BlankPixel = '.';
const char SetPixel = '*';

class GdsFontCompiler : public FontCompiler
/**
@internalComponent
*/
	{
public:
	int WriteFont();
protected:
	int WriteHeader();
	void WriteFooter();
	void WriteCharData(int);
	void WriteBlankLines(int,int);
	void WriteFormattedData(char*,int=0);
	void WriteFormattedData(int,int=0);
	void WriteNewLine();
	};
#endif
