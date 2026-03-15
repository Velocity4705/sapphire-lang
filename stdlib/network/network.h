#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ===== Phase 1: Socket Programming =====

// TCP
void* net_tcp_socket_create(void);
void  net_tcp_socket_destroy(void* sock);
int   net_tcp_connect(void* sock, const char* host, int port);
int   net_tcp_bind(void* sock, int port);
int   net_tcp_listen(void* sock, int backlog);
void* net_tcp_accept(void* sock);
int   net_tcp_send(void* sock, const char* data, int len);
int   net_tcp_recv(void* sock, int max_len);
const char* net_tcp_recv_data(void* sock);
int   net_tcp_close(void* sock);
int   net_tcp_is_connected(void* sock);
const char* net_tcp_remote_addr(void* sock);
int   net_tcp_remote_port(void* sock);
int   net_tcp_set_nonblocking(void* sock, int enabled);
int   net_tcp_set_timeout(void* sock, int ms);
int   net_tcp_bytes_available(void* sock);

// UDP
void* net_udp_socket_create(void);
void  net_udp_socket_destroy(void* sock);
int   net_udp_bind(void* sock, int port);
int   net_udp_send_to(void* sock, const char* data, int len, const char* host, int port);
int   net_udp_recv_from(void* sock, int max_len);
const char* net_udp_recv_data(void* sock);
const char* net_udp_recv_addr(void* sock);
int   net_udp_recv_port(void* sock);
int   net_udp_close(void* sock);
int   net_udp_set_broadcast(void* sock, int enabled);
int   net_udp_set_timeout(void* sock, int ms);

// Address resolution
const char* net_resolve_host(const char* hostname);
const char* net_local_ip(void);
const char* net_hostname(void);

// ===== Phase 2: Protocol Implementations =====

// DNS client
void* net_dns_create(void);
void  net_dns_destroy(void* dns);
const char* net_dns_lookup(void* dns, const char* hostname);
const char* net_dns_reverse(void* dns, const char* ip);
int   net_dns_lookup_count(void* dns);
const char* net_dns_lookup_result(void* dns, int idx);
void  net_dns_set_server(void* dns, const char* server);
const char* net_dns_server(void* dns);

// SMTP client
void* net_smtp_create(const char* host, int port);
void  net_smtp_destroy(void* smtp);
int   net_smtp_connect(void* smtp);
int   net_smtp_auth(void* smtp, const char* user, const char* pass);
int   net_smtp_send_mail(void* smtp, const char* from, const char* to, const char* subject, const char* body);
int   net_smtp_disconnect(void* smtp);
int   net_smtp_is_connected(void* smtp);
const char* net_smtp_last_response(void* smtp);
void  net_smtp_set_tls(void* smtp, int enabled);

// FTP client
void* net_ftp_create(const char* host, int port);
void  net_ftp_destroy(void* ftp);
int   net_ftp_connect(void* ftp);
int   net_ftp_login(void* ftp, const char* user, const char* pass);
int   net_ftp_list(void* ftp, const char* path);
int   net_ftp_file_count(void* ftp);
const char* net_ftp_file_name(void* ftp, int idx);
int   net_ftp_file_size(void* ftp, int idx);
int   net_ftp_download(void* ftp, const char* remote, const char* local);
int   net_ftp_upload(void* ftp, const char* local, const char* remote);
int   net_ftp_mkdir(void* ftp, const char* path);
int   net_ftp_delete(void* ftp, const char* path);
int   net_ftp_disconnect(void* ftp);
const char* net_ftp_cwd(void* ftp);

// SSH client
void* net_ssh_create(const char* host, int port);
void  net_ssh_destroy(void* ssh);
int   net_ssh_connect(void* ssh);
int   net_ssh_auth_password(void* ssh, const char* user, const char* pass);
int   net_ssh_auth_key(void* ssh, const char* user, const char* key_path);
int   net_ssh_exec(void* ssh, const char* command);
const char* net_ssh_output(void* ssh);
const char* net_ssh_stderr(void* ssh);
int   net_ssh_exit_code(void* ssh);
int   net_ssh_upload(void* ssh, const char* local, const char* remote);
int   net_ssh_download(void* ssh, const char* remote, const char* local);
int   net_ssh_disconnect(void* ssh);
int   net_ssh_is_connected(void* ssh);

