#ifndef DNS_Table_h
#define DNS_Table_h 

#define DOMAIN_NAME_SIZE 100
#define IP_ADDRESS_SIZE 20

#include<stdlib.h>

// NULL-based Linked-List
struct dns_table{
	char *domain_name;
	char **ips;
	int num_ips;
	struct dns_table *next;
};
typedef struct dns_table DNS_Table;


DNS_Table* make_dns_entry(char *domain_name, char *ip){
	DNS_Table *dns_table = (DNS_Table*)malloc(sizeof(DNS_Table));
	dns_table->domain_name = (char*)malloc(sizeof(char)*DOMAIN_NAME_SIZE);
	dns_table->ips = (char**)malloc(sizeof(char*));
	*(dns_table->ips+0) = (char*)malloc*sizeof(char)*IP_ADDRESS_SIZE);
	memcpy(dns_table->domain_name, domain_name, sizeof(domain_name));
	memcpy(*(dns_table->ips+0), domain_name, sizeof(domain_name));
	dns_table->num_ips = 1;
	dns_table->next = NULL;
	return dns_table;
}


DNS_Table add_domain(char *domain_name, char *ip, DNS_Table *table){
	DNS_Table *handle = table;
	DNS_Table *entry = NULL;
	DNS_Table *prev = NULL;
	char *ip_parser;
	while(handle!=NULL){
		if(strcmp(handle->domain_name, domain_name)==0){
			entry = handle;
		}
		for(i=0;i<handle->num_ips;i++){
			if(strcmp(*(handle->ips+i), ip)==0){
				return NULL;     // IP already exists
			}
		}
		prev = handle;
		handle = handle->next;
	}
	if(entry!=NULL){
		entry->ips = (char**)realloc(entry->ips, sizeof(char*)*(entry->num_ips+1));
		*(entry->ips+entry->num_ips) = (char*)malloc(sizeof(char)*IP_ADDRESS_SIZE);
		*(entry->ips+entry->num_ips) = ip;
	}
	else{
		entry = make_dns_entry(domain_name, ip);
		if(prev==NULL){
			table = entry;
		}
		else{
			prev->next = entry;
		}
	}
	return table;
}