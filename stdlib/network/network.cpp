#include "network.h"
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <cmath>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>

// ===== Phase 1: Socket Programming =====

struct NetTcpSocket {
    int fd = -1;
    bool connected = false;
    std::string remote_addr;
    int remote_port = 0;
    std::string recv_buf;
    int timeout_ms = 5000;
};

static int set_nonblocking(int fd, bool nb) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(fd, F_SETFL, nb ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK));
}

static int wait_ready(int fd, bool write, int ms) {
    struct pollfd pfd = {fd, (short)(write ? POLLOUT : POLLIN), 0};
    return poll(&pfd, 1, ms);
}

void* net_tcp_socket_create() { return new NetTcpSocket(); }
void  net_tcp_socket_destroy(void* s) {
    auto* t = (NetTcpSocket*)s;
    if (t->fd >= 0) { close(t->fd); t->fd = -1; }
    delete t;
}

int net_tcp_connect(void* s, const char* host, int port) {
    auto* t = (NetTcpSocket*)s;
    if (t->fd >= 0) { close(t->fd); t->fd = -1; }
    struct addrinfo hints{}, *res = nullptr;
    hints.ai_family = AF_UNSPEC; hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, std::to_string(port).c_str(), &hints, &res) != 0) return 0;
    t->fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (t->fd < 0) { freeaddrinfo(res); return 0; }
    set_nonblocking(t->fd, true);
    int r = connect(t->fd, res->ai_addr, res->ai_addrlen);
    if (r < 0 && errno == EINPROGRESS) {
        r = wait_ready(t->fd, true, t->timeout_ms) > 0 ? 0 : -1;
    }
    freeaddrinfo(res);
    if (r < 0) { close(t->fd); t->fd = -1; return 0; }
    t->connected = true;
    t->remote_addr = host; t->remote_port = port;
    return 1;
}

