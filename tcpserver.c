#include <gio/gio.h>
#include <glib.h>
#include <strings.h>

#define MSG_SIZE 128

#include <libnotify/notify.h>


gboolean incomingConnecttion(GThreadedSocketService *service, GSocketConnection *connection, GObject *o, gpointer data){
    GSocketAddress *sockaddr = g_socket_connection_get_remote_address(connection, NULL);
    GInetAddress *addr = g_inet_socket_address_get_address(G_INET_SOCKET_ADDRESS(sockaddr));
    guint16 port = g_inet_socket_address_get_port(G_INET_SOCKET_ADDRESS(sockaddr));
    g_message("New Connection %p-%p-[%s:%d]", connection, g_thread_self(), g_inet_address_to_string(addr), port);

    gchar message[MSG_SIZE] = {0,};
    GInputStream *istream = g_io_stream_get_input_stream(G_IO_STREAM(connection));
    GOutputStream *ostream = g_io_stream_get_output_stream(G_IO_STREAM(connection));
    GError *error = NULL;
    gssize rs = 1;
    while (rs > 0) {
        bzero(message, MSG_SIZE);
        rs = g_input_stream_read(istream, message, MSG_SIZE, NULL, &error);
        if (error) {
            g_error("ERROR-%d:%s", error->code, error->message);
        } else {
            g_message("<--%p-%p-[%s:%d]-Received message: %s", connection, g_thread_self(),
                      g_inet_address_to_string(addr), port, message);

            NotifyNotification *Hello = NOTIFY_NOTIFICATION(data);
            notify_notification_update(Hello, "New Message.", message, "/home/wayne/Pictures/alipay.png");
            notify_notification_show(Hello, &error);

            GDateTime *current = g_date_time_new_now_local();
            gint ms = g_date_time_get_microsecond(current);
            gchar *strtime = g_date_time_format(current, "%F %T");
            g_snprintf(message, MSG_SIZE, "--> %s-%d from server\n", strtime, ms);
            g_date_time_unref(current);
            g_free(strtime);
            g_output_stream_write(ostream, message, MSG_SIZE, NULL, &error);
        }
    }
    g_io_stream_close(G_IO_STREAM(connection), NULL, &error);

    if (error) g_error("ERROR-%d:%s", error->code, error->message);
}

int main(int argc, char *argv[]) {

    notify_init(argv[0]);
    NotifyNotification *Hello = notify_notification_new(argv[0], "This is an example Text Body.",
                                                        "/home/wayne/Pictures/alipay.png");
    notify_notification_show(Hello, NULL);
    notify_notification_set_timeout(Hello, 1);

    GThreadedSocketService *service = G_THREADED_SOCKET_SERVICE(g_threaded_socket_service_new(-1));
    guint16 port = 1500;
    GError *error = NULL;
    g_socket_listener_add_inet_port(G_SOCKET_LISTENER(service), port, NULL, &error);
    if (error) g_error("%s", error->message);
    g_signal_connect(service, "run", G_CALLBACK(incomingConnecttion), Hello);

    g_socket_service_start(G_SOCKET_SERVICE(service));
    g_message("TCP server-[%s:%d] is running...", "127.0.0.1", port);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
    g_main_loop_unref(loop);

    g_object_unref(G_OBJECT(Hello));
    notify_uninit();

    return 0;
}

