#include "../../src/jso_array.h"
#include "../../src/jso.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
 
/* A test case that tests appending array item. */
static void jso_test_array_append(void **state) {
    (void) state; /* unused */

    jso_value val1, val2, val3, *val;
    JSO_VALUE_SET_INT(val1, 1);
    JSO_VALUE_SET_INT(val2, 2);
    JSO_VALUE_SET_INT(val3, 3);

    jso_array *arr = jso_array_alloc();
    jso_array_append(arr, &val1);
    jso_array_append(arr, &val2);
    jso_array_append(arr, &val3);

    jso_int i = 1;
    JSO_ARRAY_FOREACH(arr, val)
    {
        assert_int_equal(i++, JSO_IVAL_P(val));
    }
    JSO_ARRAY_FOREACH_END;

    jso_array_free(arr);
}
 
int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(jso_test_array_append),
    };
 
    return cmocka_run_group_tests(tests, NULL, NULL);
}