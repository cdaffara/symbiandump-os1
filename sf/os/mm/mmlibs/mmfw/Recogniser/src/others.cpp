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

#include "parsers.h"

//
// This file format is treated differently from all the others in that if the
// file extension is not recognised then the file is not recognised.
// Only once the extension is recognised does it try to match any header data.
//
void TRAMParser::DoRecognise(const TDesC& aFileExt, CReader& aReader, TMatch& aMatch)
	{
	TBool extMatch = (aFileExt.MatchF(TPtrC(KExtRAM)) != KErrNotFound);
	if (extMatch)
		{
		aMatch.iMime = KMimeRAM;
		TPtrC8 header = MAKE_TPtrC8(KSigRAM);
		
		if (aReader.Match(header))
			{
			aMatch.iConfidence = KConfProbable;
			}
		else
			{
			aMatch.iConfidence = KConfPossible;
			}
		}
	}


//
// SDP and XPS have almost identical header signatures, so when it comes to
// recognise an SDP file with a bad extension, it can be incorrectly identified
// as an XPS file.
//
void TSDPParser::DoRecognise(const TDesC& aFileExt, CReader& aReader, TMatch& aMatch)
	{
	TBool extMatch = (aFileExt.MatchF(TPtrC(KExtSDP)) != KErrNotFound);
	if (extMatch)
		{
		aMatch.iMime = KMimeSDP;
		TPtrC8 header = MAKE_TPtrC8(KSigSDP);
		
		if (aReader.Match(header))
			{
			aMatch.iConfidence = KConfProbable;
			}
		else
			{
			aMatch.iConfidence = KConfPossible;
			}
		}
	}


//
// See TSDPParser::DoRecognise comment.
//
void TXPSParser::DoRecognise(const TDesC& aFileExt, CReader& aReader, TMatch& aMatch)
	{
	TBool extMatch = (aFileExt.MatchF(TPtrC(KExtXPS)) != KErrNotFound);
	if (extMatch)
		{
		aMatch.iMime = KMimeXPS;
		TPtrC8 header = MAKE_TPtrC8(KSigXPS);
		
		if (aReader.Match(header))
			{
			aMatch.iConfidence = KConfProbable;
			}
		else
			{
			aMatch.iConfidence = KConfPossible;
			}
		}
	}
	
