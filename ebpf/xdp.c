#include "bpf_all.h"

/* Internet Control Message Protocol	*/
#define IPPROTO_ICMP 1

struct {
    __uint(type, BPF_MAP_TYPE_XSKMAP);
    __type(key, u32);
    __type(value, u32);
    __uint(max_entries, 1);
} xdp_sockets SEC(".maps");

SEC("xdp")
int xdp_fn(struct xdp_md *ctx)
{
    void *data = ctx_ptr(ctx, data);
    void *data_end = ctx_ptr(ctx, data_end);

    struct ethhdr *eth;
    eth = (typeof(eth))data;
    if ((void *)(eth + 1) > data_end)
        return XDP_PASS;

    if (eth->h_proto != bpf_ntohs(ETH_P_IP))
        return XDP_PASS;

    struct iphdr *iph;
    iph = (typeof(iph))(eth + 1);
    if ((void *)(iph + 1) > data_end)
        return XDP_PASS;

    if (iph->protocol != IPPROTO_ICMP)
        return XDP_PASS;

    struct icmphdr *ih;
    ih = (typeof(ih))((void *)iph + (iph->ihl*4));
    if ((void *)(ih + 1) > data_end)
        return XDP_PASS;

    if (ih->type != ICMP_ECHO)
        return XDP_PASS;

    return bpf_redirect_map(&xdp_sockets, 0, 0);
}