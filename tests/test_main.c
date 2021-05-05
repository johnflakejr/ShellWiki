#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../include/util.h"


/*
    Maybe use these later? Depends
*/
int init_suite(void) {return 0;}
int clean_suite(void) {return 0;}

int main()
{
    printf("Test Suite for ShellWiki\n");
    
    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }

    CU_pSuite suite1 = CU_add_suite("Shellwiki Tests", init_suite, clean_suite);

    if (NULL == suite1)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (CUE_SUCCESS != CU_basic_run_tests())
    {
      return CU_get_error();
    }

    CU_cleanup_registry();

    return 0;
}

//End of file

