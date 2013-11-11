// QR3ProductPlugin.h
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
#ifndef QR3PRODUCTPLUGIN_H
#define QR3PRODUCTPLUGIN_H

enum TPluginEnumValues
	{
	// More types will be added to this as we support more plugins.
	// Anyone writing a plugin will be assigned a set of EPluginx values that they should use.
	// This is to avoid conflicts

	EPlugin1CommandsOrControls = 0x3000,
	EPlugin1Tabs = 0x3490,
	EPlugin1Max = 0x3499,

	EPlugin2CommandsOrControls = 0x3500,
	EPlugin2Tabs = 0x3990,
	EPlugin2Max = 0x3999,

	EPluginMax = 0x9999
	};

#define KQr3ProductPluginEcomUid 0x102831C5

#ifdef __cplusplus
// We reuse this file as an HRH as well, so we have to escape everything for when we're being run by rcomp

#include <e32base.h>
#include <COECOBS.H> // TCoeEvent
#include <COEDEF.H> // For key handling stuff
#include <W32STD.H> // Ditto

class CQikMultiPageViewBase;
class CQikCommand;
class CCoeControl;

class MProductPluginV1 // This corresponds to version 1 in your ECOM rss file
	{
	// Do not add any virtual functions to this class! If you need to do that, create a new class MProductPluginV2 
	// that derives from MProductPluginV1 and add them there. This ensures BC between plugin versions. If you do, be
	// sure to update CDefaultProductPlugin to not call through to plugins too old to support the new methods.
	// Or better yet talk to me first! -Tom
public:
	enum TError
		{
		ECouldntSetDebugPort,
		};

	enum TValueType
		{
		EHal,
		EKey,
		EFeature,
		};

	// Non-UI device attributes
	virtual TPtrC8 TraceFlagName(TInt aWord, TInt aBit)=0;
	virtual void FormatValue(TDes& aDes, TValueType aType, TInt aAttrib, TInt aVal)=0;
	virtual TUint32 GetDeviceType()=0;
	virtual TBool HandleErrorL(TError aType, TInt aErr)=0; // Return ETrue to handle.
	virtual TBool ConsumeMemoryKeys(TBool aDisk, TDes& aCheckboxName, TInt& aUpKeyCode, TInt& aDownKeyCode, TInt& aLogKeyCode)=0;
	virtual void Release(TBool aAppIsShuttingDown)=0; // aAppIsShuttingDown may be useful for things with complex interactions with CEikonEnv

	// View related functions
	virtual TKeyResponse OfferKeyEventL(TKeyEvent& aEvent, TEventCode aCode)=0;
	virtual void DoViewConstructL(CQikMultiPageViewBase* aParentView)=0;
	virtual TBool DoDynInitOrDeleteCommandL(CQikCommand*& aCommand, const CCoeControl& aControlAddingCommands)=0;
	virtual void DoHandleControlEventL(CCoeControl *aControl, MCoeControlObserver::TCoeEvent aEventType)=0;

	virtual void RefreshTabL(TInt aTab)=0;
	};

class MProductPluginV2 : public MProductPluginV1
	{
public:
	virtual void EnablePanicChecking(TBool aEnable)=0; // This function should disable any device-specific crash capture behaviour that may be invoked as a result of calls to User::Panic. This is because QResources is about to call code that might panic, but it should continue regardless and not (eg) cause the phone to reboot on debug builds.
	virtual TBool PanicCheckingIsEnabled()=0;
	};

class MProductPluginV3 : public MProductPluginV2
	{
public:
	virtual void GetFeatureUidsL(RArray<TUid>& aUids)=0; // Populates aUids with any device-specific entries in feature registry
	};

class MProductPluginV4 : public MProductPluginV3
	{
public:
	virtual TBool ConsumeMemoryKeys(TBool aDisk, TDes& aCheckboxName, TInt& aUpKeyCode, TInt& aUpScanCode, TInt& aDownKeyCode, TInt& aDownScanCode, TInt& aLogKeyCode, TInt& aLogScanCode)=0;
	};

/*
 * MProductPlugin is always typedef'd to the latest plugin version
 * This means that plugin writers can always derive from MProductPlugin,
 * and not usually have to worry about it, but you get the BC advantages of
 * versioning. If there is a new version which you don't want to implement when
 * you rebuild your plugin, then just change the declaration to explicitly derive
 * from the old version (eg change "class Cx : public CBase, public MProductPlugin" to 
 * "class Cx : public CBase, public MProductPluginV1"
 *
 * Remember that what you derive from must match the version number in your ecom rss
 * file! (Strictly speaking, the rss version num must be <= the class derived)
 */
typedef MProductPluginV4 MProductPlugin;

#endif
#endif
