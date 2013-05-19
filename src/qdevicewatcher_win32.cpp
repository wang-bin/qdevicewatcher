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

//TODO: If wince works, use Q_OS_WIN
#ifdef Q_OS_WIN32
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>

#include "qdevicechangeevent.h"

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#include <dbt.h>

#ifndef DBT_CUSTOMEVENT
#  define DBT_CUSTOMEVENT 0x8006
#endif

#define CONFIG_NOTIFICATION 0

/*
 *http://www.codeproject.com/Articles/14500/Detecting-Hardware-Insertion-and-or-Removal
 *http://blog.csdn.net/sxbluebird/article/details/2445145
*/
#if CONFIG_NOTIFICATION
static HDEVNOTIFY hDevNotify = 0;
#define HID_CLASSGUID {0x4d1e55b2, 0xf16f, 0x11cf,{ 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30}}
static const GUID WceusbshGUID = { 0x25dbce51, 0x6c8f, 0x4a72, 0x8a,0x6d,0xb5,0x4c,0x2b,0x4f,0xc8,0x35 };
static const GUID GUID_DEVINTERFACE_USBSTOR = { 0xA5DCBF10L, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } };
static const GUID InterfaceClassGuid = GUID_DEVINTERFACE_USBSTOR;//(GUID)HID_CLASSGUID; //GUID_DEVINTERFACE_USBSTOR
#endif //CONFIG_NOTIFICATION

Q_CORE_EXPORT HINSTANCE qWinAppInst();

static inline QStringList drivesFromMask(quint32 driveBits) //driveBits ->unitmask
{
		QStringList ret;
#if 1  //a disk with multiple partitions removed
		char driveName[] = "A:/";
		driveBits &= 0x3ffffff;
		while (driveBits) {
			if (driveBits & 0x1)
				ret.append(QString::fromLatin1(driveName));
			++driveName[0];
			driveBits >>= 1;
		}
		return ret;
#else
		char i = 0;
		for (; i<26; ++i) {
			if (driveBits & 0x1)
				break;
			driveBits >>= 1;
		}
		QChar c(i + 'A');
		return ret << QString(c) + ":";
#endif
}

void static UpdateDevice(PDEV_BROADCAST_DEVICEINTERFACE pDevInf, WPARAM wParam)
{
/*
	// dbcc_name:
	// \\?\USB#Vid_04e8&Pid_503b#0002F9A9828E0F06#{a5dcbf10-6530-11d2-901f-00c04fb951ed}
	// convert to
	// USB\Vid_04e8&Pid_503b\0002F9A9828E0F06
	ASSERT(lstrlen(pDevInf->dbcc_name) > 4);
	wchar_t *szDevId = pDevInf->dbcc_name+4;
	int idx = szDevId.ReverseFind(_T('#'));
	ASSERT( -1 != idx );
	szDevId.Truncate(idx);
	szDevId.Replace(_T('#'), _T('\\'));
	szDevId.MakeUpper();

	CString szClass;
	idx = szDevId.Find(_T('\\'));
	ASSERT(-1 != idx );
	szClass = szDevId.Left(idx);

	// if we are adding device, we only need present devices
	// otherwise, we need all devices
	DWORD dwFlag = DBT_DEVICEARRIVAL != wParam
		? DIGCF_ALLCLASSES : (DIGCF_ALLCLASSES | DIGCF_PRESENT);
	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, szClass, NULL, dwFlag);
	if( INVALID_HANDLE_VALUE == hDevInfo )
	{
		zDebug("SetupDiGetClassDevs(): " + _com_error(GetLastError()).ErrorMessage());
		return;
	}

	SP_DEVINFO_DATA* pspDevInfoData = (SP_DEVINFO_DATA*)HeapAlloc(GetProcessHeap(), 0, sizeof(SP_DEVINFO_DATA));
	pspDevInfoData->cbSize = sizeof(SP_DEVINFO_DATA);
	for(int i=0; SetupDiEnumDeviceInfo(hDevInfo,i,pspDevInfoData); ++i) {
		DWORD DataT ;
		DWORD nSize=0 ;
		TCHAR buf[MAX_PATH];
		if (!SetupDiGetDeviceInstanceId(hDevInfo, pspDevInfoData, buf, sizeof(buf), &nSize)) {
			zDebug("SetupDiGetDeviceInstanceId(): " + _com_error(GetLastError()).ErrorMessage());
			break;
		}

		if (szDevId == buf) {
			// device found
			if (SetupDiGetDeviceRegistryProperty(hDevInfo, pspDevInfoData,
				SPDRP_FRIENDLYNAME, &DataT, (PBYTE)buf, sizeof(buf), &nSize)) {
				// do nothing
			} else if (SetupDiGetDeviceRegistryProperty(hDevInfo, pspDevInfoData,
				SPDRP_DEVICEDESC, &DataT, (PBYTE)buf, sizeof(buf), &nSize)) {
				// do nothing
			} else {
				lstrcpy(buf, _T("Unknown"));
			}
			// update UI
			break;
		}
	}

	if (pspDevInfoData)
		HeapFree(GetProcessHeap(), 0, pspDevInfoData);
	SetupDiDestroyDeviceInfoList(hDevInfo);
*/
}

