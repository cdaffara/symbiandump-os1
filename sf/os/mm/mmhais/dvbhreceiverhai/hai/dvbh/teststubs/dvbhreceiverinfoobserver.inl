// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @internalComponent
 @prototype
*/

#ifndef DVBHRECEIVERINFOOBSERVER_INL
#define DVBHRECEIVERINFOOBSERVER_INL
	
template<class OBSERVER>
inline void TDvbhNotifier<OBSERVER>::SetExternalObserver(OBSERVER* aObserver)
	{
	TDvbhNotifierBase::SetExternalObserver(aObserver);
	}

#endif //DVBHRECEIVERINFOOBSERVER_INL


