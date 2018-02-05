// ConsoleApplication3.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

bool loadTextFromBinary(const char* source, char **to) {

	bool success = true;

	if (source == NULL) {
		success = false;
		goto F;
	}

	/* */
	struct stat statData;
	int error = stat(source, &statData);

	if (error != NULL) {
		success = false;
		goto F;
	}

	/* */
	FILE *fp;
	errno_t readFileError = fopen_s(&fp, source, "rb");

	if (readFileError != NULL) {
		success = false;
		goto F;
	}

	/* */
	*to = (char*) calloc(statData.st_size + 1, sizeof(char));

	if (*to == NULL) {
		success = false;
		goto A;
	}

	/* */
	int readSize = fread_s(*to, statData.st_size, sizeof(char), statData.st_size, fp);

	if (readSize != statData.st_size) {
		success = false;
		goto A;
	}
	
A:
	fclose(fp);
F:
	return success;
}

bool removeBOM(const char *from, char **to) {
	bool success = true;

	/* */
	if (from == NULL) {
		success = false;
		goto A;
	}

	/* */
	if (strlen(from) < 4) {
		success = false;
		goto A;
	}

	/* */
	if (! ((from[0] == ((char)0xEF)) && (from[1] == (char)0xBB) && (from[2] == (char)0xBF)) ){
		success = false;
		goto A;
	}

	/* */
	unsigned int size = strlen(from);

	/* */
	*to = (char*)calloc(size - 2, sizeof(char));

	if (*to == NULL) {
		success = false;
		goto A;
	}

	/* */
	errno_t err = memcpy_s(*to,size-2, from + 3, size - 3);

	if (err) {
		success = false;
		goto B;
	}

	goto A;

B:
	free(*to);

A:
	return success;
}

bool convertTextToWideText(const char* from, wchar_t **to) {

	bool success = true;
	
	/* */
	if (from == NULL) {
		success = false;
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
		success = false;
		goto A;
	}

	/* */
	*to = (wchar_t*)calloc(wideTextSize, sizeof(wchar_t));

	if (*to == NULL) {
		success = false;
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
		success = false;
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

bool convertWideTextToEscapedWideText(const wchar_t* from, wchar_t** to) {

	bool success = true;

	/* */
	if (from == NULL) {
		success = false;
		goto A;
	}

	/* */
	unsigned int size = wcslen(from);

	/* 全部エスケープしても３倍を超えることはない。１０はバッファ*/
	*to = (wchar_t*)calloc(size * 3 + 10, sizeof(wchar_t));

	if (*to == NULL) {
		success = false;
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

 bool convertWideTextToText(const wchar_t* from, char **to ) {
	 
	 bool success = true;

	 /* */
	 if (from == NULL) {
		 success = false;
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
		 success = false;
		 goto A;
	 }

	 /* */
	 *to = (char*)malloc(size * sizeof(char));
	 if (*to == NULL) {
		 success = false;
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
		 success = false;
		 goto B;
	 }

	 goto A;

B:
	 free(*to);
	
A:
	 return success;
}

bool attachBOM(const char* from, char ** to ) {

	bool success = true;

	/* */
	if (from == NULL) {
		success = false;
		goto A;
	}
	
	/* */
	unsigned int size = strlen(from);

	/* */
	*to = (char*)calloc(size +4, sizeof(char));

	if (*to == NULL) {
		success = false;
		goto A;
	}

	/* UTF-8 BOM */
	(*to)[0] = (char)0xEF;
	(*to)[1] = (char)0xBB;
	(*to)[2] = (char)0xBF;

	/* */
	errno_t err = memcpy_s(*to+3, size + 4, from , size);

	if (err) {
		success = false;
		goto B;
	}

	goto A;

B:
	free(*to);

A:
	return success;
}

bool saveTextToBinary(const char *importPath, const char* from) {

	bool success = true;

	if (importPath == NULL) {
		success = false;
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

	printf("%s %s", drive, dir);
	
	/* */
	errno_t err1 = strcat_s(dir,_MAX_DIR,"\\escapedFiles");

	/* */
	if (err1) {
		success = false;
		goto F;
	}

	/* */
	char exportDir[_MAX_EXT];
	_makepath_s(exportDir,drive,dir, NULL, NULL);
	if (_mkdir(exportDir) == 0) {
	}

	/* */
	char exportPath[_MAX_EXT];
	_makepath_s(exportPath,drive,dir,fname,ext);

	/* */
	FILE *fw;
	errno_t err2 = fopen_s(&fw, exportPath, "wb");

	if (err2) {
		success = false;
		goto F;
	}
	 
	/* */
	int size_t = fwrite(from, sizeof(char),strlen(from), fw);

	if (size_t != strlen(from)) {
		success = false;
		goto A;
	}

A:
	fclose(fw);
F:
	return success;
 }

bool getFullPath(char *from[],char **to) {

	bool success = true;

	/* */
	if (! (*++from)) {
		success = false;
		goto F;
	}

	/* */
	*to = (char*)calloc(_MAX_PATH, sizeof(char));
	if (*to == NULL) {
		success = false;
		goto F;
	}

	/* */
	char *err = _fullpath(*to, *from, _MAX_PATH);
	if (*err == NULL) {
		success = false;
		goto F;
	}

F:
	return success;
}


int main(int argc, char *argv[])
{

	char *path = NULL;
	char *importText = NULL;
	char *noBOMtext = NULL;
	wchar_t *wideNoBOMtext = NULL;
	wchar_t *escapedNoBOMWideText = NULL;
	char *escapedNoBOMText = NULL;
	char* escapedText = NULL;

	/* */
	if (!getFullPath(argv, &path)) {
		printf("引数がおかしい");
		goto A;
	}
	else {
		printf("引数ok");
	}

	/* */
	if (!loadTextFromBinary(path,&importText)) {
		printf("ファイルからテキストを読み込めない");
		goto H;
	}
	else {
		printf("読み込みok");
	}

	/* */
	if (!removeBOM(importText,&noBOMtext)) {
		printf("BOMがついていないからUTF-8じゃない");
		goto B;
	}
	else {
		printf("BOM ok");
	}

	/* */
	if (!convertTextToWideText(noBOMtext, &wideNoBOMtext)) {
		printf("M->Wが変換できなかった");
		goto C;
	}
	else {
		printf("M->W ok");
	}

	/* */
	if (!convertWideTextToEscapedWideText(wideNoBOMtext,&escapedNoBOMWideText)) {
		printf("エスケープに失敗した");
		goto D;
	}
	else {
		printf("エスケープ ok");
	}

	/* */
	if (!convertWideTextToText(escapedNoBOMWideText, &escapedNoBOMText)) {
		printf("W->Mが変換できなかった");
		goto E;
	}
	else {
		printf("W->M ok");
	}

	/* */
	if (!attachBOM(escapedNoBOMText,&escapedText)) {
		printf("BOMをつけるのに失敗");
		goto F;
	}
	else {
		printf("BOM付け ok");
	}

	
	/* */
	if (!saveTextToBinary(path,escapedText)) {
		printf("ファイルをセーブするのに失敗");
		goto G;
	}
	else {
		printf("ファイルセーブ ok");
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



