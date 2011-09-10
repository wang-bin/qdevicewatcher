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


#include "qusbwatcher_p.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QDebug>

#define _WIN32_WINNT 0x0500
#include <dbt.h>

#ifndef DBT_CUSTOMEVENT
#  define DBT_CUSTOMEVENT 0x8006
#endif

#define QDRIVECONTROLLER_DEBUG 1

Q_CORE_EXPORT HINSTANCE qWinAppInst();

static inline QStringList drivesFromMask(quint32 driveBits)
{
		QStringList ret;

		char driveName[] = "A:/";
		driveBits = (driveBits & 0x3ffffff);
		while (driveBits) {
			if (driveBits & 0x1)
				ret.append(QString::fromLatin1(driveName));
			++driveName[0];
			driveBits = driveBits >> 1;
		}

		return ret;
}

LRESULT CALLBACK dw_internal_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_DEVICECHANGE) {
		DEV_BROADCAST_HDR *lpdb = (DEV_BROADCAST_HDR *)lParam;
		switch (wParam) {
		case DBT_DEVNODES_CHANGED:
#ifdef QDRIVECONTROLLER_DEBUG
			qWarning("DBT_DEVNODES_CHANGED message received, no extended info.");
#endif
			break;

		case DBT_QUERYCHANGECONFIG:
#ifdef QDRIVECONTROLLER_DEBUG
			qWarning("DBT_QUERYCHANGECONFIG message received, no extended info.");
#endif
			break;
		case DBT_CONFIGCHANGED:
#ifdef QDRIVECONTROLLER_DEBUG
			qWarning("DBT_CONFIGCHANGED message received, no extended info.");
#endif
			break;
		case DBT_CONFIGCHANGECANCELED:
#ifdef QDRIVECONTROLLER_DEBUG
			qWarning("DBT_CONFIGCHANGECANCELED message received, no extended info.");
#endif
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
				QUsbWatcherPrivate *watcher = (QUsbWatcherPrivate *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
#else
				QUsbWatcherPrivate *watcher = (QUsbWatcherPrivate *)GetWindowLong(hwnd, GWL_USERDATA);
#endif

				if (wParam == DBT_DEVICEARRIVAL) {
					foreach (const QString &drive, drives) {
#ifdef QDRIVECONTROLLER_DEBUG
						if (db_volume->dbcv_flags & DBTF_MEDIA)
							qWarning("Drive %c: Media has been arrived.", drive.at(0).toAscii());
						else if (db_volume->dbcv_flags & DBTF_NET)
							qWarning("Drive %c: Network share has been mounted.", drive.at(0).toAscii());
						else
							qWarning("Drive %c: Device has been added.", drive.at(0).toAscii());
#endif
						watcher->emitDeviceAdded(drive);
					}
				} else if (wParam == DBT_DEVICEQUERYREMOVE) {
				} else if (wParam == DBT_DEVICEQUERYREMOVEFAILED) {
				} else if (wParam == DBT_DEVICEREMOVEPENDING) {
				} else if (wParam == DBT_DEVICEREMOVECOMPLETE) {
					foreach (const QString &drive, drives) {
#ifdef QDRIVECONTROLLER_DEBUG
						if (db_volume->dbcv_flags & DBTF_MEDIA)
							qWarning("Drive %c: Media has been removed.", drive.at(0).toAscii());
						else if (db_volume->dbcv_flags & DBTF_NET)
							qWarning("Drive %c: Network share has been unmounted.", drive.at(0).toAscii());
						else
							qWarning("Drive %c: Device has been removed.", drive.at(0).toAscii());
#endif
						watcher->emitDeviceRemoved(drive);
					}
				}
			}
			break;
		case DBT_DEVICETYPESPECIFIC:
#ifdef QDRIVECONTROLLER_DEBUG
			qWarning("DBT_DEVICETYPESPECIFIC message received, may contain an extended info.");
#endif
			break;
		case DBT_CUSTOMEVENT:
#ifdef QDRIVECONTROLLER_DEBUG
			qWarning("DBT_CUSTOMEVENT message received, contains an extended info.");
#endif
			break;
		case DBT_USERDEFINED:
#ifdef QDRIVECONTROLLER_DEBUG
			qWarning("WM_DEVICECHANGE user defined message received, can not handle.");
#endif
			break;

		default:
#ifdef QDRIVECONTROLLER_DEBUG
			qWarning("WM_DEVICECHANGE message received, unhandled value %d.", wParam);
#endif
			break;
		}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

static inline QString className()
{
	return QLatin1String("QUsbWatcherWin32_Internal_Widget") + QString::number(quintptr(dw_internal_proc));
}

static inline HWND dw_create_internal_window(const void* userData)
{
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
	wc.lpszClassName = reinterpret_cast<const wchar_t *>(className().utf16());
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
		qWarning("QUsbWatcherPrivateEngine: Failed to create internal window: %d", (int)GetLastError());
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




bool QUsbWatcherPrivate::init()
{
	hwnd = dw_create_internal_window(this);

	if (!hwnd) {
		dw_destroy_internal_window(hwnd);
	}
	return hwnd;
}

QUsbWatcherPrivate::~QUsbWatcherPrivate()
{
	dw_destroy_internal_window(hwnd);
}

void QUsbWatcherPrivate::parseDeviceInfo()
{
}

void QUsbWatcherPrivate::parseLine(const QByteArray &line)
{
	Q_UNUSED(line);
}