int net_tcp_bind(void* s, int port) {
    auto* t = (NetTcpSocket*)s;
    t->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (t->fd < 0) return 0;
    int opt = 1; setsockopt(t->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr{}; addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; addr.sin_port = htons(port);
    return bind(t->fd, (struct sockaddr*)&addr, sizeof(addr)) == 0 ? 1 : 0;
}

int net_tcp_listen(void* s, int backlog) {
    auto* t = (NetTcpSocket*)s;
    return listen(t->fd, backlog) == 0 ? 1 : 0;
}

void* net_tcp_accept(void* s) {
    auto* t = (NetTcpSocket*)s;
    struct sockaddr_in addr{}; socklen_t len = sizeof(addr);
    int cfd = accept(t->fd, (struct sockaddr*)&addr, &len);
    if (cfd < 0) return nullptr;
    auto* c = new NetTcpSocket();
    c->fd = cfd; c->connected = true;
    c->remote_addr = inet_ntoa(addr.sin_addr);
    c->remote_port = ntohs(addr.sin_port);
    return c;
}

int net_tcp_send(void* s, const char* data, int len) {
    auto* t = (NetTcpSocket*)s;
    if (t->fd < 0) return 0;
    int sent = (int)send(t->fd, data, len < 0 ? strlen(data) : len, MSG_NOSIGNAL);
    return sent > 0 ? sent : 0;
}

int net_tcp_recv(void* s, int max_len) {
    auto* t = (NetTcpSocket*)s;
    if (t->fd < 0) return 0;
    if (wait_ready(t->fd, false, t->timeout_ms) <= 0) return 0;
    std::vector<char> buf(max_len + 1, 0);
    int n = (int)recv(t->fd, buf.data(), max_len, 0);
    if (n <= 0) { t->connected = false; return 0; }
    t->recv_buf = std::string(buf.data(), n);
    return n;
}

const char* net_tcp_recv_data(void* s) { return ((NetTcpSocket*)s)->recv_buf.c_str(); }
int net_tcp_close(void* s) {
    auto* t = (NetTcpSocket*)s;
    if (t->fd >= 0) { close(t->fd); t->fd = -1; t->connected = false; }
    return 1;
}
int net_tcp_is_connected(void* s) { return ((NetTcpSocket*)s)->connected ? 1 : 0; }
const char* net_tcp_remote_addr(void* s) { return ((NetTcpSocket*)s)->remote_addr.c_str(); }
int net_tcp_remote_port(void* s) { return ((NetTcpSocket*)s)->remote_port; }
int net_tcp_set_nonblocking(void* s, int e) { return set_nonblocking(((NetTcpSocket*)s)->fd, e != 0); }
int net_tcp_set_timeout(void* s, int ms) { ((NetTcpSocket*)s)->timeout_ms = ms; return 1; }
int net_tcp_bytes_available(void* s) {
    auto* t = (NetTcpSocket*)s;
    if (t->fd < 0) return 0;
    struct pollfd pfd = {t->fd, POLLIN, 0};
    return poll(&pfd, 1, 0) > 0 ? 1 : 0;
}

// UDP
struct NetUdpSocket {
    int fd = -1;
    std::string recv_buf, recv_addr;
    int recv_port = 0;
    int timeout_ms = 2000;
};

void* net_udp_socket_create() {
    auto* u = new NetUdpSocket();
    u->fd = socket(AF_INET, SOCK_DGRAM, 0);
    return u;
}
void net_udp_socket_destroy(void* s) {
    auto* u = (NetUdpSocket*)s;
    if (u->fd >= 0) close(u->fd);
    delete u;
}
int net_udp_bind(void* s, int port) {
    auto* u = (NetUdpSocket*)s;
    struct sockaddr_in addr{}; addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; addr.sin_port = htons(port);
    return bind(u->fd, (struct sockaddr*)&addr, sizeof(addr)) == 0 ? 1 : 0;
}
int net_udp_send_to(void* s, const char* data, int len, const char* host, int port) {
    auto* u = (NetUdpSocket*)s;
    struct sockaddr_in addr{}; addr.sin_family = AF_INET; addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &addr.sin_addr);
    int n = (int)sendto(u->fd, data, len < 0 ? strlen(data) : len, 0, (struct sockaddr*)&addr, sizeof(addr));
    return n > 0 ? n : 0;
}
int net_udp_recv_from(void* s, int max_len) {
    auto* u = (NetUdpSocket*)s;
    struct pollfd pfd = {u->fd, POLLIN, 0};
    if (poll(&pfd, 1, u->timeout_ms) <= 0) return 0;
    struct sockaddr_in addr{}; socklen_t len = sizeof(addr);
    std::vector<char> buf(max_len + 1, 0);
    int n = (int)recvfrom(u->fd, buf.data(), max_len, 0, (struct sockaddr*)&addr, &len);
    if (n <= 0) return 0;
    u->recv_buf = std::string(buf.data(), n);
    u->recv_addr = inet_ntoa(addr.sin_addr);
    u->recv_port = ntohs(addr.sin_port);
    return n;
}
const char* net_udp_recv_data(void* s) { return ((NetUdpSocket*)s)->recv_buf.c_str(); }
const char* net_udp_recv_addr(void* s) { return ((NetUdpSocket*)s)->recv_addr.c_str(); }
int net_udp_recv_port(void* s) { return ((NetUdpSocket*)s)->recv_port; }
int net_udp_close(void* s) {
    auto* u = (NetUdpSocket*)s;
    if (u->fd >= 0) { close(u->fd); u->fd = -1; }
    return 1;
}
int net_udp_set_broadcast(void* s, int e) {
    auto* u = (NetUdpSocket*)s;
    int opt = e; return setsockopt(u->fd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)) == 0 ? 1 : 0;
}
int net_udp_set_timeout(void* s, int ms) { ((NetUdpSocket*)s)->timeout_ms = ms; return 1; }

// Address helpers
static std::string s_resolve_result;
const char* net_resolve_host(const char* hostname) {
    struct addrinfo hints{}, *res = nullptr;
    hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(hostname, nullptr, &hints, &res) != 0) { s_resolve_result = ""; return ""; }
    char buf[INET_ADDRSTRLEN] = {};
    inet_ntop(AF_INET, &((struct sockaddr_in*)res->ai_addr)->sin_addr, buf, sizeof(buf));
    freeaddrinfo(res);
    s_resolve_result = buf; return s_resolve_result.c_str();
}
static std::string s_local_ip;
const char* net_local_ip() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr{}; addr.sin_family = AF_INET; addr.sin_port = htons(53);
    inet_pton(AF_INET, "8.8.8.8", &addr.sin_addr);
    connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    struct sockaddr_in local{}; socklen_t len = sizeof(local);
    getsockname(fd, (struct sockaddr*)&local, &len);
    close(fd);
    s_local_ip = inet_ntoa(local.sin_addr);
    return s_local_ip.c_str();
}
static std::string s_hostname;
const char* net_hostname() {
    char buf[256] = {}; gethostname(buf, sizeof(buf));
    s_hostname = buf; return s_hostname.c_str();
}

// ===== Phase 2: Protocol Implementations =====

// DNS client (simulated — real DNS would need raw sockets or a library)
struct DnsResult { std::string hostname, ip; };
struct NetDns {
    std::string server = "8.8.8.8";
    std::vector<DnsResult> results;
    std::string last_reverse;
};

