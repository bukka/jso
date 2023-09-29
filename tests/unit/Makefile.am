AM_CFLAGS = -Wall -std=c11 -I$(top_srcdir)/src

check_PROGRAMS = jso_array_test jso_ht_test jso_object_test jso_string_test \
    schema/jso_schema_array_test schema/jso_schema_data_test schema/jso_schema_error_test \
    schema/jso_schema_keyword_array_test schema/jso_schema_keyword_object_test \
    schema/jso_schema_keyword_regexp_test schema/jso_schema_keyword_scalar_test

TESTS = jso_array_test jso_ht_test jso_object_test jso_string_test \
    schema/jso_schema_array_test schema/jso_schema_data_test schema/jso_schema_error_test \
    schema/jso_schema_keyword_array_test schema/jso_schema_keyword_object_test \
    schema/jso_schema_keyword_regexp_test schema/jso_schema_keyword_scalar_test

jso_array_test_LDADD = -lcmocka ../../src/libjso.a
jso_ht_test_LDADD = -lcmocka ../../src/libjso.a
jso_object_test_LDADD = -lcmocka ../../src/libjso.a
jso_string_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_array_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_array_test_LDFLAGS = -Wl,--wrap=jso_schema_value_free
schema_jso_schema_data_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_data_test_LDFLAGS = -Wl,--wrap=jso_ht_get_by_cstr_key
schema_jso_schema_error_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_keyword_array_test_LDFLAGS = -Wl,--wrap=jso_array_are_all_items_of_type,--wrap=jso_array_free,--wrap=jso_array_is_unique,--wrap=jso_schema_array_alloc,--wrap=jso_schema_array_append,--wrap=jso_schema_array_free,--wrap=jso_schema_data_get,--wrap=jso_schema_value_parse
schema_jso_schema_keyword_array_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_keyword_object_test_LDFLAGS = -Wl,--wrap=jso_array_free,--wrap=jso_object_add,--wrap=jso_object_alloc,--wrap=jso_object_free,--wrap=jso_object_resize,--wrap=jso_schema_data_get,--wrap=jso_schema_keyword_get_regexp_code,--wrap=jso_schema_keyword_validate_array_of_strings,--wrap=jso_schema_value_free,--wrap=jso_schema_value_parse
schema_jso_schema_keyword_object_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_keyword_regexp_test_LDFLAGS = -Wl,--wrap=jso_re_code_alloc,--wrap=jso_re_code_free,--wrap=jso_re_compile,--wrap=jso_re_get_error_message,--wrap=jso_schema_data_get
schema_jso_schema_keyword_regexp_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_keyword_scalar_test_LDFLAGS = -Wl,--wrap=jso_schema_data_check_type,--wrap=jso_schema_data_get,--wrap=jso_schema_data_get_value_fast
schema_jso_schema_keyword_scalar_test_LDADD = -lcmocka ../../src/libjso.a