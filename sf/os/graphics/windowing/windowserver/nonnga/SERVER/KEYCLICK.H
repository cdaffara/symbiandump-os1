// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of the class that manages the keyclick plugin
// 
//

#ifndef __KEYCLICK_H__
#define __KEYCLICK_H__

#ifndef __E32STD_H__
#include <e32std.h>
#endif
#ifndef __E32BASE_H__
#include <e32base.h>
#endif
#ifndef __OBJECT_H__
#include "OBJECT.H"
#endif

class CClickMaker;


#define THIRD_UID_INDEX 2

class CClick : public CWsObject
	{
public:		//Static Part
	static void InitStaticsL();
	static void DeleteStatics();
	static void KeyEvent(TEventCode aType,const TKeyEvent& aEvent);
	static void PointerEvent(const TPoint& aScreenPos,TPointerEvent aEvent);
	static void OtherEvent(TInt aType,TAny* aParam=NULL);
	inline static TBool IsHandler() {return iHandler!=NULL;}
	inline static void SetKeyClickOveride(TBool aOn) {iKeyClickOveride=aOn;}
	static void LoadNewLibraryL(const TDesC &aDllName);
public:		//Object Part
	inline CClick(CWsClient* aOwner) : CWsObject(aOwner,WS_HANDLE_CLICK) {}
	//~CClick();
	void ConstructL(const TUid& aUid);
	//Pure Virtual function from CWsObject
	void CommandL(TInt aOpcode, const TAny *aCmdData);
private:
	static inline void Unload() {DeleteStatics();}
	static inline TUid ThirdUid() {return iPlugIn.Type()[THIRD_UID_INDEX];}
private:
	TUid iThirdUid;
private:
	static CClickMaker* iHandler;
	static TBool iIsChangeable;
	static TBool iKeyClickOveride;
	static TBool iKeyClickEnabled;
	static TBool iPenClickEnabled;
	static RLibrary iPlugIn;
	static TBool iIsLoaded;
	};

#endif
