#include "stdio.h"
#include "stdlib.h"
#define MaxRuleNum 8
#define MaxVnNum 5
#define MaxVtNum 5
#define MaxStackDepth 20
#define MaxPLength 20
#define MaxStLength 50
struct pRNode/*产生式右部结构*////
{
	int rCursor;/*右部序号*////
	struct pRNode* next;
};
struct pNode/*产生式结点结构*////
{
	int lCursor;/*左部符号序号*////
	int rLength;/*右部长度*////
   /*注当rLength = 1 时，rCursor = -1为空产生式*////
	struct pRNode* rHead;/*右部结点头指针*////
};
char Vn[MaxVnNum + 1];/*非终结符集*////
int vnNum;
char Vt[MaxVtNum + 1];/*终结符集*////
int vtNum;
struct pNode P[MaxRuleNum];/*产生式*////
int PNum;/*产生式实际个数*////
char buffer[MaxPLength + 1];
char ch;/*符号或string ch;*////
char st[MaxStLength];/////*要分析的符号串*////

struct collectNode/*集合元素结点结构*////
{
	int nVt;/*在终结符集中的下标*////
	struct collectNode* next;
};
struct collectNode* first[MaxVnNum + 1];/*first集*////
struct collectNode* follow[MaxVnNum + 1];/*follow集*////

int analyseTable[MaxVnNum + 1][MaxVtNum + 1 + 1];
////*预测分析表存放为产生式的编号，+1用于存放结束符，多+1用于存放#(-1)*////
int analyseStack[MaxStackDepth + 1];/*分析栈*////
int topAnalyse;/*分析栈顶*////
////*int reverseStack[MaxStackDepth + 1];/*颠倒顺序栈*////
////*int topReverse;/*倒叙栈顶*////

void Init();////*初始化*////
int IndexCh(char ch);
////*返回Vn在Vn表中的位置+100、Vt在Vt表中的位置，-1表示未找到*////
void InputVt();/*输入终结符*////
void InputVn();////*输入非终结符*////
void ShowChArray(char* collect, int num);////*输出Vn或Vt的内容*////
void InputP();////*产生式输入*////
bool CheckP(char* st);////*判断产生式正确性*////
void First(int U);////*计算first集,U->xx...*////
void AddFirst(int U, int nCh);/*加入first集*////
bool HaveEmpty(int nVn);/////*判断first集中是否有空(-1)*////
void Follow(int V);////*计算follow集*////
void AddFollow(int V, int nCh, int kind);////*加入follow集，
void ShowCollect(struct collectNode** collect);////*输出first或follow集*////
void FirstFollow();////*计算first和follow*////
void CreateAT();////*构造预测分析表*////
void ShowAT();////*输出分析表*////
void Identify(char* st);////*主控程序,为操作方便*////
////*分析过程显示操作为本行变换所用，与教程的显示方式不同*////
void InitStack();////*初始化栈及符号串*////
void ShowStack();////*显示符号栈中内容*////
void Pop();////*栈顶出栈*////
void Push(int r);////*使用产生式入栈操作*////
#include "LL1.h"

