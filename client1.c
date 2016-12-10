#include <gtk/gtk.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 8888
#define SERVER_IP "192.168.11.215"
#define GEN_GOTAL 1  /* 普通业务总人数 */ 
#define GEN_CUR   2  /* 普通业务当前服务的号码 */
#define VIP_TOTAL 3  /* vip业务总人数 */
#define VIP_CUR   4  /* vip业务当前服务的号码 */
#define FROM_GET_NUM   5 /* 为取号机创建线程 */
#define FROM_CALL_NUM  6 /* 为叫号机创建线程 */

GtkWidget *mainwindow;
GtkWidget *mainhbox , *fristvbox , *secondvbox;
GtkWidget *normallabel , *viplabel;
GtkWidget *normalbutton , *vipbutton;
GtkWidget *normalentry , *vipentry;
GtkWidget *label;
GtkWidget *view;
GtkTextBuffer *buffer;
GtkWidget *hseparator,*vseparator;

char getnumberbuff[100];
char normalentrybuff[100];
char vipentrybuff[100];
static int currentcount = 1;
static void mydestroy(GtkWidget *widget , gpointer data) { gtk_main_quit();}
static int normalbtc(GtkWidget *widget , gpointer data)
{
	int connect_fd,ret,snd_buf,i,len,nnumber;
	sprintf(getnumberbuff,"取号成功,您的号码是%d",currentcount++);
	gtk_entry_set_text(GTK_ENTRY(normalentry),getnumberbuff);
	struct sockaddr_in srv_addr;
	connect_fd = socket(PF_INET,SOCK_STREAM,0);
	if(connect_fd < 0)
	{
		perror("cannot create communication socket");
		return 1;
	}
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
 		return 1;
 	}
 	snd_buf = FROM_GET_NUM;
 	write(connect_fd,&snd_buf,sizeof(snd_buf));
 	snd_buf = GEN_GOTAL;
 	write(connect_fd, &snd_buf, sizeof(snd_buf));
 	read(connect_fd,&nnumber,sizeof(nnumber));
 	if(0==nnumber)
 	{
 		gtk_label_set_text(GTK_LABEL(normallabel),"恭喜您,当前无人排队,静等叫号吧!");
 		close(connect_fd);		
 	}
 	else{
 	sprintf(normalentrybuff,"普通客户您好,您前面有%d人排队",nnumber);
 	gtk_label_set_text(GTK_LABEL(normallabel),normalentrybuff);
 	close(connect_fd);
 	}
}
static int vipbtc(GtkWidget *widget , gpointer data)
{
	int connect_fd,ret,snd_buf,i,len,vnumber;
	sprintf(getnumberbuff,"取号成功,您的号码是%d",currentcount++);
	gtk_entry_set_text(GTK_ENTRY(vipentry),vipentrybuff);
	struct sockaddr_in srv_addr;
	connect_fd = socket(PF_INET,SOCK_STREAM,0);
	if(connect_fd < 0)
	{
		perror("cannot create communication socket");
		return 1;
	}
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
 		return 1;
 	}
 	snd_buf = FROM_GET_NUM;
 	write(connect_fd,&snd_buf,sizeof(snd_buf));
 	snd_buf = VIP_TOTAL;
 	write(connect_fd, &snd_buf, sizeof(snd_buf));
 	read(connect_fd,&vnumber,sizeof(vnumber));
 	if(0==vnumber)
 	{
 		gtk_label_set_text(GTK_LABEL(normallabel),"恭喜您,当前无人排队,静等叫号吧!");
 		close(connect_fd);		
 	}
 	else{
 	sprintf(vipentrybuff,"VIP客户您好,您前面有%d人排队",vnumber);
 	gtk_label_set_text(GTK_LABEL(viplabel),vipentrybuff);
 	close(connect_fd);
 	}
}

int main(int argc , char *argv[])
{
	gtk_init(&argc,&argv);
	// create window .
	mainwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(mainwindow),"排队叫号");
	gtk_widget_set_usize(mainwindow,600,600);
	gtk_window_set_position(GTK_WINDOW(mainwindow),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(mainwindow),25);
	// create hbox && vbox .
	mainhbox = gtk_hbox_new(FALSE , 1);
	fristvbox = gtk_vbox_new(FALSE , 1);
	secondvbox = gtk_vbox_new(FALSE , 1);
	vseparator = gtk_vseparator_new();
	gtk_container_add(GTK_CONTAINER(mainwindow),mainhbox);
	gtk_box_pack_start(GTK_BOX(mainhbox),fristvbox,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(mainhbox), vseparator, FALSE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(mainhbox),secondvbox,TRUE,TRUE,0);
	//create label & button & entry in firstvbox
	normallabel = gtk_label_new("普通客户您好,您前面有0人排队");
	viplabel = gtk_label_new("VIP客户您好,您前面有0人排队");
	normalentry = gtk_entry_new();
	vipentry = gtk_entry_new();
	normalbutton = gtk_button_new_with_label("普通客户点击排号");
	vipbutton = gtk_button_new_with_label("VIP客户点击排号");
	hseparator = gtk_hseparator_new();
	//gtk_label_set_markup(GTK_LABEL(normallabel),
   //"<span foreground='red'font_desc='24'>普通客户您好,您前面有0人排队</span>");
   //	gtk_label_set_markup(GTK_LABEL(viplabel),
   //"<span foreground='red'font_desc='24'>VIP客户您好,您前面有0人排队</span>");
  
	gtk_box_pack_start(GTK_BOX(fristvbox),normallabel,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(fristvbox),normalbutton,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(fristvbox),normalentry,FALSE,FALSE,0);	
	gtk_box_pack_start(GTK_BOX(fristvbox),hseparator, FALSE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(fristvbox),viplabel,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(fristvbox),vipbutton,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(fristvbox),vipentry,FALSE,FALSE,0);
	//create secondvbox
	//label = gtk_label_new("当前排队信息");
	gtk_label_set_markup(GTK_LABEL(label),
   "<span foreground='red'font_desc='24'>当前排队信息</span>");
	view = gtk_text_view_new ();
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	gtk_text_buffer_set_text (buffer, "Hello, this is some text", -1);
	gtk_box_pack_start(GTK_BOX(secondvbox),label,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(secondvbox),view,TRUE,TRUE,0);
	// bind button
	g_signal_connect(G_OBJECT(mainwindow),"destroy",G_CALLBACK(mydestroy),NULL);
	g_signal_connect(G_OBJECT(normalbutton),"clicked",G_CALLBACK(normalbtc),NULL);
	g_signal_connect(G_OBJECT(vipbutton),"clicked",G_CALLBACK(vipbtc),NULL);
	gtk_widget_show_all(mainwindow);
	gtk_main();
	return 0;
}


