#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../include/util.h"


/*
    Maybe use these later? Depends
*/
int init_suite(void) {return 0;}
int clean_suite(void) {return 0;}

void test_capitalize_arg(void)
{
    char test[100];
    memset(test,0,100);
    strcpy(test,"dog");
    CU_ASSERT_EQUAL(0,strcmp("Dog", capitalize_arg(test)));

    memset(test,0,100);
    strcpy(test,"1. Start with a number.");
    CU_ASSERT_EQUAL(0,strcmp("1. Start with a number.", capitalize_arg(test)));
}

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

        
    CU_add_test(suite1, "\n\n Testing Capitalize\n\n",test_capitalize_arg);

    if (CUE_SUCCESS != CU_basic_run_tests())
    {
      return CU_get_error();
    }

    CU_cleanup_registry();

    return 0;
}

//End of file

