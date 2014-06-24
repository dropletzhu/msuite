#!/usr/bin/python

from optparse import OptionParser
from scapy.all import *
import random

v6_conf = "/proc/sys/net/ipv6/conf/"

def get_interface_mtu(interface):
	mtu_path = v6_conf + interface + "/mtu"
	f = open(mtu_path, "r")
	mtu = f.readline()
	f.close()
	return mtu

def send_v6_udp_fragment(options, mtu):
	offset = 0
	mtu = mtu & 0xfffffff8
	#ipv6 header 40 bytes; frag header 8 bytes; udp header 8 bytes
	payload_len = mtu - 56
	id = random.randint(1,65535)
	packets = []

	while offset < options.length:
		if offset + payload_len > options.length:
			payload_len = options.length - offset

		print "offset: %d, payload length: %d, mtu: %d\n" % (offset,payload_len,mtu)

		payload = '0' * payload_len
		u = UDP(sport=options.sport, dport=options.dport)
		u.add_payload(payload)
		u.len = payload_len + 8

		if offset + payload_len >= options.length:
			frag = IPv6ExtHdrFragment(offset=offset>>3, m=0, id=id, nh=17)
		else:
			frag = IPv6ExtHdrFragment(offset=offset>>3, m=1, id=id, nh=17)

		i = IPv6(src=options.src, dst=options.dst)
		i.plen = u.len + 8
		p = (i/frag/u)
		#p.show()
		packets.append(p)
		offset += payload_len + 8

	#send fragments in ascending order when count is even; send fragments 
	#in reverse order when count is odd
	if options.count%2 == 0:
		for p in packets:
			send(p)
	else:
		for i in range(0,len(packets)):
			send(packets[len(packets) - i - 1])

def send_v6_udp(options):
	mtu = int(get_interface_mtu(options.interface))
	if mtu == 0:
		print "mtu is wrong\n"
		return

	print "packet length: %d, mtu: %d\n" % (options.length, mtu)

	if options.length > mtu:
		send_v6_udp_fragment(options, mtu)
	else:
		payload = '0' * options.length
		u = UDP(sport=options.sport, dport=options.dport)
		u.add_payload(payload)
		u.len = options.length + 8
		i = IPv6(src=options.src, dst=options.dst)
		i.plen = u.len
		p = (i/u)
		#p.show()
		send(p)

if __name__ == "__main__":
	usage = "usage: ./udpclient6.py -s [src] -d [dst] -q [sport] -p [dport] -l [length] -i [intf] -c [count]\n"

	parser = OptionParser(usage)
	parser.add_option("-s", "--src", dest="src", help="source address")
	parser.add_option("-d", "--dst", dest="dst", help="destination address")
	parser.add_option("-q", "--sport", dest="sport", type="int", help="source port")
	parser.add_option("-p", "--dport", dest="dport", type="int", help="destination port")
	parser.add_option("-l", "--length", dest="length", type="int", help="packet length")
	parser.add_option("-i", "--interface", dest="interface", help="interface name")
	parser.add_option("-c", "--count", dest="count", type="int", help="packet count")
	(options, args) = parser.parse_args()

	if not options.src or not options.dst:
		parser.print_help()
	else:
		i = 0
		while i < options.count:
			send_v6_udp(options)
			i += 1
