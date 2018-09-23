#include "stdafx.h"

errno_t subUTF8(const wchar_t *source, wchar_t *baseFileName);
errno_t subUTF8B(const wchar_t *source, wchar_t *baseFileName);

errno_t loadTextFromBinary(const wchar_t *source, char **to) {

	errno_t success = 0;
	errno_t readFileError = 0;
	FILE *fp;
	int error = 0;
	struct _stat statData;
	int readSize = 0;

	if (source == NULL) {
		success = 1;
		goto F;
	}

	/* */
	error = _wstat(source, &statData);
	if (error != NULL) {
		success = 2;
		goto F;
	}

	/* */
	
	readFileError = _wfopen_s(&fp, source, L"rb");
	if (readFileError != NULL) {
		success = 3;
		goto F;
	}

	/* */
	*to = (char*)calloc(statData.st_size + 1, sizeof(char));
	if (*to == NULL) {
		success = 4;
		goto A;
	}

	/* */
	readSize = fread_s(*to, statData.st_size, sizeof(char), statData.st_size, fp);

	if (readSize != statData.st_size) {
		success = 5;
		goto A;
	}

A:
	fclose(fp);
F:
	return success;
}

errno_t removeBOM(const char *from, char **to) {

	errno_t success = 0;
	errno_t err = 0;
	unsigned int size = 0;

	/* */
	if (from == NULL) {
		success = 1;
		goto A;
	}

	/* */
	if (strlen(from) < 4) {
		success = 2;
		goto A;
	}

	/* */
	if (!((from[0] == ((char)0xEF)) && (from[1] == (char)0xBB) && (from[2] == (char)0xBF))) {
		success = 3;
		goto A;
	}

	/* */
	size = strlen(from);

	/* */
	*to = (char*)calloc(size - 2, sizeof(char));

	if (*to == NULL) {
		success = 4;
		goto A;
	}

	/* */
	err = memcpy_s(*to, size - 2, from + 3, size - 3);

	if (err) {
		success = 5;
		goto B;
	}

	goto A;

B:
	free(*to);

A:
	return success;
}

errno_t convertTextToWideText(const char* from, wchar_t **to) {

	errno_t success = 0;
	unsigned int err = 0;
	unsigned int wideTextSize = 0;

	/* */
	if (from == NULL) {
		success = 1;
		goto A;
	}

	/* */
	wideTextSize = MultiByteToWideChar(
		CP_UTF8,
		NULL,
		from,
		-1,
		NULL,
		NULL);

	if (wideTextSize == NULL) {
		success = GetLastError();
		goto A;
	}

	/* */
	*to = (wchar_t*)calloc(wideTextSize, sizeof(wchar_t));

	if (*to == NULL) {
		success = 3;
		goto A;
	}

	/* */
	err = MultiByteToWideChar(
		CP_UTF8,
		NULL,
		from,
		-1,
		*to,
		wideTextSize);

	if (err == NULL) {
		success = 4;
		goto B;
	}

	goto A;

B:
	free(*to);
A:
	return success;
}

inline wchar_t UCS2ToCP1252(int cp) {
	wchar_t result = cp;
	switch (cp) {
	case 0x20AC: result = 0x80; break;
	case 0x201A: result = 0x82; break;
	case 0x0192: result = 0x83; break;
	case 0x201E: result = 0x84; break;
	case 0x2026: result = 0x85; break;
	case 0x2020: result = 0x86; break;
	case 0x2021: result = 0x87; break;
	case 0x02C6: result = 0x88; break;
	case 0x2030: result = 0x89; break;
	case 0x0160: result = 0x8A; break;
	case 0x2039: result = 0x8B; break;
	case 0x0152: result = 0x8C; break;
	case 0x017D: result = 0x8E; break;
	case 0x2018: result = 0x91; break;
	case 0x2019: result = 0x92; break;
	case 0x201C: result = 0x93; break;
	case 0x201D: result = 0x94; break;
	case 0x2022: result = 0x95; break;
	case 0x2013: result = 0x96; break;
	case 0x2014: result = 0x97; break;
	case 0x02DC: result = 0x98; break;
	case 0x2122: result = 0x99; break;
	case 0x0161: result = 0x9A; break;
	case 0x203A: result = 0x9B; break;
	case 0x0153: result = 0x9C; break;
	case 0x017E: result = 0x9E; break;
	case 0x0178: result = 0x9F; break;
	}

	return result;
}

