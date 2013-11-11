// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @InternalComponent
*/

#ifndef TPOSITIONERCALL_H
#define TPOSITIONERCALL_H

#include <e32base.h>

class MPositioner;
class TPositionInfo;

class TPositionerCall
	{
	public:
		virtual void Call(MPositioner& client) = 0;
	};
	
class TLocationUpdate : public TPositionerCall
	{
	public:
		TLocationUpdate(TPositionInfo& aPositionInfo, TInt aError, TTime& aActualTime, TUint aMode);
		void Call(MPositioner& client);
		
	private:
		TPositionInfo& iPositionInfo;
		TInt iError;
		TTime& iActualTime;
		TUint iMode;
	};
	
class TUpdateFailed : public TPositionerCall
	{
	public:
		TUpdateFailed(TInt aError);
		void Call(MPositioner& client);
		
	private:
		TInt iError;
	};
	
class TModeUpdate : public TPositionerCall
	{
	public:
		TModeUpdate(TUint aMode);
		void Call(MPositioner& client);
		
	private:
		TInt iMode;
	};
	
class TRetBoolPositionerCall
	{
	public:
		virtual TBool Call(MPositioner& client) = 0;
	};

class TActiveClients : public TRetBoolPositionerCall
	{
	public:
		TBool Call(MPositioner& client);
	};
	
class TWarmingDownClients : public TRetBoolPositionerCall
	{
	public:
		TBool Call(MPositioner& client);
	};

class TTracking : public TRetBoolPositionerCall
	{
	public:
		TBool Call(MPositioner& client);
	};

class TRetTUintPositionerCall
	{
	public:
		virtual TUint Call(MPositioner& client) = 0;
	};

class TInactivityTimeout : public TRetTUintPositionerCall
	{
	public:
		TUint Call(MPositioner& client);
	};

#endif // TPOSITIONERCALL_H

// End of File
