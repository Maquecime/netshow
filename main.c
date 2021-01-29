#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Lib that provide struct and methods to collect local network interfaces
#include <ifaddrs.h>
//Lib that provide methods to
#include <arpa/inet.h>

// numbers of charcacters in ipv6 address (hexa format)
#define BUFFER_ADDR_LENGTH 40

// Method used to display the network interfaces on the executive machine.
void printInterfaces(struct ifaddrs* ifaddr) {
    struct ifaddrs* tmp = ifaddr;
    char * name = tmp->ifa_name;
    printf("Listing interfaces... :\n");
    printf("%s\n", tmp->ifa_name);
    tmp = tmp->ifa_next;
    while (tmp != NULL && strcmp(name, tmp->ifa_name) != 0) {
        printf("%s\n", tmp->ifa_name);
        tmp = tmp->ifa_next;
    }
}

// Method used to display the addresses(v4 and v6) of the matching interface
void printAddresses(struct ifaddrs* ifaddr, char* name) {
    struct ifaddrs* tmp = ifaddr;
    static char bufferAddresses[BUFFER_ADDR_LENGTH];
    // Use this struct to represent ipv4 adresses
    struct sockaddr_in* a4;
    // This one to represent ipv6 adresses
    struct sockaddr_in6* a6;

    printf("IPv4 & IPv6 addresses for interface %s:\n", name);

    while(tmp != NULL) {
        // store current interface name
        char * ifaName = tmp->ifa_name;
        //store current address family (ipv4 or ipv6)
        sa_family_t ifaAddrFamily = tmp->ifa_addr->sa_family;

        //checking family type and casting the address with the desired structure type
        if (tmp->ifa_addr != NULL && ifaAddrFamily == AF_INET) {
            a4 = (struct sockaddr_in*)tmp->ifa_addr;
        } else if(tmp->ifa_addr != NULL && ifaAddrFamily == AF_INET6) {
            a6 = (struct sockaddr_in6*)tmp->ifa_addr;
        }

        // dealing with v4
        if (a4 != NULL && strcmp(ifaName, name) == 0 && ifaAddrFamily == AF_INET) {
            // The address could not be converted from binary to string
            if (inet_ntop(AF_INET, &a4->sin_addr, bufferAddresses,BUFFER_ADDR_LENGTH ) == NULL) {
                printf("Error trying to convert Ipv4 address\n");
                exit(1);
            } else {
                printf("\tIpv4 : %s\n",  bufferAddresses);
            }
        }

        // dealing with v6
        if (a6 != NULL && strcmp(ifaName, name) == 0 && ifaAddrFamily == AF_INET6) {
            // The address could not be converted from binary to string
            if (inet_ntop(AF_INET6, &a6->sin6_addr, bufferAddresses,BUFFER_ADDR_LENGTH) == NULL) {
                printf("Error trying to convert ipv6 address\n");
                exit(1);
            } else {
                printf("\tIPv6 : %s\n", bufferAddresses);
            }
        }

        tmp = tmp->ifa_next;
    }
}

void printHelper() {
    printf("How to use netshow :\n");
    printf("\t-a : display every network interface on your machine\n");
    printf("\t-i <name> : display adresses information matching the <name> of the interface\n");
}

int main(int argc, char* argv[]) {
    struct ifaddrs* ifaddr;

    if (getifaddrs(&ifaddr) != 0) {
        perror("Could not get ifaddrs");
        exit(1);
    }

    if (argc <= 1) {
        printf("You must fill an argument, type 'netshow -h | --help' to get some more information\n");
        return 0;
    } else {
        if (strcmp(argv[1], "-a") == 0) {
            printInterfaces(ifaddr);
        } else if (strcmp(argv[1], "-i") == 0) {
            if(argc <=2){
                printf("You must fill an interface name, use 'netshow -h | --help' to get some more information\n");
                return 0;
            }
            printAddresses(ifaddr, argv[2]);
        } else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            printHelper();
        }
    }
    freeifaddrs(ifaddr);
    return 0;
}