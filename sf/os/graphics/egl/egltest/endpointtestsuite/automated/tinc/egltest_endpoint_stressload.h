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
 @test
 @internalComponent - Internal Symbian test code
*/

#include "egltest_endpoint_stress_engine.h"
#include "egltest_endpoint_engine.h"

_LIT(KEGL_EndpointStressLoad, "EGL_EndpointStressLoad");


NONSHARABLE_CLASS(CEglTest_TestStep_StressLoad): public CStressTestEngine
    {
public:
    static CEglTest_TestStep_StressLoad *New();

protected:
    TThreadFunction GetThreadFunction(TInt aThreadNumber);
private:
    CEglTest_TestStep_StressLoad();
    static TInt LoadGpuProcessor(TAny *aSelf);
    void LoadGpuProcessorL();
    static TInt LoadGpuMemory(TAny *aSelf);
    void LoadGpuMemoryL();

private:
    EGLDisplay iDisplay;
    };
