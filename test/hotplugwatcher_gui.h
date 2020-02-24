/******************************************************************************
	hotplugwatcher_gui.h: gui application to watch hotplug event
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

#ifndef HOTPLUGWATCHER_GUI_H
#define HOTPLUGWATCHER_GUI_H

#include <QWidget>

class QTextBrowser;
class QSystemTrayIcon;
class QLabel;
class QPushButton;
class QToolButton;
class QDeviceWatcher;
class HotplugWatcher_GUI : public QWidget
{
    Q_OBJECT
public:
    explicit HotplugWatcher_GUI(QWidget *parent = 0);
    ~HotplugWatcher_GUI();

    void appendMessage(const QString &msg);

public slots:
    void toggleWatch();
    void showDetail(bool show);
    void slotDeviceAdded(const QString &dev);
    void slotDeviceRemoved(const QString &dev);
    void slotDeviceChanged(const QString &dev);

private:
    QDeviceWatcher *watcher;

    QLabel *state;
    QTextBrowser *msg_view;
    QPushButton *button;
    QToolButton *detail_button;
    QSystemTrayIcon *tray;
};

#endif // HOTPLUGWATCHER_GUI_H