// ===== Phase 3: Network Utilities =====

// Packet capture
void* net_capture_create(const char* interface_name);
void  net_capture_destroy(void* cap);
int   net_capture_start(void* cap);
int   net_capture_stop(void* cap);
int   net_capture_packet_count(void* cap);
const char* net_capture_packet_src(void* cap, int idx);
const char* net_capture_packet_dst(void* cap, int idx);
const char* net_capture_packet_proto(void* cap, int idx);
int   net_capture_packet_size(void* cap, int idx);
const char* net_capture_packet_data(void* cap, int idx);
void  net_capture_set_filter(void* cap, const char* filter);
const char* net_capture_to_json(void* cap);

// Network monitor
void* net_monitor_create(void);
void  net_monitor_destroy(void* mon);
void  net_monitor_add_interface(void* mon, const char* iface);
int   net_monitor_sample(void* mon);
int   net_monitor_interface_count(void* mon);
const char* net_monitor_interface_name(void* mon, int idx);
double net_monitor_bytes_sent(void* mon, int idx);
double net_monitor_bytes_recv(void* mon, int idx);
double net_monitor_packets_sent(void* mon, int idx);
double net_monitor_packets_recv(void* mon, int idx);
const char* net_monitor_to_json(void* mon);

// Bandwidth tester
void* net_bandwidth_create(void);
void  net_bandwidth_destroy(void* bw);
double net_bandwidth_test_upload(void* bw, const char* host, int port, int bytes);
double net_bandwidth_test_download(void* bw, const char* host, int port, int bytes);
double net_bandwidth_last_upload_mbps(void* bw);
double net_bandwidth_last_download_mbps(void* bw);
const char* net_bandwidth_report(void* bw);

// Latency / ping
void* net_ping_create(void);
void  net_ping_destroy(void* ping);
double net_ping_host(void* ping, const char* host, int count);
double net_ping_min(void* ping);
double net_ping_max(void* ping);
double net_ping_avg(void* ping);
double net_ping_jitter(void* ping);
int   net_ping_packet_loss(void* ping);
const char* net_ping_report(void* ping);

// ===== Phase 4: Async Networking =====

// Event loop
void* net_event_loop_create(void);
void  net_event_loop_destroy(void* loop);
int   net_event_loop_add_socket(void* loop, void* sock, const char* event, const char* callback_id);
int   net_event_loop_remove_socket(void* loop, void* sock);
int   net_event_loop_run_once(void* loop, int timeout_ms);
int   net_event_loop_run(void* loop, int max_iterations);
int   net_event_loop_stop(void* loop);
int   net_event_loop_pending_count(void* loop);
const char* net_event_loop_next_event(void* loop);
int   net_event_loop_is_running(void* loop);

// Connection pool
void* net_pool_create(const char* host, int port, int max_connections);
void  net_pool_destroy(void* pool);
void* net_pool_acquire(void* pool);
int   net_pool_release(void* pool, void* conn);
int   net_pool_size(void* pool);
int   net_pool_active(void* pool);
int   net_pool_idle(void* pool);
const char* net_pool_host(void* pool);
int   net_pool_port(void* pool);
void  net_pool_set_timeout(void* pool, int ms);

// Async request
void* net_async_request_create(void);
void  net_async_request_destroy(void* req);
int   net_async_get(void* req, const char* url);
int   net_async_post(void* req, const char* url, const char* body);
int   net_async_is_done(void* req);
int   net_async_status_code(void* req);
const char* net_async_response(void* req);
const char* net_async_error(void* req);
double net_async_elapsed_ms(void* req);

// Load balancer
void* net_lb_create(const char* strategy);
void  net_lb_destroy(void* lb);
void  net_lb_add_backend(void* lb, const char* host, int port, int weight);
const char* net_lb_next_host(void* lb);
int   net_lb_next_port(void* lb);
int   net_lb_backend_count(void* lb);
void  net_lb_mark_down(void* lb, const char* host, int port);
void  net_lb_mark_up(void* lb, const char* host, int port);
const char* net_lb_strategy(void* lb);
const char* net_lb_stats(void* lb);

#ifdef __cplusplus
}
#endif
