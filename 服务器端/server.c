#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <malloc.h>
#include <sys/wait.h>
#include <netdb.h>
#include <stdlib.h>
#include <pthread.h>

/* 定义端口号，须大于1024 */
#define PORT 8888

//#define BUF_SIZE 1024

/* 信息宏 */
#define GEN_TOTAL 1  /* 普通业务总人数 */ 
#define GEN_CUR  2  /* 普通业务当前服务的号码 */
#define VIP_TOTAL 3  /* vip业务总人数 */
#define VIP_CUR   4  /* vip业务当前服务的号码 */


#define FROM_GET_NUM  5 /* 为取号机创建线程 */
#define FROM_CALL_NUM  6 /* 为叫号机创建线程 */
#define LOGIN  7 /* 为登陆创建线程 */

/*用户名密码结构*/
typedef struct user
{
    char user_name[20];  /* 用户名 */
    char user_code[20];  /* 密码 */

}info_user;

info_user user;

typedef struct Node
{
	int data;	//数据域
	struct Node * pNext;	//指针域
}NODE, * PNODE;

typedef struct Queue
{
	PNODE front;	//队首指针
	PNODE rear;		//队尾指针
}QUEUE;
/* 存储服务人数信息结构定义 */
typedef struct
{
    int gen_total_num;  /* 普通业务总人数 */
    int gen_cur_num; /* 普通业务当前服务的号码 */

int vip_total_num; /* vip业务总人数 */
int vip_cur_num; /* vip业务当前服务的号码 */
}info_bank;

QUEUE genQ,vipQ;
info_bank info_people;
int qnum;   //队列人数


/* 线程执行函数负责读写 */
void *server_client( void *arg );
/* 初始化服务信息 */
void init_info(info_bank *info_people);

void save_info_getnum(info_bank *info_people, int operation); /*保存取号机发来的信息*/
void save_info_call(info_bank *info_people, int operation); /*保存叫号机发来的信息*/
void init_Queue(QUEUE * pQ);
void insert_Queue(QUEUE * pQ, int val);	//在队列中插入输入的元素
int delete_Queue(QUEUE * pQ);		//删除对首元素
int traverse_Queue(QUEUE * pQ);		//遍历+统计个数
int is_empty(QUEUE * pQ);			//判断队列是否为空
void clear_Queue(QUEUE * pQ);		//清空队列

int main(int argc, char *argv[])
{
    	socklen_t clt_addr_len;
	int listen_fd;
	int com_fd;
	int ret;
	int i;
	static char recv_buf;
	int len;


	pthread_t tid;

	struct sockaddr_in clt_addr;
	struct sockaddr_in srv_addr;

	init_info(&info_people);
	init_Queue(&genQ);//普通用户队列初始化
	init_Queue(&vipQ);//vip用户队列初始化

	/* 创建套接字用于服务器的监听 */
	listen_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0)
	{
	    perror("cannot create listening socket");
	return 1;
	}

	/* 填充关于服务器的套节字信息 */
	memset(&srv_addr, 0, sizeof(srv_addr));
	srv_addr.sin_family=AF_INET;
	srv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	srv_addr.sin_port=htons(PORT);


	/* 将服务器和套节字绑定 */
	ret = bind(listen_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
	if (ret == -1)
	{
	    perror("cannot bind server socket");
	close(listen_fd);
	return 1;
	}

	/* 监听指定端口，连接5个客户端 */
	ret = listen(listen_fd,5);
	if (ret == -1)
	{
	    perror("cannot listen the client connect request");
	close(listen_fd);
	return 1;
}

while(1)
{
    	len = sizeof(clt_addr);
	com_fd = accept(listen_fd, (struct sockaddr *)&clt_addr, &len);
	if (com_fd<0)
	{
	    if (errno == EINTR)
	{
	    continue;
	}
	else
	{
	    perror("cannot accept client connect request");
	close(listen_fd);
	return 1;
	}
}	
	/* 打印建立连接的客户端产生的套节字 */
	printf("com_fd=%d\n", com_fd);
	if ((pthread_create(&tid,NULL,server_client,&com_fd))==-1)
	{
	    perror("pthread_create error");
	close(listen_fd);
	close(com_fd);
	return 1;
	}
	}
	return 0;
}


//线程执行函数负责读写
void *server_client(void *arg)
{
	int size,j;
	int operation;
	    int server_type,recv_buf,call_buf;
	int *parg = (int *)arg;
	int new_fd = *parg;
	
	printf("new_fd=%d\n",new_fd);

	/* 每创建一个线程时：读取一个数字，确认为哪个客户端服务 */
   	 read(new_fd,&server_type,sizeof(server_type));

if (server_type == FROM_GET_NUM) 
{
	
	qnum = 0;
	printf("operation machine that get number \n");
	read(new_fd,&recv_buf,sizeof(recv_buf));

	operation = recv_buf;

	//printf("operation = %d\n", operation);
	//printf("Message from client recv_buf = %d\n",recv_buf);

	save_info_getnum(&info_people, operation);
	/*把当前排队人数发送给取号机*/
	
	write(new_fd,&qnum,sizeof(int));
	/* 把保存人数信息的结构体发送跟取号端 */
        //write(new_fd, &info_people, sizeof(info_people));
	
	printf("qum = %d", qnum);
	sleep(2);	
	
}
	/* 处理叫号机 */

if (server_type == FROM_CALL_NUM)	
	{
	while(1)
	{
		    printf("AAAAA\n");
		    read(new_fd,&call_buf,sizeof(call_buf));
	operation = call_buf;
	printf("operation = %d\n", operation);
	printf("Message from client call_buf: %d\n",call_buf);


	save_info_call(&info_people, operation);
	write(new_fd, &info_people, sizeof(info_people));	
	sleep(2);

	}

	}

	close(new_fd); // 杀掉一个进程后就全部死掉的原因在这吗？？？？？？？？。
	return 0;
}

