// DBProcessDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DBProcess.h"
#include "DBProcessDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CDBProcessDlg 对话框




CDBProcessDlg::CDBProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDBProcessDlg::IDD, pParent)
    , m_hExitEvent(NULL)
    , m_hWorkThread(NULL)
    , m_bManual(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDBProcessDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_LOG, m_LogList);
}

BEGIN_MESSAGE_MAP(CDBProcessDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDOK, &CDBProcessDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CDBProcessDlg::OnBnClickedCancel)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BTN_TEST, &CDBProcessDlg::OnBnClickedBtnTest)
END_MESSAGE_MAP()


// CDBProcessDlg 消息处理程序

BOOL CDBProcessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	Init ();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDBProcessDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDBProcessDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDBProcessDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDBProcessDlg::OnBnClickedOk()
{

}

void CDBProcessDlg::OnBnClickedCancel()
{
    if(NULL != m_hExitEvent)
    {
        SetEvent (m_hExitEvent);
    }
    WaitForSingleObject(m_hWorkThread, INFINITE);

    CloseHandle (m_hWorkThread);
    CloseHandle (m_hExitEvent);
    OnCancel();
}

void CDBProcessDlg::GetConfig()
{
    m_strConfigPath = GetConfigPath ();
    char cTmp[256] = {0};

    m_iDBEnable = GetPrivateProfileInt("CONFIG", "Enable", 0, m_strConfigPath);

    GetPrivateProfileString("CONFIG", "DBServerIP", "", cTmp, 256, m_strConfigPath);
    m_strDBServerIP = cTmp;

    GetPrivateProfileString("CONFIG", "DBName", "", cTmp, 256, m_strConfigPath);
    m_strDBName = cTmp;

    GetPrivateProfileString("CONFIG", "DBUser", "", cTmp, 256, m_strConfigPath);
    m_strDBUser = cTmp;

    GetPrivateProfileString("CONFIG", "DBPwd", "", cTmp, 256, m_strConfigPath);
    m_strDBPwd = cTmp;
    
    ShowLog ("数据库信息:%s %s", m_strDBServerIP, m_strDBName);
}

CString CDBProcessDlg::GetConfigPath()
{
    //获取配置文件路径
    CString strFilePath;
    TCHAR   AppFullPath[_MAX_PATH];   
    GetModuleFileName(NULL,AppFullPath,_MAX_PATH);   
    CString   strAppPath;   
    strAppPath = AppFullPath;
    int   iAppPosition;   
    iAppPosition=strAppPath.ReverseFind('\\');   
    strFilePath = strAppPath.Mid(0,iAppPosition+1); 
    CString strFileName = _T("Config.ini");
    return strFilePath + strFileName; 
}

void CDBProcessDlg::GetVersionInfo(void)
{
    CString strFilePath;
    TCHAR AppFullPath[_MAX_PATH] = {0};   
    GetModuleFileName(NULL, AppFullPath, _MAX_PATH);   
    CString strAppPath = AppFullPath;
    int iAppPosition = strAppPath.ReverseFind('\\');   
    strFilePath = strAppPath.Mid(0, iAppPosition+1); 

    CString strTemp = _T("");
    CTime tFileTime  = time(NULL);
    CString   strFile = _T("");
    CFileFind cFilefind;
    strFile.Format("%sDBProcess.exe", strFilePath);

    if(cFilefind.FindFile(strFile))
    {
        cFilefind.FindNextFile();
        cFilefind.GetLastWriteTime(tFileTime);  
        strTemp.Format(_T("数据处理服务器 Build %04d%02d%02d"), tFileTime.GetYear(),tFileTime.GetMonth(),tFileTime.GetDay());
        SetWindowText(strTemp);
    }else
    {
        SetWindowText("数据处理服务器 Build20120301");
    }
    cFilefind.Close();
}

void CDBProcessDlg::Init()
{
    GetVersionInfo ();
    m_LogList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
    m_LogList.InsertColumn(0, TEXT("消息"), LVCFMT_CENTER, 510);
    SetTimer(1, 500, NULL);

    GetConfig ();
    
    m_hExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    ResetEvent (m_hExitEvent);
    m_hWorkThread = CreateThread(NULL, 0, AutoWorkThread , this, 0, NULL);
    if(NULL == m_hWorkThread)
    {
        ShowLog("服务器启动失败，创建主工作线程失败，请重新启动");
    }
    else
    {
        ShowLog("服务器启动成功");
    }
    
    LOG(LEVEL_OUTPUT, "程序启动成功");
}

