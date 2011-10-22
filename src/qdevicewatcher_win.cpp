/******************************************************************************
	QDeviceWatcherPrivate: watching depends on platform
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


#include "qdevicewatcher_p.h"

#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>

#include "qdevicechangeevent.h"

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#include <dbt.h>

#ifndef DBT_CUSTOMEVENT
#  define DBT_CUSTOMEVENT 0x8006
#endif

Q_CORE_EXPORT HINSTANCE qWinAppInst();

static inline QStringList drivesFromMask(quint32 driveBits)
{
		QStringList ret;

		char driveName[] = "A:/";
		driveBits &= 0x3ffffff;
		while (driveBits) {
			if (driveBits & 0x1)
				ret.append(QString::fromLatin1(driveName));
			++driveName[0];
			driveBits >>= 1;
		}

		return ret;
}

LRESULT CALLBACK dw_internal_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_DEVICECHANGE) {
		DEV_BROADCAST_HDR *lpdb = (DEV_BROADCAST_HDR *)lParam;
		switch (wParam) {
		case DBT_DEVNODES_CHANGED:
			zDebug("DBT_DEVNODES_CHANGED message received, no extended info.");
			break;
		case DBT_QUERYCHANGECONFIG:
			zDebug("DBT_QUERYCHANGECONFIG message received, no extended info.");
			break;
		case DBT_CONFIGCHANGED:
			zDebug("DBT_CONFIGCHANGED message received, no extended info.");
			break;
		case DBT_CONFIGCHANGECANCELED:
			zDebug("DBT_CONFIGCHANGECANCELED message received, no extended info.");
			break;

		case DBT_DEVICEARRIVAL:
		case DBT_DEVICEQUERYREMOVE:
		case DBT_DEVICEQUERYREMOVEFAILED:
		case DBT_DEVICEREMOVEPENDING:
		case DBT_DEVICEREMOVECOMPLETE:
			if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME) {
				DEV_BROADCAST_VOLUME *db_volume = (DEV_BROADCAST_VOLUME *)lpdb;
				QStringList drives = drivesFromMask(db_volume->dbcv_unitmask);
#ifdef GWLP_USERDATA
				QDeviceWatcherPrivate *watcher = (QDeviceWatcherPrivate *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
#else
				QDeviceWatcherPrivate *watcher = (QDeviceWatcherPrivate *)GetWindowLong(hwnd, GWL_USERDATA);
#endif

				QDeviceChangeEvent *event = 0;
				if (wParam == DBT_DEVICEARRIVAL) {
					foreach (const QString &drive, drives) {
						if (db_volume->dbcv_flags & DBTF_MEDIA)
							zDebug("Drive %c: Media has been arrived.", drive.at(0).toAscii());
						else if (db_volume->dbcv_flags & DBTF_NET)
							zDebug("Drive %c: Network share has been mounted.", drive.at(0).toAscii());
						else
							zDebug("Drive %c: Device has been added.", drive.at(0).toAscii());
						watcher->emitDeviceAdded(drive);
						if (!watcher->event_receivers.isEmpty())
							event = new QDeviceChangeEvent(QDeviceChangeEvent::Add, drive);
					}
				} else if (wParam == DBT_DEVICEQUERYREMOVE) {
				} else if (wParam == DBT_DEVICEQUERYREMOVEFAILED) {
				} else if (wParam == DBT_DEVICEREMOVEPENDING) {
				} else if (wParam == DBT_DEVICEREMOVECOMPLETE) {
					foreach (const QString &drive, drives) {
						if (db_volume->dbcv_flags & DBTF_MEDIA)
							zDebug("Drive %c: Media has been removed.", drive.at(0).toAscii());
						else if (db_volume->dbcv_flags & DBTF_NET)
							zDebug("Drive %c: Network share has been unmounted.", drive.at(0).toAscii());
						else
							zDebug("Drive %c: Device has been removed.", drive.at(0).toAscii());
						watcher->emitDeviceRemoved(drive);
						if (!watcher->event_receivers.isEmpty())
							event = new QDeviceChangeEvent(QDeviceChangeEvent::Remove, drive);
					}
				}
				if (event != 0 && !watcher->event_receivers.isEmpty()) {
					foreach(QObject* obj, watcher->event_receivers) {
						QCoreApplication::postEvent(obj, event, Qt::HighEventPriority);
					}
				}
			}
			break;
		case DBT_DEVICETYPESPECIFIC:
			zDebug("DBT_DEVICETYPESPECIFIC message received, may contain an extended info.");
			break;
		case DBT_CUSTOMEVENT:
			zDebug("DBT_CUSTOMEVENT message received, contains an extended info.");
			break;
		case DBT_USERDEFINED:
			zDebug("WM_DEVICECHANGE user defined message received, can not handle.");
			break;
		default:
			qWarning("WM_DEVICECHANGE message received, unhandled value %d.", wParam);
			break;
		}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

static inline QString className()
{
	return QLatin1String("QDeviceWatcherPrivateWin32_Internal_Widget") + QString::number(quintptr(dw_internal_proc));
}

static inline HWND dw_create_internal_window(const void* userData)
{
	QString className = ::className();
	HINSTANCE hi = qWinAppInst();

	WNDCLASS wc;
	wc.style = 0;
	wc.lpfnWndProc = dw_internal_proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hi;
	wc.hIcon = 0;
	wc.hCursor = 0;
	wc.hbrBackground = 0;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = reinterpret_cast<const wchar_t *>(className.utf16());
	RegisterClass(&wc);

	HWND hwnd = CreateWindow(wc.lpszClassName,       // classname
							 wc.lpszClassName,       // window name
							 0,                      // style
							 0, 0, 0, 0,             // geometry
							 0,                      // parent
							 0,                      // menu handle
							 hi,                     // application
							 0);                     // windows creation data.
	if (!hwnd) {
		qWarning("QDeviceWatcherPrivate: Failed to create internal window: %d", (int)GetLastError());
	} else if (userData) {
#ifdef GWLP_USERDATA
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)userData);
#else
		SetWindowLong(hwnd, GWL_USERDATA, (LONG)userData);
#endif
	}

	return hwnd;
}

static inline void dw_destroy_internal_window(HWND hwnd)
{
	if (hwnd)
		DestroyWindow(hwnd);

	UnregisterClass(reinterpret_cast<const wchar_t *>(className().utf16()), qWinAppInst());
}



QDeviceWatcherPrivate::~QDeviceWatcherPrivate()
{
	stop();
}

bool QDeviceWatcherPrivate::start()
{
	init();
	hwnd = dw_create_internal_window(this);

	if (!hwnd) {
		dw_destroy_internal_window(hwnd);
	}
	return hwnd;
}

bool QDeviceWatcherPrivate::stop()
{
	dw_destroy_internal_window(hwnd);
	return true;
}

bool QDeviceWatcherPrivate::init()
{
	return true;
}

void QDeviceWatcherPrivate::parseDeviceInfo()
{
}
