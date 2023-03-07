#include <iostream>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netpacket/packet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <iomanip>
#include <unistd.h>
#include <linux/if.h>
#include <string.h>

void print_sys_info(){
    // Массив буфер.
    char buf[NI_MAXHOST];
    gethostname(buf, 10);
    std::cout << "Host name:" << std::setw(30) << buf << std::endl; // Вывод hostname.
    struct ifaddrs *ifaddr;
    int family, s;
    // Обработка ошибок, если вдруг подтянуть адреса не удалось.
    if(getifaddrs(&ifaddr)){
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }
    // Пробег по связанному списку всех адресов на компьютере.
    for(struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next){
       family = ifa->ifa_addr->sa_family;
       // Если адрес IPv4 или IPv6.
       if(family  == AF_INET || family == AF_INET6){
           // Дотягиваемся до адрес, и записываем его в буфер который инициализировали раньше.
           s = getnameinfo(ifa->ifa_addr,(family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6), buf, NI_MAXHOST, NULL, 0,NI_NUMERICHOST);
           // Организованный вывод взависимости от типа адреса.
           if(family == AF_INET)
               std::cout << "IPv4 address " << ifa->ifa_name << ":" << std::setw(30) << buf << std::endl;
           if(family == AF_INET6 )
               std::cout << "IPv6 address " << ifa->ifa_name << ":" << std::setw(49) << buf << std::endl;
       }
       // Если пакет
       if(family == AF_PACKET){
           struct sockaddr_ll *g = (struct sockaddr_ll*)ifa->ifa_addr;
           std::cout << "MAC address " << ifa->ifa_name << std::setw(30) << g->sll_addr;
           // По символьно выписываем мак адрес.
           for(int i = 0; i < g->sll_halen; ++i){
               std::cout << (g->sll_addr[i]) <<  ((i+1 != g->sll_halen) ? ':' : '\n');
           }
       }
    }
    freeifaddrs(ifaddr);
}

int main(){
    std::cout << "\nHello " << getlogin() << std::endl;
    std::cout << "This program will print your computer network information. \n" << std::endl;
    print_sys_info();
}
