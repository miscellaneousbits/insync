#pragma once

#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers
#define WINVER 0x0603       // We don't support Windows XP
#define _WIN32_WINNT WINVER
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS  // some CString constructors will be explicit
#define _AFX_ALL_WARNINGS

#include <stdlib.h>
#include <time.h>

// MFC includes
#include <afxcmn.h>
#include <afxdlgs.h>
#include <afxdtctl.h>
#include <afxeditbrowsectrl.h>
#include <afxcontrolbars.h>
#include <afxvisualmanager.h>
#include <afxocc.h>

// Windows includes (must come after MFC includes)
#include <atldef.h>
#include <atlbase.h>
#include <atlutil.h>

#include <Aclapi.h>
#include <intrin.h>
#include <lm.h>
#include <Mstask.h>
#include <propkey.h>
#include <propvarutil.h>
#include <regex>
#include <Softpub.h>
#include <versionhelpers.h>
#include <Vss.h>
#include <VsWriter.h>
#include <VsBackup.h>
#include <ShellScalingAPI.h>
#include <PowrProf.h>

// STL support
#include <atomic>
#include <list>
#include <set>
#include <map>
#include <memory>

using namespace std;

#define _WIN32_DCOM

using namespace std;
#include <Wbemidl.h>

// The resource ids
#include "Resource.h"

// Utilities
#include "CmdLnInfo.h"
#include "ComboBox.h"
#include "DataTypes.h"
#include "DirListBox.h"
#include "FilePath.h"
#include "helpcontext.h"
#include "InsyncButton.h"
#include "InSyncCheckbox.h"
#include "InsyncDlg.h"
#include "InsyncListBox.h"
#include "Nodes.h"
#include "ScopedLock.h"
#include "Worker.h"
#include "Run.h"
#include "ToolTipCtrl.h"
#include "Util.h"
#include "Versioninfo.h"
#include "VssHelper.h"
#include "XML.h"
#include "XMLOptions.h"

// Dialogs
#include "ExcludedDirectoriesDlg.h"
#include "JobDialog.h"
#include "MainDlg.h"

// The application
#include "InSync.h"

// Finally the last MFC
#include <afxwin.h>

// Link libraries
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Mpr.lib")
#pragma comment(lib, "mstask.lib")
#pragma comment(lib, "VssApi.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "PowrProf.lib")