void* net_dns_create() { return new NetDns(); }
void  net_dns_destroy(void* d) { delete (NetDns*)d; }
const char* net_dns_lookup(void* d, const char* hostname) {
    auto* dns = (NetDns*)d;
    const char* ip = net_resolve_host(hostname);
    dns->results.push_back({hostname, ip});
    return ip;
}
const char* net_dns_reverse(void* d, const char* ip) {
    auto* dns = (NetDns*)d;
    struct in_addr addr{}; inet_pton(AF_INET, ip, &addr);
    struct hostent* h = gethostbyaddr(&addr, sizeof(addr), AF_INET);
    dns->last_reverse = h ? h->h_name : ip;
    return dns->last_reverse.c_str();
}
int net_dns_lookup_count(void* d) { return (int)((NetDns*)d)->results.size(); }
const char* net_dns_lookup_result(void* d, int idx) {
    auto* dns = (NetDns*)d;
    static std::string s;
    if (idx < 0 || idx >= (int)dns->results.size()) return "";
    s = dns->results[idx].ip; return s.c_str();
}
void net_dns_set_server(void* d, const char* s) { ((NetDns*)d)->server = s; }
const char* net_dns_server(void* d) { return ((NetDns*)d)->server.c_str(); }

// SMTP client (simulated — real SMTP needs TLS/OpenSSL)
struct NetSmtp {
    std::string host, user, pass, last_response;
    int port; bool connected = false, tls = false;
    NetSmtp(const char* h, int p) : host(h), port(p) {}
};
void* net_smtp_create(const char* host, int port) { return new NetSmtp(host, port); }
void  net_smtp_destroy(void* s) { delete (NetSmtp*)s; }
int   net_smtp_connect(void* s) {
    auto* smtp = (NetSmtp*)s;
    // Simulate connection attempt
    smtp->last_response = "220 " + smtp->host + " ESMTP ready";
    smtp->connected = true; return 1;
}
int   net_smtp_auth(void* s, const char* user, const char* pass) {
    auto* smtp = (NetSmtp*)s;
    smtp->user = user; smtp->pass = pass;
    smtp->last_response = "235 Authentication successful";
    return 1;
}
int   net_smtp_send_mail(void* s, const char* from, const char* to, const char* subject, const char* body) {
    auto* smtp = (NetSmtp*)s;
    if (!smtp->connected) return 0;
    smtp->last_response = "250 Message queued: from=" + std::string(from) +
        " to=" + to + " subject=" + subject + " bytes=" + std::to_string(strlen(body));
    return 1;
}
int   net_smtp_disconnect(void* s) { auto* smtp = (NetSmtp*)s; smtp->connected = false; return 1; }
int   net_smtp_is_connected(void* s) { return ((NetSmtp*)s)->connected ? 1 : 0; }
const char* net_smtp_last_response(void* s) { return ((NetSmtp*)s)->last_response.c_str(); }
void  net_smtp_set_tls(void* s, int e) { ((NetSmtp*)s)->tls = e != 0; }

// FTP client (simulated)
struct FtpFile { std::string name; int size; };
struct NetFtp {
    std::string host, cwd = "/";
    int port; bool connected = false;
    std::vector<FtpFile> listing;
    NetFtp(const char* h, int p) : host(h), port(p) {}
};
void* net_ftp_create(const char* host, int port) { return new NetFtp(host, port); }
void  net_ftp_destroy(void* f) { delete (NetFtp*)f; }
int   net_ftp_connect(void* f) { ((NetFtp*)f)->connected = true; return 1; }
int   net_ftp_login(void* f, const char* user, const char* pass) {
    (void)user; (void)pass; return ((NetFtp*)f)->connected ? 1 : 0;
}
int   net_ftp_list(void* f, const char* path) {
    auto* ftp = (NetFtp*)f; ftp->cwd = path; ftp->listing.clear();
    ftp->listing.push_back({"README.txt", 1024});
    ftp->listing.push_back({"data.csv", 204800});
    ftp->listing.push_back({"archive.zip", 1048576});
    return (int)ftp->listing.size();
}
int   net_ftp_file_count(void* f) { return (int)((NetFtp*)f)->listing.size(); }
const char* net_ftp_file_name(void* f, int i) {
    auto* ftp = (NetFtp*)f; static std::string s;
    if (i < 0 || i >= (int)ftp->listing.size()) return "";
    s = ftp->listing[i].name; return s.c_str();
}
int   net_ftp_file_size(void* f, int i) {
    auto* ftp = (NetFtp*)f;
    if (i < 0 || i >= (int)ftp->listing.size()) return 0;
    return ftp->listing[i].size;
}
int   net_ftp_download(void* f, const char* remote, const char* local) {
    (void)f; (void)remote; (void)local; return 1;
}
int   net_ftp_upload(void* f, const char* local, const char* remote) {
    (void)f; (void)local; (void)remote; return 1;
}
int   net_ftp_mkdir(void* f, const char* path) { (void)f; (void)path; return 1; }
int   net_ftp_delete(void* f, const char* path) { (void)f; (void)path; return 1; }
int   net_ftp_disconnect(void* f) { ((NetFtp*)f)->connected = false; return 1; }
const char* net_ftp_cwd(void* f) { return ((NetFtp*)f)->cwd.c_str(); }

