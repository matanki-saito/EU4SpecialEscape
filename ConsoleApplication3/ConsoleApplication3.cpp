// ConsoleApplication3.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

errno_t loadTextFromBinary(const char* source, char **to) {

	errno_t success = 0;

	if (source == NULL) {
		success = 1;
		goto F;
	}

	/* */
	struct stat statData;
	int error = stat(source, &statData);

	if (error != NULL) {
		success = 2;
		goto F;
	}

	/* */
	FILE *fp;
	errno_t readFileError = fopen_s(&fp, source, "rb");

	if (readFileError != NULL) {
		success = 3;
		goto F;
	}

	/* */
	*to = (char*) calloc(statData.st_size + 1, sizeof(char));

	if (*to == NULL) {
		success = 4;
		goto A;
	}

	/* */
	int readSize = fread_s(*to, statData.st_size, sizeof(char), statData.st_size, fp);

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
	if (! ((from[0] == ((char)0xEF)) && (from[1] == (char)0xBB) && (from[2] == (char)0xBF)) ){
		success = 3;
		goto A;
	}

	/* */
	unsigned int size = strlen(from);

	/* */
	*to = (char*)calloc(size - 2, sizeof(char));

	if (*to == NULL) {
		success = 4;
		goto A;
	}

	/* */
	errno_t err = memcpy_s(*to,size-2, from + 3, size - 3);

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
	
	/* */
	if (from == NULL) {
		success = 1;
		goto A;
	}

	/* */
	unsigned int wideTextSize = MultiByteToWideChar(
		CP_UTF8,
		MB_PRECOMPOSED,
		from,
		-1,
		NULL,
		NULL);

	if (wideTextSize == NULL) {
		success = 2;
		goto A;
	}

	/* */
	*to = (wchar_t*)calloc(wideTextSize, sizeof(wchar_t));

	if (*to == NULL) {
		success = 3;
		goto A;
	}

	/* */
	unsigned int err = MultiByteToWideChar(
		CP_UTF8,
		MB_PRECOMPOSED,
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

	/* */
	if (from == NULL) {
		success = 1;
		goto A;
	}

	/* */
	unsigned int size = wcslen(from);

	/* 全部エスケープしても３倍を超えることはない。１０はバッファ*/
	*to = (wchar_t*)calloc(size * 3 + 10, sizeof(wchar_t));

	if (*to == NULL) {
		success = 2;
		goto A;
	}

	/* */
	int toIndex = 0;
	for (int fromIndex = 0; fromIndex < size; fromIndex++) {
		const wchar_t cp = from[fromIndex];

		/* 上位バイト */
		byte high = (cp >> 8) & 0x000000FF;

		/* 下位バイト */
		byte low = cp & 0x000000FF;

		/* 2byteじゃない */
		if (high == 0) {
			(*to)[toIndex++] = cp;
			continue;
		}

		byte escapeChr = 0x10;

		/* high byteより決定 */
		switch (high) {
		case 0xA4:case 0xA3:case 0xA7:case 0x24:case 0x5B:case 0x00:case 0x5C:
		case 0x20:case 0x0D:case 0x0A:case 0x22:case 0x7B:case 0x7D:case 0x40:
		case 0x80:case 0x7E:case 0xBD:
			escapeChr += 2;
			break;
		default:
			break;
		}

		/* low byteより決定 */
		switch (low) {
		case 0xA4:case 0xA3:case 0xA7:case 0x24:case 0x5B:case 0x00:case 0x5C:
		case 0x20:case 0x0D:case 0x0A:case 0x22:case 0x7B:case 0x7D:case 0x40:
		case 0x80:case 0x7E:case 0xBD:
			escapeChr++;
			break;
		default:
			break;
		}

		switch (escapeChr) {
		case 0x11:
			low += 15;
			break;
		case 0x12:
			high -= 9;
			break;
		case 0x13:
			low += 15;
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

errno_t convertWideTextToText(const wchar_t* from, char **to ) {
	 
	 errno_t success = 0;

	 /* */
	 if (from == NULL) {
		 success = 1;
		 goto A;
	 }

	 /* */
	 int size = WideCharToMultiByte(
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
	 int err = WideCharToMultiByte(
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

errno_t attachBOM(const char* from, char ** to ) {

	errno_t success = 0;

	/* */
	if (from == NULL) {
		success = 1;
		goto A;
	}
	
	/* */
	unsigned int size = strlen(from);

	/* */
	*to = (char*)calloc(size +4, sizeof(char));

	if (*to == NULL) {
		success = 2;
		goto A;
	}

	/* UTF-8 BOM */
	(*to)[0] = (char)0xEF;
	(*to)[1] = (char)0xBB;
	(*to)[2] = (char)0xBF;

	/* */
	errno_t err = memcpy_s(*to+3, size + 4, from , size);

	if (err) {
		success = 3;
		goto B;
	}

	goto A;

B:
	free(*to);

A:
	return success;
}

errno_t saveTextToBinary(const char *importPath, const char* from) {

	errno_t success = 0;

	if (importPath == NULL) {
		success = 1;
		goto F;
	}

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	/* */
	_splitpath_s(
		importPath,
		drive,
		dir,
		fname,
		ext
	);
	
	/* */
	errno_t strcat_err = strcat_s(dir,_MAX_DIR,"\\escapedFiles");

	/* */
	if (strcat_err) {
		success = 2;
		goto F;
	}

	/* */
	char exportDir[_MAX_EXT];
	_makepath_s(exportDir,drive,dir, NULL, NULL);
	if (_mkdir(exportDir) == 0) {
		success = -1;
	}

	/* */
	char exportPath[_MAX_EXT];
	_makepath_s(exportPath,drive,dir,fname,ext);

	/* */
	FILE *fw;
	errno_t fopen_err = fopen_s(&fw, exportPath, "wb");

	if (fopen_err) {
		success = 3;
		goto F;
	}
	 
	/* */
	int file_size = fwrite(from, sizeof(char),strlen(from), fw);

	if (file_size != strlen(from)) {
		success = 4;
		goto A;
	}

A:
	fclose(fw);
F:
	return success;
 }

errno_t getFullPath(char *from[],char **to) {

	errno_t success = 0;

	/* */
	if (! (*++from)) {
		success = 1;
		goto F;
	}

	/* */
	*to = (char*)calloc(_MAX_PATH, sizeof(char));
	if (*to == NULL) {
		success = 2;
		goto F;
	}

	/* */
	char *err = _fullpath(*to, *from, _MAX_PATH);
	if (*err == NULL) {
		success = 3;
		goto F;
	}

F:
	return success;
}


int main(int argc, char *argv[])
{
	errno_t ans = 0;
	char *path = NULL;
	char *importText = NULL;
	char *noBOMtext = NULL;
	wchar_t *wideNoBOMtext = NULL;
	wchar_t *escapedNoBOMWideText = NULL;
	char *escapedNoBOMText = NULL;
	char* escapedText = NULL;

	/* */
	ans = getFullPath(argv, &path);
	if (ans > 0) {
		printf("引数異常:%d", ans);
		goto A;
	}
	else {
		printf("%-10s %5s\n","引数","[OK]");
	}

	/* */
	ans = loadTextFromBinary(path, &importText);
	if (ans > 0) {
		printf("ファイルからテキストを読み込めない:%d",ans);
		goto H;
	}
	else {
		printf("%-10s %5s\n","読み込み","[OK]");
	}

	/* */
	ans = removeBOM(importText, &noBOMtext);
	if (ans > 0) {
		printf("BOMがついていないからUTF-8じゃない:%d", ans);
		goto B;
	}
	else {
		printf("%-10s %5s\n", "BOM", "[OK]");
	}

	/* */
	ans = convertTextToWideText(noBOMtext, &wideNoBOMtext);
	if (ans > 0) {
		printf("M->Wが変換できなかった:%d",ans);
		goto C;
	}
	else {
		printf("%-10s %5s\n", "M->W", "[OK]");
	}

	/* */
	ans = convertWideTextToEscapedWideText(wideNoBOMtext, &escapedNoBOMWideText);
	if (ans > 0) {
		printf("エスケープに失敗した:%d",ans);
		goto D;
	}
	else {
		printf("%-10s %5s\n", "エスケープ", "[OK]");
	}

	/* */
	ans = convertWideTextToText(escapedNoBOMWideText, &escapedNoBOMText);
	if (ans > 0) {
		printf("W->Mが変換できなかった:%d",ans);
		goto E;
	}
	else {
		printf("%-10s %5s\n", "W->M", "[OK]");
	}

	/* */
	ans = attachBOM(escapedNoBOMText, &escapedText);
	if (ans > 0) {
		printf("BOMをつけるのに失敗:%d",ans);
		goto F;
	}
	else {
		printf("%-10s %5s\n", "BOM付け", "[OK]");
	}

	
	/* */
	ans = saveTextToBinary(path, escapedText);
	if (ans > 0) {
		printf("ファイルをセーブするのに失敗:%d",ans);
		goto G;
	}
	else {
		printf("%-10s %5s\n", "セーブ", "[OK]");
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

H: 
	free(path);

A:
	return 0;

}



