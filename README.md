# ping-latency-injector

`ping-latency-injector` is a latency injector for `ping` on server side.

On server, if it receives an ECHOREQUEST ICMPv4 packet, `ping-latency-injector` is able to
delay to reply the ECHOREPLY ICMPv4 packet.

## Goody

It's really interesting to hide the network distance on server, which is detect
by `ping`.

In hence, bad guys are unable to detect the real network distance between
servers by `ping`.

However, it's easy to calculate the real latency by subtracting the specified
injected latency.

## How it works?

It works with XDP and AF_XDP.

1. XDP program check the packet whether it's an ECHOREQUEST ICMPv4 packet.
2. XDP redirects the packet to AF_XDP.
3. AF_XDP sends the packet to user-space application.
4. After the specified latency, application transforms the packet to an
   ECHOREPLY one.
5. Application sends the transformed packets by AF_XDP.

## Build and run

```bash
# git clone git@github.com:Asphaltt/ping-latency-injector.git
# cd ping-latency-injector
# go generate && go build
# ./ping-latency-injector -h
Usage of ./ping-latency-injector:
  -D, --dev string   device to inject latency to ping
  -L, --lat string   latency to delay, 1ms <= latency <= 10s (default "1ms")
pflag: help requested
# ./ping-latency-injector -D enp0s8 -L 600ms
2023/03/12 16:01:37 Attached XDP to enp0s8

# echo On other host terminal
# ping -s 1400 -c3 192.168.1.138
PING 192.168.1.138 (192.168.1.138): 1400 data bytes
1408 bytes from 192.168.1.138: icmp_seq=0 ttl=64 time=600.888 ms
1408 bytes from 192.168.1.138: icmp_seq=1 ttl=64 time=0.661 ms  # Cancel suddenly
1408 bytes from 192.168.1.138: icmp_seq=2 ttl=64 time=0.495 ms

--- 192.168.1.138 ping statistics ---
3 packets transmitted, 3 packets received, 0.0% packet loss
round-trip min/avg/max/stddev = 0.495/200.681/600.888/282.989 ms
```

## License

BSD-3 License