inline wchar_t cp1252ToUCS2(byte cp) {
	wchar_t result = cp;
	switch (cp) {
	case 0x80: result = 0x20AC; break;
	case 0x82: result = 0x201A; break;
	case 0x83: result = 0x0192; break;
	case 0x84: result = 0x201E; break;
	case 0x85: result = 0x2026; break;
	case 0x86: result = 0x2020; break;
	case 0x87: result = 0x2021; break;
	case 0x88: result = 0x02C6; break;
	case 0x89: result = 0x2030; break;
	case 0x8A: result = 0x0160; break;
	case 0x8B: result = 0x2039; break;
	case 0x8C: result = 0x0152; break;
	case 0x8E: result = 0x017D; break;
	case 0x91: result = 0x2018; break;
	case 0x92: result = 0x2019; break;
	case 0x93: result = 0x201C; break;
	case 0x94: result = 0x201D; break;
	case 0x95: result = 0x2022; break;
	case 0x96: result = 0x2013; break;
	case 0x97: result = 0x2014; break;
	case 0x98: result = 0x02DC; break;
	case 0x99: result = 0x2122; break;
	case 0x9A: result = 0x0161; break;
	case 0x9B: result = 0x203A; break;
	case 0x9C: result = 0x0153; break;
	case 0x9E: result = 0x017E; break;
	case 0x9F: result = 0x0178; break;
	}

	return result;

}

errno_t convertWideTextToEscapedWideText(const wchar_t* from, wchar_t** to) {

	errno_t success = 0;
	int toIndex = 0;
	unsigned int size = 0;

	/* */
	if (from == NULL) {
		success = 1;
		goto A;
	}

	/* */
	size = wcslen(from);

	/* 全部エスケープしても３倍を超えることはない。１０はバッファ*/
	*to = (wchar_t*)calloc(size * 3 + 10, sizeof(wchar_t));

	if (*to == NULL) {
		success = 2;
		goto A;
	}

	/* */
	toIndex = 0;
	for (unsigned int fromIndex = 0; fromIndex < size; fromIndex++) {
		wchar_t cp = from[fromIndex];

		/* */
		if (UCS2ToCP1252(cp) != cp) {
			(*to)[toIndex++] = cp;
			continue;
		}

		/* ずらす */
		if (cp > 0x100 && cp < 0xA00) {
			cp = cp + 0xE000;
		}

		/* 上位バイト */
		byte high = (cp >> 8) & 0x000000FF;

		/* 下位バイト */
		byte low = cp & 0x000000FF;

		byte escapeChr = 0x10;

		/* 2byteじゃない */
		if (high == 0) {
			(*to)[toIndex++] = cp;
			continue;
		}

		/* high byteより決定 */
		switch (high) {
		case 0xA4:case 0xA3:case 0xA7:case 0x24:case 0x5B:case 0x00:case 0x5C:
		case 0x20:case 0x0D:case 0x0A:case 0x22:case 0x7B:case 0x7D:case 0x40:
		case 0x80:case 0x7E:case 0x2F:case 0xBD:case 0x3B:case 0x5D:case 0x5F:
		case 0x3D:case 0x23:
			escapeChr += 2;
			break;
		default:
			break;
		}

		/* low byteより決定 */
		switch (low) {
		case 0xA4:case 0xA3:case 0xA7:case 0x24:case 0x5B:case 0x00:case 0x5C:
		case 0x20:case 0x0D:case 0x0A:case 0x22:case 0x7B:case 0x7D:case 0x40:
		case 0x80:case 0x7E:case 0x2F:case 0xBD:case 0x3B:case 0x5D:case 0x5F:
		case 0x3D:case 0x23:
			escapeChr++;
			break;
		default:
			break;
		}

		switch (escapeChr) {
		case 0x11:
			low += 14;
			break;
		case 0x12:
			high -= 9;
			break;
		case 0x13:
			low += 14;
			high -= 9;
			break;
		case 0x10:
		default:
			break;
		}

		(*to)[toIndex++] = escapeChr;
		(*to)[toIndex++] = cp1252ToUCS2(low);
		(*to)[toIndex++] = cp1252ToUCS2(high);
	}

A:
	return success;
}

