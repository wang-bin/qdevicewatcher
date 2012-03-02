/******************************************************************************
	hotplugwatcher_gui.h: gui application to watch hotplug event
	Copyright (C) 2012 Wang Bin <wbsecg1@gmail.com>
	
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
******************************************************************************/


#ifndef HOTPLUGWATCHER_GUI_H
#define HOTPLUGWATCHER_GUI_H

#include <QtGui/QWidget>

class QTextBrowser;
class QSystemTrayIcon;
class QLabel;
class QPushButton;
class QDeviceWatcher;
class HotplugWatcher_GUI : public QWidget
{
	Q_OBJECT
public:
	explicit HotplugWatcher_GUI(QWidget *parent = 0);
	~HotplugWatcher_GUI();

	void appendMessage(const QString& msg);
	
public slots:
	void toggleWatch();
	void slotDeviceAdded(const QString& dev);
	void slotDeviceRemoved(const QString& dev);
	void slotDeviceChanged(const QString& dev);

private:
	QDeviceWatcher *watcher;

	QLabel *state;
	QTextBrowser *msg_view;
	QPushButton *button;
	QSystemTrayIcon *tray;
};

#endif // HOTPLUGWATCHER_GUI_H
