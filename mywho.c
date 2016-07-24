#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <utmp.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

//���ñ�־λ��������־�����в���
bool opt_H = false;
bool opt_q = false;
bool opt_b = false;

/**mywho�����ǵ���ϵͳ�����ļ��ĺ��ĳ���
*�����ȵ���ϵͳ�����ļ��Ľӿں�����Ȼ�������������ݱ���������
*����ѡ��ı��Ϊ��Ȼ�������ͬ����Ϣ
*�ر�ϵͳ�����ļ�
*/

static int mywho()
{
	/**��ϵͳ�������ļ��У��ṩ�����ļ� utmp �� wtmp �����ļ�
	* �������ļ���¼�����ݽṹ����utmp, ����Ҫ����һ��utmp���ݽṹ
	*/
	struct utmp *um;

	//�������淽���˿ɶ���ʱ���ַ���
	char timebuf[24];

	//������ѡ��Ϊ -q, ���������û�����
	int users = 0;

	//���������в���Ϊ -q �����
	if (opt_q)
	{
		/** getutent ������������һ����¼�������Ҫ������򿪸��ļ���
		*����һ��ָ�� utmp ��ָ�룬���Ѵ��ļ�β�˵�ʱ���ؿ�ָ�롣
		*/
		while ((um = getutent()))
		{
			/*  ���� utmp �ṹ��ut_type�򣬹��˳���ͨ����  */
			if (um->ut_type != USER_PROCESS)
				continue;
			printf("%-2s  ", um->ut_user);
			users += 1;
		}
		printf("\n# users=%d\n", users);
		// �ر��ļ�
		endutent();
		return 0;
	}

	//��ӡ��������ͷ��
	if (opt_H)
		printf("%-12s%-12s%-20.20s  %s\n", "NAME", "LINE", "TIME", "COMMENT");

	/** ģ�� who -b ��ӡ���һ�� boot ʱ��
	*���Ҫ�õ�wtmp�ļ�������Ҫ����utmpname(_PATH_WTMP);
	*/
	if (opt_b) 
	{
		time_t tm;
		int n = 0;
		utmpname(_PATH_WTMP);

		//��¼ wtmp �ļ��е� boot ��Ŀ
		while (um = getutent())
		{
			/*  ���� utmp �ṹ��ut_type�򣬹��˳� boot ʱ��  */
			if (um->ut_type != BOOT_TIME)
				continue;
			n++;
		}

		//���½� wtmp �ļ����ƣ����ļ���ʼ����
		setutent();

		//�����һ�� boot ��¼
		while (n--) 
		{
			um = getutent();

			//���� utmp �ṹ��ut_type�򣬹��˳� boot ʱ��
			if (um->ut_type != BOOT_TIME)
			{
				n++;
				continue;
			}
		}

		// ʱ��ת���ɷ����˶��ĸ�ʽ
		tm = (time_t)(um->ut_tv.tv_sec);
		strftime(timebuf, 24, "%F %R", localtime(&tm));
		// ��ӡ�����Ϣ
		printf("system boot  %-20.20s\n", timebuf);

		//�ر������ļ�
		endutent();
		exit(0);
	}

	//�˴�������� utmp �ļ�������
	while ((um = getutent()))
	{
		// ���� utmp �ṹ��ut_type�򣬹��˳���ͨ����  
		if (um->ut_type != USER_PROCESS)
			continue;
		time_t tm;
		tm = (time_t)(um->ut_tv.tv_sec);
		strftime(timebuf, 24, "%F %R", localtime(&tm));
		printf("%-12s%-12s%-20.20s  (%s)\n", um->ut_user, um->ut_line, timebuf, um->ut_host);
	}
	endutent();
	return 0;
}

int main(int argc, char *argv[])
{
	int c;
	int ok;

	//����������ѡ��
	while ((c = getopt(argc, argv, "Hqb")) != -1)
	{
		switch (c)
		{
		case 'H':
			opt_H = true;
			break;
		case 'q':
			opt_q = true;
			break;
		case 'b':
			opt_b = true;
			break;
		default:
			exit(1);
		}
	}

	//�����м��
	if (argc != optind)
		printf("fault command!\n");

	//����mywho����
	ok = mywho();

	if (!ok)
		return 0;
	else
		exit(1);
}

