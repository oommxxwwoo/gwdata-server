#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>

#include "gw.h"
#include "seriport.h"
#include "json_server.h"
#include "p208_server.h"

struct gwdata_server server;

void init_server()
{
        signal(SIGHUP,SIG_IGN);
        signal(SIGPIPE,SIG_IGN);

        server.el = aeCreateEventLoop(1024);

        server.json_clients = listCreate();
        server.p208_clients = listCreate();
        server.seriports = listCreate();

        server.json_next_client_id = 1;

        server.json_fd = anetTcpServer(server.json_neterr,51001,NULL,511);
        anetNonBlock(NULL,server.json_fd);

        server.p208_fd = anetTcpServer(server.p208_neterr,51000,NULL,511);
        anetNonBlock(NULL,server.p208_fd);

        aeCreateFileEvent(server.el,server.json_fd,AE_READABLE,json_server_acceptHandler,NULL);
        aeCreateFileEvent(server.el,server.p208_fd,AE_READABLE,p208_server_acceptHandler,NULL);
        
        gwseriport_create("/dev/ttyUSB0",115200);
        gwseriport_create("/dev/ttyUSB1",115200);
        gwseriport_create("/dev/ttyUSB2",115200);
        gwseriport_create("/dev/ttyUSB3",115200);

        server.global_sensor_data = sdlist_create();


}

int main()
{
        init_server();
        aeMain(server.el);

        return 0;
}