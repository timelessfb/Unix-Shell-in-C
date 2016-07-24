#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <stdbool.h>

#define BUFFERSIZE 1024
#define COPYMORE 0644

/*���ڴ����Ŀ¼�ļ����Ƶ�Ŀ¼�ļ��Ĳ���������Ĳ���Ӧ����Ŀ¼·��*/
int copyD2D(char *src, char *dest);

/*���ڴ�����ļ����Ƶ��ļ��Ĳ���������Ĳ������ļ���*/
int copyF2F(char *src, char *dest);

/*�ж�filename�Ƿ���Ŀ¼��*/
bool isdir(char *filename);

/*�ַ�����ת*/
char *strrev(char *str);

/*main�������ڽ��������д������Ĳ��������ж��Ƿ���Ϲ淶��
Ȼ����ݲ�ͬ��������ò�ͬ���Ӻ�����*/
int 
main(int argc, char **argv)
{
	/*���-r/-Rѡ���ѡ�����ݹ鸴���ļ���*/
	bool opt_r = false;

	/*���-lѡ�-lѡ�������Ӳ����*/
	bool opt_l = false;

	/*���-sѡ�-sѡ���������������*/
	bool opt_s = false;

	/*���ڼ�¼Դ�ļ�*/
	char *src = NULL;

	/*���ڼ�¼Ŀ���ļ�*/
	char *dest = NULL;

	/*��¼ѡ����ַ�*/
	char c;

	/*ѭ����������в����е�ѡ��*/
	while ((c = getopt(argc, argv, "rRls")) != -1)
	{
		switch (c)
		{
		/*����-r,-R,�򽫱�ǵݹ�ظ���Ŀ¼���ѡ�opt_r����Ϊ��*/
		case 'R':
		case 'r':
			opt_r = true;
			break;

		/*����-l,�򽫱�Ǵ���Ӳ���ӵ�ѡ�opt_l����Ϊ��*/
		case 'l':
			opt_l = true;
			break;

		/*����-s,�򽫱�Ǵ����������ӵ�ѡ�opt_s����Ϊ��*/
		case 's':
			opt_s = true;
			break;
		}
	}

	/*�����в�����Ӧ���������ļ�������û�У��������ʾ������ֹ����*/
	if (optind >= argc - 1) {
		printf("ȱ�ٲ�����\n");
		exit(1);
	}

	/*�������в����ж�ȡԴ�ļ���Ŀ���ļ���*/
	src = argv[optind];
	dest = argv[optind + 1];
	
	/*����opt_lѡ�����٣�����Ӧ������
	��Ϊ�棬�򴴽�Ӳ���ӣ�ʹ��link������*/
	if (opt_l)
	{	
		if (isdir(src)) {
			printf("Ŀ¼���ܴ���Ӳ����\n");
			exit(1);
		}

		/*link �����ķ���ֵ�����ɹ����򷵻�0������������-1*/
		if ((link(src, dest)) == 0)
			return 0;
		else 
		{
			printf("����Ӳ����ʧ��\n");
			exit(1);
		}
	}

	/*����opt_sѡ�����٣�����Ӧ������
	��Ϊ�棬�򴴽��������ӣ�ʹ��symlink������*/
	if (opt_s)
	{
		if (isdir(src)) {
			printf("Ŀ¼���ܴ�����������\n");
			exit(1);
		}

		/*synlink �����ķ���ֵ�����ɹ����򷵻�0������������-1*/
		if ((symlink(src, dest)) == 0)
			return 0;
		else
		{
			printf("������������ʧ��\n");
			exit(1);
		}
	}


	if (!isdir(src))
	{	
		/*��Դ�ļ�src����Ŀ¼��ֱ�ӵ���copyF2F������*/
		if ((copyF2F(src, dest)) == 0)
			return 0;
		else 
		{
			printf("�����ļ�ʧ��\n");
			exit(1);
		}
	}

	else if (isdir(src))
	{
		if (!isdir(dest))
		{
			printf("���ܽ�һ��Ŀ¼���Ƶ�һ���ļ�\n");
			exit(1);
		}

		/*��Դ�ļ�src��Ŀ���ļ�dest����Ŀ¼��ֱ�ӵ���copyD2D������*/
		else if (isdir(dest) && opt_r)
		{
			if (copyD2D(src, dest) != 0) 
			{
				printf("Ŀ¼����ʧ��\n");
				exit(1);
			}
			else
				return 0;
		}

		else 
		{
			printf("����Ŀ¼��Ҫ��-rѡ��\n");
			exit(1);
		}
	}
	else 
	{
		printf("�ò������Ϸ�");
		exit(1);
	}

	return 0;
}

