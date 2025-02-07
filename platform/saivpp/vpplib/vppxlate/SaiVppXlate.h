/*
 * Copyright (c) 2023 Cisco and/or its affiliates.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __SAI_VPP_XLATE_H_
#define __SAI_VPP_XLATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <netinet/in.h>

    typedef enum {
	VPP_NEXTHOP_NORMAL = 1,
	VPP_NEXTHOP_LOCAL = 2
    } vpp_nexthop_type_e;

    typedef struct vpp_ip_addr_ {
	int sa_family;
	union {
	    struct sockaddr_in ip4;
	    struct sockaddr_in6 ip6;
	} addr;
    } vpp_ip_addr_t;

    typedef struct vpp_ip_nexthop_ {
	vpp_ip_addr_t addr;
        const char *hwif_name;
	uint8_t weight;
        uint8_t preference;
	vpp_nexthop_type_e type;
        uint32_t flags;
    } vpp_ip_nexthop_t;

    typedef struct vpp_ip_route_ {
	vpp_ip_addr_t prefix_addr;
	unsigned int prefix_len;
        uint32_t vrf_id;
        bool is_multipath;
        unsigned int nexthop_cnt;
        vpp_ip_nexthop_t nexthop[0];
    } vpp_ip_route_t;

    typedef enum  {
        VPP_ACL_ACTION_API_DENY = 0,
        VPP_ACL_ACTION_API_PERMIT = 1,
        VPP_ACL_ACTION_API_PERMIT_STFULL = 2,
    } vpp_acl_action_e;

    typedef struct  _vpp_acl_rule {
        vpp_acl_action_e action;
        vpp_ip_addr_t src_prefix;
        vpp_ip_addr_t src_prefix_mask;
        vpp_ip_addr_t dst_prefix;
        vpp_ip_addr_t dst_prefix_mask;
        int proto;
        uint16_t srcport_or_icmptype_first;
        uint16_t srcport_or_icmptype_last;
        uint16_t dstport_or_icmpcode_first;
        uint16_t dstport_or_icmpcode_last;
        uint8_t tcp_flags_mask;
        uint8_t tcp_flags_value;
    } vpp_acl_rule_t;

    typedef struct _vpp_acl_ {
        char *acl_name;
        uint32_t count;
        vpp_acl_rule_t rules[0];
    } vpp_acl_t;

    typedef enum {
        VPP_IP_API_FLOW_HASH_SRC_IP = 1,
        VPP_IP_API_FLOW_HASH_DST_IP = 2,
        VPP_IP_API_FLOW_HASH_SRC_PORT = 4,
        VPP_IP_API_FLOW_HASH_DST_PORT = 8,
        VPP_IP_API_FLOW_HASH_PROTO = 16,
        VPP_IP_API_FLOW_HASH_REVERSE = 32,
        VPP_IP_API_FLOW_HASH_SYMETRIC = 64,
        VPP_IP_API_FLOW_HASH_FLOW_LABEL = 128,
    } vpp_ip_flow_hash_mask_e;

    typedef struct vpp_intf_status_ {
	char hwif_name[64];
	bool link_up;
    } vpp_intf_status_t;

    typedef enum {
	VPP_INTF_LINK_STATUS = 1,
    } vpp_event_type_e;

    typedef union vpp_event_data_ {
	vpp_intf_status_t intf_status;
    } vpp_event_data_t;

    typedef struct vpp_event_info_ {
	struct vpp_event_info_ *next;
	vpp_event_type_e type;
	vpp_event_data_t data;
    } vpp_event_info_t;

    typedef void (*vpp_event_free_fn)(vpp_event_info_t *);

    typedef struct vpp_event_queue_ {
	vpp_event_info_t *head;
	vpp_event_info_t **tail;
	vpp_event_free_fn free;
    } vpp_event_queue_t;

    extern vpp_event_info_t * vpp_ev_dequeue();
    extern void vpp_ev_free(vpp_event_info_t *evp);

    extern int init_vpp_client();
    extern int refresh_interfaces_list();
    extern int configure_lcp_interface(const char *hwif_name, const char *hostif_name, bool is_add);
    extern int create_loopback_instance(const char *hwif_name, uint32_t instance);
    extern int delete_loopback(const char *hwif_name, uint32_t instance);
    extern int get_sw_if_idx(const char *ifname);
    extern int create_sub_interface(const char *hwif_name, uint32_t sub_id, uint16_t vlan_id);
    extern int delete_sub_interface(const char *hwif_name, uint32_t sub_id);
    extern int set_interface_vrf(const char *hwif_name, uint32_t sub_id, uint32_t vrf_id, bool is_ipv6);
    extern int interface_ip_address_add_del(const char *hw_ifname, vpp_ip_route_t *prefix, bool is_add);
    extern int interface_set_state (const char *hwif_name, bool is_up);
    extern int hw_interface_set_mtu(const char *hwif_name, uint32_t mtu);
    extern int sw_interface_set_mtu(const char *hwif_name, uint32_t mtu, int type);

    extern int ip_vrf_add(uint32_t vrf_id, const char *vrf_name, bool is_ipv6);
    extern int ip_vrf_del(uint32_t vrf_id, const char *vrf_name, bool is_ipv6);

    extern int ip4_nbr_add_del(const char *hwif_name, struct sockaddr_in *addr,
			       bool is_static, uint8_t *mac, bool is_add);
    extern int ip6_nbr_add_del(const char *hwif_name, struct sockaddr_in6 *addr,
			       bool is_static, uint8_t *mac, bool is_add);
    extern int ip_route_add_del(vpp_ip_route_t *prefix, bool is_add);
    extern int vpp_ip_flow_hash_set(uint32_t vrf_id, uint32_t mask, int addr_family);

    extern int vpp_acl_add_replace(vpp_acl_t *in_acl, uint32_t *acl_index, bool is_replace);
    extern int vpp_acl_del(uint32_t acl_index);
    extern int vpp_acl_interface_bind(const char *hwif_name, uint32_t acl_index,
				      bool is_input);
    extern int vpp_acl_interface_unbind(const char *hwif_name, uint32_t acl_index,
					bool is_input);
    extern int interface_get_state(const char *hwif_name, bool *link_is_up);
    extern int vpp_sync_for_events();

#ifdef __cplusplus
}
#endif

#endif