/*
 http://msdn.microsoft.com/en-us/library/windows/desktop/aa363246%28v=vs.85%29.aspx
typedef struct _DEV_BROADCAST_HDR {
  DWORD dbch_size;
  DWORD dbch_devicetype;
  DWORD dbch_reserved;
} DEV_BROADCAST_HDR, *PDEV_BROADCAST_HDR;

dbch_size:
	The size of this structure, in bytes.
	If this is a user-defined event, this member must be the size of this header, plus the size of the variable-length data in the _DEV_BROADCAST_USERDEFINED structure.
dbch_devicetype:
	The device type, which determines the event-specific information that follows the first three members. This member can be one of the following values.
	Value	Meaning
	DBT_DEVTYP_DEVICEINTERFACE 0x00000005
	Class of devices. This structure is a DEV_BROADCAST_DEVICEINTERFACE structure.
	DBT_DEVTYP_HANDLE 0x00000006
	File system handle. This structure is a DEV_BROADCAST_HANDLE structure.
	DBT_DEVTYP_OEM 0x00000000
	OEM- or IHV-defined device type. This structure is a DEV_BROADCAST_OEM structure.
	DBT_DEVTYP_PORT 0x00000003
	Port device (serial or parallel). This structure is a DEV_BROADCAST_PORT structure.
	DBT_DEVTYP_VOLUME 0x00000002
	Logical volume. This structure is a DEV_BROADCAST_VOLUME structure.
dbch_reserved:
	Reserved; do not use.
WM_DEVICECHANGE限制:
1 只有顶层窗体的程序才能收到这个消息
2 仅仅串口、磁盘发生改变，才对每个程序广播这个消息
*/
LRESULT CALLBACK dw_internal_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_DEVICECHANGE) {
		DEV_BROADCAST_HDR *lpdb = (DEV_BROADCAST_HDR *)lParam;
		zDebug("Device type address: %#x", lpdb);
		if (lpdb) {
			if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME) {
				zDebug("DBT_DEVTYP_VOLUME");
			} else if (lpdb->dbch_devicetype == DBT_DEVTYP_PORT) {
				zDebug("DBT_DEVTYP_PORT");
			} else if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
				zDebug("DBT_DEVTYP_DEVICEINTERFACE");
			} else if (lpdb->dbch_devicetype == DBT_DEVTYP_OEM) {
				zDebug("DBT_DEVTYP_OEM");
			} else {
				zDebug("Unknow device type");
			}
		}

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
				QList<QDeviceChangeEvent *> events;
				QString action_str("add");
				QDeviceChangeEvent::Action action = QDeviceChangeEvent::Add;
				if (wParam == DBT_DEVICEARRIVAL) {
				} else if (wParam == DBT_DEVICEQUERYREMOVE) {
				} else if (wParam == DBT_DEVICEQUERYREMOVEFAILED) {
				} else if (wParam == DBT_DEVICEREMOVEPENDING) {
				} else if (wParam == DBT_DEVICEREMOVECOMPLETE) {
					action_str = "remove";
					action = QDeviceChangeEvent::Remove;
				}
				foreach (const QString &drive, drives) {
					if (db_volume->dbcv_flags & DBTF_MEDIA)
                        zDebug("Drive %c: Media has been removed.", drive.at(0).toLatin1());
					else if (db_volume->dbcv_flags & DBTF_NET)
                        zDebug("Drive %c: Network share has been removed.", drive.at(0).toLatin1());
					else
                        zDebug("Drive %c: Device has been removed.", drive.at(0).toLatin1());
					watcher->emitDeviceAction(drive, action_str);
					if (!watcher->event_receivers.isEmpty())
						events.append(new QDeviceChangeEvent(action, drive));
				}
				if (!events.isEmpty() && !watcher->event_receivers.isEmpty()) {
					foreach(QObject* obj, watcher->event_receivers) {
						foreach(QDeviceChangeEvent* event, events) {
							QCoreApplication::postEvent(obj, event, Qt::HighEventPriority);
						}
					}
				}
			} else if (lpdb->dbch_devicetype == DBT_DEVTYP_PORT) {
				zDebug("DBT_DEVTYP_PORT");
				PDEV_BROADCAST_PORT pDevPort = (PDEV_BROADCAST_PORT)lpdb;
			} else if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
				//RegisterDeviceNotification()
				zDebug("DBT_DEVTYP_DEVICEINTERFACE");
				PDEV_BROADCAST_DEVICEINTERFACE pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)lpdb;
				UpdateDevice(pDevInf, wParam);
			} else if (lpdb->dbch_devicetype == DBT_DEVTYP_OEM) {
				zDebug("DBT_DEVTYP_OEM");
				DEV_BROADCAST_OEM *pDevOem = (DEV_BROADCAST_OEM*)lpdb;
			} else if (lpdb->dbch_devicetype == DBT_DEVTYP_HANDLE) {
				zDebug("DBT_DEVTYP_HANDLE");
				PDEV_BROADCAST_HANDLE pDevHnd = (PDEV_BROADCAST_HANDLE)lpdb;
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

	HWND hwnd = CreateWindow(wc.lpszClassName,	   // classname
							 wc.lpszClassName,	   // window name
							 0,					  // style
							 0, 0, 0, 0,			 // geometry
							 0,					  // parent
							 0,					  // menu handle
							 hi,					 // application
							 0);					 // windows creation data.
	if (!hwnd) {
		qWarning("QDeviceWatcherPrivate: Failed to create internal window: %d", (int)GetLastError());
#if CONFIG_NOTIFICATION
	} else {
		DEV_BROADCAST_DEVICEINTERFACE NotificationFilter ;
		ZeroMemory(&NotificationFilter, sizeof(NotificationFilter)) ;
		NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
		NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		NotificationFilter.dbcc_classguid = InterfaceClassGuid;
		//3rd param with DEVICE_NOTIFY_ALL_INTERFACE_CLASSES and dbcc_classguid will be ignored
		//http://msdn.microsoft.com/en-us/library/windows/desktop/aa363431(v=vs.85).aspx
		hDevNotify = RegisterDeviceNotification(hwnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
		if (!hDevNotify) {
			zDebug("");
		}
	}
#else
	} else if (userData) {
#ifdef GWLP_USERDATA
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)userData);
#else
		SetWindowLong(hwnd, GWL_USERDATA, (LONG)userData);
#endif
	}
#endif //CONFIG_NOTIFICATION
	return hwnd;
}

static inline void dw_destroy_internal_window(HWND hwnd)
{
	if (hwnd)
		DestroyWindow(hwnd);

#if CONFIG_NOTIFICATION
	UnregisterDeviceNotification(hDevNotify);
#endif
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

#endif //Q_OS_WIN32