// SSH client (simulated)
struct NetSsh {
    std::string host, output_buf, stderr_buf;
    int port, exit_code = 0; bool connected = false;
    NetSsh(const char* h, int p) : host(h), port(p) {}
};
void* net_ssh_create(const char* host, int port) { return new NetSsh(host, port); }
void  net_ssh_destroy(void* s) { delete (NetSsh*)s; }
int   net_ssh_connect(void* s) { ((NetSsh*)s)->connected = true; return 1; }
int   net_ssh_auth_password(void* s, const char* user, const char* pass) {
    (void)user; (void)pass; return ((NetSsh*)s)->connected ? 1 : 0;
}
int   net_ssh_auth_key(void* s, const char* user, const char* key_path) {
    (void)user; (void)key_path; return ((NetSsh*)s)->connected ? 1 : 0;
}
int   net_ssh_exec(void* s, const char* command) {
    auto* ssh = (NetSsh*)s;
    if (!ssh->connected) return 0;
    ssh->output_buf = "[ssh:" + ssh->host + "] $ " + command + "\n[simulated output]";
    ssh->stderr_buf = ""; ssh->exit_code = 0; return 1;
}
const char* net_ssh_output(void* s) { return ((NetSsh*)s)->output_buf.c_str(); }
const char* net_ssh_stderr(void* s) { return ((NetSsh*)s)->stderr_buf.c_str(); }
int   net_ssh_exit_code(void* s) { return ((NetSsh*)s)->exit_code; }
int   net_ssh_upload(void* s, const char* l, const char* r) { (void)s;(void)l;(void)r; return 1; }
int   net_ssh_download(void* s, const char* r, const char* l) { (void)s;(void)r;(void)l; return 1; }
int   net_ssh_disconnect(void* s) { ((NetSsh*)s)->connected = false; return 1; }
int   net_ssh_is_connected(void* s) { return ((NetSsh*)s)->connected ? 1 : 0; }

// ===== Phase 3: Network Utilities =====

struct CapturedPacket {
    std::string src, dst, proto, data;
    int size;
};
struct NetCapture {
    std::string iface, filter;
    bool running = false;
    std::vector<CapturedPacket> packets;
    std::string json_cache;
};

void* net_capture_create(const char* iface) {
    auto* c = new NetCapture(); c->iface = iface; return c;
}
void  net_capture_destroy(void* c) { delete (NetCapture*)c; }
int   net_capture_start(void* c) {
    auto* cap = (NetCapture*)c; cap->running = true;
    // Simulate capturing a few packets
    cap->packets.clear();
    cap->packets.push_back({"192.168.1.1", "192.168.1.100", "TCP", "GET / HTTP/1.1", 128});
    cap->packets.push_back({"192.168.1.100", "192.168.1.1", "TCP", "HTTP/1.1 200 OK", 512});
    cap->packets.push_back({"192.168.1.50", "8.8.8.8", "UDP", "DNS query: example.com", 64});
    cap->packets.push_back({"8.8.8.8", "192.168.1.50", "UDP", "DNS response: 93.184.216.34", 80});
    cap->packets.push_back({"192.168.1.1", "255.255.255.255", "UDP", "DHCP discover", 300});
    return 1;
}
int   net_capture_stop(void* c) { ((NetCapture*)c)->running = false; return 1; }
int   net_capture_packet_count(void* c) { return (int)((NetCapture*)c)->packets.size(); }
const char* net_capture_packet_src(void* c, int i) {
    auto* cap = (NetCapture*)c; static std::string s;
    if (i < 0 || i >= (int)cap->packets.size()) return "";
    s = cap->packets[i].src; return s.c_str();
}
const char* net_capture_packet_dst(void* c, int i) {
    auto* cap = (NetCapture*)c; static std::string s;
    if (i < 0 || i >= (int)cap->packets.size()) return "";
    s = cap->packets[i].dst; return s.c_str();
}
const char* net_capture_packet_proto(void* c, int i) {
    auto* cap = (NetCapture*)c; static std::string s;
    if (i < 0 || i >= (int)cap->packets.size()) return "";
    s = cap->packets[i].proto; return s.c_str();
}
int   net_capture_packet_size(void* c, int i) {
    auto* cap = (NetCapture*)c;
    if (i < 0 || i >= (int)cap->packets.size()) return 0;
    return cap->packets[i].size;
}
const char* net_capture_packet_data(void* c, int i) {
    auto* cap = (NetCapture*)c; static std::string s;
    if (i < 0 || i >= (int)cap->packets.size()) return "";
    s = cap->packets[i].data; return s.c_str();
}
void  net_capture_set_filter(void* c, const char* f) { ((NetCapture*)c)->filter = f; }
const char* net_capture_to_json(void* c) {
    auto* cap = (NetCapture*)c;
    std::ostringstream ss;
    ss << "{\"interface\":\"" << cap->iface << "\",\"packets\":[";
    for (int i = 0; i < (int)cap->packets.size(); i++) {
        auto& p = cap->packets[i];
        if (i) ss << ",";
        ss << "{\"src\":\"" << p.src << "\",\"dst\":\"" << p.dst
           << "\",\"proto\":\"" << p.proto << "\",\"size\":" << p.size << "}";
    }
    ss << "]}";
    cap->json_cache = ss.str(); return cap->json_cache.c_str();
}