errno_t convertWideTextToText(const wchar_t* from, char **to) {

	errno_t success = 0;
	int size = 0;
	int err = 0;

	/* */
	if (from == NULL) {
		success = 1;
		goto A;
	}

	/* */
	size = WideCharToMultiByte(
		CP_UTF8,
		0,
		from,
		-1,
		NULL,
		0,
		NULL,
		NULL
	);

	if (size == NULL) {
		success = 2;
		goto A;
	}

	/* */
	*to = (char*)malloc(size * sizeof(char));
	if (*to == NULL) {
		success = 3;
		goto A;
	}

	/* */
	err = WideCharToMultiByte(
		CP_UTF8,
		0,
		from,
		-1,
		*to,
		size,
		NULL,
		NULL
	);

	if (err == NULL) {
		success = 4;
		goto B;
	}

	goto A;

B:
	free(*to);

A:
	return success;
}

errno_t attachWildCard(const wchar_t* from, wchar_t ** to) {

	errno_t error = 0;
	unsigned int size = 0;
	errno_t err = 0;

	/* */
	if (from == NULL) {
		error = 1;
		goto A;
	}

	/* */
	size = wcslen(from);

	/* */
	*to = (wchar_t*)calloc(size + 3, sizeof(wchar_t));

	if (*to == NULL) {
		error = 2;
		goto A;
	}

	/* */
	err = wmemcpy_s(*to, size, from, size);
	if (err) {
		error = 3;
		goto B;
	}

	/* */
	(*to)[size] = L'\\';
	(*to)[size + 1] = L'*';

	/* */
	goto A;

B:
	free(*to);

A:
	return error;
}

errno_t attachBOM(const char* from, char ** to) {

	errno_t error = 0;
	unsigned int size = 0;

	/* */
	if (from == NULL) {
		error = 1;
		goto A;
	}

	/* */
	size = strlen(from);

	/* */
	*to = (char*)calloc(size + 4, sizeof(char));
	if (*to == NULL) {
		error = 2;
		goto A;
	}

	/* UTF-8 BOM */
	(*to)[0] = (char)0xEF;
	(*to)[1] = (char)0xBB;
	(*to)[2] = (char)0xBF;

	/* */
	if (memcpy_s(*to + 3, size + 4, from, size)) {
		error = 3;
		goto B;
	}

	/* */
	goto A;

B:
	free(*to);

A:
	return error;
}

/* Cpp code */
const std::wregex FILE_NAME_PATTERN1(L"^(.+)\.utf8b(\.[a-zA-Z0-9]+)$");
const std::wregex FILE_NAME_PATTERN2(L"^(.+)\.(\.[a-zA-Z0-9_]+).utf8$");
inline errno_t getBaseFileName(const wchar_t *source, wchar_t **to, const std::wregex pattern) {

	errno_t error = 0;
	std::wcmatch match;

	/* */
	try {
		if (!std::regex_match(source, match, pattern)) {
			/* Not match */
			*to = NULL;
			goto A;
		}

		/* */
		unsigned int size = wcslen(source);

		/* */
		*to = (wchar_t*)calloc(size, sizeof(wchar_t));
		if (*to == NULL) {
			error = 1;
			goto A;
		}

		/* */
		std::wstring tmp = match.str(1); /* + match.str( */
		wcscat_s(*to, tmp.length() + 1, tmp.c_str());
	}
	catch (std::regex_error& e) {
		error = 1;
	}

A:
	return error;
}

errno_t getFullPath(wchar_t *from[], wchar_t **to) {

	errno_t error = 0;
	wchar_t *err = NULL;

	/* */
	if (!(*++from)) {
		error = 1;
		goto F;
	}

	/* */
	*to = (wchar_t*)calloc(_MAX_PATH, sizeof(wchar_t));
	if (*to == NULL) {
		error = 2;
		goto F;
	}

	/* */
	err = _wfullpath(*to, *from, _MAX_PATH);
	if (*err == NULL) {
		error = 3;
		goto A;
	}

	/* */
	goto F;

A:
	free(*to);

F:
	return error;
}

