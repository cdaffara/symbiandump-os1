WIN Tunnel serial driver for EKA2


Usage
-----

Compile this module for either WINS or WINSCW
bldmake bldfiles
abld build wins
or
bldmake bldfiles
abld build winscw

Set WIN_TUNNEL emulator property by either
A) Add the following to epoc.ini

WIN_TUNNEL=[Commport],[Hostname],[TCPport]

Commport is Emulator Comm port to reroute to tunnel machine. (Must be less than 10)
Hostname is the tunnel machine. (Either ip address or DNS name)
TCPport  is the port on the tunnel machine to use.

E.G.
WIN_TUNNEL=9,10.35.2.53,110

To use more than one serial port the syntax is as follows
WIN_TUNNEL=9,10.35.2.53,110;8,10.35.2.59,110



b) Start emulator passing in WIN_TUNNEL setting

E.G.
epoc -dWIN_TUNNEL=9,10.35.2.53,110 --