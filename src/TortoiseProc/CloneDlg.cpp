// CloneDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TortoiseProc.h"
#include "CloneDlg.h"
#include "BrowseFolder.h"
#include "MessageBox.h"
#include "AppUtils.h"
// CCloneDlg dialog

IMPLEMENT_DYNCREATE(CCloneDlg, CResizableStandAloneDialog)

CCloneDlg::CCloneDlg(CWnd* pParent /*=NULL*/)
	: CResizableStandAloneDialog(CCloneDlg::IDD, pParent)
	, m_Directory(_T(""))
{
    m_bAutoloadPuttyKeyFile = CAppUtils::IsSSHPutty();
}

CCloneDlg::~CCloneDlg()
{
}

void CCloneDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableStandAloneDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_URLCOMBO, m_URLCombo);
    DDX_Control(pDX, IDC_PUTTYKEYFILE, m_PuttyKeyCombo);
	DDX_Text(pDX, IDC_CLONE_DIR, m_Directory);
    DDX_Check(pDX,IDC_PUTTYKEY_AUTOLOAD, m_bAutoloadPuttyKeyFile);

}

BOOL CCloneDlg::OnInitDialog()
{
	CResizableStandAloneDialog::OnInitDialog();
	AddAnchor(IDC_URLCOMBO, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_CLONE_BROWSE_URL, TOP_RIGHT);
	AddAnchor(IDC_CLONE_DIR, TOP_LEFT,TOP_RIGHT);
	AddAnchor(IDC_CLONE_DIR_BROWSE, TOP_RIGHT);
	AddAnchor(IDOK,BOTTOM_RIGHT);
	AddAnchor(IDCANCEL,BOTTOM_RIGHT);

    AddAnchor(IDC_GROUP_CLONE,TOP_LEFT,BOTTOM_RIGHT);
    AddAnchor(IDC_PUTTYKEYFILE_BROWSE,BOTTOM_RIGHT);
    AddAnchor(IDC_PUTTYKEY_AUTOLOAD,BOTTOM_LEFT);
    AddAnchor(IDC_PUTTYKEYFILE,BOTTOM_LEFT,BOTTOM_RIGHT);

	this->AddOthersToAnchor();

	m_URLCombo.SetURLHistory(TRUE);
	m_URLCombo.LoadHistory(_T("Software\\TortoiseGit\\History\\repoURLS"), _T("url"));
	if(m_URL.IsEmpty())
		m_URLCombo.SetCurSel(0);
	else
		m_URLCombo.SetWindowText(m_URL);

	CWnd *window=this->GetDlgItem(IDC_CLONE_DIR);
	if(window)
		SHAutoComplete(window->m_hWnd, SHACF_FILESYSTEM);

    m_PuttyKeyCombo.SetPathHistory(TRUE);
    m_PuttyKeyCombo.LoadHistory(_T("Software\\TortoiseGit\\History\\puttykey"), _T("key"));
    m_PuttyKeyCombo.SetCurSel(0);

    this->GetDlgItem(IDC_PUTTYKEY_AUTOLOAD)->EnableWindow( CAppUtils::IsSSHPutty() );
    this->GetDlgItem(IDC_PUTTYKEYFILE)->EnableWindow(m_bAutoloadPuttyKeyFile);
    this->GetDlgItem(IDC_PUTTYKEYFILE_BROWSE)->EnableWindow(m_bAutoloadPuttyKeyFile);
       
    EnableSaveRestore(_T("CloneDlg"));
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CCloneDlg, CResizableStandAloneDialog)
	ON_BN_CLICKED(IDC_CLONE_BROWSE_URL, &CCloneDlg::OnBnClickedCloneBrowseUrl)
	ON_BN_CLICKED(IDC_CLONE_DIR_BROWSE, &CCloneDlg::OnBnClickedCloneDirBrowse)
    ON_BN_CLICKED(IDC_PUTTYKEYFILE_BROWSE, &CCloneDlg::OnBnClickedPuttykeyfileBrowse)
    ON_BN_CLICKED(IDC_PUTTYKEY_AUTOLOAD, &CCloneDlg::OnBnClickedPuttykeyAutoload)
END_MESSAGE_MAP()



// CCloneDlg message handlers

void CCloneDlg::OnOK()
{
	this->m_URLCombo.GetWindowTextW(m_URL);
	m_URL.Trim();
	UpdateData(TRUE);
	if(m_URL.IsEmpty()||m_Directory.IsEmpty())
	{
		CMessageBox::Show(NULL,_T("URL or Dir can't empty"),_T("TortiseGit"),MB_OK);
		return;
	}

	m_URLCombo.SaveHistory();
    m_PuttyKeyCombo.SaveHistory();

    this->m_PuttyKeyCombo.GetWindowText(m_strPuttyKeyFile );
	CResizableDialog::OnOK();

}

void CCloneDlg::OnCancel()
{
	CResizableDialog::OnCancel();
}

void CCloneDlg::OnBnClickedCloneBrowseUrl()
{
	// TODO: Add your control notification handler code here
	CBrowseFolder browseFolder;
	browseFolder.m_style = BIF_EDITBOX | BIF_NEWDIALOGSTYLE | BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
	CString strCloneDirectory;
	this->m_URLCombo.GetWindowTextW(strCloneDirectory);
	if (browseFolder.Show(GetSafeHwnd(), strCloneDirectory) == CBrowseFolder::OK) 
	{
		this->m_URLCombo.SetWindowTextW(strCloneDirectory);
	}
}

void CCloneDlg::OnBnClickedCloneDirBrowse()
{
	// TODO: Add your control notification handler code here
	CBrowseFolder browseFolder;
	browseFolder.m_style = BIF_EDITBOX | BIF_NEWDIALOGSTYLE | BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
	CString strCloneDirectory = this->m_Directory;
	if (browseFolder.Show(GetSafeHwnd(), strCloneDirectory) == CBrowseFolder::OK) 
	{
		UpdateData(TRUE);
		m_Directory = strCloneDirectory;
		UpdateData(FALSE);
	}
}

void CCloneDlg::OnEnChangeCloneDir()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDHtmlDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CCloneDlg::OnBnClickedPuttykeyfileBrowse()
{
    // TODO: Add your control notification handler code here
    CFileDialog dlg(TRUE,NULL,
					NULL,
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					_T("Putty Private Key(*.ppk)|*.ppk|All Files(*.*)|*.*||"));
	
	this->UpdateData();
	if(dlg.DoModal()==IDOK)
	{
        this->m_PuttyKeyCombo.SetWindowText( dlg.GetPathName() );
	}

}

void CCloneDlg::OnBnClickedPuttykeyAutoload()
{
    // TODO: Add your control notification handler code here
    this->UpdateData();
    this->GetDlgItem(IDC_PUTTYKEYFILE)->EnableWindow(m_bAutoloadPuttyKeyFile);
    this->GetDlgItem(IDC_PUTTYKEYFILE_BROWSE)->EnableWindow(m_bAutoloadPuttyKeyFile);

}
