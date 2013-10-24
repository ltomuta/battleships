Battleships
===========

Battleships is a Qt Quick example game of the well-known guessing game played
by two people. This demo uses the Bluetooth connectivity module of
the Qt Mobility API. In this game you have to guess where your opponent's
battleships are on the 10 x 10 grid and sink them before the opponent
sinks your ships. The game has both single player and two-player modes. In the
two-player mode, two devices are connected over a Bluetooth connection.

This example application is hosted in Nokia Developer Projects:
- http://projects.developer.nokia.com/battleships

For more information on implementation and porting, visit the wiki pages:
- http://projects.developer.nokia.com/battleships/wiki
- http://projects.developer.nokia.com/battleships/wiki/DesignConsiderations


1. Usage
-------------------------------------------------------------------------------

Battleships uses Qt Mobility's Bluetooth connectivity API to scan and
communicate with nearby devices. The communication between devices is
implemented on top of the RFCOMM protocol.


2. Prerequisites
-------------------------------------------------------------------------------

 - Qt basics
 - Qt Quick basics


3. Project structure and implementation
-------------------------------------------------------------------------------

3.1 Folders
-----------

 |                           The root folder contains the license information
 |                           and this file (release notes).
 |
 |- bin                      Contains the compiled binaries of the application.
 |
 |- doc                      Contains documentation for the application, such as
 |                           diagrams and Gimp projects.
 |
 |- gfx                      Contains application graphics.
 |
 |- src                      Contains the project and Qt/C++ source code files.
    |
    |- icons                 Contains the application icons.
    |
    |- qml                   Contains the QML and Javascript files.
    |  |
    |  |- gfx                Contains application graphics.
    |
    |- qmlapplicationviewer  Contains the application viewer Qt/C++ source
    |                        files.
    |
    |- qtc_packaging         Contains the Harmattan (Debian) packaging files.
    |
    |- rsc                   Contains Qt resource files.


3.2 Used APIs/QML elements/Qt Quick Components
----------------------------------------------

Qt Mobility/Bluetooth Connectivity for the communication.

QML Particles element to provide particle effects.



4. Compatibility
-------------------------------------------------------------------------------

 - Symbian devices with Qt 4.7.4 or higher.
 - MeeGo 1.2 Harmattan.

Tested to work on Nokia C7-00, Nokia E7-00, Nokia N8-00, Nokia 700, Nokia N9.
Developed with Qt SDK 1.1.4.

4.1 Required capabilities
-------------------------

None. The application can be self-signed on Symbian.


4.2 Known Issues
----------------

None.


5. Building, installing, and running the application
-------------------------------------------------------------------------------

5.1 Preparations
----------------

Check that you have the latest Qt SDK installed in the development environment
and the latest Qt version on the device.

Qt Quick Components 1.0 or higher is required.

5.2 Using the Qt SDK
--------------------

You can install and run the application on the device by using the Qt SDK.
Open the project in the SDK, set up the correct target (depending on the device
platform), and click the Run button. For more details about this approach,
visit the Qt Getting Started section at Nokia Developer
(http://www.developer.nokia.com/Develop/Qt/Getting_started/).

5.3 Symbian device
------------------

Make sure your device is connected to your computer. Locate the .sis
installation file and open it with Ovi Suite. Accept all requests from Ovi
Suite and the device. Note that you can also install the application by copying
the installation file onto your device and opening it with the Symbian File
Manager application.

After the application is installed, locate the application icon from the
application menu and launch the application by tapping the icon.

5.4 Nokia N9 and Nokia N950
---------------------------

Copy the application Debian package onto the device. Locate the file with the
device and run it; this will install the application. Note that you can also
use the terminal application and install the application by typing the command
'dpkg -i <package name>.deb' on the command line. To install the application
using the terminal application, make sure you have the right privileges 
to do so (e.g. root access).

Once the application is installed, locate the application icon from the
application menu and launch the application by tapping the icon.


6. License
-------------------------------------------------------------------------------

See the license text file delivered with this project. The license file is also
available online at
http://projects.developer.nokia.com/battleships/browser/trunk/Licence.txt


7. Related documentation
-------------------------------------------------------------------------------
Qt Quick Components
- http://doc.qt.nokia.com/qt-components-symbian-1.0/index.html
- http://harmattan-dev.nokia.com/docs/library/html/qt-components/qt-components.html


8. Version history
-------------------------------------------------------------------------------

1.0 Added new graphics, improved the UX.
0.6 Added fix to Bluetooth  
0.5 Initial version, problems with Bluetooth.
