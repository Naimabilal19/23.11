#include "CriticalSectionDlg.h"

CriticalSectionDlg* CriticalSectionDlg::ptr = NULL;

CRITICAL_SECTION cs;
TCHAR str[50];

CriticalSectionDlg::CriticalSectionDlg(void)
{
	ptr = this;
}
CriticalSectionDlg::~CriticalSectionDlg(void)
{
	DeleteCriticalSection(&cs);
}
void CriticalSectionDlg::Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
}
BOOL CriticalSectionDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	InitializeCriticalSection(&cs);
	return TRUE;
}

DWORD WINAPI WriteToFiles(LPVOID lp)
{
	srand(time(0));
	EnterCriticalSection(&cs);   //захват секции

	ofstream out(TEXT("array.txt"));
	if (!out.is_open())
	{
		MessageBox(0, TEXT("Error"), TEXT(" Критическая секция"), MB_OK);
		return 1;
	}
	int A[100];
	for (int i = 0; i < 100; i++)
	{
		A[i] = rand() % 50;
		out << A[i] << ' ';
	}
	out.close();

	ofstream copy2(TEXT("Copyarray2.txt"));
	CopyFile(L"C:\\Users\\naima\\Desktop\\CriticalSection\\CriticalSection\\array.txt",
		L"C:\\Users\\naima\\Desktop\\CriticalSection\\CriticalSection\\Copyarray2.txt", 0);  //1 копия
	if (copy2.is_open()) {
		MessageBox(0, TEXT("Копия файла 1"), TEXT(""), MB_OK);
	}
	copy2.close();
	ofstream copy3(TEXT("Copyarray3.txt"));
	CopyFile(L"C:\\Users\\naima\\Desktop\\CriticalSection\\CriticalSection\\array.txt",
		L"C:\\Users\\naima\\Desktop\\CriticalSection\\CriticalSection\\Copyarray3.txt", 0); //2 копия
	if (copy3.is_open()) {
		MessageBox(0, TEXT("Копия файла 2"), TEXT(""), MB_OK);
	}
	copy3.close();

	LeaveCriticalSection(&cs);       //освобождаем секцию
	return 0;
}

DWORD WINAPI ReadFromFiles(LPVOID lp)
{
	EnterCriticalSection(&cs); //захват секции
	ifstream in(TEXT("Copyarray2.txt"));
	if (!in.is_open()) {
		MessageBox(0, TEXT("Error"), TEXT("Error"), MB_OK);
		return 1;
	}
	in.close();

	ifstream in1(TEXT("Copyarray3.txt"));
	if (!in1.is_open()) {
		MessageBox(0, TEXT("Error"), TEXT("Error"), MB_OK);
		return 1;
	}
	in1.close();

	CopyFile(L"Copyarray2.txt", L"result.txt", 0);      //копируем в результирующий документ
	CopyFile(L"Copyarray3.txt", L"result.txt", 0);

	LeaveCriticalSection(&cs);       //освобождаем секцию
	return 0;
}


void CriticalSectionDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if (id == IDC_BUTTON1)
	{
		HANDLE hThread = CreateThread(NULL, 0, WriteToFiles, 0, 0, NULL);
		CloseHandle(hThread);
		hThread = CreateThread(NULL, 0, ReadFromFiles, 0, 0, NULL);
		CloseHandle(hThread);
	}
}

BOOL CALLBACK CriticalSectionDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}
	return FALSE;
}