#include <stdio.h>
#include <getopt.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/stat.h>

#define CH_ATIME 1
#define CH_MTIME 2

//���崴���ļ�ʱ��ģʽ���˴����û����飬�������õ�Ȩ�޶��ǿɶ���д��
#define MODE_RW_UGO (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

//��־�ļ�access time �� modify time�ĸı����
static int change_times;

// �����(-c)ѡ����Ҳ�������������������ļ������򲻴��� 
static bool no_create;

//�������µ�access time �� modify time��ʱ��ʹ��
static struct timespec newtime[2];

//mytouch������ĵĺ���ģ�飬���ڴ������߸����ļ���ʱ�����
static bool
mytouch(const char *file)
{
	bool ok;
	int fd = -1;
	if (!no_create)
	{
		fd = open(file, O_CREAT | O_WRONLY, MODE_RW_UGO);
	}
	//���������У����û�м�⵽��-a����-m������change_times == (CH_ATIME | CH_MTIME)��\
	������ѡ��ֻ�øı�����һ��ʱ�����
	if (change_times != (CH_ATIME | CH_MTIME))
	{
		//ֻ�趨����һ��ʱ��ֵ��
		if (change_times == CH_MTIME)
			//���change_times == CH_MTIME������-m��,����Ӧ���޸�access time\
			��timespec�ṹ��tv_nsec����ΪUTIME_OMIT;�ο�utimensat�������÷�
		    newtime[0].tv_nsec = UTIME_OMIT;
		else
			//���change_times == CH_MTIME������-a��,����Ӧ���޸�modify time\
			��timespec�ṹ��tv_nsec����ΪUTIME_OMIT;�ο�utimensat�������÷�
			newtime[1].tv_nsec = UTIME_OMIT;
	}
		//AT_FDCWD��ʾ��ǰ����Ŀ¼��
        ok = (utimensat(AT_FDCWD, file, newtime, 0) == 0);
	return true;
}

int
main(int argc, char **argv)
{
	int c;
	bool ok = true;
	change_times = 0;
	no_create = false;
	//���������еõ������ѡ�����'-'��ͷ�Ĳ�����Ŀǰֻ֧������ѡ��-a, -c, -m��
	while ((c = getopt(argc, argv, "acm")) != -1)
	{
		switch (c)
		{
		case 'a':
			change_times |= CH_ATIME;
			break;

		case 'c':
			no_create = true;
			break;

		case 'm':
			change_times |= CH_MTIME;
			break;

		default:
			printf("fault option!");
		}
	}

	if (change_times == 0)
		change_times = CH_ATIME | CH_MTIME;
	//��newtime[0].
    newtime[0].tv_nsec = UTIME_NOW;
    newtime[1].tv_nsec = UTIME_NOW;
	//���optind == argc�������������ļ����֡�
	if (optind == argc)
	{
		printf("missing file operand\n");
	} 
	//��Զ���ļ����ֵ���mytouch����
	for (; optind < argc; ++optind)
		ok &= mytouch(argv[optind]);

	exit(ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
