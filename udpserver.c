#include <gio/gio.h>
#include <glib.h>
#define BLOCK_SIZE 128

static gboolean gio_read_socket (GIOChannel *channel,GIOCondition condition,gpointer data)
{
    char buf[BLOCK_SIZE];
    gsize bytes_read;
    GError *error = NULL;

    if (condition & G_IO_HUP) return FALSE; /* this channel is done */
    g_io_channel_read_chars (channel, buf, sizeof (buf), &bytes_read,&error);
    g_assert (error == NULL);

    buf[bytes_read] = '\0';
//    g_print ("%s", buf);
    g_message("%p-Received message: %s", g_thread_self(), buf);


    int *a = data;
    *a = *a + 1;

    return TRUE;
}

gboolean idleCpt (gpointer user_data){
    int *a = user_data;
//    g_print("%d\n", *a);
//    sleep(1);
    return TRUE;
}
int main (int argc, char **argv)
{
    GSocket * s_udp;
    GError *err = NULL;
    int idIdle = -1, dataI = 0;
    guint16 udp_port = 1500;
    GSocketAddress * gsockAddr = G_SOCKET_ADDRESS(g_inet_socket_address_new(g_inet_address_new_any(G_SOCKET_FAMILY_IPV4), udp_port));
    s_udp = g_socket_new(G_SOCKET_FAMILY_IPV4,G_SOCKET_TYPE_DATAGRAM,G_SOCKET_PROTOCOL_UDP,&err);

    g_assert(err == NULL);

    if (s_udp == NULL) {
        g_print("Error");
        return 1;
    }
    if (g_socket_bind(s_udp, gsockAddr, TRUE, NULL) == FALSE){
        g_print("Error bind\n");
        return 1;
    }

    g_assert(err == NULL);

    int fd = g_socket_get_fd(s_udp);
    GIOChannel* channel = g_io_channel_unix_new(fd);
    guint source = g_io_add_watch(channel, G_IO_IN,(GIOFunc) gio_read_socket, &dataI);
    g_io_channel_unref(channel);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
//    idIdle = g_idle_add(idleCpt, &dataI);
    g_main_loop_run(loop);
    g_main_loop_unref(loop);
}