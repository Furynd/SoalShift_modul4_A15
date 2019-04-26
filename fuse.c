#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#include <sys/types.h>
#include <sys/stat.h>


static const char *dirpath = "/home/mrx/Documents";
char s[100]="qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";
int map[127];
int key=17;

void strr(char *s)
{
	char c;
	int len=strlen(s);
	int i;
    for(i=0; i<len/2; i++)
	{
		c=s[i];
		s[i]=s[len-i-1];
		s[len-i-1]=c;
	}
}


void crip(char *s1)
{
    for(int i=0; s1[i]!='\0'; i++)
		if(s1[i] != '/') s1[i]=s[ (map[(int)s1[i]]+key)%94 ];
}

void dcrip(char *s1)
{
    for(int i=0; s1[i]!='\0'; i++)
    	if(s1[i] != '/')
		{
			int nilai = (map[(int)s1[i]]-key)%94;
			while(nilai<0) nilai+=94;
			s1[i]=s[nilai];
		} 
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  	int res;
	char fpath[1000];
	char path_tmp[1000];
	strcpy(path_tmp,path);
    crip(path_tmp);

	sprintf(fpath,"%s%s",dirpath,path_tmp);
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
  char path_tmp[1000];
  strcpy(path_tmp,path);
  crip(path_tmp);

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path_tmp);
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) 
	{
		int flag=1;
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		strcpy(path_tmp,de->d_name);

		// Jika Bukan "." || ".." do->dcrip
		if(strcmp(de->d_name,".") && strcmp(de->d_name,".."))
		{
			char lok[1000];
			strcpy(lok,fpath);
			strcat(lok,"/");
			strcat(lok,path_tmp);

			// Nomor Tiga
			struct stat stats;
			stat(lok, &stats);
			struct tm dt;
			struct passwd *pw = getpwuid(stats.st_uid);
			struct group  *gr = getgrgid(stats.st_gid);
			
			 if( (!strcmp(pw->pw_name,"chipset") || !strcmp(pw->pw_name,"ic_controller")) 
           				  && !strcmp(gr->gr_name,"rusak") && !(stats.st_mode & R_OK))
			{
				flag=0;
				// Tulis log
				char miris[]="/filemiris.txt";
				crip(miris);
				dcrip(path_tmp);

				strcpy(lok,fpath);
				strcat(lok,miris);

				FILE *fptr;
				fptr = fopen(lok,"a");
				fprintf(fptr,"Nama File : %s\n",path_tmp);
				fprintf(fptr,"Owner_id : %d\n",stats.st_uid);
				fprintf(fptr,"Group_id : %d\n",stats.st_gid);
				fprintf(fptr,"Modified on: %d-%d-%d %d:%d:%d\n\n", dt.tm_mday, dt.tm_mon, dt.tm_year + 1900, 
																   dt.tm_hour, dt.tm_min, dt.tm_sec);
				fclose(fptr);

				// Hapus File Berbahaya
				remove(lok);
			}
		}

		if(flag)
		{
			if(strcmp(de->d_name,".") && strcmp(de->d_name,"..")) dcrip(path_tmp);
			res = (filler(buf,path_tmp, &st, 0));
			if(res!=0) break;
		}
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
  	char fpath[1000];
	char path_tmp[1000];
	strcpy(path_tmp,path);
	crip(path_tmp);

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path_tmp);
	int res = 0;
  	int fd = 0 ;

	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_write(const char *path, const char *buf, size_t size,off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
	char fpath[1000];
	char path_tmp[1000];
	strcpy(path_tmp,path);
	crip(path_tmp);

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path_tmp);

	(void) fi;
	fd = open(fpath, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];
	char path_tmp[1000];
	strcpy(path_tmp,path);
	crip(path_tmp);

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path_tmp);

	res = chmod(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	int res;
	char fpath[1000];
	char path_tmp[1000];
	strcpy(path_tmp,path);
	crip(path_tmp);

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path_tmp);

	res = lchown(fpath, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];
	char path_tmp[1000];
	strcpy(path_tmp,path);
	crip(path_tmp);

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path_tmp);
	res = mkdir(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *path)
{
	int res;
	char fpath[1000];
	char path_tmp[1000];
	strcpy(path_tmp,path);
	crip(path_tmp);

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path_tmp);
	res = rmdir(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi)
{

    (void) fi;

    int res;
	char fpath[1000];
	char path_tmp[1000];
	strcpy(path_tmp,path);
	crip(path_tmp);

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path_tmp);
    res = creat(fpath, mode);
    if(res == -1)
	return -errno;

    close(res);

    return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
	int res;
	char fpath[1000];
	char path_tmp[1000];
	strcpy(path_tmp,path);
	crip(path_tmp);

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path_tmp);
	res = truncate(fpath, size);
	
	// Mengambil namafile
	char namafile[100];
	memset(namafile,0,sizeof(namafile));
	int i;
	int j=0;
	for(i=strlen(path_tmp); path_tmp[i-1]!='/' && i>0; i--)
	{
		namafile[j]=path_tmp[i-1];
		j++;
	}
	strr(namafile);

	// Memotong lokasi fix
	char lokasi[1000];
	strncat(lokasi,fpath,strlen(fpath)-j);
	char backup[10]="Backup";
	crip(backup);
	strcat(lokasi,backup);

	// Membuat directory
	struct stat st = {0};
	if (stat(lokasi, &st) == -1) {
		mkdir(lokasi, 0777);
	}

	// Memindahkan file
	strcat(lokasi,"/");
	strcat(lokasi,namafile);
	rename(fpath,lokasi);
	

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *path)
{
	int res;
	char fpath[1000];
	char path_tmp[1000];
	strcpy(path_tmp,path);
	crip(path_tmp);

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path_tmp);
	res = unlink(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
	int res;
	char fr[1000];
	char too[1000];
	strcpy(fr,from);
	strcpy(too,to);
	crip(fr);
	crip(too);

	char d[1000];
	char s[1000];
	strcpy(d,dirpath);
	strcat(d,too);
	strcpy(s,dirpath);
	strcat(s,fr);

	res = rename(s,d);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	int res;
	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	char fpath[1000];
	char path_tmp[1000];
	strcpy(path_tmp,path);
	crip(path_tmp);

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path_tmp);

	res = utimes(fpath, tv);
	if (res == -1)
		return -errno;

	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.chmod		= xmp_chmod,
	.mkdir		= xmp_mkdir,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
	.create 	= xmp_create,
	.utimens	= xmp_utimens,

	.rmdir		= xmp_rmdir,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
	.read		= xmp_read,
	.write		= xmp_write,
	.unlink		= xmp_unlink,
	.rename		= xmp_rename,
};

int main(int argc, char *argv[])
{
	umask(0);
	// Mapping
	for(int i=0; s[i]!='\0'; i++) map[(int)s[i]]=i;

	return fuse_main(argc, argv, &xmp_oper, NULL);
}