/* http://www14.big.or.jp/~ken1/tech/tech5.html */
errno_t digDir(const wchar_t *source)
{
	errno_t success = 0;
	wchar_t subpath[_MAX_PATH];
	wchar_t temp[_MAX_PATH];
	HANDLE h;
	WIN32_FIND_DATA lp;

	/* */
	errno_t err = wcscpy_s(temp, source);
	if (err > 0) {
		success = 1;
		goto A;
	}

	/* */
	h = FindFirstFile(temp, &lp);
	if (INVALID_HANDLE_VALUE == h) {
		success = 2;
		goto A;
	}

	/* delete last '*' */
	temp[wcslen(temp) - 1] = '\0';

	/* */
	do
	{
		/* isDir */
		if ((lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			(wcscmp(lp.cFileName, L"..") != 0) &&
			(wcscmp(lp.cFileName, L".") != 0)
			) {
			/* TODO:check len */
			int len = wsprintf(subpath, L"%s%s\\*", temp, lp.cFileName);

			/* */
			errno_t err2 = digDir(subpath);
			if (err2 > 0) {
				success = 3;
				goto B;
			}
		}

		/* isFile */
		if ((lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) {
			/* */
			wchar_t *baseFileName;
			errno_t err3 = getBaseFileName(lp.cFileName, &baseFileName,FILE_NAME_PATTERN1);
			if (err3 > 0) {
				success = 4;
				goto B;
			}

			/* */
			if (baseFileName != NULL) {
				/* TODO:check len */
				wchar_t fileFullPath[_MAX_PATH];
				int len = wsprintf(fileFullPath, L"%s%s", temp, lp.cFileName);

				/* */
				errno_t errSub = subUTF8B(fileFullPath, baseFileName);

				free(baseFileName);

				if (errSub > 0) {
					goto B;
				}
			}
			else {
				errno_t err4 = getBaseFileName(lp.cFileName, &baseFileName, FILE_NAME_PATTERN2);
				if (err4 > 0) {
					success = 4;
					goto B;
				}
				if (baseFileName != NULL) {
					/* TODO:check len */
					wchar_t fileFullPath[_MAX_PATH];
					int len = wsprintf(fileFullPath, L"%s%s", temp, lp.cFileName);

					/* */
					errno_t errSub = subUTF8(fileFullPath, baseFileName);

					free(baseFileName);

					if (errSub > 0) {
						goto B;
					}
				}
			}
		}
	} while (FindNextFile(h, &lp));

B:
	FindClose(h);

A:
	return success;
}

/**/
int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
	errno_t error = 0;
	wchar_t *fullPath = NULL;
	wchar_t *attachWildCardFullPath = NULL;

	/* */
	error = getFullPath(argv, &fullPath);
	if (error > 0) {
		printf("引数異常:%d", error);
		goto A;
	}

	/* */
	error = attachWildCard(fullPath, &attachWildCardFullPath);
	if (error > 0) {
		printf("ワイルドカード添付異常:%d", error);
		goto B;
	}

	/* */
	error = digDir(attachWildCardFullPath);
	if (error > 0) {
		printf("ディレクトリ検出異常:%d", error);
		goto C;
	}

C:
	free(attachWildCardFullPath);

B:
	free(fullPath);

A:
	return 0;
}

errno_t saveTextToBinary2(const wchar_t *source, const wchar_t *baseFileName, const char* from) {

	errno_t success = 0;
	int file_size = 0;
	errno_t fopen_err = 0;

	if (source == NULL) {
		success = 1;
		goto F;
	}

	wchar_t drive[_MAX_DRIVE];
	wchar_t dir[_MAX_DIR];
	wchar_t fname[_MAX_FNAME];
	wchar_t ext[_MAX_EXT];

	/* */
	_wsplitpath_s(
		source,
		drive,
		dir,
		fname,
		ext
	);

	/* */
	wchar_t exportPath[_MAX_EXT];
	_wmakepath_s(exportPath, drive, dir, fname, L"");

	/* */
	FILE *fw;
	fopen_err = _wfopen_s(&fw, exportPath, L"wb");

	if (fopen_err) {
		success = 3;
		goto F;
	}

	/* */
	file_size = fwrite(from, sizeof(char), strlen(from), fw);

	if (file_size != strlen(from)) {
		success = 4;
		goto A;
	}

A:
	fclose(fw);
F:
	return success;
}

errno_t saveTextToBinary(const wchar_t *source, const wchar_t *baseFileName, const char* from) {

	errno_t success = 0;
	int file_size = 0;
	errno_t fopen_err = 0;

	if (source == NULL) {
		success = 1;
		goto F;
	}

	wchar_t drive[_MAX_DRIVE];
	wchar_t dir[_MAX_DIR];
	wchar_t fname[_MAX_FNAME];
	wchar_t ext[_MAX_EXT];

	/* */
	_wsplitpath_s(
		source,
		drive,
		dir,
		fname,
		ext
	);

	/* */
	wchar_t exportPath[_MAX_EXT];
	_wmakepath_s(exportPath, drive, dir, baseFileName, ext);

	/* */
	FILE *fw;
	fopen_err = _wfopen_s(&fw, exportPath, L"wb");

	if (fopen_err) {
		success = 3;
		goto F;
	}

	/* */
	file_size = fwrite(from, sizeof(char), strlen(from), fw);

	if (file_size != strlen(from)) {
		success = 4;
		goto A;
	}

A:
	fclose(fw);
F:
	return success;
}


/* */
errno_t subUTF8(const wchar_t *source, wchar_t *baseFileName) {
	errno_t ans = 0;

	char *importText = NULL;
	char *noBOMtext = NULL;
	wchar_t *wideNoBOMtext = NULL;
	wchar_t *escapedNoBOMWideText = NULL;
	char *escapedNoBOMText = NULL;
	char* escapedText = NULL;

	/* */
	ans = loadTextFromBinary(source, &importText);
	if (ans > 0) {
		printf("ファイルからテキストを読み込めない:%d", ans);
		goto A;
	}

	/* */
	ans = convertTextToWideText(importText, &wideNoBOMtext);
	if (ans > 0) {
		printf("M->Wが変換できなかった:%d", ans);
		goto C;
	}

	/* */
	ans = convertWideTextToEscapedWideText(wideNoBOMtext, &escapedNoBOMWideText);
	if (ans > 0) {
		printf("エスケープに失敗した:%d", ans);
		goto D;
	}

	/* */
	ans = convertWideTextToText(escapedNoBOMWideText, &escapedNoBOMText);
	if (ans > 0) {
		printf("W->Mが変換できなかった:%d", ans);
		goto E;
	}

	/* */
	ans = attachBOM(escapedNoBOMText, &escapedText);
	if (ans > 0) {
		printf("BOMをつけるのに失敗:%d", ans);
		goto F;
	}

	/* */
	ans = saveTextToBinary2(source, baseFileName, escapedText);
	if (ans > 0) {
		printf("ファイルをセーブするのに失敗:%d", ans);
	}

G:
	free(escapedText);

F:
	free(escapedNoBOMText);

E:
	free(escapedNoBOMWideText);

D:
	free(wideNoBOMtext);

C:
	free(noBOMtext);

B:
	free(importText);

A:
	return ans;
}


/* */
errno_t subUTF8B(const wchar_t *source, wchar_t *baseFileName) {
	errno_t ans = 0;

	char *importText = NULL;
	char *noBOMtext = NULL;
	wchar_t *wideNoBOMtext = NULL;
	wchar_t *escapedNoBOMWideText = NULL;
	char *escapedNoBOMText = NULL;
	char* escapedText = NULL;

	/* */
	ans = loadTextFromBinary(source, &importText);
	if (ans > 0) {
		printf("ファイルからテキストを読み込めない:%d", ans);
		goto A;
	}

	/* */
	ans = removeBOM(importText, &noBOMtext);
	if (ans > 0) {
		printf("BOMがついていないからUTF-8じゃない:%d", ans);
		goto B;
	}

	/* */
	ans = convertTextToWideText(noBOMtext, &wideNoBOMtext);
	if (ans > 0) {
		printf("M->Wが変換できなかった:%d", ans);
		goto C;
	}

	/* */
	ans = convertWideTextToEscapedWideText(wideNoBOMtext, &escapedNoBOMWideText);
	if (ans > 0) {
		printf("エスケープに失敗した:%d", ans);
		goto D;
	}

	/* */
	ans = convertWideTextToText(escapedNoBOMWideText, &escapedNoBOMText);
	if (ans > 0) {
		printf("W->Mが変換できなかった:%d", ans);
		goto E;
	}

	/* */
	ans = attachBOM(escapedNoBOMText, &escapedText);
	if (ans > 0) {
		printf("BOMをつけるのに失敗:%d",ans);
		goto F;
	}

	
	/* */
	ans = saveTextToBinary(source, baseFileName, escapedText);
	if (ans > 0) {
		printf("ファイルをセーブするのに失敗:%d", ans);
		goto G;
	}

G:
	free(escapedText);

F:
	free(escapedNoBOMText);

E:
	free(escapedNoBOMWideText);

D:
	free(wideNoBOMtext);

C:
	free(noBOMtext);

B:
	free(importText);

A:
	return ans;
}
