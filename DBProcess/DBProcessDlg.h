// DBProcessDlg.h : ͷ�ļ�
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
    vector<int> vecCommonTime;  //��ͬ����ʱ��

    tag_Course_Info()
    {
        iTeacherId = 0;
        iTeacherTime = 0;
    }
}CourseInfo, *LPCourseInfo;

// CDBProcessDlg �Ի���
class CDBProcessDlg : public CDialog
{
// ����
public:
	CDBProcessDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DBPROCESS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
        str.Format ("%s��ʱ:%dms", m_strName, GetTickCount () - m_lTicks);
        TRACE(str);
    }

private:
    long m_lTicks;
    CString m_strName;
};