void CDBProcessDlg::ShowLog(LPCTSTR ptzFormat, ...)
{
    TCHAR tzText[1024] = "";
    va_list vlArgs;
    va_start(vlArgs, ptzFormat);
    wvsprintf(tzText, ptzFormat, vlArgs);
    va_end(vlArgs);

    CTime  nowTime = CTime::GetCurrentTime();
    CString strTmp = nowTime.Format("%Y-%m-%d %H:%M:%S");
    strTmp.AppendFormat(": %s", tzText);

    m_LogVectorLock.Lock();
    m_LogVector.push_back(strTmp);
    m_LogVectorLock.Unlock();
}

void CDBProcessDlg::OnTimer(UINT_PTR nIDEvent)
{
    int iItemCount = 0;
    m_LogVectorLock.Lock();
    for (UINT i = 0; i < m_LogVector.size(); i++)
    {
        iItemCount = m_LogList.GetItemCount();
        //日志输出条数为5000条时清空
        if (iItemCount > 5000)
        {
            m_LogList.DeleteAllItems();
            iItemCount = 0;
        }
        m_LogList.InsertItem(iItemCount, m_LogVector[i]);	
        //自动滚动
        m_LogList.EnsureVisible(iItemCount, TRUE);
    }
    m_LogVector.clear();
    m_LogVectorLock.Unlock();

    CDialog::OnTimer(nIDEvent);
}

DWORD WINAPI CDBProcessDlg::AutoWorkThread(LPVOID lParam)
{
    CDBProcessDlg* pThis = (CDBProcessDlg*)lParam;

    pThis->AutoWorkFunc ();

    return 0;
}

void CDBProcessDlg::AutoWorkFunc()
{
    LOG(LEVEL_OUTPUT, "工作线程启动----------");
    for (;;)
    {
        GetCourseInfo ();

        if(WAIT_OBJECT_0 == WaitForSingleObject (m_hExitEvent, CHECK_TIME))
        {
            break;
        }
    }

    LOG(LEVEL_OUTPUT, "工作线程退出----------");
}

bool CDBProcessDlg::GetCourseInfo(int iWorkMode)
{
    CAdoConnection adoConn;
    vector<CourseInfo> vecCourseInfo;
    int iTime = 0;
    CString strSQL;

    if(!adoConn.ConnectSQLServer (m_strDBServerIP, m_strDBName, m_strDBUser, m_strDBPwd))
    {
        ShowLog ("连接数据库失败。 IP:%s", m_strDBServerIP);
        LOG(LEVEL_ERROR, "连接数据库失败。 IP:%s", m_strDBServerIP);
        return false;
    }
    
    if(0 == iWorkMode)
    {
        strSQL = "SELECT * FROM Qz_Course c where c.IsPay = 0 and c.[Time] <= DATEADD(HH, -1, GETDATE()) and c.[Time] > DATEADD(HH, -2, GETDATE())";
        LOG(LEVEL_INFO, "%s自动计算时间", __FUNCTION__);
    }
    else if(1 == iWorkMode)
    {
        strSQL = "SELECT * FROM Qz_Course c where c.IsPay = 0";
        LOG(LEVEL_INFO, "%s手动计算时间", __FUNCTION__);
    }
    
    //CString strSQL = "SELECT * FROM Qz_Course c where c.IsPay = 0 and c.ID = 1";

    CAdoRecordSet adoRecordSet(&adoConn);

    if(!adoRecordSet.Open (strSQL, adCmdText, adOpenDynamic, adLockBatchOptimistic))
    {
        LOG(LEVEL_ERROR, "查询课程数据失败。SQL:", strSQL);
        return false;
    }
    while(!adoRecordSet.IsEOF ())
    {
        CourseInfo info;
        adoRecordSet.GetCollect ("TeacherId", info.iTeacherId);
        adoRecordSet.GetCollect ("ID", info.strId);
        adoRecordSet.GetCollect ("StudentId", info.strStuId);
        adoRecordSet.GetCollect ("ClassMode", info.iMode);
        adoRecordSet.GetCollect ("ClassId", info.iClassId);
        adoRecordSet.GetCollect ("CourseId", info.strCourseId);

        vecCourseInfo.push_back (info);
        LOG(LEVEL_INFO, "读取待计算的课程: ID:%d CourseId:%s", info.strId, info.strCourseId);
        adoRecordSet.MoveNext ();
    }

    ResolveStuId(vecCourseInfo);
    
    for (int i = 0; i < vecCourseInfo.size (); i++)
    {
        if(9 == vecCourseInfo[i].iMode)
        {
            int iChiefId = 0;
            if(GetChiefId (adoConn, iChiefId, vecCourseInfo[i].iClassId))
            {
                vecCourseInfo[i].vecStuId.clear ();
                vecCourseInfo[i].vecStuId.push_back (iChiefId);
            }
        }

        GetOnlineTime (adoConn, vecCourseInfo[i]);

        strSQL.Format ("UPDATE Qz_Course SET SonlineTime = '%s', TonlineTime = %d, "
            "CommonTime = '%s' WHERE ID = %s", GetStudentOnlineTime (vecCourseInfo[i]), 
            vecCourseInfo[i].iTeacherTime, GetCommonOnlineTime (vecCourseInfo[i]), 
            vecCourseInfo[i].strId);

        LOG(LEVEL_INFO, "更新课程时间, SQL:%s", strSQL);
        adoConn.Execute (strSQL);
        ShowLog ("计算课程时间成功，ID:%s, 课程编号:%s", vecCourseInfo[i].strId, 
            vecCourseInfo[i].strCourseId);
    }

    //for (int i = 0; i < vecCourseInfo.size (); i++)
    //{
    //    strSQL.Format ("UPDATE Qz_Course SET SonlineTime = '%s', TonlineTime = %d, "
    //        "CommonTime = '%s' WHERE ID = %s", GetStudentOnlineTime (vecCourseInfo[i]), 
    //        vecCourseInfo[i].iTeacherTime, GetCommonOnlineTime (vecCourseInfo[i]), 
    //        vecCourseInfo[i].strId);

    //    LOG(LEVEL_INFO, "更新课程时间, SQL:%s", strSQL);
    //    adoConn.Execute (strSQL);
    //    ShowLog ("计算课程时间成功，ID:%s, 课程编号:%s", vecCourseInfo[i].strId, 
    //        vecCourseInfo[i].strCourseId);
    //}
    
    return true;
}


