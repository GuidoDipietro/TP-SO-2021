#include "CUnit/Basic.h"
#include <unistd.h>

#include "include/test_protocolo.h"

extern CU_TestInfo tests_protocolo[];
extern CU_TestInfo tests_ejemplo[]; // ver test_ejemplo.c

CU_SuiteInfo suites[] = {
    { "suite_protocolo", NULL, NULL, crear_conexiones, cerrar_conexiones, tests_protocolo },
    { "suite_ejempol", NULL, NULL, NULL, NULL, tests_ejemplo },
    CU_SUITE_INFO_NULL
};

// por si hay alguien con curiosidad, los CU_SuiteInfo son asi:
//{ "suit1", init_suite_func, clean_suite_func, setup_suite_func, teardown_suite_func, test_array1}

int main() {
    CU_initialize_registry();

    CU_register_suites(suites);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}