/*�ú������ڴ�����Ŀ¼�����*/
int 
copyD2D(char *src_dir, char *dest_dir)
{
	DIR *dp = NULL;
	struct dirent *dirp;
	char tempDest[256];
	char tempSrc[256];
	strcpy(tempDest, dest_dir);
	strcpy(tempSrc, src_dir);

	/*ʹ��opendir������src_dirĿ¼�����ָ���Ŀ¼���ֵ�ָ��*/
	if ((dp = opendir(src_dir)) == NULL)
		return 1;
	else
	{	
		/*ʹ��readdir������ȡdp��ָ��Ŀ¼��Ŀ¼����ָ����һ��dirent�ṹ��ָ��*/
		while ((dirp = readdir(dp)))
		{
			struct stat file_stat;
			if (!isdir(dirp->d_name))
			{	
				/*��dirent�ṹ�е�d_name��Ա�������ӵ��ϼ�Ŀ¼�ַ���*/
				strcat(tempDest, dirp->d_name);
				strcat(tempSrc, dirp->d_name);

				/*�˴�ת��Ϊ�ļ����ƺ����ķ�ʽ����Ŀ¼����*/
				copyF2F(tempSrc, tempDest);

				/*ͨ���ַ���������������tempDest��tempSrc��ԭΪ�ϼ���Ŀ¼��*/
				strcpy(tempDest, dest_dir);
				strcpy(tempSrc, src_dir);
			}
		}
		/*�ر�Ŀ¼*/
		closedir(dp);
		return 0;
	}
}

/*�ж�filename�Ƿ�ΪĿ¼�ļ�*/
bool 
isdir(char *filename)
{
	struct stat fileInfo;

	if (stat(filename, &fileInfo) >= 0)
		if (S_ISDIR(fileInfo.st_mode))
			return true;
		else 
			return false;
}

/*�ú���ͨ��read,write�Ȼ�����ϵͳ����������ļ��Ŀ�������*/
int 
copyF2F(char *src_file, char *dest_file)
{
	int in_fd, out_fd, n_chars;
	char buf[BUFFERSIZE];

	/*���Ŀ���ļ���һ��Ŀ¼����ôĬ�����ڸ�Ŀ¼�½���һ����Դ�ļ�ͬ�����ļ�*/
	if (isdir(dest_file)) 
	{	
		char c;
		char temp[10] = { '\0' };
		char *r_temp;
		int n = strlen(src_file);
		int m = 0;

		/*��ȡԴ�ļ������һ���ļ�����ΪĿ���ļ���*/
		while ((c = src_file[n - 1]) != '/') 
		{
			temp[m] = c;
			m++;
			n--;
		}
		r_temp = strrev(temp);
		strcat(dest_file, r_temp);
	}

	/* �Կɶ�ģʽ��Դ�ļ� */
	if ((in_fd = open(src_file, O_RDONLY)) == -1)
	{
		printf("%s�ļ���ȡʧ�ܣ�",src_file);
		return 1;
	
	}

	/* O_WRONLY�����Զ�д�ķ�ʽ��Ŀ���ļ���O_CREATѡ��������ļ��������򴴽�,
	COPYMORE = 0644���ļ������߿ɶ���д�������ɶ� */
	if ((out_fd = open(dest_file, O_WRONLY | O_CREAT, COPYMORE)) == -1)
		return 1;


	/* ͨ��read��writeϵͳ����ʵ���ļ��ĸ��� */
	while ((n_chars = read(in_fd, buf, BUFFERSIZE)) > 0)
	{
		if (write(out_fd, buf, n_chars) != n_chars)
		{
			printf("%s�ļ�дʧ�ܣ�", dest_file);
			return 1;
		}


		if (n_chars == -1)
		{
			printf("%s�ļ���ȡʧ�ܣ�", src_file);
			return 1;
		}
	}


	/* �ر��ļ� */
	if (close(in_fd) == -1 || close(out_fd) == -1)
	{
		printf("�ļ��ر�ʧ�ܣ�");
		return 1;
	}
	return 0;
}

/*�ַ�����ת*/
char * strrev(char *str)

{

	int i = strlen(str) - 1, j = 0;

	char ch;
	while (i>j)
	{
		ch = str[i];
		str[i] = str[j];
		str[j] = ch;
		i--;
		j++;
	}
	return str;
}