bool CDBProcessDlg::GetOnlineTime(CAdoConnection& adoConn, CourseInfo& struInfo)
{
    int iTime = 0;

    struInfo.iTeacherTime = GetUserOnlineTime (adoConn, struInfo.strId, struInfo.iTeacherId);

    for (size_t i = 0; i < struInfo.vecStuId.size (); i++)
    {
        iTime = GetUserOnlineTime (adoConn, struInfo.strId, struInfo.vecStuId[i]);
        struInfo.vecStuTime.push_back (iTime);
    }

    GetCommonTime (adoConn, struInfo);
    
    return true;
}

int CDBProcessDlg::GetUserOnlineTime(CAdoConnection& adoConn, CString strCourseId, int iUserId)
{
    int iTicks = GetTickCount ();
    int iTime = 0;
    CAdoRecordSet adoRecordSet(&adoConn);
    CString strSQL = "";
    int iType = 0;
    int iOnlineCount = 0;
    int iOfflineCount = 0;
    COleDateTime tBaseTime(2012, 1, 1, 0, 0, 0);
    COleDateTime tOnline;
    COleDateTime tOffLine;
    COleDateTimeSpan tOnlineSpan;
    COleDateTimeSpan tOffLineSpan;


    strSQL.Format ("SELECT * FROM Qz_CourseLog where CourseId = %s and UserId = %d ORDER BY ID", 
        strCourseId, iUserId);
    if(!adoRecordSet.Open (strSQL))
    {
        LOG(LEVEL_ERROR, "%s查询用户上下线数据失败。SQL: %s", __FUNCTION__, strSQL);
        return false;
    }

    while (!adoRecordSet.IsEOF ())
    {
        //寻找上线
        adoRecordSet.GetCollect ("LType", iType);
        if(2 == iType)
        {
            adoRecordSet.MoveNext();
            continue;
        }
        adoRecordSet.GetCollect ("Time", tOnline);
        tOnlineSpan = tOnline - tBaseTime;
        ////////////////////////////////////////////////
        
        ///////寻找与之对应的下线//////////////
        adoRecordSet.MoveNext();
        if(adoRecordSet.IsEOF ())   //已经到尾
        {
            break;
        }
        adoRecordSet.GetCollect ("LType", iType);
        if(1 == iType)  //如果为上线，放弃上一条上线数据
        {
            continue;
        }
        ///找到对应下线
        adoRecordSet.GetCollect ("Time", tOffLine);
        tOffLineSpan = tOffLine - tBaseTime;
        adoRecordSet.MoveNext ();

        iOnlineCount += tOnlineSpan.GetTotalMinutes ();
        iOfflineCount += tOffLineSpan.GetTotalMinutes ();
    }

    iTime = iOfflineCount - iOnlineCount;

    iTicks = GetTickCount () - iTicks;
#ifdef _DEBUG
    LOG (LEVEL_INFO, "TEST---%s用时:%dms", __FUNCTION__, iTicks);
#endif

    return iTime;
}

