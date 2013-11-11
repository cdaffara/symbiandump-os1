/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* NetDial Script Commands inline functions.
* 
*
*/



/**
 @file Scommand.inl
*/

#ifndef __SCOMMAND_INL__
#define __SCOMMAND_INL__

#include "SCOMMAND.H"

inline TPtr CLabelSearch::LabelName() const
/**
Returns label name
*/
	{
	return iLabelName->Des();
	}


inline CCommChatString* CLabelSearch::ChatString() const
/**
Returns chat string
*/
	{
	return iChatString;
	}

inline void CLabelSearch::Set(TLabelSearchStatus aStatus,const TLinePosition& aPosition)
/**
Sets iStatus and iPosition.
*/
	{
	iStatus=aStatus;
	iPosition=aPosition;
	}	

inline void CLoopCommand::Loop(TBool aLoop)
/**
Set iLoop.
*/
	{
	iLoop=aLoop;
	}

#endif
