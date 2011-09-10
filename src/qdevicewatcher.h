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

#ifndef QUSBWATCHER_H
#define QUSBWATCHER_H

#include <QtCore/QObject>


class QDeviceWatcherPrivate;

class QDeviceWatcher : public QObject
{
    Q_OBJECT
	Q_DECLARE_PRIVATE(QDeviceWatcher)
public:
	explicit QDeviceWatcher(QObject *parent = 0);
	~QDeviceWatcher();

signals:
	void deviceAdded(const QString& dev);
	void deviceChanged(const QString& dev); //when umounting the device
	void deviceRemoved(const QString& dev);

public slots:
	void slotDeviceAdded(const QString& dev) { qDebug("%s: add %s", __FUNCTION__, qPrintable(dev));}
	void slotDeviceRemoved(const QString& dev) { qDebug("%s: remove %s", __FUNCTION__, qPrintable(dev));}
protected:
	QDeviceWatcherPrivate *d_ptr;
};

#endif // QUSBWATCHER_H
