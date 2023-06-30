AM_CFLAGS = -Wall -std=c11 -I$(top_srcdir)/src

check_PROGRAMS = jso_array_test jso_ht_test jso_object_test jso_string_test \
    schema/jso_schema_array_test schema/jso_schema_data_test

TESTS = jso_array_test jso_ht_test jso_object_test jso_string_test \
    schema/jso_schema_array_test schema/jso_schema_data_test

jso_array_test_LDADD = -lcmocka ../../src/libjso.a
jso_ht_test_LDADD = -lcmocka ../../src/libjso.a
jso_object_test_LDADD = -lcmocka ../../src/libjso.a
jso_string_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_array_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_array_test_LDFLAGS = -Wl,--wrap=jso_schema_value_free
schema_jso_schema_data_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_data_test_LDFLAGS = -Wl,--wrap=jso_ht_get_by_cstr_key
