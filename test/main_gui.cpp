/******************************************************************************
	main_gui.cpp: gui application to watch hotplug event
    Copyright (C) 2012-2015 Wang Bin <wbsecg1@gmail.com>
	
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/

#include "hotplugwatcher_gui.h"
#include <QApplication>

static HotplugWatcher_GUI *gui = 0;

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#define qInstallMessageHandler qInstallMsgHandler
void MsgOuput(QtMsgType type, const char *msg)
{
#else
void MsgOuput(QtMsgType type, const QMessageLogContext &, const QString &msg)
{
#endif
    Q_UNUSED(type)
    if (gui)
        gui->appendMessage(msg);
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(MsgOuput);
    QApplication a(argc, argv);

    HotplugWatcher_GUI hotplug;
    gui = &hotplug;
    hotplug.show();
    qDebug("Hotplug watcher(libQDeviceWatcher test app)\nwbsecg1@gmail.com\n");
    return a.exec();
}
