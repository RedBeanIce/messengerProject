#include <gtk/gtk.h>
#include <stdlib.h>


typedef struct MyData Data;
struct MyData 
{
	GtkWidget *window;
	GtkWidget *button;
	GtkWidget *label;
	GtkWidget *textView;
	GtkWidget *textBuf;
	GtkWidget *textField;
};

// 전송 버튼 콜백함수 
G_MODULE_EXPORT void on_bSend_clicked (GtkWidget *button, Data *data)
{
	// text view 버퍼 가져오기
	GtkWidget *buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->textView));
	
	gchar *message = gtk_entry_get_text (GTK_ENTRY (data->textField));	

	
	// 버퍼에 값 추가.
	gtk_text_buffer_insert_at_cursor (buf, "\n", -1);
	gtk_text_buffer_insert_at_cursor (buf, "->", -1);
	gtk_text_buffer_insert_at_cursor (buf, message, -1);
	

	
}

// 서버 오픈 버튼 콜백함수.
G_MODULE_EXPORT void on_bOpen_clicked (GtkButton *button, Data *data)
{
	gtk_label_set_text (GTK_LABEL (data->label), "서버 오픈");
	
	// text view 버퍼 가져오기
	GtkWidget *buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->textView));
	// 대화 창 버퍼초기화 및 시작말 설정하기 
	gtk_text_buffer_set_text (buf, "clear ", -1);
}


int main (int argc, char *argv[])
{
	GtkBuilder *gtkbuilder;
	GError *error;
	Data *data;
	gtk_init (&argc, &argv);
	
	/* 빌더 생성 및 UI 파일 열기 */
	gtkbuilder = gtk_builder_new ();
	
	if ( !gtk_builder_add_from_file (gtkbuilder,"servergui.glade",NULL)) 
	{
		//g_print ("UI 파일을 읽을 때 오류 발생!\n");
		//g_print ("메시지: %s\n", error->message);
		//g_free (error);
		//return (1);
	}

	data = g_slice_new (Data);
	data->window = GTK_WIDGET(gtk_builder_get_object (gtkbuilder, "window"));
	data->label = GTK_WIDGET(gtk_builder_get_object (gtkbuilder, "label"));
	data->textBuf = GTK_WIDGET(gtk_builder_get_object (gtkbuilder, "textBuf"));
	data->button = GTK_WIDGET(gtk_builder_get_object (gtkbuilder, "bOpen"));
	data->textView = GTK_WIDGET(gtk_builder_get_object (gtkbuilder, "textView"));
	data->textField = GTK_WIDGET(gtk_builder_get_object (gtkbuilder, "tFieldMessage"));
	data->button = GTK_WIDGET(gtk_builder_get_object (gtkbuilder, "bSend"));

	gtk_builder_connect_signals (gtkbuilder, data);
	g_object_unref (G_OBJECT (gtkbuilder));
	gtk_widget_show_all (data->window);
	gtk_main ();
	g_slice_free (Data, data);
	return 0;
} 