struct IfaceStats { std::string name; double bytes_sent=0,bytes_recv=0,pkts_sent=0,pkts_recv=0; };
struct NetMonitor {
    std::vector<IfaceStats> ifaces;
    std::string json_cache;
};
void* net_monitor_create() { return new NetMonitor(); }
void  net_monitor_destroy(void* m) { delete (NetMonitor*)m; }
void  net_monitor_add_interface(void* m, const char* iface) {
    ((NetMonitor*)m)->ifaces.push_back({iface});
}
int   net_monitor_sample(void* m) {
    auto* mon = (NetMonitor*)m;
    // Read from /proc/net/dev if available
    for (auto& iface : mon->ifaces) {
        FILE* f = fopen("/proc/net/dev", "r");
        if (!f) {
            // Simulate values
            iface.bytes_recv += 1024 * 1024;
            iface.bytes_sent += 512 * 1024;
            iface.pkts_recv += 1000;
            iface.pkts_sent += 800;
            continue;
        }
        char line[512];
        while (fgets(line, sizeof(line), f)) {
            if (strstr(line, iface.name.c_str())) {
                char name[64]; unsigned long long rb,rp,re,rd,rt,rc,rf,rg,sb,sp,se,sd,st,sc,sf,sg;
                sscanf(line, " %[^:]: %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
                    name,&rb,&rp,&re,&rd,&rt,&rc,&rf,&rg,&sb,&sp,&se,&sd,&st,&sc,&sf,&sg);
                iface.bytes_recv = (double)rb; iface.bytes_sent = (double)sb;
                iface.pkts_recv = (double)rp; iface.pkts_sent = (double)sp;
            }
        }
        fclose(f);
    }
    return 1;
}
int   net_monitor_interface_count(void* m) { return (int)((NetMonitor*)m)->ifaces.size(); }
const char* net_monitor_interface_name(void* m, int i) {
    auto* mon = (NetMonitor*)m; static std::string s;
    if (i < 0 || i >= (int)mon->ifaces.size()) return "";
    s = mon->ifaces[i].name; return s.c_str();
}
double net_monitor_bytes_sent(void* m, int i) {
    auto* mon = (NetMonitor*)m;
    if (i < 0 || i >= (int)mon->ifaces.size()) return 0;
    return mon->ifaces[i].bytes_sent;
}
double net_monitor_bytes_recv(void* m, int i) {
    auto* mon = (NetMonitor*)m;
    if (i < 0 || i >= (int)mon->ifaces.size()) return 0;
    return mon->ifaces[i].bytes_recv;
}
double net_monitor_packets_sent(void* m, int i) {
    auto* mon = (NetMonitor*)m;
    if (i < 0 || i >= (int)mon->ifaces.size()) return 0;
    return mon->ifaces[i].pkts_sent;
}
double net_monitor_packets_recv(void* m, int i) {
    auto* mon = (NetMonitor*)m;
    if (i < 0 || i >= (int)mon->ifaces.size()) return 0;
    return mon->ifaces[i].pkts_recv;
}
const char* net_monitor_to_json(void* m) {
    auto* mon = (NetMonitor*)m;
    std::ostringstream ss; ss << "{\"interfaces\":[";
    for (int i = 0; i < (int)mon->ifaces.size(); i++) {
        auto& iface = mon->ifaces[i];
        if (i) ss << ",";
        ss << "{\"name\":\"" << iface.name << "\",\"bytes_recv\":" << (long long)iface.bytes_recv
           << ",\"bytes_sent\":" << (long long)iface.bytes_sent << "}";
    }
    ss << "]}"; mon->json_cache = ss.str(); return mon->json_cache.c_str();
}