void CDBProcessDlg::GetCommonTime(CAdoConnection& adoConn, CourseInfo& struInfo)
{
    int iTicks = GetTickCount ();
    int iType = 0;
    CString strSQL = "";
    CAdoRecordSet adoRecordSet(&adoConn);
    map<int, int> mapTeacherTime;
    map<int, int>::iterator iter;
    COleDateTime tBaseTime(2012, 1, 1, 0, 0, 0);
    COleDateTime tOnline;
    COleDateTime tOffLine;
    COleDateTimeSpan tSpan;
    COleDateTimeSpan tSpan2;
    int iCount = 0;

    ///构造老师数据map
    strSQL.Format ("SELECT * FROM Qz_CourseLog where CourseId = %s and UserId = %d ORDER BY ID", 
        struInfo.strId, struInfo.iTeacherId);
    if(!adoRecordSet.Open (strSQL))
    {
        LOG(LEVEL_ERROR, "%s查询老师上下线数据失败。SQL: %s", __FUNCTION__, strSQL);
        return;
    }
    
    while (!adoRecordSet.IsEOF ())
    {
        //寻找上线
        adoRecordSet.GetCollect ("LType", iType);
        if(2 == iType)
        {
            adoRecordSet.MoveNext();
            continue;
        }
        adoRecordSet.GetCollect ("Time", tOnline);
        ////////////////////////////////////////////////

        ///////寻找与之对应的下线//////////////
        adoRecordSet.MoveNext();
        if(adoRecordSet.IsEOF ())   //已经到尾
        {
            break;
        }
        adoRecordSet.GetCollect ("LType", iType);
        if(1 == iType)  //如果为上线，放弃上一条上线数据
        {
            continue;
        }
        ///找到对应下线
        adoRecordSet.GetCollect ("Time", tOffLine);
        adoRecordSet.MoveNext ();

        tSpan = tOffLine - tOnline;
        tSpan2 = tOnline - tBaseTime;
        for (int i = 0; i < tSpan.GetTotalMinutes (); i++)
        {
            mapTeacherTime.insert (make_pair<int, int> (tSpan2.GetTotalMinutes () + i, 1));
        }
    }
    
    ////////////////////////////////////////////////////////////////

    //计算同时在线时间
    for (size_t i = 0; i < struInfo.vecStuId.size (); i++)
    {
        adoRecordSet.Close ();
        iCount = 0;

        strSQL.Format ("SELECT * FROM Qz_CourseLog where CourseId = %s and UserId = %d ORDER BY ID", 
            struInfo.strId, struInfo.vecStuId[i]);
        if(!adoRecordSet.Open (strSQL))
        {
            LOG(LEVEL_ERROR, "%s查询学生上下线数据失败。SQL: %s", __FUNCTION__, strSQL);
            break;
        }

        while (!adoRecordSet.IsEOF ())
        {
            //寻找上线
            adoRecordSet.GetCollect ("LType", iType);
            if(2 == iType)
            {
                adoRecordSet.MoveNext();
                continue;
            }
            adoRecordSet.GetCollect ("Time", tOnline);
            ////////////////////////////////////////////////

            ///////寻找与之对应的下线//////////////
            adoRecordSet.MoveNext();
            if(adoRecordSet.IsEOF ())   //已经到尾
            {
                break;
            }
            adoRecordSet.GetCollect ("LType", iType);
            if(1 == iType)  //如果为上线，放弃上一条上线数据
            {
                continue;
            }
            ///找到对应下线
            adoRecordSet.GetCollect ("Time", tOffLine);
            adoRecordSet.MoveNext ();

            tSpan = tOffLine - tOnline;
            tSpan2 = tOnline - tBaseTime;
            for (int j = 0; j < tSpan.GetTotalMinutes (); j++)
            {
                iter = mapTeacherTime.find (tSpan2.GetTotalMinutes () + j);
                if(iter != mapTeacherTime.end ())
                {
                    iCount++;
                }
            }
        }

        struInfo.vecCommonTime.push_back (iCount);
    }


    iTicks = GetTickCount () - iTicks;
#ifdef _DEBUG
    LOG (LEVEL_INFO, "TEST---%s用时:%dms", __FUNCTION__, iTicks);
#endif
}


