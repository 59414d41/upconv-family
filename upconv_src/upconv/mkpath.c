//---------------------------------------------------------------------------
// Function   : _splitpath
// Description: パスを分割する
// ---
//
#include <stdio.h>
#include <string.h>

void _splitpath(const char *path,char *drive,char *dir,char *fname,char *ext)
{
	char *p1,*p2;

	if (drive != NULL) drive[0] = '\0';
	if (dir   != NULL) dir[0] = '\0';
	if (fname != NULL) fname[0] = '\0';
	if (ext   != NULL) ext[0] = '\0';
	
	if (!(path == NULL || strlen(path) == 0)) {
		if (dir != NULL) {
			strcpy(dir,path);
			p1 = dir;
			do {
				p1 = strchr(p1,'\\');
				if (p1 != NULL) {
					*p1 = '/';
					p1++;
					if (strlen(p1) < 1) break;
				} else {
					break;
				}
			} while (1);
			p1 = strrchr(dir,'/');
			if (p1 != NULL && dir != p1) {
				if (strlen(p1 + 1) > 0) p1[1] = '\0';
			} else {
				dir[0] = '/';
				dir[1] = '\0';
			}
		}

		if (fname != NULL) {
			p1 = strrchr(path,'\\');
			if (p1 != NULL && strlen(p1 + 1) > 0) {
				strcpy(fname,p1 + 1);
			} else {
				p1 = strrchr(path,'/');
				if (p1 != NULL && strlen(p1 + 1) > 0) {
					strcpy(fname,p1 + 1);
				} else if (p1 == NULL) {
					strcpy(fname,path);
				}
			}
			p1 = strrchr(fname,'.');
			if (p1 != NULL) {
				*p1 = '\0';
			}
		}
		if (ext != NULL) {
			p1 = strrchr(path,'\\');
			if (p1 == NULL) {
				p1 = strrchr(path,'/');
				if (p1 == NULL) {
					p1 = path;
				} else {
					p1++;
				}
			} else {
				p1++;
			}
			if (strlen(p1) > 0) {
				p2 = strrchr(p1,'.');
				if (p2 != NULL) {
					strcpy(ext,p2);
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
// Function   : _makepath
// Description: パスを結合する
// ---
//
void _makepath(char *path,const char *drive,const char *dir,const char *fname,const char *ext)
{
	int len;
	if (path != NULL) {
		path[0] = '\0';
		
		if (drive != NULL) {
			strcat(path,drive);
		}
		if (dir != NULL) {
			strcat(path,dir);
			len = strlen(path);
			if (len > 0 && path[len - 1] != '/') {
				strcat(path,"/");
			}
		}
		if (fname != NULL) {
			strcat(path,fname);
		}
		if (ext != NULL) {
			strcat(path,ext);
		}
	}
}

void main()
{
	static char filename[256];
	static char drive[256];
	static char dir[256];
	static char fname[256];
	static char ext[256];

	strcpy(filename,"/a.txt");
	_splitpath(filename,drive,dir,fname,ext);
	printf("filename:%s,%s,%s,%s\n",drive,dir,fname,ext);
	_makepath(filename,drive,dir,fname,ext);
	printf("mk filename:%s\n",filename);

	strcpy(filename,"/a");
	_splitpath(filename,drive,dir,fname,ext);
	printf("filename:%s,%s,%s,%s\n",drive,dir,fname,ext);
	_makepath(filename,drive,dir,fname,ext);
	printf("mk filename:%s\n",filename);

	strcpy(filename,"/a/b/c/d.txt");
	_splitpath(filename,drive,dir,fname,ext);
	printf("filename:%s,%s,%s,%s\n",drive,dir,fname,ext);
	_makepath(filename,drive,dir,fname,ext);
	printf("mk filename:%s\n",filename);

	strcpy(filename,"/a/b/c/");
	_splitpath(filename,drive,dir,fname,ext);
	printf("filename:%s,%s,%s,%s\n",drive,dir,fname,ext);
	_makepath(filename,drive,dir,fname,ext);
	printf("mk filename:%s\n",filename);

	strcpy(filename,"a.txt");
	_splitpath(filename,drive,dir,fname,ext);
	printf("filename:%s,%s,%s,%s\n",drive,dir,fname,ext);
	_makepath(filename,drive,dir,fname,ext);
	printf("mk filename:%s\n",filename);

}

