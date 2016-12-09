#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>

#define PORT 8888
#define SERVER_IP "192.168.11.215"

#define LOGIN  7 /* 为登陆创建线程 */

GtkWidget *client_window;  //登陆窗口
GtkWidget *callnum_window; //柜员机窗口
GtkWidget *check_window;   //经理检查系统

GtkWidget *client_vbox;
GtkWidget *callnum_vbox;
GtkWidget *check_vbox;
GtkWidget *client_entry1, *client_entry2;

/*定义一个用户名密码的结构体*/
typedef struct
{
	char name[20];
	char code[20];
}info_user;

info_user user;

void on_clicked_button(GtkWidget *widget, gpointer data)
{
	gchar text1[20],text2[20];
	
	strcpy(text1, gtk_entry_get_text(GTK_ENTRY(client_entry1)));
	strcpy(text2, gtk_entry_get_text(GTK_ENTRY(client_entry2)));
	
	int connect_fd;
	int ret;
	int snd_buf;
	int i;
	int j;

	//int port;
	int len;

	static struct sockaddr_in srv_addr;
	//init_info(&user);
	
	strcpy(user.name,text1);
    strcpy(user.code,text2);
	
	/*创建套节字用于客户端的连接 */
	connect_fd=socket(PF_INET, SOCK_STREAM, 0);
	if (connect_fd < 0)
	{
    	perror("cannot create communication socket");
		//return 1;
	}
	
	/* 填充关于服务器的套节字信息 */
	memset(&srv_addr, 0, sizeof(srv_addr));
	srv_addr.sin_family=AF_INET;
	srv_addr.sin_addr.s_addr=inet_addr(SERVER_IP);
	srv_addr.sin_port=htons(PORT);
	
	/*连接指定的服务器 */
	ret=connect(connect_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
	if(ret==-1)
	{
    	perror("cannot connect to the server");
		close(connect_fd);
		//return 1;
	}
	
	/* 发送一个宏给服务器，服务器收到宏后就知道是登陆端发来的 */
	snd_buf =LOGIN;
	write(connect_fd,&snd_buf,sizeof(snd_buf));
	//memset(snd_buf, 0, BUF_SIZE);
	
	write(connect_fd, &user,sizeof(user));
    
    int a;
    /* 接收服务器发来的判断用户类型的关键字 */
	read(connect_fd,&a,sizeof(a));
	
	close(connect_fd);
	
	if(a==1)
	{
		g_print("%s\n",user.name);
		g_print("%s\n",user.code);
	
		gtk_widget_destroy(client_window);
		gtk_container_add(GTK_CONTAINER(callnum_window),callnum_vbox);
		gtk_widget_show_all(callnum_window);
		gtk_main();
	
		gtk_main_quit();
	}
	
	else if(a==2)
	{
		g_print("%s\n",user.name);
		g_print("%s\n",user.code);
	
		gtk_widget_destroy(client_window);
		gtk_container_add(GTK_CONTAINER(check_window),check_vbox);
		gtk_widget_show_all(check_window);
		gtk_main();
	
		gtk_main_quit();
	}
	
	else
		
	  gtk_main_quit();
	
}

int main(int argc ,char *argv[])
{
	//编辑窗口
	/*GtkWidget *client_window;  //登陆窗口
	GtkWidget *callnum_window; //柜员机窗口
	GtkWidget *check_window;   //经理检查系统*/
	
	GtkWidget *client_hbox1,*client_hbox2,*client_hbox3;
	
	/*GtkWidget *client_vbox;
	GtkWidget *callnum_vbox;
	GtkWidget *check_vbox;*/
	
	GtkWidget *client_button1,*client_button2;
	GtkWidget *callnum_button;
	GtkWidget *check_button;
	
	GtkWidget *client_label1, *client_label2;
	GtkWidget *callnum_label;
	GtkWidget *check_label;
	
	GtkWidget *check_view;
	GtkTextBuffer *check_buffer;
	GtkWidget *check_fixed;

	gtk_init(&argc ,&argv);

	client_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	callnum_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	check_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	//登陆界面布局
	
	gtk_window_set_title(GTK_WINDOW(client_window),"登陆");
	gtk_window_set_position(GTK_WINDOW(client_window),GTK_WIN_POS_CENTER);
	gtk_widget_set_usize(client_window,200,100);

	client_vbox = gtk_vbox_new(FALSE,0);
	client_hbox1 = gtk_hbox_new(FALSE,0);
	client_hbox2 = gtk_hbox_new(FALSE,0);
	client_hbox3 = gtk_hbox_new(FALSE,0);

	client_button1 = gtk_button_new_with_label("登陆");
	client_button2 = gtk_button_new_with_label("取消");

	gtk_container_add(GTK_CONTAINER(client_window),client_vbox);

	gtk_box_pack_start(GTK_BOX(client_vbox),client_hbox1,TRUE,TRUE,3);
	gtk_box_pack_start(GTK_BOX(client_vbox),client_hbox2,TRUE,TRUE,3);
	gtk_box_pack_start(GTK_BOX(client_vbox),client_hbox3,TRUE,TRUE,3);

	client_entry1 = gtk_entry_new();
	client_label1 = gtk_label_new("柜员：");
	client_entry2 = gtk_entry_new();
	client_label2 = gtk_label_new("密码：");
	
	gtk_entry_set_visibility(GTK_ENTRY(client_entry2),FALSE);

	gtk_box_pack_start(GTK_BOX(client_hbox1),client_label1,TRUE,TRUE,3);
	gtk_box_pack_start(GTK_BOX(client_hbox1),client_entry1,TRUE,TRUE,3);

	gtk_box_pack_start(GTK_BOX(client_hbox2),client_label2,TRUE,TRUE,3);
	gtk_box_pack_start(GTK_BOX(client_hbox2),client_entry2,TRUE,TRUE,3);
	
	gtk_box_pack_start(GTK_BOX(client_hbox3),client_button1,TRUE,TRUE,3);
	gtk_box_pack_start(GTK_BOX(client_hbox3),client_button2,TRUE,TRUE,3);
	
	//柜员机界面布局

	gtk_window_set_title(GTK_WINDOW(callnum_window),"柜员机");
	gtk_window_set_position(GTK_WINDOW(callnum_window),GTK_WIN_POS_CENTER);
	gtk_widget_set_usize(callnum_window,200,100);
	
	callnum_vbox = gtk_vbox_new(FALSE,0);
	callnum_button = gtk_button_new_with_label("叫号");
	callnum_label = gtk_label_new("1号普通用户正在办理业务.");
		
	gtk_box_pack_start(GTK_BOX(callnum_vbox),callnum_button,TRUE,TRUE,3);
	gtk_box_pack_start(GTK_BOX(callnum_vbox),callnum_label,TRUE,TRUE,3);
	
	//经理检查系统界面布局
	
	gtk_window_set_title(GTK_WINDOW(check_window),"经理检查系统");
	gtk_window_set_position(GTK_WINDOW(check_window),GTK_WIN_POS_CENTER);
	gtk_widget_set_usize(check_window,500,500);
	gtk_container_set_border_width(GTK_CONTAINER(check_window),40);
	
	check_vbox = gtk_vbox_new(FALSE,0);
	check_label = gtk_label_new("统计信息如下");
	check_button = gtk_button_new_with_label("刷新");
	check_view = gtk_text_view_new ();
	check_fixed = gtk_fixed_new();
	check_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (check_view));
	
	gtk_text_buffer_set_text (check_buffer, "Hello, this is some text", -1);
	gtk_fixed_put(GTK_FIXED(check_fixed), check_button, 380, -20);
	
	gtk_box_pack_start(GTK_BOX(check_vbox),check_label,FALSE,FALSE,30);
	gtk_box_pack_start(GTK_BOX(check_vbox),check_view,TRUE,TRUE,10);
	gtk_box_pack_start(GTK_BOX(check_vbox),check_fixed,FALSE,FALSE,20);
	
	//按钮管理
	
	//登陆界面按钮控制
	g_signal_connect(G_OBJECT(client_button1),
                     "clicked",
                     G_CALLBACK(on_clicked_button),
                     NULL);

	g_signal_connect(G_OBJECT(client_button2),
                     "clicked",
                     G_CALLBACK(gtk_main_quit),
                     NULL);

	g_signal_connect(G_OBJECT(client_window),
                     "delete_event",
                     G_CALLBACK(gtk_main_quit),
                     NULL);
    
    //柜员机界面按钮控制                 
    g_signal_connect(G_OBJECT(callnum_button),
                     "clicked",
                     G_CALLBACK(gtk_main_quit),
                     NULL);

	g_signal_connect(G_OBJECT(callnum_window),
                     "delete_event",
                     G_CALLBACK(gtk_main_quit),
                     NULL);
    
    //经理检查系统界面按钮控制                 
    g_signal_connect(G_OBJECT(check_button),
                     "clicked",
                     G_CALLBACK(gtk_main_quit),
                     NULL);

	g_signal_connect(G_OBJECT(check_window),
                     "delete_event",
                     G_CALLBACK(gtk_main_quit),
                     NULL);

	gtk_widget_show_all(client_window);

	gtk_main();
 
	return 0;
	
}
