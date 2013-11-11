// Utils.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#ifndef UTILS_H
#define UTILS_H

#include <fshell/QR3ProductPlugin.h>
#include <fshell/qr3dll.h>

void HR(TDes& aDes, TInt64 aSize); // Human readable formatting of aSize as a memory size (eg 1500 -> "1.46 KB")
void HR(TDes8& aDes, TInt64 aSize); 

TPtrC ToString(TProcessPriority aPriority);
TPtrC ToString(TThreadPriority aPriority);
TPtrC ToString(TExitType aType);
HBufC* ToStringL(SCapabilitySet& aCaps);
TPtrC ToString(User::TCritical aCritical);
TPtrC ToStringHal(TInt aHalAttribute);
void ToStringHalVal(TDes& aDes, TInt aHalAttribute, TInt aHalValue);

void PrettyName(TInt aType, TDes& aName); // aType is a TKernelObjectType

//extern const char*const* KKTraceFlags[];
TPtrC8 TraceFlagName(TInt aWord, TInt aBit);

extern char const* const* const KCloggerTags[];

//extern const char* KFileServerTraceFlags[];

class CEcomWatcher;

NONSHARABLE_CLASS(CDefaultProductPlugin) : public CBase, public MProductPlugin
	{
public:
	static void LoadProductPluginL();
	~CDefaultProductPlugin();

	// From MProductPluginV1
	TPtrC8 TraceFlagName(TInt aWord, TInt aBit);
	void FormatValue(TDes& aDes, TValueType aType, TInt aAttrib, TInt aVal);
	TUint32 GetDeviceType();
	TBool HandleErrorL(TError aType, TInt aErr);
	TKeyResponse OfferKeyEventL(TKeyEvent& aEvent, TEventCode aType);
	void RefreshTabL(TInt aTab);
	TBool DoDynInitOrDeleteCommandL(CQikCommand*& aCommand, const CCoeControl& aControlAddingCommands);
	void DoViewConstructL(CQikMultiPageViewBase* aParentView);
	void DoHandleControlEventL(CCoeControl *aControl, MCoeControlObserver::TCoeEvent aEventType);
	TBool ConsumeMemoryKeys(TBool aDisk, TDes& aCheckboxName, TInt& aUpKeyCode, TInt& aDownKeyCode, TInt& aLogKeyCode);
	void Release(TBool aAppIsShuttingDown);

	// From MProductPluginV2
	void EnablePanicChecking(TBool aEnable);
	TBool PanicCheckingIsEnabled();

	// From MProductPluginV3
	void GetFeatureUidsL(RArray<TUid>& aUids);

	// From MProductPluginV4
	TBool ConsumeMemoryKeys(TBool aDisk, TDes& aCheckboxName, TInt& aUpKeyCode, TInt& aUpScanCode, TInt& aDownKeyCode, TInt& aDownScanCode, TInt& aLogKeyCode, TInt& aLogScanCode);
	
	// Internal
	void EcomChangedL();
	void ConstructL();
	void DestroyPlugins();
	void RescanPluginsL();
	void LoadPluginL(TUid aImplUid, TInt aVersion);
	void DisablePanicCheckPushL(); // Disables panic checks, pushes an item onto the cleanupstack that restores them
	static void EnablePanicCheck(TAny* aSelf);

private:
	REComSession iEcom;
	RImplInfoPtrArray iEcomPluginList;
	RPointerArray<MProductPlugin> iPlugins;
	MProductPluginV1* iPluginForThisDevice; // Not owned, may be NULL. Is explicitly typed to be MProductPluginV1 to remind people they have to check the version before calling any non-V1 functions
	TInt iPluginForThisDeviceVersion; // Will be 0 if iPluginForThisDevice == NULL
	RArray<TUid> iPluginDestructors;	
	TCallBack iPluginsChangedCallback;
	CEcomWatcher* iEcomChangeWatcher;
	CQikMultiPageViewBase* iParentView;
	TBool iShuttingDown;
	};

#ifdef __DLL__
#define gPlugin ((CDefaultProductPlugin*)Dll::Tls())
#else
extern CDefaultProductPlugin* gPlugin;
#endif

void StartProfilerL();
TBool IsProfilerRunning();
void StopProfiler();

TUint CriticalToFlags(User::TCritical aCrit);
User::TCritical FlagsToCritical(TUint aFlags);


#endif
