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

/* ����˿ںţ������1024 */
#define PORT 8888

//#define BUF_SIZE 1024

/* ��Ϣ�� */
#define GEN_TOTAL 1  /* ��ͨҵ�������� */ 
#define GEN_CUR  2  /* ��ͨҵ��ǰ����ĺ��� */
#define VIP_TOTAL 3  /* vipҵ�������� */
#define VIP_CUR   4  /* vipҵ��ǰ����ĺ��� */


#define FROM_GET_NUM  5 /* Ϊȡ�Ż������߳� */
#define FROM_CALL_NUM  6 /* Ϊ�кŻ������߳� */
#define LOGIN  7 /* Ϊ��½�����߳� */

/*�û�������ṹ*/
typedef struct user
{
    char user_name[20];  /* �û��� */
    char user_code[20];  /* ���� */

}info_user;

info_user user;

typedef struct Node
{
	int data;	//������
	struct Node * pNext;	//ָ����
}NODE, * PNODE;

typedef struct Queue
{
	PNODE front;	//����ָ��
	PNODE rear;		//��βָ��
}QUEUE;
/* �洢����������Ϣ�ṹ���� */
typedef struct
{
    int gen_total_num;  /* ��ͨҵ�������� */
    int gen_cur_num; /* ��ͨҵ��ǰ����ĺ��� */

int vip_total_num; /* vipҵ�������� */
int vip_cur_num; /* vipҵ��ǰ����ĺ��� */
}info_bank;

QUEUE genQ,vipQ;
info_bank info_people;
int qnum;   //��������


/* �߳�ִ�к��������д */
void *server_client( void *arg );
/* ��ʼ��������Ϣ */
void init_info(info_bank *info_people);

void save_info_getnum(info_bank *info_people, int operation); /*����ȡ�Ż���������Ϣ*/
void save_info_call(info_bank *info_people, int operation); /*����кŻ���������Ϣ*/
void init_Queue(QUEUE * pQ);
void insert_Queue(QUEUE * pQ, int val);	//�ڶ����в��������Ԫ��
int delete_Queue(QUEUE * pQ);		//ɾ������Ԫ��
int traverse_Queue(QUEUE * pQ);		//����+ͳ�Ƹ���
int is_empty(QUEUE * pQ);			//�ж϶����Ƿ�Ϊ��
void clear_Queue(QUEUE * pQ);		//��ն���

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
	init_Queue(&genQ);//��ͨ�û����г�ʼ��
	init_Queue(&vipQ);//vip�û����г�ʼ��

	/* �����׽������ڷ������ļ��� */
	listen_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0)
	{
	    perror("cannot create listening socket");
	return 1;
	}

	/* �����ڷ��������׽�����Ϣ */
	memset(&srv_addr, 0, sizeof(srv_addr));
	srv_addr.sin_family=AF_INET;
	srv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	srv_addr.sin_port=htons(PORT);


	/* �����������׽��ְ� */
	ret = bind(listen_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
	if (ret == -1)
	{
	    perror("cannot bind server socket");
	close(listen_fd);
	return 1;
	}

	/* ����ָ���˿ڣ�����5���ͻ��� */
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
	/* ��ӡ�������ӵĿͻ��˲������׽��� */
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


//�߳�ִ�к��������д
void *server_client(void *arg)
{
	int size,j;
	int operation;
	    int server_type,recv_buf,call_buf;
	int *parg = (int *)arg;
	int new_fd = *parg;
	
	printf("new_fd=%d\n",new_fd);

	/* ÿ����һ���߳�ʱ����ȡһ�����֣�ȷ��Ϊ�ĸ��ͻ��˷��� */
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
	/*�ѵ�ǰ�Ŷ��������͸�ȡ�Ż�*/
	
	write(new_fd,&qnum,sizeof(int));
	/* �ѱ���������Ϣ�Ľṹ�巢�͸�ȡ�Ŷ� */
        //write(new_fd, &info_people, sizeof(info_people));
	
	printf("qum = %d", qnum);
	sleep(2);	
	
}
	/* ����кŻ� */

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

	close(new_fd); // ɱ��һ�����̺��ȫ��������ԭ�������𣿣���������������
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

	/*����ȡ�Ż���������Ϣ*/
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


/*����кŻ���������Ϣ*/
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
	//Ϊͷ�������ڴ�
	pQ->front = (PNODE)malloc(sizeof(NODE));
	if(pQ->front==NULL)
	{
		printf("�ڴ����ʧ�ܣ���ȫ�˳�!\n");
		exit(-1);
	}
	else
	{
		pQ->rear = pQ->front;
		pQ->rear->pNext = NULL;
		printf("��ʽ���д����ɹ���\n");
	}

}

void insert_Queue(QUEUE * pQ, int val)
{
	int k = val;	
	//����һ��������������
	PNODE pNew = (PNODE)malloc(sizeof(NODE));
	pNew->data = val;
	pQ->rear->pNext = pNew;
	pQ->rear = pNew;
	printf("��ǰ���� = %d\n", k);	

}

int delete_Queue(QUEUE * pQ)
{
	int number = -1;  //���׳����ĺ���
	if(is_empty(pQ))
	{
		printf("�����ѿգ��޷����C��\n");
		exit(-1);
	}
	else
	{
		//��������ָ�룬���ͷų�ս��λ�ڴ�
		PNODE r = (PNODE)malloc(sizeof(NODE));
		r = pQ->front->pNext;
		//printf("����Ԫ���ǣ�%d\n",r->data);
		number = r->data;		
		
		pQ->front->pNext = r->pNext;
		//�ͷ��ڴ�
		free(r);
	
	}
	return number;	
}

int traverse_Queue(QUEUE * pQ)
{
	int num = 0;
	if(is_empty(pQ))
	{
		printf("�����ѿգ��޷����C��\n");
		//return num;
	}
	else
	{

		//����ָ��
		PNODE p = (PNODE)malloc(sizeof(NODE));
		p = pQ->front->pNext;
		while(p!=pQ->rear)
		{
			//printf("%d ",p->data);
			//����ָ������
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
		//����ָ��
		PNODE p = (PNODE)malloc(sizeof(NODE));
		p = pQ->front->pNext;
		pQ->front->pNext = p->pNext;
		free(p);

	}
	
	printf("���������ɣ�\n");
}


