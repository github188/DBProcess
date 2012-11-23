// DBProcessDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include <vector>
#include <map>
using namespace std;
#include "Ado.h"
#include "AdoRecordSet.h"

#define CHECK_TIME      1000 * 60 * 30

typedef struct tag_Course_Info
{
    int         iMode;
    int         iClassId;
    CString     strCourseId;
    CString     strId;
    int         iTeacherId;
    int         iTeacherTime;
    CString     strStuId;
    vector<int> vecStuId;
    vector<int> vecStuTime;     
    vector<int> vecCommonTime;  //共同在线时间

    tag_Course_Info()
    {
        iTeacherId = 0;
        iTeacherTime = 0;
    }
}CourseInfo, *LPCourseInfo;

// CDBProcessDlg 对话框
class CDBProcessDlg : public CDialog
{
// 构造
public:
	CDBProcessDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DBPROCESS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();

    CString m_strConfigPath;
    int     m_iDBEnable;
    CString m_strDBServerIP;
    CString m_strDBName;
    CString m_strDBUser;
    CString m_strDBPwd;

    vector<CString> m_LogVector;
    CLock           m_LogVectorLock;
    HANDLE m_hExitEvent;
    HANDLE m_hWorkThread;

    bool m_bManual;

    void GetConfig();

    void Init();
    
    CString GetConfigPath();
    void GetVersionInfo(void);
    CListCtrl m_LogList;
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    void ShowLog(LPCTSTR ptzFormat, ...);

    static DWORD WINAPI AutoWorkThread(LPVOID lParam);
    void AutoWorkFunc();

    static DWORD WINAPI WorkThread(LPVOID lParam);
    void WorkFunc();

    bool GetCourseInfo(int iWorkMode = 0);
    void ResolveStuId(vector<CourseInfo> & vecInfo);
    bool GetOnlineTime(CAdoConnection& adoConn, CourseInfo& struInfo);
    int GetUserOnlineTime(CAdoConnection& adoConn, CString strCourseId, int iUserId);
    void GetCommonTime(CAdoConnection& adoConn, CourseInfo& struInfo);
    bool GetChiefId(CAdoConnection& adoConn, int& iChiefId, int iClassId);
    CString GetStudentOnlineTime(CourseInfo& struInfo);
    CString GetCommonOnlineTime(CourseInfo& struInfo);

    afx_msg void OnBnClickedBtnTest();
};

class CodeSpend
{
public:
    CodeSpend(){m_lTicks = GetTickCount ();};

    void Start(LPCTSTR strName = "")
    {
        m_strName = strName;
        m_lTicks = GetTickCount ();
    }

    void End()
    {
        CString str;
        str.Format ("%s用时:%dms", m_strName, GetTickCount () - m_lTicks);
        TRACE(str);
    }

private:
    long m_lTicks;
    CString m_strName;
};