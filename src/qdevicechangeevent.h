/******************************************************************************
	QDeviceChangeEvent: Device change event
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

#ifndef QDEVICECHANGEEVENT_H
#define QDEVICECHANGEEVENT_H

#include <QtCore/QEvent>
#include <QtCore/QString>
#include "qdevicewatcher_global.h"

class Q_DLL_EXPORT QDeviceChangeEvent : public QEvent
{
	Q_ENUMS(Action)
public:
	enum Action { Add, Remove, Change};
	//static const Type EventType; //VC link error

	explicit QDeviceChangeEvent(Action action, const QString& device);

	Action action() const {return m_action;}
	QString device() const {return m_device;}
	static Type registeredType()
	{
		static Type EventType = static_cast<Type>(registerEventType());
		return EventType;
	}

private:
	Action m_action;
	QString m_device;
};

#endif // QDEVICECHANGEEVENT_H
