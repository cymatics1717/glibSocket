#include <gio/gio.h>
#include <glib.h>
#include <strings.h>
#define MSG_SIZE 128

gboolean incomingConnecttion(GThreadedSocketService *service, GSocketConnection *connection, GObject *o, gpointer data){
    GSocketAddress *sockaddr = g_socket_connection_get_remote_address(connection, NULL);
    GInetAddress *addr = g_inet_socket_address_get_address(G_INET_SOCKET_ADDRESS(sockaddr));
    guint16 port = g_inet_socket_address_get_port(G_INET_SOCKET_ADDRESS(sockaddr));
    g_message("New Connection %p-%p-[%s:%d]",connection, g_thread_self(), g_inet_address_to_string(addr), port);

    gchar message[MSG_SIZE];
    GInputStream *instream = g_io_stream_get_input_stream(G_IO_STREAM(connection));
    GError *error = NULL;
    gssize rs =1;
    while(rs > 0){
//    gsize bytes_read = 2;
//    gboolean success = TRUE;
//    success = g_input_stream_read_all (instream, message, MSG_SIZE-1, &bytes_read, NULL, &error);
        rs = g_input_stream_read(instream, message, MSG_SIZE, NULL, &error);
        if (error) {
            g_error("ERROR-%d:%s", error->code, error->message);
        } else {
            g_message("%p-%p-[%s:%d]-Received message: %s", connection, g_thread_self(), g_inet_address_to_string(addr), port, message);
            message[rs] ='\0';
//            bzero(message,MSG_SIZE);
        }
    }
    g_io_stream_close(G_IO_STREAM(connection), NULL, &error);

    if (error) g_error("ERROR-%d:%s", error->code, error->message);
}

int main(int argc, char *argv[]) {
    GThreadedSocketService *service = G_THREADED_SOCKET_SERVICE(g_threaded_socket_service_new(-1));
    guint16 port = 1500;
    GError *error = NULL;
    g_socket_listener_add_inet_port(G_SOCKET_LISTENER(service), port, NULL, &error);
    if (error) g_error("%s", error->message);
    g_signal_connect(service, "run", G_CALLBACK(incomingConnecttion), NULL);

    g_socket_service_start(G_SOCKET_SERVICE(service));
    g_message("TCP server-[%s:%d] is running...", "127.0.0.1",port);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
    g_main_loop_unref(loop);

    return 0;
}

