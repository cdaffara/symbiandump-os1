// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// contains the inline functions of the RSmsSocketBuf class
// 
//

/**
 @file
 
 @return how much the socket is lagging for read
 If positive the buffer needs to read more from the socket in order to be synchronised
 @publishedAll
 
*/
inline TInt RSmsSocketBuf::Lag(TRead) const
	{
	return Ptr(ERead)-End(ERead);
	}


/**
 *  @return how much the socket is lagging for write
 *  If positive the buffer need to write more to the socket in order to be synchronised
 */
inline TInt RSmsSocketBuf::Lag(TWrite) const
	{
	return Ptr(EWrite)-iBuffer;
	}
