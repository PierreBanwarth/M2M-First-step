#!/bin/sh

# Create the tap (ethernet frame) as opposed to tun (ip frames) 
tunctl -u felipec -t tap0
ifconfig tap0 192.168.100.1 up

# NAT: as root in the host:
echo 1 > /proc/sys/net/ipv4/ip_forward
iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
iptables -I FORWARD 1 -i tap0 -j ACCEPT
iptables -I FORWARD 1 -o tap0 -m state --state RELATED,ESTABLISHED -j ACCEPT

# Update: You need to run your guest like this:
#   $ qemu-kvm -hda winxp.cow -m 512 -net nic -net tap,ifname=tap0,script=no
#
#In your guest:
# ip addr: 192.168.100.2
# gateway: 192.168.100.1
# dns: 8.8.8.8


