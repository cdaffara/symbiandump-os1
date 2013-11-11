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

#ifndef TEIR_H
#define TEIR_H

// forward declarations
#include <e32test.h>
#include <es_sock.h>
#include <f32file.h>
#include <bt_sock.h>
#include <btsdp.h>
#include <c32comm.h>
//#include "debug.h"
#include <btmanclient.h>
//#include <hcierrors.h>
#include <btextnotifiers.h>
#include <bluetooth/eirpublisher.h>
#include <bluetooth/eirpublishermanufacturerspecificdata.h>
#include <bttypes.h>
#include <bluetooth/eirdatacodec.h>

class CActiveNameRequester;
//class CActiveInquirer;
class CActiveHostResolver;
class CActiveConnector;
class CActiveConsole;
class CActiveBasebandDisconnector;
class CEirPublisherTestName;
class CEirPublisherTestUuid16;
class CEirPublisherTestUuid128;
class CEirPublisherTestManuSpecific;
class CEirPublisherTestTxPowerLevel;

class CEirPublisherManufacturerSpecificData;
class CEirApp;
	
enum TTestEirTag
	{
	ETestDeviceName = 0,
	ETestUuid16,
	ETestUuid128,
	ETestSetMSD,
	ETestRemoveMSD,
	};

class CManufacturerDataNotifier : public CBase, public MManufacturerSpecificDataPublisherNotifier
	{
public:
	static CManufacturerDataNotifier* NewL(CEirApp& aEirApp);
	CManufacturerDataNotifier(CEirApp& aEirApp);
	~CManufacturerDataNotifier();
	void ConstructL();
	
	// From MManufacturerSpecificDataPublisherNotifier
	virtual void MmsdpnNotifyAvailableDataSize(TUint aAvailableDataSize);
	virtual void MmsdpnNotifyError(TInt aResult);
private:
	CEirApp& iEirApp;
	};

class CEirApp : public CBase
	{
	public:

		static CEirApp* NewL(CConsoleBase& aConsole);
		void NewNameRequesterL(TBool aEirInstead, TBool aIgnoreCache);
		//void CancelNameRequester();
		//void DeleteNameRequester();
		void NewInquirerL(TUint aIAC, TBool aDoNames, TBool aIgnoreCache, TBool aDoEir, TBool forever=EFalse);
		void CancelInquirer();
		void DeviceFound(TBTDevAddr aAddress);
		void InquiryComplete();
		void NewNameArrayRequesterL();

		void NewEirPublisherTestL(TTestEirTag aTag);

		void InitialiseL();
		void StopProtocols();
		void ShowAddress();

		void StartL();
		void Stop();
		~CEirApp();

		RSocketServ& SocketServ(); 
		void SetPublishManuData(TUint aAvailableDataSize);
		void Register16BitSDPRecordL();
		void Register128BitSDPRecordL();

	private:
		CEirApp(CConsoleBase& aConsole);
		void ConstructL(CConsoleBase& aConsole);
		void CreateSdpRecordL(const TUUID& aUUID, TBool aIsPublicBrowseGroup = ETrue, TBool aIsL2cap = ETrue);
	private:
		RSocketServ		iSocketServer;
		CActiveNameRequester* iNameRequester;
		//CActiveInquirer* iInquirer;
		CActiveConsole*	iActiveConsole;
		CConsoleBase& iConsole;

		RPointerArray<CActiveHostResolver> iResolvers;
		RArray<TBTDevAddr>	iDevicesFound;

		CEirPublisherManufacturerSpecificData* iEirPublisherTestManuSpecific;

		CManufacturerDataNotifier* iManuNotifier;
		RSdpDatabase iSdpDB;
		RSdp iSdp;

	};


class CActiveConsole : public CActive
	{
	public:
		static CActiveConsole* NewL(CConsoleBase& aConsole, CEirApp& aApp);

		void RunL();
		void DoCancel();
		void TestMenu();
		void DrawCursor();
		~CActiveConsole();

	private:
		CActiveConsole(CConsoleBase& aConsole, CEirApp& aApp);
		void ConstructL();
		void RequestKey();
		TInt SetHostName();
	TBool ProcessKey(TChar aCh);

	private:
		CConsoleBase&		iConsole;
		CEirApp&	iApp;
	};

class CActiveHostResolver : public CActive
	{
	// abstract class for derivation - deriveees implement RunL and Start
	public:
		void DoCancel();
		~CActiveHostResolver();
		void SetLocalNameL();

	protected:
		CActiveHostResolver(CConsoleBase& aConsole, CEirApp& aApp);
		void StartServerL();
		void StartHostResolverL();
		void SetIAC(TUint aIAC);
		void ConstructL();
		virtual void Start() = 0;

	protected:
		RHostResolver		iHostResolver;
		RSocketServ			iSocketServer;
		TProtocolDesc		iProtocolInfo;
		CConsoleBase&		iConsole; //root console
		CEirApp&	iApp;
		CConsoleBase*		iHROutputConsole;
		TInquirySockAddr	iInquiryRequest;

	private:
		TInt RunError(TInt aError);

	private:
		static TInt			iHRNumber;
	};


class CActiveNameRequester : public CActiveHostResolver
	{
	public:
		static CActiveNameRequester* NewL(CConsoleBase& aConsole, CEirApp& aApp, TBool aEirInstead, TBool aIgnoreCache);
		void RunL();
	protected:
		virtual void PrintResults();

		CActiveNameRequester(CConsoleBase& aConsole, CEirApp& aApp, TBool aEirInstead, TBool aIgnoreCache);
		void ConstructL();
	private:
		void Start();
	private:
		TNameEntry			iResult;
		TBool				iEirInstead;
		TBool				iIgnoreCache;
	};

class CActiveInquirer : public CActiveHostResolver
	{
	public:
		static CActiveInquirer* NewL(CConsoleBase& aConsole, CEirApp& aApp, TUint aIAC, TBool aDoNames, TBool aIgnoreCache, TBool aDoEir, TBool forever);
		void RunL();
	private:
		void ConstructL(TUint aIAC);
		void Start();
		CActiveInquirer(CConsoleBase& aConsole, CEirApp& aApp, TBool aDoNames, TBool aIgnoreCache, TBool aDoEir, TBool forever);
	private:
		TNameEntry			iResult;
		TBool				iDoNames;
		TBool				iIgnoreCache;
		TBool				iDoEir;
		TBool				iForever;
	};


class CActiveNameArrayRequester : public CActiveNameRequester
// A 'high-level' name request iterator
	{
public:
	static CActiveNameArrayRequester* NewL(CConsoleBase& aConsole, CEirApp& aApp, RArray<TBTDevAddr>& aNameArray);

private: // from CActive
	void RunL();

private:
	virtual void PrintResults();
	void DoSingleNameRequest();
	void ConstructL();
	void Start();
	CActiveNameArrayRequester(CConsoleBase& aConsole, CEirApp& aApp, RArray<TBTDevAddr>& aNameArray);

private: // owned
	TNameEntry			iResult;
	TInt				iCurrentNameIndex;

private: // unowned
	RArray<TBTDevAddr>&	iNameArray;
	};

#endif // TEIR_H
