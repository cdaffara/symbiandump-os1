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



/**
 * @file
 *
 * implements inline methods of RComm
 */

inline void RComm::SetSignals(TUint aSetMask, TUint aClearMask)
/** Sets or clears RS232 output lines (DTR and RTS).

For many applications, these lines will be read and set under driver control 
as determined by the handshaking options selected.

@param aSetMask Set the handshaking lines in the mask. 
@param aClearMask Clear the handshaking lines in the mask. 
@publishedAll
@released
*/
	{
	SetSignalsToMark(aSetMask);
	SetSignalsToSpace(aClearMask);
	}
