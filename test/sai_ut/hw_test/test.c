#include <stdio.h>
#include "sai.h"

#include <string.h>

#define OBJECTS_COUNT 2

typedef struct {
    sai_object_id_t           oid;
    sai_object_id_t           members[OBJECTS_COUNT];
}lag_obj_t;

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

    sai_lag_api_t             *lag_api;
    sai_object_id_t           lag_oid;
    sai_object_id_t           lag_member_oid;

    lag_obj_t                 lags[OBJECTS_COUNT];

    status = sai_api_initialize(0, &test_services);
    if (status != SAI_STATUS_SUCCESS) {
        fprintf(stderr, "Error! Function sai_api_initialize() failed with status=%d.\n", status);
        return 1;
    }

    status = sai_api_query(SAI_API_LAG, (void**)&lag_api);
    if (status != SAI_STATUS_SUCCESS) {
        fprintf(stderr, "Error! Function sai_api_query() failed with status=%d.\n", status);
        return 1;
    }

    memset(lags, 0, sizeof(lag_obj_t));

    for (int32_t i = 0; i < OBJECTS_COUNT; ++i) {
        status = lag_api->create_lag(&lag_oid, 0, NULL);
        if (status != SAI_STATUS_SUCCESS) {
            printf("Failed to create a LAG, status=%d\n", status);
            return 1;
        }
        lags[i].oid = lag_oid;
        for (int32_t j = 0; j < OBJECTS_COUNT; ++j) {
            status = lag_api->create_lag_member(&lag_member_oid, 0, NULL);
            if (status != SAI_STATUS_SUCCESS) {
                printf("Failed to create a LAG member, status=%d\n", status);
                return 1;
            }
            lags[i].members[j] = lag_member_oid;
        }
    }

    for (int32_t i = 0; i < OBJECTS_COUNT; ++i) {
        status = lag_api->get_lag_attribute(lags[i].oid, 0, NULL);
        if (status != SAI_STATUS_SUCCESS) {
            printf("Failed to get a LAG attributes, status=%d\n", status);
            return 1;
        }
    }

    for (int32_t i = 0; i < OBJECTS_COUNT; ++i) {
        status = lag_api->get_lag_member_attribute(lags[i].members[0], 0, NULL);
        if (status != SAI_STATUS_SUCCESS) {
            printf("Failed to get a LAG attributes, status=%d\n", status);
            return 1;
        }
    }

    status = lag_api->remove_lag_member(lags[0].members[1]);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove a LAG member, status=%d\n", status);
        return 1;
    }

    status = lag_api->get_lag_attribute(lags[0].oid, 0, NULL);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get a LAG attributes, status=%d\n", status);
        return 1;
    }

    status = lag_api->remove_lag_member(lags[1].members[0]);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove a LAG member, status=%d\n", status);
        return 1;
    }

    status = lag_api->get_lag_attribute(lags[1].oid, 0, NULL);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to get a LAG attributes, status=%d\n", status);
        return 1;
    }

    status = lag_api->remove_lag_member(lags[0].members[0]);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove a LAG member, status=%d\n", status);
        return 1;
    }

    status = lag_api->remove_lag_member(lags[1].members[1]);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove a LAG member, status=%d\n", status);
        return 1;
    }

    status = lag_api->remove_lag(lags[1].oid);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove a LAG, status=%d\n", status);
        return 1;
    }

    status = lag_api->remove_lag(lags[0].oid);
    if (status != SAI_STATUS_SUCCESS) {
        printf("Failed to remove a LAG, status=%d\n", status);
        return 1;
    }

    status = sai_api_uninitialize();
    if (status != SAI_STATUS_SUCCESS) {
        fprintf(stderr, "Error! Function sai_api_uninitialize() failed with status=%d.\n", status);
        return 1;
    }

    return 0;
}