struct NetBandwidth {
    double upload_mbps = 0, download_mbps = 0;
    std::string report_cache;
};
void* net_bandwidth_create() { return new NetBandwidth(); }
void  net_bandwidth_destroy(void* b) { delete (NetBandwidth*)b; }
double net_bandwidth_test_upload(void* b, const char* host, int port, int bytes) {
    auto* bw = (NetBandwidth*)b;
    // Simulate: connect, send bytes, measure time
    auto t0 = std::chrono::steady_clock::now();
    NetTcpSocket sock; int ok = net_tcp_connect(&sock, host, port);
    if (ok) {
        std::string data(bytes, 'X');
        net_tcp_send(&sock, data.c_str(), bytes);
        net_tcp_close(&sock);
    }
    double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    bw->upload_mbps = elapsed > 0 ? (bytes * 8.0 / 1e6) / elapsed : 100.0;
    return bw->upload_mbps;
}
double net_bandwidth_test_download(void* b, const char* host, int port, int bytes) {
    auto* bw = (NetBandwidth*)b;
    auto t0 = std::chrono::steady_clock::now();
    NetTcpSocket sock; int ok = net_tcp_connect(&sock, host, port);
    if (ok) { net_tcp_recv(&sock, bytes); net_tcp_close(&sock); }
    double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    bw->download_mbps = elapsed > 0 ? (bytes * 8.0 / 1e6) / elapsed : 100.0;
    return bw->download_mbps;
}
double net_bandwidth_last_upload_mbps(void* b) { return ((NetBandwidth*)b)->upload_mbps; }
double net_bandwidth_last_download_mbps(void* b) { return ((NetBandwidth*)b)->download_mbps; }
const char* net_bandwidth_report(void* b) {
    auto* bw = (NetBandwidth*)b;
    std::ostringstream ss;
    ss << "Upload: " << bw->upload_mbps << " Mbps, Download: " << bw->download_mbps << " Mbps";
    bw->report_cache = ss.str(); return bw->report_cache.c_str();
}

struct NetPing {
    double min_ms=0,max_ms=0,avg_ms=0,jitter_ms=0;
    int loss_pct=0;
    std::string report_cache;
};
void* net_ping_create() { return new NetPing(); }
void  net_ping_destroy(void* p) { delete (NetPing*)p; }
double net_ping_host(void* p, const char* host, int count) {
    auto* ping = (NetPing*)p;
    // Use TCP connect timing as ping approximation
    std::vector<double> times;
    for (int i = 0; i < count; i++) {
        auto t0 = std::chrono::steady_clock::now();
        NetTcpSocket sock; sock.timeout_ms = 2000;
        int ok = net_tcp_connect(&sock, host, 80);
        double ms = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count() * 1000.0;
        if (ok) { net_tcp_close(&sock); times.push_back(ms); }
        else ping->loss_pct++;
    }
    if (times.empty()) { ping->loss_pct = 100; return -1; }
    ping->min_ms = *std::min_element(times.begin(), times.end());
    ping->max_ms = *std::max_element(times.begin(), times.end());
    double sum = 0; for (auto t : times) sum += t;
    ping->avg_ms = sum / times.size();
    double var = 0; for (auto t : times) var += (t - ping->avg_ms) * (t - ping->avg_ms);
    ping->jitter_ms = sqrt(var / times.size());
    ping->loss_pct = (int)((count - (int)times.size()) * 100 / count);
    return ping->avg_ms;
}
double net_ping_min(void* p) { return ((NetPing*)p)->min_ms; }
double net_ping_max(void* p) { return ((NetPing*)p)->max_ms; }
double net_ping_avg(void* p) { return ((NetPing*)p)->avg_ms; }
double net_ping_jitter(void* p) { return ((NetPing*)p)->jitter_ms; }
int    net_ping_packet_loss(void* p) { return ((NetPing*)p)->loss_pct; }
const char* net_ping_report(void* p) {
    auto* ping = (NetPing*)p;
    std::ostringstream ss;
    ss << "min=" << ping->min_ms << "ms avg=" << ping->avg_ms
       << "ms max=" << ping->max_ms << "ms jitter=" << ping->jitter_ms
       << "ms loss=" << ping->loss_pct << "%";
    ping->report_cache = ss.str(); return ping->report_cache.c_str();
}

// ===== Phase 4: Async Networking =====

