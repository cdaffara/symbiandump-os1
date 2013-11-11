/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef MLOGICALCOMPONENTOBSERVER_H
#define MLOGICALCOMPONENTOBSERVER_H


/**
 * An interface to a set of logical audio processing unit callbacks
 * 
 * This serves as the method of communication between the context and the logical audio processing units.
 *
 * The class is a mixin and is intended to be inherited by the context class that is
 * interested in observing the audio processing unit settings changes. 
 * 
 */
class MLogicalComponentObserver
	{
public:
	virtual void ReceiveComponentSettingsChange(TUid aId) = 0;
	};

#endif // MLOGICALCOMPONENTOBSERVER_H