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

#ifndef REGUI_H
#define REGUI_H

// forward declarations
#include <e32test.h>
#include <btmanclient.h>

class CLocalDeviceUser;
class CActiveRegistryUser;

class CRegistryViewObserver;

class MActiveConsoleNotify
	{
public:
	virtual void KeyPressedL(TChar aKey) = 0;
	};

class MViewNotify
	{
public:
	virtual void ViewChanged() =0;
	};

class CActiveConsole : public CActive
	{
	public:
		static CActiveConsole* NewL(MActiveConsoleNotify& aNotify);
		void RequestKey();
		
		inline CConsoleBase& Console() const 
			{
			return *iConsole;
			};

		~CActiveConsole();
		static CConsoleBase* AutoSizeNewL(const TDesC& aTitle, TSize aSize);

	private:
		void RunL();
		TInt RunError(TInt aError);
		void DoCancel();
		void DrawCursor();
		CActiveConsole(MActiveConsoleNotify& aNotify);
		void ConstructL();

	private:
		CConsoleBase*			iConsole;
		MActiveConsoleNotify&	iNotify;
	};

class CRegUIApp : public CBase, public MActiveConsoleNotify
	{
public:
	static CRegUIApp* NewL();

	void StartL();
	void CreateViewL(const TBTRegistrySearch& aSearch);
	void CreateLocalDeviceL();
	void Stop();
	~CRegUIApp();
	void TestMenu();
	TBTDeviceName GetName();
	CActiveRegistryUser* NewRegUserPleaseL();
	void KindlyRemoveRegUserPleaseL(CActiveRegistryUser* aUser);
	void KindlyRemoveLocalDeviceUserPlease();
	void Error(TInt aError);
	
public: // From MActiveConsoleNotify
	void KeyPressedL(TChar aKey);

private:
	CRegUIApp();
	void ConstructL();
	void StartRegistryServerL();
private:
	RBTRegServ		iRegServ;
	CActiveConsole*	iActiveConsole;

	CLocalDeviceUser*	iLocalDeviceUser;
	RPointerArray<CActiveRegistryUser> iUsers;
	};
	
	

class CActiveRegistryUser : public CActive, public MActiveConsoleNotify, public MViewNotify
	{
	public:
		~CActiveRegistryUser();
		static CActiveRegistryUser* NewL(CRegUIApp& aApp,
										 RBTRegServ& aRegServ);

		void Search(const TBTRegistrySearch& aSearch);
		void Unpair();
		void UnpairAllDevicesInView();
		void Delete();
		void ChangeFriendlyNameL();
		void RequireAuthorisation(const TBool aAuthorisationRequired);
		void SetCookieL();
		
		void DestroyIfDone();

	private:
		CActiveRegistryUser(CRegUIApp& aApp, RBTRegServ& aRegServ);
		void DoCancel();
		void RunL();
		TInt RunError(TInt aError);
		void ConstructL();
		TBool PreProcessKey(TChar aKey);
		void DoDisplayDevice(TInt aIndex, const CBTDevice& aDevice);
		void LogCoD(TUint aCoD);
		void MenuL();
		void ChooseDeviceL();
		void RegisterViewChangeNotifier();
		
	private: // From MActiveConsoleNotify
		void KeyPressedL(TChar aKey);

	private: // From MViewNotify
		void ViewChanged();
	
	private:
		enum TState
			{
			ECreatingView,
			EUsingResponse,
			ESelecting,
			EUnpairing,
			EDeleting,
			EChangingFriendlyName,
			EChangingAuthorisation,
			ESettingCookie,
			EReady,
			EDone,
			};
	private:
		RBTRegistry				iView;
		RBTRegServ&				iRegistryServer;
		CRegUIApp&				iApp;
		CActiveConsole*			iActiveConsole;
		CBTRegistryResponse*	iResponseHandler;
		TBTRegistrySearch		iSearch;
		TBTNamelessDevice		iDevice;
		TState					iState;
		CActiveRegistryUser*	iChildUser;
		CRegistryViewObserver*  iViewObserver;
	};

class CLocalDeviceUser : public CBase, public MActiveConsoleNotify
	{
	public:
		~CLocalDeviceUser();
		static CLocalDeviceUser* NewL(CRegUIApp& aApp,
										 RBTRegServ& aRegServ);

	private:
		CLocalDeviceUser(CRegUIApp& aApp, RBTRegServ& aRegServ);
		void ConstructL();
		void MenuL();
						
	private: // From MActiveConsoleNotify
		void KeyPressedL(TChar aKey);
	
	private:
		TBTLocalDevice			iLocalDeviceSettings;
		RBTLocalDevice			iLocalDevice;
		RBTRegServ&				iRegistryServer;
		CRegUIApp&				iApp;
		CActiveConsole*			iActiveConsole;
	};

class CRegistryViewObserver : public CActive
	{
public:
	static CRegistryViewObserver* New(MViewNotify& aNotifiee, RBTRegistry& aView);
	~CRegistryViewObserver();
private:
	CRegistryViewObserver(MViewNotify& aNotifiee, RBTRegistry& aView);
	void Queue();
private:
	virtual void RunL();
	virtual void DoCancel();
private:
	MViewNotify&    iNotifiee;
	RBTRegistry&    iView;
	};

#endif // REGUI_H
