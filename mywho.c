#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <utmp.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

//设置标志位，用来标志命令行参数
bool opt_H = false;
bool opt_q = false;
bool opt_b = false;

/**mywho函数是调用系统数据文件的核心程序；
*它首先调用系统数据文件的接口函数，然后逐条将其数据保存下来；
*根据选项的标记为，然后输出不同的信息
*关闭系统数据文件
*/

static int mywho()
{
	/**在系统的数据文件中，提供两个文件 utmp 和 wtmp 两个文件
	* 这两个文件记录的数据结构就是utmp, 所以要声明一个utmp数据结构
	*/
	struct utmp *um;

	//用来保存方便人可读的时间字符串
	char timebuf[24];

	//当命令选型为 -q, 用来保存用户数量
	int users = 0;

	//处理命令行参数为 -q 的情况
	if (opt_q)
	{
		/** getutent 函数用来读下一条记录，如果需要，还会打开该文件。
		*返回一个指向 utmp 的指针，当已达文件尾端的时返回空指针。
		*/
		while ((um = getutent()))
		{
			/*  利用 utmp 结构的ut_type域，过滤出普通进程  */
			if (um->ut_type != USER_PROCESS)
				continue;
			printf("%-2s  ", um->ut_user);
			users += 1;
		}
		printf("\n# users=%d\n", users);
		// 关闭文件
		endutent();
		return 0;
	}

	//打印各栏标题头部
	if (opt_H)
		printf("%-12s%-12s%-20.20s  %s\n", "NAME", "LINE", "TIME", "COMMENT");

	/** 模拟 who -b 打印最后一次 boot 时间
	*这个要用到wtmp文件，所以要设置utmpname(_PATH_WTMP);
	*/
	if (opt_b) 
	{
		time_t tm;
		int n = 0;
		utmpname(_PATH_WTMP);

		//记录 wtmp 文件中的 boot 条目
		while (um = getutent())
		{
			/*  利用 utmp 结构的ut_type域，过滤出 boot 时间  */
			if (um->ut_type != BOOT_TIME)
				continue;
			n++;
		}

		//重新将 wtmp 文件反绕，从文件开始处读
		setutent();

		//读最后一个 boot 记录
		while (n--) 
		{
			um = getutent();

			//利用 utmp 结构的ut_type域，过滤出 boot 时间
			if (um->ut_type != BOOT_TIME)
			{
				n++;
				continue;
			}
		}

		// 时间转换成方便人读的格式
		tm = (time_t)(um->ut_tv.tv_sec);
		strftime(timebuf, 24, "%F %R", localtime(&tm));
		// 打印相关信息
		printf("system boot  %-20.20s\n", timebuf);

		//关闭数据文件
		endutent();
		exit(0);
	}

	//此处处理的是 utmp 文件的内容
	while ((um = getutent()))
	{
		// 利用 utmp 结构的ut_type域，过滤出普通进程  
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

	//分析命令行选项
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

	//命令行检错
	if (argc != optind)
		printf("fault command!\n");

	//调用mywho程序
	ok = mywho();

	if (!ok)
		return 0;
	else
		exit(1);
}

