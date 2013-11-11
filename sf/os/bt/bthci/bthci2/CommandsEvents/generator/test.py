# Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#


def makeCreateTestStepImpl(aEntries):
    impl_str = '\t'

    for e in aEntries:
        impl_str += 'if(aStepName == K' + makeTestStepName(e) + ')\n\t\t{\t\treturn new C' + makeTestStepName(e) + ';\n\t\t}\n\telse '

    impl_str += '\n\t\t{\n\t\treturn NULL;\n\t\t}'
    return impl_str

def writeTestServer(aEntries, aHeaderTemplate, aSourceTemplate):
    test = {'CREATE_TEST_STEP_IMPL' : makeCreateTestStepImpl(aEntries)}

def writeTests(aEntries, 
