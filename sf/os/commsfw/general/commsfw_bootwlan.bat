md \testframework
md \testframework\ncpkickstart\
copy z:\testdata\configs\commsfw_startedservices.xml c:\testframework\ncpkickstart\startedservices.xml
attrib c:\testframework\ncpkickstart\startedservices.xml -r
md \private
md \private\A0000C67
REM NcpKickStart.exe copies this CRE file to c:\private\10202be9\persists\cccccc00.cre
copy z:\testdata\configs\WLAN_template_cccccc00.cre c:\private\A0000C67\cccccc00.cre
attrib c:\private\A0000C67\cccccc00.cre -r
NcpKickStart.exe
z:\sys\bin\htiframework.exe