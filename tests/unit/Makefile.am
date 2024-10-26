AM_CFLAGS = -Wall -std=c11 -I$(top_srcdir)/src

check_PROGRAMS = jso_array_test jso_builder_test jso_ht_test jso_object_test jso_string_test \
    schema/jso_schema_array_test schema/jso_schema_data_test schema/jso_schema_error_test \
    schema/jso_schema_keyword_array_test schema/jso_schema_keyword_freer_test \
    schema/jso_schema_keyword_object_test schema/jso_schema_keyword_regexp_test \
    schema/jso_schema_keyword_scalar_test schema/jso_schema_keyword_single_test \
    schema/jso_schema_keyword_test schema/jso_schema_keyword_types_test \
    schema/jso_schema_keyword_union_test schema/jso_schema_value_allocator_test \
    schema/jso_schema_value_freer_test schema/jso_schema_value_init_test \
    schema/jso_schema_value_parser_test

TESTS = jso_array_test jso_builder_test jso_ht_test jso_object_test jso_string_test \
    schema/jso_schema_array_test schema/jso_schema_data_test schema/jso_schema_error_test \
    schema/jso_schema_keyword_array_test schema/jso_schema_keyword_freer_test \
    schema/jso_schema_keyword_object_test schema/jso_schema_keyword_regexp_test \
    schema/jso_schema_keyword_scalar_test schema/jso_schema_keyword_single_test \
    schema/jso_schema_keyword_test schema/jso_schema_keyword_types_test \
    schema/jso_schema_keyword_union_test schema/jso_schema_value_allocator_test \
    schema/jso_schema_value_freer_test schema/jso_schema_value_init_test \
    schema/jso_schema_value_parser_test
     

jso_array_test_LDADD = -lcmocka ../../src/libjso.a
jso_builder_test_LDADD = -lcmocka ../../src/libjso.a
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
schema_jso_schema_keyword_freer_test_LDFLAGS = -Wl,--wrap=jso_schema_keyword_free_any,--wrap=jso_schema_keyword_free_string,--wrap=jso_schema_keyword_free_regexp,--wrap=jso_schema_keyword_free_array,--wrap=jso_schema_keyword_free_array_of_strings,--wrap=jso_schema_keyword_free_array_of_schema_objects,--wrap=jso_schema_keyword_free_object,--wrap=jso_schema_keyword_free_schema_object,--wrap=jso_schema_keyword_free_object_of_schema_objects
schema_jso_schema_keyword_freer_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_keyword_object_test_LDFLAGS = -Wl,--wrap=jso_array_free,--wrap=jso_object_add,--wrap=jso_object_alloc,--wrap=jso_object_free,--wrap=jso_object_resize,--wrap=jso_schema_data_get,--wrap=jso_schema_keyword_get_regexp_code,--wrap=jso_schema_keyword_validate_array_of_strings,--wrap=jso_schema_value_free,--wrap=jso_schema_value_parse
schema_jso_schema_keyword_object_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_keyword_regexp_test_LDFLAGS = -Wl,--wrap=jso_re_code_alloc,--wrap=jso_re_code_free,--wrap=jso_re_compile,--wrap=jso_re_get_error_message,--wrap=jso_schema_data_get
schema_jso_schema_keyword_regexp_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_keyword_scalar_test_LDFLAGS = -Wl,--wrap=jso_schema_data_check_type,--wrap=jso_schema_data_get,--wrap=jso_schema_data_get_value_fast,--wrap=jso_value_free
schema_jso_schema_keyword_scalar_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_keyword_single_test_LDFLAGS = -Wl,--wrap=jso_schema_keyword_get_any,--wrap=jso_schema_keyword_get_null,--wrap=jso_schema_keyword_get_bool,--wrap=jso_schema_keyword_get_int,--wrap=jso_schema_keyword_get_uint,--wrap=jso_schema_keyword_get_number,--wrap=jso_schema_keyword_get_string,--wrap=jso_schema_keyword_get_regexp,--wrap=jso_schema_keyword_get_array,--wrap=jso_schema_keyword_get_array_of_strings,--wrap=jso_schema_keyword_get_array_of_schema_objects,--wrap=jso_schema_keyword_get_object,--wrap=jso_schema_keyword_get_schema_object,--wrap=jso_schema_keyword_get_object_of_schema_objects,--wrap=jso_schema_keyword_get_object_of_schema_objects_or_array_of_strings,--wrap=jso_schema_keyword_get_regexp_object_of_schema_objects
schema_jso_schema_keyword_single_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_keyword_test_LDFLAGS = -Wl,--wrap=jso_schema_keyword_free,--wrap=jso_schema_keyword_get_ex,--wrap=jso_schema_keyword_get_union_of_2_types
schema_jso_schema_keyword_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_keyword_types_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_keyword_union_test_LDFLAGS = -Wl,--wrap=jso_schema_data_get_value_fast,--wrap=jso_schema_data_type_error,--wrap=jso_schema_keyword_get_ex,--wrap=jso_schema_keyword_get_types
schema_jso_schema_keyword_union_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_value_allocator_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_value_freer_test_LDFLAGS =  -Wl,--wrap=jso_re_code_free,--wrap=jso_schema_keyword_free,--wrap=jso_schema_reference_free,--wrap=jso_schema_uri_clear
schema_jso_schema_value_freer_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_value_init_test_LDFLAGS = -Wl,--wrap=jso_schema_keyword_set,--wrap=jso_schema_value_alloc,--wrap=jso_schema_value_data_alloc,--wrap=jso_schema_value_free
schema_jso_schema_value_init_test_LDADD = -lcmocka ../../src/libjso.a
schema_jso_schema_value_parser_test_LDFLAGS = -Wl,--wrap=jso_schema_array_alloc,--wrap=jso_schema_array_append,--wrap=jso_schema_array_free,--wrap=jso_schema_data_check_type,--wrap=jso_schema_data_get_value_fast,--wrap=jso_schema_keyword_set,--wrap=jso_schema_keyword_set_union_of_2_types,--wrap=jso_schema_keyword_validate_array_of_strings,--wrap=jso_schema_value_free,--wrap=jso_schema_value_init
schema_jso_schema_value_parser_test_LDADD = -lcmocka ../../src/libjso.a