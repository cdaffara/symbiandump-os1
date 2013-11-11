
// GlobalEffectTest.cpp

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

#include "GlobalEffectTest.h"
#include "TestGlobalEffect.hrh"
#include "TestEffect.h"

#include <mmfstdglblaudioeffect.h>


//
// RGlobalEffectPrimitiveTest
//

RGlobalEffectPrimitiveTest* RGlobalEffectPrimitiveTest::NewL(TBool aAllocTest)
	{
	RGlobalEffectPrimitiveTest* self = new (ELeave) RGlobalEffectPrimitiveTest(aAllocTest);
	return self;	
	}
	
RGlobalEffectPrimitiveTest::RGlobalEffectPrimitiveTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-MMF-GEF-U-001");
	}
	
TVerdict RGlobalEffectPrimitiveTest::DoTestStepL()
	{
	INFO_PRINTF1(_L("Check can actually open test GlobalEffect plugin"));
	TVerdict result = EPass;
	
	__MM_HEAP_MARK;

	CTestGlobalEffect* plugin = NULL;
	
	TRAPD(error, plugin = CTestGlobalEffect::NewL(NULL));
	
	if (error==KErrNone)
		{
		INFO_PRINTF1(_L("plugin opened as expected"));
		delete plugin;
		}
	else
		{
		INFO_PRINTF2(_L("CreateImplementationL() failed - %d"), error);
		result = EFail;
		}
	
	__MM_HEAP_MARKEND;
	
	return result;
	}
	
//
// RGlobalEffectStdTest
//

RGlobalEffectStdTest* RGlobalEffectStdTest::NewL(TBool aAllocTest)
	{
	RGlobalEffectStdTest* self = new (ELeave) RGlobalEffectStdTest(aAllocTest);
	return self;	
	}
	
RGlobalEffectStdTest::RGlobalEffectStdTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-MMF-GEF-U-002");
	}
	
TVerdict RGlobalEffectStdTest::DoTestStepL()
	{
	INFO_PRINTF1(_L("Check standard plugins cleanup return errors"));
	TVerdict result = EPass;
	
	{
	__MM_HEAP_MARK;

	CMmfGlobalEqEffect* plugin = NULL;
	
	TRAPD(error, plugin = CMmfGlobalEqEffect::NewL(NULL));
	
	if (error==KErrNone)
		{
		INFO_PRINTF1(_L("GlobalEq plugin opened managed to open!"));
		delete plugin;
		result = EFail;
		}
	else if (error==KErrNotSupported)
		{
		INFO_PRINTF1(_L("GlobalEq plugin not supported - as expected"));		
		}
	else
		{
		INFO_PRINTF2(_L("GlobalEq CreateImplementationL() failed - %d"), error);
		result = EFail;
		}
	
	__MM_HEAP_MARKEND;
	}
	
	{
	__MM_HEAP_MARK;

	CMmfGlobalMegaEffect* plugin = NULL;
	
	TRAPD(error, plugin = CMmfGlobalMegaEffect::NewL(NULL));
	
	if (error==KErrNone)
		{
		INFO_PRINTF1(_L("GlobalMega plugin opened managed to open!"));
		delete plugin;
		result = EFail;
		}
	else if (error==KErrNotSupported)
		{
		INFO_PRINTF1(_L("GlobalMega plugin not supported - as expected"));		
		}
	else
		{
		INFO_PRINTF2(_L("GlobalMega CreateImplementationL() failed - %d"), error);
		result = EFail;
		}
	
	__MM_HEAP_MARKEND;
	}
	
	return result;
	}
	
//
// RGlobalEffectMainTest
//

RGlobalEffectMainTest* RGlobalEffectMainTest::NewL(TBool aAllocTest)
	{
	RGlobalEffectMainTest* self = new (ELeave) RGlobalEffectMainTest(aAllocTest);
	return self;	
	}
	
RGlobalEffectMainTest::RGlobalEffectMainTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-MMF-GEF-U-003");
	}
	
