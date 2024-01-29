#include <stdio.h>
#include "sai.h"

const char* test_profile_get_value(
    _In_ sai_switch_profile_id_t profile_id,
    _In_ const char* variable)
{
    return 0;
}

int test_profile_get_next_value(
    _In_ sai_switch_profile_id_t profile_id,
    _Out_ const char** variable,
    _Out_ const char** value)
{
    return -1;
}

const service_method_table_t test_services = {
    test_profile_get_value,
    test_profile_get_next_value
};

int main()
{
    sai_status_t              status;
    sai_lag_api_t            *lag_api;
    sai_switch_api_t         *switch_api;
    sai_object_id_t           lag_oid[2];
    sai_object_id_t           lag_member_oid[4];
    sai_attribute_t           switch_attrs[2];
    sai_switch_notification_t notifications;
    sai_object_id_t           port_list[64];
    sai_attribute_t           lag_attrs[2];
    sai_attribute_t           lag_member_attrs[2];

    status = sai_api_initialize(0, &test_services);
    if (status != SAI_STATUS_SUCCESS) {
        fprintf(stderr, "Error! Function sai_api_initialize() failed with status=%d.\n", status);
        return 1;
    }

    status = sai_api_query(SAI_API_SWITCH, (void**)&switch_api);
    if (status != SAI_STATUS_SUCCESS) {
        fprintf(stderr, "Error! Function sai_api_query() failed with status=%d.\n", status);
        return 1;
    }

    status = switch_api->initialize_switch(0, "HW_ID", 0, &notifications);
    if (status != SAI_STATUS_SUCCESS) {
        fprintf(stderr, "Error! Function sai_switch_api_t::initialize_switch() failed with status=%d.\n", status);
        return 1;
    }

    switch_attrs[0].id = SAI_SWITCH_ATTR_PORT_LIST;
    switch_attrs[0].value.objlist.list = port_list;
    switch_attrs[0].value.objlist.count = 64;
    status = switch_api->get_switch_attribute(1, switch_attrs);
    for (int32_t ii = 1; ii < switch_attrs[0].value.objlist.count + 1; ii++) {
        printf("Port #%d OID: 0x%lX\n", ii, switch_attrs[0].value.objlist.list[ii]);
        port_list[ii - 1] = switch_attrs[0].value.objlist.list[ii];
    }

    status = sai_api_query(SAI_API_LAG, (void**)&lag_api);
    if (status != SAI_STATUS_SUCCESS) {
        fprintf(stderr, "Error! Function sai_api_query() failed with status=%d.\n", status);
        return 1;
    }

    status = lag_api->create_lag(&lag_oid[0], 0, NULL);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to create a LAG, status=%d\n", status);
        return 1;
    }

    lag_attrs[0].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    lag_attrs[0].value.oid = port_list[0];
    lag_attrs[1].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    lag_attrs[1].value.oid = lag_oid[0];

    status = lag_api->create_lag_member(&lag_member_oid[0], 2, lag_attrs);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to create a LAG, status=%d\n", status);
        return 1;
    }

    lag_attrs[0].value.oid = port_list[1];
    status = lag_api->create_lag_member(&lag_member_oid[1], 2, lag_attrs);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to create a LAG, status=%d\n", status);
        return 1;
    }

    status = lag_api->create_lag(&lag_oid[1], 0, NULL);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to create a LAG, status=%d\n", status);
        return 1;
    }

    lag_attrs[0].value.oid = port_list[2];
    lag_attrs[1].value.oid = lag_oid[1];

    status = lag_api->create_lag_member(&lag_member_oid[2], 2, lag_attrs);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to create a LAG, status=%d\n", status);
        return 1;
    }

    lag_attrs[0].value.oid = port_list[3];
    status = lag_api->create_lag_member(&lag_member_oid[3], 2, lag_attrs);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to create a LAG, status=%d\n", status);
        return 1;
    }

    sai_object_id_t lag_port_list[64];
    lag_attrs[0].id = SAI_LAG_ATTR_PORT_LIST;
    lag_attrs[0].value.objlist.list = lag_port_list;
    lag_attrs[0].value.objlist.count = 64;

    status = lag_api->get_lag_attribute(lag_oid[0], 1, lag_attrs);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get a LAG_ATTRIBUTE, status=%d\n", status);
        return 1;
    }

    status = lag_api->get_lag_attribute(lag_oid[1], 1, lag_attrs);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get a LAG_ATTRIBUTE, status=%d\n", status);
        return 1;
    }

    lag_member_attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    status = lag_api->get_lag_member_attribute(lag_member_oid[0], 1, lag_member_attrs);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get a LAG_MEMBER_ATTRIBUTE, status=%d\n", status);
        return 1;
    }
    printf("GET LAG_MEMBER#0x%lX. LAG_ID = 0x%lX\n", lag_member_oid[0], lag_member_attrs[0].value.oid);

    lag_member_attrs[0].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    status = lag_api->get_lag_member_attribute(lag_member_oid[2], 1, lag_member_attrs);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get a LAG_MEMBER_ATTRIBUTE, status=%d\n", status);
        return 1;
    }
    printf("GET LAG_MEMBER#0x%lX. PORT_ID = 0x%lX\n", lag_member_oid[2], lag_member_attrs[0].value.oid);

    status = lag_api->remove_lag_member(lag_member_oid[1]);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove a LAG_MEMBER, status=%d\n", status);
        return 1;
    }

    status = lag_api->get_lag_attribute(lag_oid[0], 1, lag_attrs);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get a LAG_ATTRIBUTE, status=%d\n", status);
        return 1;
    }

    status = lag_api->remove_lag_member(lag_member_oid[2]);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove a LAG_MEMBER, status=%d\n", status);
        return 1;
    }

    status = lag_api->get_lag_attribute(lag_oid[1], 1, lag_attrs);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get a LAG_ATTRIBUTE, status=%d\n", status);
        return 1;
    }

    status = lag_api->remove_lag_member(lag_member_oid[0]);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove a LAG_MEMBER, status=%d\n", status);
        return 1;
    }

    status = lag_api->remove_lag_member(lag_member_oid[3]);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove a LAG_MEMBER, status=%d\n", status);
        return 1;
    }

    status = lag_api->remove_lag(lag_oid[0]);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove a LAG_MEMBER, status=%d\n", status);
        return 1;
    }

    status = lag_api->remove_lag(lag_oid[1]);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove a LAG_MEMBER, status=%d\n", status);
        return 1;
    }

    switch_api->shutdown_switch(0);

    status = sai_api_uninitialize();
    if (status != SAI_STATUS_SUCCESS) {
        fprintf(stderr, "Error! Function sai_api_uninitialize() failed with status=%d.\n", status);
        return 1;
    }

    return 0;
}
