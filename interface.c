//生成二维码（暂时不支持自定义logo）, add by songxu
//pText: 被编码的文本
//pPath: 二维码图片路径
//nPixel: 二维码每个点所占像素数
//nMargin: 二维码外边距大小
//bkColor: 背景颜色，通常为白色
//fgColor: 前景颜色，通常为黑色
//nErrorLevel: 二维码容错级别，0: 最低，1：中等，2：较高，3：最高。若想给二维码添加logo，需要提高容错率
void MakeQRCode(char *pText, char *pPath, int nPixel = 3, int nMargin = 3, COLORREF bkColor = RGB(255, 255, 255), COLORREF fgColor = RGB(0, 0, 0), int nErrorLevel = 0);

void MakeQRCode(char *pText, char *pPath, int nPixel/* = 3*/, int nMargin/* = 3*/, COLORREF bkColor/* = RGB(255, 255, 255)*/, COLORREF fgColor/* = RGB(0, 0, 0)*/, int nErrorLevel/* = 0*/)
{	
	if(!pText || !pPath)
		return;
	if(nPixel <= 0 || nMargin < 0 || nErrorLevel < 0 || nErrorLevel > 3)
		return;

	//为防止pText内容过长，预先把pText写入临时文件
	char textPath[MAX_PATH] = {0}, lpTempPathBuffer[MAX_PATH] = {0};
	GetTempPathA(MAX_PATH, lpTempPathBuffer); 
	GetTempFileNameA(lpTempPathBuffer, "QRT", 0, textPath);
	HANDLE hFile = ::CreateFileA(textPath, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hFile)
		return;
	DWORD dwWritten = 0;
	::WriteFile(hFile, pText, strlen(pText), &dwWritten, NULL);
	::CloseHandle(hFile);

	//二维码编码程序路径
	char exePath[MAX_PATH] = {0}, modulePath[MAX_PATH] = {0};
	char curDir[MAX_PATH] = {0}, drive[_MAX_DRIVE] = {0}, dir[MAX_PATH] = {0};
	::GetModuleFileNameA(NULL, modulePath, MAX_PATH);
	_splitpath(modulePath, drive, dir, NULL, NULL);
	strcpy(curDir, drive), strcat(curDir, dir);
	strcat(exePath, curDir), strcat(exePath, "qrcode.exe");

	//错误率转化
	char chLevel = 'L';
	switch(nErrorLevel)
	{
	case 0:
		chLevel = 'L';
		break;
	case 1:
		chLevel = 'M';
		break;
	case 2:
		chLevel = 'Q';
		break;
	case 3:
		chLevel = 'H';
		break;
	}

	//命令行参数
	char cmd[32768] = {0};
	sprintf(cmd, "cmd.exe /c %s -o %s -s %d -m %d --background=%06X --foreground=%06X -l %c < %s", exePath, pPath, nPixel, nMargin, bkColor, fgColor, chLevel, textPath);

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	// Start the child process. 
	if( !::CreateProcessA(NULL,   // No module name (use command line)
		cmd,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		CREATE_NO_WINDOW,
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi )           // Pointer to PROCESS_INFORMATION structure
		) 
	{
		return ;
	}

	// Wait until child process exits.
	WaitForSingleObject( pi.hProcess, INFINITE );

	// Close process and thread handles. 
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
}