TVerdict RGlobalEffectMainTest::DoTestStepL()
	{
	INFO_PRINTF1(_L("Check basic API calls on TestPlugin"));
	TVerdict result = EPass;
	
	__MM_HEAP_MARK;

	CTestGlobalEffect* plugin = NULL;
	
	TRAPD(error, plugin = CTestGlobalEffect::NewL(NULL));
	
	if (error!=KErrNone)
		{
		INFO_PRINTF2(_L("CreateImplementationL() failed - %d"), error);
		return EFail;
		}
		
	CleanupStack::PushL(plugin);
	
	{
	// check Capability
	TUint value = plugin->Capability(EFalse);
	if (value!=0)
		{
		INFO_PRINTF2(_L("Capability(EFalse): expected 0 got - %d"), value);
		result = EFail;
		}
	value = plugin->Capability(ETrue);
	if (value!=0xffff)
		{
		INFO_PRINTF2(_L("Capability(ETrue): expected -1 got - %d"), value);
		result = EFail;
		}
	}
	
	// check settings by des, uid or value
	HBufC8* tempDes = NULL;
	TUid tempUid;
	TInt tempInt;
	TPckg<TInt> tempPackage (tempInt);
	{
	// initially (stage 1) nothing set, so SettingsByUidL() and ExtractValuesL()
	// should leave KErrNotSupported, and SettingsByDesL() should return "1234"
	
	TRAP(error, tempDes = plugin->SettingsByDesL());
	if (error==KErrNone)
		{
		CleanupStack::PushL(tempDes);
		if (*tempDes!=_L8("1234"))
			{
			HBufC* tempDes16 = HBufC::NewMaxLC(tempDes->Length());
			tempDes16->Des().Copy(*tempDes);
			INFO_PRINTF2(_L("Stage1: SettingsByDesL() expected \"1234\" got \"%S\""), tempDes16);
			result = EFail;
			CleanupStack::PopAndDestroy(tempDes16);
			}
		CleanupStack::PopAndDestroy(tempDes);	
		tempDes = NULL;	
		}
	else
		{
		INFO_PRINTF2(_L("Stage1: SettingsByDesL() returned %d"), error);
		result = EFail;
		}
	
	TRAP(error, tempUid = plugin->SettingsByUidL());	
	if (error!=KErrNotSupported)
		{
		INFO_PRINTF3(_L("Stage1: SettingsByUidL() expected KErrNotSupported got %d(0x%x)"), error, tempUid.iUid);
		result = EFail;
		}
		
	TRAP(error, plugin->ExtractValuesL(tempPackage));
	if (error!=KErrNotSupported)
		{
		INFO_PRINTF3(_L("Stage1: ExtractValuesL() expected KErrNotSupported got %d(%d)"), error, tempInt);
		result = EFail;
		}
	
	}
		
	{
	// at stage 2, we set a value by UID, so now SettingsByUid returns something sensible
	TUid sampleUid = {0x1234567}; // any silly value will do
	plugin->SetSettingsByUidL(sampleUid);
	
	TRAP(error, tempDes = plugin->SettingsByDesL());
	if (error==KErrNone)
		{
		CleanupStack::PushL(tempDes);
		if (*tempDes!=_L8("1234"))
			{
			HBufC* tempDes16 = HBufC::NewMaxLC(tempDes->Length());
			tempDes16->Des().Copy(*tempDes);
			INFO_PRINTF2(_L("Stage2: SettingsByDesL() expected \"1234\" got \"%S\""), tempDes16);
			result = EFail;
			CleanupStack::PopAndDestroy(tempDes16);
			}
		CleanupStack::PopAndDestroy(tempDes);	
		tempDes = NULL;	
		}
	else
		{
		INFO_PRINTF2(_L("Stage2: SettingsByDesL() returned %d"), error);
		result = EFail;
		}
	
	TRAP(error, tempUid = plugin->SettingsByUidL());	
	if (error!=KErrNone)
		{
		INFO_PRINTF3(_L("Stage2: SettingsByUidL() expected KErrNone got %d(0x%x)"), error, tempUid.iUid);
		result = EFail;
		}
	else if (tempUid != sampleUid)
		{
		INFO_PRINTF3(_L("Stage2: SettingsByUidL() expected return of 0x%x got 0x%x"), sampleUid.iUid, tempUid.iUid);		
		}
		
	TRAP(error, plugin->ExtractValuesL(tempPackage));
	if (error!=KErrNotSupported)
		{
		INFO_PRINTF3(_L("Stage2: ExtractValuesL() expected KErrNotSupported got %d(%d)"), error, tempInt);
		result = EFail;
		}
	
	}
		
	{
	// at stage 3, we set a value by des, so again only SettingsByDesL() gives valid results
	_LIT8(KSampleDes, "89a");
	plugin->SetSettingsByDesL(KSampleDes);
	
	TRAP(error, tempDes = plugin->SettingsByDesL());
	if (error==KErrNone)
		{
		CleanupStack::PushL(tempDes);
		if (*tempDes!=KSampleDes)
			{
			HBufC* tempDes16 = HBufC::NewMaxLC(tempDes->Length());
			tempDes16->Des().Copy(*tempDes);
			INFO_PRINTF2(_L("Stage3: SettingsByDesL() expected \"89a\" got \"%S\""), tempDes16);
			result = EFail;
			CleanupStack::PopAndDestroy(tempDes16);
			}
		CleanupStack::PopAndDestroy(tempDes);	
		tempDes = NULL;	
		}
	else
		{
		INFO_PRINTF2(_L("Stage3: SettingsByDesL() returned %d"), error);
		result = EFail;
		}
	
	TRAP(error, tempUid = plugin->SettingsByUidL());	
	if (error!=KErrNotSupported)
		{
		INFO_PRINTF3(_L("Stage3: SettingsByUidL() expected KErrNotSupported got %d(0x%x)"), error, tempUid.iUid);
		result = EFail;
		}
		
	TRAP(error, plugin->ExtractValuesL(tempPackage));
	if (error!=KErrNotSupported)
		{
		INFO_PRINTF3(_L("Stage3: ExtractValuesL() expected KErrNotSupported got %d(%d)"), error, tempInt);
		result = EFail;
		}
	}
		
	{
	// at stage 4, we set a value by package buffer, so only that way returns valid value
	const TInt sampleInt = 10;
	TPckgC<TInt> sampleBuffer (sampleInt);
	plugin->SetByValuesL(sampleBuffer);
	
	TRAP(error, tempDes = plugin->SettingsByDesL());
	if (error==KErrNone)
		{
		CleanupStack::PushL(tempDes);
		HBufC* tempDes16 = HBufC::NewMaxLC(tempDes->Length());
		tempDes16->Des().Copy(*tempDes);
		INFO_PRINTF2(_L("Stage4: SettingsByDesL() expected KErrNotSupported got \"%S\""), tempDes16);
		result = EFail;
		CleanupStack::PopAndDestroy(2, tempDes);	
		tempDes = NULL;	
		}
	else if (error!=KErrNotSupported)
		{
		INFO_PRINTF2(_L("Stage4: SettingsByDesL() expected KErrNotSupported returned %d"), error);
		result = EFail;
		}
	
	TRAP(error, tempUid = plugin->SettingsByUidL());	
	if (error!=KErrNotSupported)
		{
		INFO_PRINTF3(_L("Stage4: SettingsByUidL() expected KErrNotSupported got %d(0x%x)"), error, tempUid.iUid);
		result = EFail;
		}
		
	TRAP(error, plugin->ExtractValuesL(tempPackage));
	if (error!=KErrNone)
		{
		INFO_PRINTF3(_L("Stage4: ExtractValuesL() expected KErrNone got %d(%d)"), error, tempInt);
		result = EFail;
		}
	else if (tempInt!=sampleInt)
		{
		INFO_PRINTF3(_L("Stage4: ExtractValuesL() expected %d got %d"), sampleInt, tempInt);
		result = EFail;
		}
	}
	
	{
	// Check for Enabled() and IsActive() - should be false to start. poke value and show have
	// changed. Have to use custom command for SetActive() since not really the way to drive
	// - this should relate to the underlying implementation
	TBool enabled = plugin->IsEnabled();
	TBool active = plugin->IsActive();
	if (enabled!=EFalse || active!=EFalse)
		{
		INFO_PRINTF3(_L("Check1: active(%d) and enabled(%d) should have been false"), active, enabled);
		result = EFail;
		}
	plugin->SetEnabledL(ETrue);
	plugin->SetActive(ETrue);
	enabled = plugin->IsEnabled();
	active = plugin->IsActive();
	if (enabled==EFalse || active==EFalse)
		{
		INFO_PRINTF3(_L("Check2: active(%d) and enabled(%d) should have been true"), active, enabled);
		result = EFail;
		}
	}
	
	{
	// check preset - for test this ties up with the test plugin
	const TInt KPresetsCount = 3;
	MMmfGlobalAudioPresetList* list = plugin->KnownPresetsL();

	// reproduce what the plugin will do and check
	if (list->MdcaCount()!=KPresetsCount)
		{
		INFO_PRINTF3(_L("Expected %d presets, got %d"), KPresetsCount, list->MdcaCount());
		result = EFail;
		}
	else
		{
		for (TInt index=0; index<KPresetsCount; index++)
			{
			TBuf<10> name;
			name.Num(index);
			TUid tempUid={index};
			
			TUid foundUid = list->GAPUidPoint(index);
			const TPtrC foundName = list->MdcaPoint(index);
			
			if (foundUid!=tempUid)
				{
				INFO_PRINTF4(_L("Index %d, expected %x found %x"), index, tempUid.iUid, foundUid.iUid);
				result = EFail;
				}
			if (foundName != name)
				{
				INFO_PRINTF4(_L("Index %d, expected %S found %S"), index, &name, &foundName);
				result = EFail;
				}
			}
		
		}
	}

	CleanupStack::PopAndDestroy(plugin);
	
	__MM_HEAP_MARKEND;
	
	return result;
	}
	
