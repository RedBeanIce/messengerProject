#include <gtk/gtk.h>
#include <stdlib.h>

typedef struct _Data Data;
struct _Data {

    GtkWidget *window;
    GtkWidget *startBt; // 접속 버튼 
    GtkWidget *portNum; // 포트번호 저장
    GtkWidget *chatTextView; // 텍스트 박스 공간
    GtkWidget *sendMsg; // 작성 텍스트 공간 
    GtkWidget *sendBt; // 메세지 전송 버튼 

};

// 종료 이벤트 처리 콜백함수 
G_MODULE_EXPORT void quit (GtkWidget *window,gpointer data){
    gtk_main_quit ();
}


// 서버 접속 콜백 함수
G_MODULE_EXPORT void on_startBt_clicked(GtkButton *button, Data *data){
    //텍스트 필드 값 받아옴
    GtkTextBuffer* buffer =gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->chatTextView));
    GtkTextIter end;

    // 텍스트 버퍼 끝부터 추가.
    gtk_text_buffer_get_end_iter(buffer,&end);

    //마지막에 텍스트 추가.
    gtk_text_buffer_insert(buffer,&end,"서버 접속 시도\n", -1);
 
}


// 메세지 보내는 버튼 콜백함수 
G_MODULE_EXPORT void on_sendBt_clicked(GtkButton *button, Data *data){

    // text view 버퍼 가져오기
    GtkWidget *buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (data->chatTextView));

    gchar *message = gtk_entry_get_text (GTK_ENTRY (data->sendMsg));	

	
    // 버퍼에 값 추가.
    gtk_text_buffer_insert_at_cursor (buf, "\n", -1);
    gtk_text_buffer_insert_at_cursor (buf, "나 : ", -1);
    gtk_text_buffer_insert_at_cursor (buf, message, -1);
 
}



int main (int argc, char *argv[]){
    GtkBuilder *builder;
    GError *error;
    Data *data;
    gtk_init (&argc, &argv);
    /* 빌더 생성 및 UI 파일 열기 */
    builder = gtk_builder_new ();
    if (!gtk_builder_add_from_file(builder,"clientgui.glade",NULL)) {
        printf("잘못된 파일명");
    }
 
    data = g_slice_new (Data); /*메모리 버퍼 초기화*/

    data->window = GTK_WIDGET(gtk_builder_get_object (builder, "gtkWindow"));
    data->portNum = GTK_WIDGET(gtk_builder_get_object (builder, "portNum"));
    data->startBt = GTK_WIDGET(gtk_builder_get_object (builder, "startBt"));
    data->sendMsg = GTK_WIDGET(gtk_builder_get_object (builder, "sendMsg"));
    data->chatTextView = GTK_WIDGET(gtk_builder_get_object (builder, "textView"));
    data->sendBt = GTK_WIDGET(gtk_builder_get_object (builder, "sendBt"));

    gtk_builder_connect_signals (builder, data);

    g_object_unref (G_OBJECT (builder));

    gtk_widget_show_all (data->window);

    gtk_main ();

    g_slice_free (Data, data);

    return 0;
} 
