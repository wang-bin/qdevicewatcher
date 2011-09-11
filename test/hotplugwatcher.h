/******************************************************************************
	Name: description
	Copyright (C) 2011 Wang Bin <wbsecg1@gmail.com>

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

#ifndef HOTPLUGWATCHER_H
#define HOTPLUGWATCHER_H

#include <QtCore/QObject>
#include "qdevicewatcher.h"

#ifndef __GNUC__
#define __PRETTY_FUNCTION__  __FUNCTION__
#endif

class HotplugWatcher : public QObject
{
	Q_OBJECT
public:
	HotplugWatcher(QObject *parent = 0):QObject(parent) {
		watcher = new QDeviceWatcher(this);
		connect(watcher, SIGNAL(deviceAdded(QString)), SLOT(slotDeviceAdded(QString)));
		connect(watcher, SIGNAL(deviceRemoved(QString)), SLOT(slotDeviceRemoved(QString)));
	}

public slots:
	void slotDeviceAdded(const QString& dev) { qDebug("%s: add %s", __PRETTY_FUNCTION__, qPrintable(dev));}
	void slotDeviceRemoved(const QString& dev) { qDebug("%s: remove %s", __PRETTY_FUNCTION__, qPrintable(dev));}

private:
	QDeviceWatcher *watcher;
};

#endif // HOTPLUGWATCHER_H