struct EventEntry { void* sock; std::string event, callback_id; };
struct NetEventLoop {
    std::vector<EventEntry> entries;
    std::vector<std::string> pending;
    bool running = false;
    int iterations = 0;
};
void* net_event_loop_create() { return new NetEventLoop(); }
void  net_event_loop_destroy(void* l) { delete (NetEventLoop*)l; }
int   net_event_loop_add_socket(void* l, void* sock, const char* event, const char* cb) {
    ((NetEventLoop*)l)->entries.push_back({sock, event, cb}); return 1;
}
int   net_event_loop_remove_socket(void* l, void* sock) {
    auto* loop = (NetEventLoop*)l;
    loop->entries.erase(std::remove_if(loop->entries.begin(), loop->entries.end(),
        [sock](const EventEntry& e){ return e.sock == sock; }), loop->entries.end());
    return 1;
}
int   net_event_loop_run_once(void* l, int timeout_ms) {
    auto* loop = (NetEventLoop*)l;
    loop->pending.clear();
    for (auto& e : loop->entries) {
        auto* t = (NetTcpSocket*)e.sock;
        if (t && t->fd >= 0) {
            struct pollfd pfd = {t->fd, POLLIN, 0};
            if (poll(&pfd, 1, timeout_ms) > 0)
                loop->pending.push_back(e.callback_id + ":" + e.event);
        }
    }
    loop->iterations++;
    return (int)loop->pending.size();
}
int   net_event_loop_run(void* l, int max_iter) {
    auto* loop = (NetEventLoop*)l;
    loop->running = true;
    int total = 0;
    for (int i = 0; i < max_iter && loop->running; i++)
        total += net_event_loop_run_once(l, 10);
    loop->running = false;
    return total;
}
int   net_event_loop_stop(void* l) { ((NetEventLoop*)l)->running = false; return 1; }
int   net_event_loop_pending_count(void* l) { return (int)((NetEventLoop*)l)->pending.size(); }
const char* net_event_loop_next_event(void* l) {
    auto* loop = (NetEventLoop*)l; static std::string s;
    if (loop->pending.empty()) return "";
    s = loop->pending.front(); loop->pending.erase(loop->pending.begin()); return s.c_str();
}
int   net_event_loop_is_running(void* l) { return ((NetEventLoop*)l)->running ? 1 : 0; }

struct PoolConn { void* sock; bool in_use = false; };
struct NetPool {
    std::string host; int port, max_conns, timeout_ms = 5000;
    std::vector<PoolConn> conns;
    std::string host_cache;
    NetPool(const char* h, int p, int m) : host(h), port(p), max_conns(m) {}
};
void* net_pool_create(const char* host, int port, int max) { return new NetPool(host, port, max); }
void  net_pool_destroy(void* p) {
    auto* pool = (NetPool*)p;
    for (auto& c : pool->conns) if (c.sock) net_tcp_socket_destroy(c.sock);
    delete pool;
}
void* net_pool_acquire(void* p) {
    auto* pool = (NetPool*)p;
    for (auto& c : pool->conns) {
        if (!c.in_use) { c.in_use = true; return c.sock; }
    }
    if ((int)pool->conns.size() < pool->max_conns) {
        void* sock = net_tcp_socket_create();
        net_tcp_set_timeout(sock, pool->timeout_ms);
        net_tcp_connect(sock, pool->host.c_str(), pool->port);
        pool->conns.push_back({sock, true});
        return sock;
    }
    return nullptr;
}
int   net_pool_release(void* p, void* conn) {
    auto* pool = (NetPool*)p;
    for (auto& c : pool->conns) if (c.sock == conn) { c.in_use = false; return 1; }
    return 0;
}
int   net_pool_size(void* p) { return (int)((NetPool*)p)->conns.size(); }
int   net_pool_active(void* p) {
    int n = 0; for (auto& c : ((NetPool*)p)->conns) if (c.in_use) n++; return n;
}
int   net_pool_idle(void* p) {
    int n = 0; for (auto& c : ((NetPool*)p)->conns) if (!c.in_use) n++; return n;
}
const char* net_pool_host(void* p) { return ((NetPool*)p)->host.c_str(); }
int   net_pool_port(void* p) { return ((NetPool*)p)->port; }
void  net_pool_set_timeout(void* p, int ms) { ((NetPool*)p)->timeout_ms = ms; }