void CDBProcessDlg::ResolveStuId(vector<CourseInfo> & vecInfo)
{
    vector<CString> vecID;
    int iIndex = -1;
    CString strStuId;
    CString strTmp;

    for (size_t i = 0; i < vecInfo.size (); i++)
    {
        strStuId = vecInfo[i].strStuId;
        while(1)
        {
            iIndex = strStuId.Find ("}");
            if(iIndex < 0)
            {
                break;
            }
            
            strTmp = strStuId.Left (iIndex);
            strStuId.Delete (0, iIndex + 1);
            
            strTmp.Remove ('{');
            strTmp.Remove (';');
            strTmp.Remove ('}');
            vecInfo[i].vecStuId.push_back (atoi (strTmp.GetBuffer (0)));
        }   
    }
}


void CDBProcessDlg::OnBnClickedBtnTest()
{
   /* CAdoConnection adoConn;

    if(!adoConn.ConnectSQLServer ("DENGBING-PC\\SQLEXPRESS", "TestDB", "admin", "123456"))
    {
        ShowLog ("TEST---连接数据库失败");
        return;
    }

    CourseInfo info;
    info.iTeacherId = 1;
    info.strId = "1001";
    info.strStuId = "{2}";
    info.vecStuId.push_back (2);

    GetOnlineTime(adoConn, info);*/

    if(m_bManual)
    {
        AfxMessageBox ("正在进行手动计算....");
        return;
    }

    m_bManual = true;
    if(NULL != m_hExitEvent)
    {
        SetEvent (m_hExitEvent);
    }
    WaitForSingleObject(m_hWorkThread, INFINITE);
    CloseHandle (m_hWorkThread);
    m_hWorkThread = NULL;

    ShowLog ("停止自动计算线程成功。开始手动计算...");
    
    HANDLE hHandle = CreateThread(NULL, 0, WorkThread , this, 0, NULL);
    CloseHandle (hHandle);


}

bool CDBProcessDlg::GetChiefId(CAdoConnection& adoConn, int& iChiefId, int iClassId)
{
    CAdoRecordSet adoRecordSet(&adoConn);
    CString strSQL = "";
    bool bRtn = false;
    iChiefId = 0;
    
    if(iClassId <= 0)
    {
        return false;
    }

    strSQL.Format ("select ChiefID from Qz_Class where ClassId = %d", iClassId);

    if(!adoRecordSet.Open (strSQL))
    {
        LOG(LEVEL_ERROR, "%s查询班主任ID失败。SQL: %s", __FUNCTION__, strSQL);
        return false;
    }

    if (!adoRecordSet.IsEOF ())
    {
        adoRecordSet.GetCollect ("ChiefID", iChiefId);
        bRtn = true;
    }

    return bRtn;
}

CString CDBProcessDlg::GetStudentOnlineTime(CourseInfo& struInfo)
{
    CString strTime;
    CString strTmp;
    int iTime = 0;

    for(int i = 0; i < struInfo.vecStuId.size (); i++)
    {
        if(struInfo.vecStuTime.size () >= i + 1)
        {
            iTime = struInfo.vecStuTime[i];
        }
        else
        {
            iTime = 0;
        }

        strTmp.Format ("%d:%d|", struInfo.vecStuId[i], iTime);
        strTime += strTmp;
    }

    strTime.Delete (strTime.GetLength () - 1, 1);
    return strTime;
}

CString CDBProcessDlg::GetCommonOnlineTime(CourseInfo& struInfo)
{
    CString strTime;
    CString strTmp;
    int iTime = 0;

    for(int i = 0; i < struInfo.vecStuId.size (); i++)
    {
        if(struInfo.vecCommonTime.size () >= i + 1)
        {
            iTime = struInfo.vecCommonTime[i];
        }
        else
        {
            iTime = 0;
        }

        strTmp.Format ("%d:%d|", struInfo.vecStuId[i], iTime);
        strTime += strTmp;
    }

    strTime.Delete (strTime.GetLength () - 1, 1);
    return strTime;
}

DWORD WINAPI CDBProcessDlg::WorkThread(LPVOID lParam)
{
    CDBProcessDlg* pThis = (CDBProcessDlg*)lParam;

    pThis->WorkFunc ();

    return 0;
}

void CDBProcessDlg::WorkFunc()
{
    LOG(LEVEL_OUTPUT, "手动计算开始----------");
    GetCourseInfo (1);
    LOG(LEVEL_OUTPUT, "手动计算开始结束----------");

    ShowLog ("手动计算完成。恢复自动计算线程。");
    ResetEvent (m_hExitEvent);
    m_hWorkThread = CreateThread(NULL, 0, AutoWorkThread , this, 0, NULL);
    if(NULL == m_hWorkThread)
    {
        ShowLog("创建主工作线程失败，恢复自动计算失败");
    }
    else
    {
        ShowLog("恢复自动计算成功");
    }
    m_bManual = false;
}