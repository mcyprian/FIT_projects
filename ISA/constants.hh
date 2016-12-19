/*
 * @file constants.hh
 * @author Michal Cyprian <xcypri01@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * subject ISA, FIT BUT
 * Definition of macros and constant data.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

/**	DHCP OPTIONS **/
#define PAD 0
#define SUBNET 1
#define TIME_OFFSET 2
#define ROUTERS 3
#define TIME_SERVERS 4
#define NAME_SERVERS 5
#define DOMAIN_NAME_SERVERS 6
#define LOG_SERVER 7
#define COOKIE_SERVERS 8
#define LPR_SERVERS 9
#define IMPRESS_SERVER 10
#define RESOURCE_LOCATION_SERVERS 11
#define HOST_NAME 12
#define BOOT_SIZE 13
#define MERIT_DUMP 14
#define DOMAIN_NAME 15
#define SWAP_SERVER 16
#define ROOT_PATH 17
#define EXTENSIONS_PATH 18
#define IP_FORWARDING 19
#define NON_LOCAL_SOURCE_ROUTING 20
#define POLICY_FILTER 21
#define MAX_DGRAM_REASSEMBLY 22
#define DEFAULT_IP_TTL 23
#define PATH_MTU_AGING_TIMEOUT 24
#define PATH_MTU_PLATEAU_TABLE 25
#define INTERFACE_MTU 26
#define ALL_SUBNETS_LOCAL 27
#define BROADCAST_ADDRESS 28
#define PERFORM_MASK_DISCOVERY 29
#define MASK_SUPPLIER 30
#define ROUTER_DISCOVERY 31
#define ROUTER_SOLICITATION_ADDRESS 32
#define STATIC_ROUTES 33
#define TRAILER_ENCAPSULATION 34
#define ARP_CACHE_TIMEOUT 35
#define IEEE802_3_ENCAPSULATION 36
#define DEFAULT_TCP_TTL 37
#define TCP_KEEPALIVE_INTERVAL 38
#define TCP_KEEPALIVE_GARBAGE 39
#define NIS_SERVERS 41
#define NTP_SERVERS 42
#define VENDOR_ENCAPSULATED_OPTIONS 43
#define NETBIOS_NAME_SERVERS 44
#define NETBIOS_DD_SERVER 45
#define NETBIOS_NODE_TYPE 46
#define NETBIOS_SCOPE 47
#define FONT_SERVERS 48
#define X_DISPLAY_MANAGER 49
#define DHCP_REQUESTED_ADDRESS 50
#define DHCP_LEASE_TIME 51
#define DHCP_OPTION_OVERLOAD 52
#define DHCP_MESSAGE_TYPE 53
#define DHCP_SERVER_IDENTIFIER 54
#define DHCP_PARAMETER_REQUEST_LIST 55
#define DHCP_MESSAGE 56
#define DHCP_MAX_MESSAGE_SIZE 57
#define DHCP_RENEWAL_TIME 58
#define DHCP_REBINDING_TIME 59
#define VENDOR_CLASS_IDENTIFIER 60
#define DHCP_CLIENT_IDENTIFIER 61
#define NWIP_DOMAIN_NAME 62
#define NWIP_SUBOPTIONS 63
#define NISPLUS_DOMAIN 64
#define NISPLUS_SERVER 65
#define TFTP_SERVER 66
#define BOOTFILE 67
#define MOBILE_IP_HOME_AGENT 68
#define SMTP_SERVER 69
#define POP3_SERVER 70
#define NNTP_SERVER 71
#define WWW_SERVER 72
#define FINGER_SERVER 73
#define IRC_SERVER 74
#define STREETTALK_SERVER 75
#define STDA_SERVER 76
#define USER_CLASS 77
#define FQDN 81
#define DHCP_AGENT_OPTIONS 82
#define NDS_SERVERS 85
#define NDS_TREE_NAME 86
#define NDS_CONTEXT 87
#define CLIENT_LAST_TRANSACTION_TIME 91
#define ASSOCIATED_IP 92
#define USER_AUTHENTICATION_PROTOCOL 98
#define AUTO_CONFIGURE 116
#define NAME_SERVICE_SEARCH 117
#define SUBNET_SELECTION 118
#define DOMAIN_SEARCH 119
#define CLASSLESS_ROUTE 121
#define END 255

const char *option_names[68] = {
    "Pad",                                               // 0
    "Subnet Mask",                                       // 1
    "Time Offset",                                       // 2
    "Router",                                            // 3
    "Time Server",                                       // 4
    "Name Server",                                       // 5
    "Domain Name Server",                                // 6
    "Log Server",                                        // 7
    "Cookie Server",                                     // 8
    "LRP Server",                                        // 9
    "Implress Server",                                   // 10
    "Resource Location Server",                          // 11
    "Host Name",                                         // 12
    "Boot File Size",                                    // 13
    "Merit Dump File",                                   // 14
    "Domain Name",                                       // 15
    "Swap Server",                                       // 16
    "Root Path",                                         // 17
    "Extension Path",                                    // 18
    "IP Forwarding Enable/Disable",                      // 19
    "Non-Local Source Routing Enable/Disable",           // 20
    "Policy Filter",                                     // 21
    "Maximum Datagram Reassembly Size",                  // 22
    "Default IP Time-to-live",                           // 23
    "Path MTU Aging Timeout",                            // 24
    "Path MTU Plateau Table",                            // 25
    "Interface MTU",                                     // 26
    "All Subnets are Local",                             // 27
    "Broadcast Address",                                 // 28
    "Perform Mask Discovery",                            // 29
    "Mask Supplier",                                     // 30
    "Perform Router Discovery",                          // 31
    "Router Solicitation Address",                       // 32
    "Static Route",                                      // 33
    "Trailer Encapsulation Option",                      // 34
    "ARP Cache Timeout",                                 // 35
    "Ethernet Encapsulation",                            // 36
    "TCP Default TTL",                                   // 37
    "TCP Keepalive Interval",                            // 38
    "TCP Keepalive Garbage",                             // 39
    "Network Information Service Domain",                // 40
    "Network Information Servers",                       // 41
    "Network Time Protocol Servers",                     // 42
    "Vendor Specific Information",                       // 43
    "NetBIOS over TCP/IP Name Server",                   // 44
    "NetBIOS over TCP/IP Datagram Distribution Server",  // 45
    "",                                                  // 46
    "",                                                  // 47
    "",                                                  // 48
    "",                                                  // 49
    "Requested IP address",                              // 50
    "IP address Lease Time",                             // 51
    "Option Overload",                                   // 52
    "DHCP Message Type",                                 // 53
    "Server Identifier",                                 // 54
    "Parameter Request List",                            // 55
    "Message",                                           // 56
    "Maximum DHCP Message Size",                         // 57
    "",                                                  // 58
    "",                                                  // 59
    "",                                                  // 60
    "",                                                  // 61
    "",                                                  // 62
    "",                                                  //
    ""                                                   //
};

#endif  // !CONSTANTS_H