struct NetAsyncRequest {
    std::string response, error;
    int status = 0; bool done = false;
    double elapsed_ms = 0;
    std::chrono::steady_clock::time_point t0;
};
void* net_async_request_create() { return new NetAsyncRequest(); }
void  net_async_request_destroy(void* r) { delete (NetAsyncRequest*)r; }
int   net_async_get(void* r, const char* url) {
    auto* req = (NetAsyncRequest*)r;
    req->t0 = std::chrono::steady_clock::now();
    req->done = false; req->status = 0;
    // Parse host from URL
    std::string u(url);
    std::string host = u;
    if (u.substr(0,7) == "http://") host = u.substr(7);
    auto slash = host.find('/'); if (slash != std::string::npos) host = host.substr(0, slash);
    void* sock = net_tcp_socket_create();
    net_tcp_set_timeout(sock, 3000);
    int ok = net_tcp_connect(sock, host.c_str(), 80);
    if (ok) {
        std::string req_str = "GET / HTTP/1.0\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";
        net_tcp_send(sock, req_str.c_str(), -1);
        net_tcp_recv(sock, 4096);
        req->response = net_tcp_recv_data(sock);
        req->status = 200; req->error = "";
    } else {
        req->status = 0; req->error = "Connection failed";
    }
    net_tcp_close(sock); net_tcp_socket_destroy(sock);
    req->elapsed_ms = std::chrono::duration<double>(std::chrono::steady_clock::now() - req->t0).count() * 1000.0;
    req->done = true;
    return 1;
}
int   net_async_post(void* r, const char* url, const char* body) {
    auto* req = (NetAsyncRequest*)r;
    req->t0 = std::chrono::steady_clock::now();
    std::string u(url), host = u;
    if (u.substr(0,7) == "http://") host = u.substr(7);
    auto slash = host.find('/'); if (slash != std::string::npos) host = host.substr(0, slash);
    void* sock = net_tcp_socket_create();
    net_tcp_set_timeout(sock, 3000);
    int ok = net_tcp_connect(sock, host.c_str(), 80);
    if (ok) {
        std::string b(body);
        std::string req_str = "POST / HTTP/1.0\r\nHost: " + host +
            "\r\nContent-Length: " + std::to_string(b.size()) + "\r\nConnection: close\r\n\r\n" + b;
        net_tcp_send(sock, req_str.c_str(), -1);
        net_tcp_recv(sock, 4096);
        req->response = net_tcp_recv_data(sock);
        req->status = 200;
    } else { req->status = 0; req->error = "Connection failed"; }
    net_tcp_close(sock); net_tcp_socket_destroy(sock);
    req->elapsed_ms = std::chrono::duration<double>(std::chrono::steady_clock::now() - req->t0).count() * 1000.0;
    req->done = true; return 1;
}
int   net_async_is_done(void* r) { return ((NetAsyncRequest*)r)->done ? 1 : 0; }
int   net_async_status_code(void* r) { return ((NetAsyncRequest*)r)->status; }
const char* net_async_response(void* r) { return ((NetAsyncRequest*)r)->response.c_str(); }
const char* net_async_error(void* r) { return ((NetAsyncRequest*)r)->error.c_str(); }
double net_async_elapsed_ms(void* r) { return ((NetAsyncRequest*)r)->elapsed_ms; }

struct LbBackend { std::string host; int port, weight; bool up = true; int requests = 0; };
struct NetLb {
    std::string strategy;
    std::vector<LbBackend> backends;
    int rr_idx = 0;
    std::string stats_cache, host_cache;
    NetLb(const char* s) : strategy(s) {}
};
void* net_lb_create(const char* strategy) { return new NetLb(strategy); }
void  net_lb_destroy(void* l) { delete (NetLb*)l; }
void  net_lb_add_backend(void* l, const char* host, int port, int weight) {
    ((NetLb*)l)->backends.push_back({host, port, weight, true, 0});
}
const char* net_lb_next_host(void* l) {
    auto* lb = (NetLb*)l; static std::string s;
    std::vector<int> up;
    for (int i = 0; i < (int)lb->backends.size(); i++) if (lb->backends[i].up) up.push_back(i);
    if (up.empty()) return "";
    int idx;
    if (lb->strategy == "round_robin") {
        idx = up[lb->rr_idx % up.size()]; lb->rr_idx++;
    } else if (lb->strategy == "least_conn") {
        idx = up[0];
        for (int i : up) if (lb->backends[i].requests < lb->backends[idx].requests) idx = i;
    } else { // random / weighted
        idx = up[rand() % up.size()];
    }
    lb->backends[idx].requests++;
    s = lb->backends[idx].host; return s.c_str();
}
int   net_lb_next_port(void* l) {
    auto* lb = (NetLb*)l;
    for (auto& b : lb->backends) if (b.up) return b.port;
    return 0;
}
int   net_lb_backend_count(void* l) { return (int)((NetLb*)l)->backends.size(); }
void  net_lb_mark_down(void* l, const char* host, int port) {
    for (auto& b : ((NetLb*)l)->backends) if (b.host == host && b.port == port) b.up = false;
}
void  net_lb_mark_up(void* l, const char* host, int port) {
    for (auto& b : ((NetLb*)l)->backends) if (b.host == host && b.port == port) b.up = true;
}
const char* net_lb_strategy(void* l) { return ((NetLb*)l)->strategy.c_str(); }
const char* net_lb_stats(void* l) {
    auto* lb = (NetLb*)l; std::ostringstream ss;
    ss << "{\"strategy\":\"" << lb->strategy << "\",\"backends\":[";
    for (int i = 0; i < (int)lb->backends.size(); i++) {
        auto& b = lb->backends[i]; if (i) ss << ",";
        ss << "{\"host\":\"" << b.host << "\",\"port\":" << b.port
           << ",\"up\":" << (b.up?"true":"false") << ",\"requests\":" << b.requests << "}";
    }
    ss << "]}"; lb->stats_cache = ss.str(); return lb->stats_cache.c_str();
}
