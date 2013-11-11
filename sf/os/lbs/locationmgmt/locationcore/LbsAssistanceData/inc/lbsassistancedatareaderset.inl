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
// lbsassistancedatautil.inl
// 
//

/**
 Gets one of the assistance data readers in the set.
 @param aReaderPtr The pointer to the reader to be retrieved.
 @return KErrNone if successful, one of the system wide error codes otherwise.
 @released 
*/
template <typename T>
	inline TInt RLbsAssistanceDataReaderSet::GetDataReader(T*&  aReaderPtr)
		{
		return DoGet(aReaderPtr);		
		}