//
// RGlobalEffectCallbackTest
//

const TUid KUidTestGlbNotification = {KUidTestGlbNotificationDefine};


RGlobalEffectCallbackTest* RGlobalEffectCallbackTest::NewL(TBool aAllocTest)
	{
	RGlobalEffectCallbackTest* self = new (ELeave) RGlobalEffectCallbackTest(aAllocTest);
	return self;	
	}
	
RGlobalEffectCallbackTest::RGlobalEffectCallbackTest(TBool /*aAllocTest*/)
	{
	iTestStepName = _L("MM-MMF-GEF-U-004");
	}
	
void RGlobalEffectCallbackTest::GAEEventNotificationL(CMmfGlobalAudioEffect* aEffect, 
                                                      TUid aEventUid, 
                                                      const TDesC8& aParam)
	{
	INFO_PRINTF1(_L("Callback"));
	if (aEffect!=iEffect)
		{
		INFO_PRINTF3(_L("Effect is %x, expected %d"), aEffect, iEffect);
		iResult = EFail;
		}
	if (KUidTestGlbNotification != aEventUid)
		{
		INFO_PRINTF3(_L("Uid is %x, expected %x"), aEventUid.iUid, KUidTestGlbNotification.iUid);
		iResult = EFail;
		}
	if (aParam != KNullDesC8)
		{
		INFO_PRINTF2(_L("Param=\"%S\", expected \"\""), &aParam);
		iResult = EFail;
		}
	iCalledBack = ETrue;
	}

	
TVerdict RGlobalEffectCallbackTest::DoTestStepL()
	{
	INFO_PRINTF1(_L("Check notification callback"));
	iResult = EPass;
	
	__MM_HEAP_MARK;

	iEffect = CTestGlobalEffect::NewL(this);
	CleanupStack::PushL(iEffect);
	
	// for test, just generate test synchronously
	iEffect->RequestNotificationL(KUidTestGlbNotification);
	iEffect->GenCallbackL();
	
	if (!iCalledBack)
		{
		INFO_PRINTF1(_L("Callback seemingly did not happen"));
		iResult = EFail;
		}
	
	CleanupStack::PopAndDestroy(iEffect);
	
	__MM_HEAP_MARKEND;
	
	return iResult;
	}
	
