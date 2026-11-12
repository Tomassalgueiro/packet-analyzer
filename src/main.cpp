#include <pcap/pcap.h>
#include <iostream>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <cstring>

void packet_handler(u_char *user, const struct pcap_pkthdr *header, const u_char *packet){

    if (header->caplen < sizeof(struct ether_header))
        return;

    struct ether_header eth;
    std::memcpy(&eth, packet, sizeof(eth));

    uint16_t ether_type = ntohs(eth.ether_type);

    if (ether_type != ETHERTYPE_IP)
        return;

    struct ip ip_header;
    std::memcpy(&ip_header,packet + sizeof(struct ether_header),sizeof(ip_header));

    int ip_header_len = ip_header.ip_hl * 4;

    if (ip_header_len < 20 ||
        header->caplen < sizeof(struct ether_header) + ip_header_len)
        return;

    char src_ip[INET_ADDRSTRLEN];
    char dst_ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &ip_header.ip_src, src_ip, sizeof(src_ip));
    inet_ntop(AF_INET, &ip_header.ip_dst, dst_ip, sizeof(dst_ip));

    std::cout << src_ip << " -> " << dst_ip << '\n';
}


void catch_packets(pcap_if_t *device, char* errbuf){

	pcap_t *handle = pcap_open_live(device->name,BUFSIZ,1,1000,errbuf);
	if (handle == nullptr){
		std::cerr << "couldnt open device" << errbuf << '\n';
		return;
	}

	std::cout << "Capturing packets on: " << device->name << '\n';
	int result = pcap_loop(handle, 0, packet_handler, nullptr);

	if (result == -1) {
	    std::cerr << "Capture error: " << pcap_geterr(handle) << '\n';
	}

	pcap_close(handle);
}

int main(){

	pcap_if_t *devs;
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_if_t *selected_dev = nullptr;
	int count = 0, device_index = -1, index = 0;
	if(pcap_findalldevs(&devs, errbuf)){
		fprintf(stderr, "Couldn't find devices: %s\n", errbuf);
		return 2;
	}

	std::cout <<  "Device List" << std::endl;
	for (pcap_if_t *dev = devs; dev != nullptr; dev = dev->next){
		printf("\t %d.: %s\n", count++, dev->name);
	}
	count--;

	printf("Insert a number to select device (0 - %d): ", count);
	std::cin >> device_index;

	if (device_index < 0 || device_index > count) {
	    std::cerr << "Invalid device number\n";
	    pcap_freealldevs(devs);
	    return 1;
	}

	for (pcap_if_t *dev = devs; dev != nullptr; dev = dev->next){
		if (index == device_index){
			selected_dev = dev;
			break;
		}
		index++;
	}

	if (selected_dev == nullptr){
		std::cerr << "invalid device number\n";
		pcap_freealldevs(devs);
		return 1;

	}
		
	catch_packets(selected_dev, errbuf);

	pcap_freealldevs(devs);

	return 0;

}