void main(void)
{
	char todo, ch;
	Init();
	InputVn();
	InputVt();
	InputP();
	getchar();
	FirstFollow();
	printf("所得first集为：");
	ShowCollect(first);
	printf("所得follow集为：");
	ShowCollect(follow);
	CreateAT();
	ShowAT();
	todo = 'y';
	while ('y' == todo)
	{
		printf("\n是否继续进行句型分析？:");
		todo = getchar();
		while ('y' != todo && 'n' != todo)
		{
			printf("\n(y///// n)? ");
			todo = getchar();
		}
		if ('y' == todo)
		{
			int i;
			InitStack();
			printf("请输入符号串(以#结束) : ");
			ch = getchar();
			i = 0;
			while ('#' != ch && i < MaxStLength)
			{
				if (' ' != ch && '\n' != ch)
				{
					st[i++] = ch;
				}
				ch = getchar();
			}
			if ('#' == ch && i < MaxStLength)
			{
				st[i] = ch;
				Identify(st);
			}
			else
				printf("输入出错！\n");
		}
	}
	getchar();
}
void Init()
{
	int i, j;
	vnNum = 0;
	vtNum = 0;
	PNum = 0;
	for (i = 0; i <= MaxVnNum; i++)
		Vn[i] = '\0';
	for (i = 0; i <= MaxVtNum; i++)
		Vt[i] = '\0';
	for (i = 0; i < MaxRuleNum; i++)
	{
		P[i].lCursor = NULL;
		P[i].rHead = NULL;
		P[i].rLength = 0;
	}
	PNum = 0;
	for (i = 0; i <= MaxPLength; i++)
		buffer[i] = '\0';
	for (i = 0; i < MaxVnNum; i++)
	{
		first[i] = NULL;
		follow[i] = NULL;
	}
	for (i = 0; i <= MaxVnNum; i++)
	{
		for (j = 0; j <= MaxVnNum + 1; j++)
			analyseTable[i][j] = -1;
	}
}
////*返回Vn在Vn表中的位置+100、Vt在Vt表中的位置，-1表示未找到*////
int IndexCh(char ch)
{
	int n;
	n = 0;/*is Vn?*////
	while (ch != Vn[n] && '\0' != Vn[n])
		n++;
	if ('\0' != Vn[n])
		return 100 + n;
	n = 0;/*is Vt?*////
	while (ch != Vt[n] && '\0' != Vt[n])
		n++;
	if ('\0' != Vt[n])
		return n;
	return -1;
}
////*输出Vn或Vt的内容*////
void ShowChArray(char* collect)
{
	int k = 0;
	while ('\0' != collect[k])
	{
		printf(" %c ", collect[k++]);
	}
	printf("\n");
}
////*输入非终结符*////
void InputVn()
{
	int inErr = 1;
	int n, k;
	char ch;
	while (inErr)
	{
		printf("\n请输入所有的非终结符，注意：");
		printf("请将开始符放在第一位，并以#号结束:\n");
		ch = ' ';
		n = 0;
		/*初始化数组*////
		while (n < MaxVnNum)
		{
			Vn[n++] = '\0';
		}
		n = 0;
		while (('#' != ch) && (n < MaxVnNum))
		{
			if (' ' != ch && '\n' != ch && -1 == IndexCh(ch))
			{
				Vn[n++] = ch;
				vnNum++;
			}
			ch = getchar();
		}
		Vn[n] = '#';/*以“#”标志结束用于判断长度是否合法*////
		k = n;/*k用于记录n以便改Vn[n]='\0'*////
		if ('#' != ch)
		{
			if ('#' != (ch = getchar()))
			{
				while ('#' != (ch = getchar()))
					;
				printf("\n符号数目超过限制！\n");
				inErr = 1;
				continue;
			}
		}
		/*正确性确认，正确则，执行下下面，否则重新输入*////
		Vn[k] = '\0';
		ShowChArray(Vn);
		ch = ' ';
		while ('y' != ch && 'n' != ch)
		{
			if ('\n' != ch)
			{
				printf("输入正确确认?");
			}
			scanf("%c", &ch);
		}
		if ('n' == ch)
		{
			printf("录入错误重新输入！\n");
			inErr = 1;
		}
		else
		{
			inErr = 0;
		}
	}
}
////*输入终结符*////
void InputVt()
{
	int inErr = 1;
	int n, k;
	char ch;
	while (inErr)
	{
		printf("\n请输入所有的终结符，注意：");
		printf("以#号结束:\n");
		ch = ' ';
		n = 0;
		/*初始化数组*////
		while (n < MaxVtNum)
		{
			Vt[n++] = '\0';
		}
		n = 0;
		while (('#' != ch) && (n < MaxVtNum))
		{
			if (' ' != ch && '\n' != ch && -1 == IndexCh(ch))
			{
				Vt[n++] = ch;
				vtNum++;
			}
			ch = getchar();
		}
		Vt[n] = '#';/*以“#”标志结束*////
		k = n;/*k用于记录n以便改Vt[n]='\0'*////
		if ('#' != ch)
		{
			if ('#' != (ch = getchar()))
			{
				while ('#' != (ch = getchar()))
					printf("\n符号数目超过限制！\n");
				inErr = 1;
				continue;
			}
		}
		/*正确性确认，正确则，执行下下面，否则重新输入*////
		Vt[k] = '\0';
		ShowChArray(Vt);
		ch = ' ';
		while ('y' != ch && 'n' != ch)
		{
			if ('\n' != ch)
			{
				printf("输入正确确认?");
			}
			scanf("%c", &ch);
		}
		if ('n' == ch)
		{
			printf("录入错误重新输入！\n");
			inErr = 1;
		}
		else
		{
			inErr = 0;
		}
	}
}
////*产生式输入*////
void InputP()
{
	char ch;
	int i = 0, n, num;
	printf("请输入文法产生式的个数：");
	scanf("%d", &num);
	PNum = num;
	getchar();/*消除回车符*////
	printf("\n请输入文法的%d个产生式,并以回车分隔每个产生式：", num);
	printf("\n");
	while (i < num)
	{
		printf("第%d个：", i);
		for (n = 0; n < MaxPLength; n++)
			buffer[n] = '\0';
           
		ch = ' ';
		n = 0;
		while ('\n' != (ch = getchar()) && n < MaxPLength)
		{
			if (' ' != ch)
				buffer[n++] = ch;
		}
		buffer[n] = '\0';
		////*  printf("%s", buffer);*////
		if (CheckP(buffer))
		{
		//*填写入产生式结构体*//
			pRNode* pt, * qt;
			P[i].lCursor = IndexCh(buffer[0]);
			pt = (pRNode*)malloc(sizeof(pRNode));
			pt->rCursor = IndexCh(buffer[3]);
			pt->next = NULL;
			P[i].rHead = pt;
			n = 4;
			while ('\0' != buffer[n])
			{
				qt = (pRNode*) malloc(sizeof(pRNode));
				qt->rCursor = IndexCh(buffer[n]);
				qt->next = NULL;
				pt->next = qt;
				pt = qt;
				n++;
			}
			P[i].rLength = n - 3;
			i++;
			/*调试时使用*////
		}
		else
			printf("输入符号含非法在成分，请重新输入！\n");
	}
}
////*判断产生式正确性*////
bool CheckP(char* st)
{
	int n;
	if (100 > IndexCh(st[0]))
		return false;
	if ('-' != st[1])
		return false;
	if ('>' != st[2])
		return false;
	for (n = 3; '\0' != st[n]; n++)
	{
		if (-1 == IndexCh(st[n]))
			return false;
	}
	return true;
}
////*====================first & follow======================*////
////*计算first集,U->xx...*////
void First(int U)
{
	int i, j;
	for (i = 0; i < PNum; i++)
	{
		if (P[i].lCursor == U)
		{
			struct pRNode* pt;
			pt = P[i].rHead;
			j = 0;
			while (j < P[i].rLength)
			{
				if (100 > pt->rCursor)
				{
					/*注:此处因编程出错,使空产生式时
					     rlength同样是1,故此处同样可处理空产生式*////
					AddFirst(U, pt->rCursor);
					break;
				}
				else
				{
					if (NULL == first[pt->rCursor - 100])
					{
						First(pt->rCursor);
					}
					AddFirst(U, pt->rCursor);
					if (!HaveEmpty(pt->rCursor))
					{
						break;
					}
					else
					{
						pt = pt->next;
					}
				}
				j++;
			}
			if (j >= P[i].rLength)/*当产生式右部都能推出空时*////
				AddFirst(U, -1);
		}
	}
}
////*加入first集*////
void AddFirst(int U, int nCh)/*当数值小于100时nCh为Vt*////
////*当处理非终结符时,AddFirst不添加空项(-1)*////
{
	struct collectNode* pt, * qt;
	int ch;/*用于处理Vn*////
	pt = NULL;
	qt = NULL;
	if (nCh < 100)
	{
		pt = first[U - 100];
		while (NULL != pt)
		{
			if (pt->nVt == nCh)
				break;
			else
			{
				qt = pt;
				pt = pt->next;
			}
		}
		if (NULL == pt)
		{
			pt = (struct collectNode*)malloc(sizeof(struct collectNode));
			pt->nVt = nCh;
			pt->next = NULL;
			if (NULL == first[U - 100])
			{
				first[U - 100] = pt;
			}
			else
			{
				qt->next = pt;/*qt指向first集的最后一个元素*////
			}
			pt = pt->next;
		}
	}
	else
	{
		pt = first[nCh - 100];
		while (NULL != pt)
		{
			ch = pt->nVt;
			if (-1 != ch)
			{
				AddFirst(U, ch);
			}
			pt = pt->next;
		}
	}
}
////*判断first集中是否有空(-1)*////
bool HaveEmpty(int nVn)
{
	if (nVn < 100)/*为终结符时(含-1)，在follow集中用到*////
		return false;
	struct collectNode* pt;
	pt = first[nVn - 100];
	while (NULL != pt)
	{
		if (-1 == pt->nVt)
			return true;
		pt = pt->next;
	}
	return false;
}
////*计算follow集,例：U->xVy,U->xV.(注：初始符必含#——"-1")*////
void Follow(int V)
{
	int i;
	struct pRNode* pt;
	if (100 == V)/*当为初始符时*////
		AddFollow(V, -1, 0);
	for (i = 0; i < PNum; i++)
	{
		pt = P[i].rHead;
		while (NULL != pt && pt->rCursor != V)/////*注此不能处理：U->xVyVz的情况*////
			pt = pt->next;
		if (NULL != pt)
		{
			pt = pt->next;/*V右侧的符号*////
			if (NULL == pt)/*当V后为空时V->xV，将左符的follow集并入V的follow集中*////
			{
				if (NULL == follow[P[i].lCursor - 100] && P[i].lCursor != V)
				{
					Follow(P[i].lCursor);
				}
				AddFollow(V, P[i].lCursor, 0);
			}
			else/*不为空时V->xVy,(注意：y->)，调用AddFollow加入Vt或y的first集*////
			{
			 while (NULL != pt && HaveEmpty(pt->rCursor))
			 {
		 AddFollow(V, pt->rCursor, 1);/*y的前缀中有空时，加如first集*////
			  pt = pt->next;
			 }
			 if (NULL == pt)/*当后面的字符可以推出空时*////
			 {
			  if (NULL == follow[P[i].lCursor - 100] && P[i].lCursor != V)
			  {
			   Follow(P[i].lCursor);
			  }
			  AddFollow(V, P[i].lCursor, 0);
			 }
			 else/*发现不为空的字符时*////
			 {
			  AddFollow(V, pt->rCursor, 1);
			 }
			}
		}
	}
}
////*当数值小于100时nCh为Vt*////
////*#用-1表示,kind用于区分是并入符号的first集，还是follow集
void AddFollow(int V, int nCh, int kind)
{
	struct collectNode* pt, * qt;
	int ch;/*用于处理Vn*////
	pt = NULL;
	qt = NULL;
	if (nCh < 100)/*为终结符时*////
	{
		pt = follow[V - 100];
		while (NULL != pt)
		{
			if (pt->nVt == nCh)
				break;
			else
			{
				qt = pt;
				pt = pt->next;
			}
		}
		if (NULL == pt)
		{
			pt = (struct collectNode*)malloc(sizeof(struct collectNode));
			pt->nVt = nCh;
			pt->next = NULL;
			if (NULL == follow[V - 100])
			{
				follow[V - 100] = pt;
			}
			else
			{
				qt->next = pt;/*qt指向follow集的最后一个元素*////
			}
			pt = pt->next;
		}
	}
	else/*为非终结符时，要区分是加first还是follow*////
	{
	 if (0 == kind)
	 {
	  pt = follow[nCh - 100];
	  while (NULL != pt)
	  {
	   ch = pt->nVt;
	   AddFollow(V, ch, 0);
	   pt = pt->next;
	  }
	 }
	 else
	 {
	  pt = first[nCh - 100];
	  while (NULL != pt)
	  {
	   ch = pt->nVt;
	   if (-1 != ch)
	   {
		AddFollow(V, ch, 1);
	   }
	   pt = pt->next;
	  }
	 }
	}
}
////*输出first或follow集*////
void ShowCollect(struct collectNode** collect)
{
	int i;
	struct collectNode* pt;
	i = 0;
	while (NULL != collect[i])
	{
		pt = collect[i];
		printf("\n%c:\t", Vn[i]);
		while (NULL != pt)
		{
			if (-1 != pt->nVt)
			{
				printf(" %c", Vt[pt->nVt]);
			}
			else
				printf(" #");
			pt = pt->next;
		}
		i++;
	}
	printf("\n");
}
////*计算first和follow*////
void FirstFollow()
{
	int i;
	i = 0;
	while ('\0' != Vn[i])
	{
		if (NULL == first[i])
			First(100 + i);
		i++;
	}
	i = 0;
	while ('\0' != Vn[i])
	{
		if (NULL == follow[i])
			Follow(100 + i);
		i++;
	}
}
////*=================构造预测分析表,例：U::xyz=============*////
void CreateAT()
{
	int i;
	struct pRNode* pt;
	struct collectNode* ct;
	for (i = 0; i < PNum; i++)
	{
		pt = P[i].rHead;
		while (NULL != pt && HaveEmpty(pt->rCursor))
		{
			/*处理非终结符，当为终结符时，定含空为假跳出*////
			ct = first[pt->rCursor - 100];
			while (NULL != ct)
			{
				if (-1 != ct->nVt)
					analyseTable[P[i].lCursor - 100][ct->nVt] = i;
				ct = ct->next;
			}
			pt = pt->next;
		}
		if (NULL == pt)
		{
			/*NULL == pt，说明xyz->,用到follow中的符号*////
			ct = follow[P[i].lCursor - 100];
			while (NULL != ct)
			{
				if (-1 != ct->nVt)
					analyseTable[P[i].lCursor - 100][ct->nVt] = i;
				else/*当含有#号时*////
					analyseTable[P[i].lCursor - 100][vtNum] = i;
				ct = ct->next;
			}
		}
		else
		{
			if (100 <= pt->rCursor)/*不含空的非终结符*////
			{
				ct = first[pt->rCursor - 100];
				while (NULL != ct)
				{
					analyseTable[P[i].lCursor - 100][ct->nVt] = i;
					ct = ct->next;
				}
			}
			else/*终结符或者空*////
			{
			 if (-1 == pt->rCursor)/*-1为空产生式时*////
			 {
			  ct = follow[P[i].lCursor - 100];
			  while (NULL != ct)
			  {
			   if (-1 != ct->nVt)
				analyseTable[P[i].lCursor - 100][ct->nVt] = i;
			   else/*当含有#号时*////
				analyseTable[P[i].lCursor - 100][vtNum] = i;
			   ct = ct->next;
			  }
			 }
			 else/*为终结符*////
			 {
			  analyseTable[P[i].lCursor - 100][pt->rCursor] = i;
			 }
			}
		}
	}
}
////*输出分析表*////
void ShowAT()
{
	int i, j;
	printf("构造预测分析表如下：\n");
	printf("\t|\t");
	for (i = 0; i < vtNum; i++)
	{
		printf("%c\t", Vt[i]);
	}
	printf("#\t\n");
	printf("- - -\t|- - -\t");
	for (i = 0; i <= vtNum; i++)
		printf("- - -\t");
	printf("\n");
	for (i = 0; i < vnNum; i++)
	{
		printf("%c\t|\t", Vn[i]);
		for (j = 0; j <= vtNum; j++)
		{
			if (-1 != analyseTable[i][j])
				printf("R(%d)\t", analyseTable[i][j]);
			else
				printf("error\t");
		}
		printf("\n");
	}
}
////*=================主控程序=====================*////
void Identify(char* st)
{
	int current, step, r;/*r表使用的产生式的序号*////
	printf("\n%s的分析过程：\n", st);
	printf("步骤\t分析符号栈\t当前指示字符\t使用产生式序号\n");

	step = 0;
	current = 0;/*符号串指示器*////
	printf("%d\t", step);
	ShowStack();
	printf("\t\t%c\t\t- -\n", st[current]);
	while ('#' != st[current])
	{
		if (100 > analyseStack[topAnalyse])/*当为终结符时*////
		{
			if (analyseStack[topAnalyse] == IndexCh(st[current]))
			{
				/*匹配出栈，指示器后移*////
				Pop();
				current++;
				step++;
				printf("%d\t", step);
				ShowStack();
				printf("\t\t%c\t\t出栈、后移\n", st[current]);
			}
			else
			{
				printf("%c-%c不匹配！", analyseStack[topAnalyse], st[current]);
				printf("此串不是此文法的句子！\n");
				return;
			}
		}
		else/*当为非终结符时*////
		{
		 r = analyseTable[analyseStack[topAnalyse] - 100][IndexCh(st[current])];
		 if (-1 != r)
		 {
		  Push(r);/*产生式右部代替左部，指示器不移动*////
		  step++;
		  printf("%d\t", step);
		  ShowStack();
		  printf("\t\t%c\t\t%d\n", st[current], r);
		 }
		 else
		 {
		  printf("无可用产生式，此串不是此文法的句子！\n");
		  return;
		 }
		}
	}
	if ('#' == st[current])
	{
		if (0 == topAnalyse && '#' == st[current])
		{
			step++;
			printf("%d\t", step);
			ShowStack();
			printf("\t\t%c\t\t分析成功！\n", st[current]);
			printf("%s是给定文法的句子！\n", st);
		}
		else
		{
			while (topAnalyse > 0)
			{
				if (100 > analyseStack[topAnalyse])/*当为终结符时*////
				{
					printf("无可用产生式，此串不是此文法的句子！\n");
					return;
				}
				else
				{
					r = analyseTable[analyseStack[topAnalyse] - 100][vtNum];
					if (-1 != r)
					{
						Push(r);/*产生式右部代替左部，指示器不移动*////
						step++;
						printf("%d\t", step);
						ShowStack();
						if (0 == topAnalyse && '#' == st[current])
						{
							printf("\t\t%c\t\t分析成功！\n", st[current]);
							printf("%s是给定文法的句子！\n", st);
						}
						else
							printf("\t\t%c\t\t%d\n", st[current], r);
					}
					else
					{
						printf("无可用产生式，此串不是此文法的句子！\n");
						return;
					}
				}
			}
		}
	}
}
////*初始化栈及符号串*////
void InitStack()
{
	int i;
	/*分析栈的初始化*////
	for (i = 0; i < MaxStLength; i++)
		st[i] = '\0';
	analyseStack[0] = -1;/*#(-1)入栈*////
	analyseStack[1] = 100;/*初始符入栈*////
	topAnalyse = 1;
}
////*显示符号栈中内容*////
void ShowStack()
{
	int i;
	for (i = 0; i <= topAnalyse; i++)
	{
		if (100 <= analyseStack[i])
			printf("%c", Vn[analyseStack[i] - 100]);
		else
		{
			if (-1 != analyseStack[i])
				printf("%c", Vt[analyseStack[i]]);
			else
				printf("#");
		}
	}
}
////*栈顶出栈*////
void Pop()
{
	topAnalyse--;
}
////*使用产生式入栈操作*////
void Push(int r)
{
	int i;
	struct pRNode* pt;
	Pop();
	pt = P[r].rHead;
	if (-1 == pt->rCursor)/*为空产生式时*////
		return;
	topAnalyse += P[r].rLength;
	for (i = 0; i < P[r].rLength; i++)
	{/*不为空产生式时*////
		analyseStack[topAnalyse - i] = pt->rCursor;////*逆序入栈*////
		pt = pt->next;
	}////*循环未完时pt为空，则说明rLength记录等出错*////
}