void init_info(info_bank *info_people)
{
	//int Qnum;
	(*info_people).gen_total_num = 0;
	(*info_people).gen_cur_num = 0;
	(*info_people).vip_total_num = 0;
	(*info_people).vip_cur_num = 0;

	printf("gen_total_num = %d \n", (*info_people).gen_total_num);
	printf("gen_cur_num = %d \n", (*info_people).gen_cur_num);
	printf("vip_total_num = %d \n", (*info_people).vip_total_num);
	printf("vip_cur_num = %d \n", (*info_people).vip_cur_num);
	}

	/*保存取号机发来的信息*/
	void save_info_getnum(info_bank *info_people, int operation)
	{
		switch (operation)
		{
		case GEN_TOTAL:
			(*info_people).gen_total_num++;
			insert_Queue(&genQ,(*info_people).gen_total_num);
			qnum = traverse_Queue(&genQ)+traverse_Queue(&vipQ)-1;
		break;

		case VIP_TOTAL:
			(*info_people).vip_total_num++;
			insert_Queue(&vipQ,(*info_people).vip_total_num);
			qnum = traverse_Queue(&vipQ)-1;
		break;
		

		default:
		printf("not this operation \n");

		}
	/*
	    printf("gen_total_num = %d \n", (*info_people).gen_total_num);
	    printf("gen_cur_num = %d \n", (*info_people).gen_cur_num);
	    printf("vip_total_num = %d \n", (*info_people).vip_total_num);
	    printf("vip_cur_num = %d \n", (*info_people).vip_cur_num);
	*/
	
	

    

}


/*保存叫号机发来的信息*/
void save_info_call(info_bank *info_people, int operation)
{
	switch (operation)
	{
	case GEN_CUR:
	(*info_people).gen_cur_num++;
	break;

	case VIP_CUR:
	(*info_people).vip_cur_num++;
	break;
	default:
	printf("not this operation \n");

	}
	    printf("gen_total_num = %d \n", (*info_people).gen_total_num);
	printf("gen_cur_num = %d \n", (*info_people).gen_cur_num);
	printf("vip_total_num = %d \n", (*info_people).vip_total_num);
	printf("vip_cur_num = %d \n", (*info_people).vip_cur_num);
}

void init_Queue(QUEUE * pQ)
{
	//为头结点分配内存
	pQ->front = (PNODE)malloc(sizeof(NODE));
	if(pQ->front==NULL)
	{
		printf("内存分配失败，安全退出!\n");
		exit(-1);
	}
	else
	{
		pQ->rear = pQ->front;
		pQ->rear->pNext = NULL;
		printf("链式队列创建成功！\n");
	}

}

void insert_Queue(QUEUE * pQ, int val)
{
	int k = val;	
	//申请一个结点来存放数据
	PNODE pNew = (PNODE)malloc(sizeof(NODE));
	pNew->data = val;
	pQ->rear->pNext = pNew;
	pQ->rear = pNew;
	printf("当前号码 = %d\n", k);	

}

int delete_Queue(QUEUE * pQ)
{
	int number = -1;  //队首出来的号码
	if(is_empty(pQ))
	{
		printf("队列已空，无法出桟！\n");
		exit(-1);
	}
	else
	{
		//定义龙套指针，来释放出战单位内存
		PNODE r = (PNODE)malloc(sizeof(NODE));
		r = pQ->front->pNext;
		//printf("出队元素是：%d\n",r->data);
		number = r->data;		
		
		pQ->front->pNext = r->pNext;
		//释放内存
		free(r);
	
	}
	return number;	
}

int traverse_Queue(QUEUE * pQ)
{
	int num = 0;
	if(is_empty(pQ))
	{
		printf("队列已空，无法出桟！\n");
		//return num;
	}
	else
	{

		//龙套指针
		PNODE p = (PNODE)malloc(sizeof(NODE));
		p = pQ->front->pNext;
		while(p!=pQ->rear)
		{
			//printf("%d ",p->data);
			//龙套指针上移
			p = p->pNext;
			num++;
		}
		printf("%d ",p->data);
		num++;
	}

	printf("\n");
	return num;

}

int is_empty(QUEUE * pQ)
{
	if(pQ->front==pQ->rear)
		return 1;
	else
		return 0;

}

void clear_Queue(QUEUE * pQ)
{
	
	while(pQ->front!=pQ->rear)
	{
		//龙套指针
		PNODE p = (PNODE)malloc(sizeof(NODE));
		p = pQ->front->pNext;
		pQ->front->pNext = p->pNext;
		free(p);

	}
	
	printf("队列清空完成！\n");
}


