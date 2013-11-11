
// MmfDevSoundCustomInterfaceObserver.h

// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @released
 @file
*/

#ifndef MMFDEVSOUNDCUSTOMINTERFACEOBSERVER_H
#define MMFDEVSOUNDCUSTOMINTERFACEOBSERVER_H

/**
@internalComponent

The class is a mixin and is intended to support for closing the Mux and Demux pair in a
DevSound Custom Interface. The function encapsulated by this class is called whenever the 
MMMFDevSoundCustomInterfaceDeMuxPlugin::RefreshL method leaves.
*/
class MMMFDevSoundCustomInterfaceObserver
	{	
public:
	
	/**
	Closes the Mux and Demux pair of a DevSound Custom Interface.
	*/
	virtual void CloseCustomInterface(TInt aIndex) = 0;
	};	

#endif // MMFDEVSOUNDCUSTOMINTERFACEOBSERVER_H
