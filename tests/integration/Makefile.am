AM_CFLAGS = -Wall -std=c11 -I$(top_srcdir)/src

check_PROGRAMS = jso_pointer_test jso_schema_draft_04_test jso_schema_draft_06_test

TESTS = jso_pointer_test jso_schema_draft_04_test jso_schema_draft_06_test

jso_pointer_test_LDADD = -lcmocka ../../src/libjso.a
jso_schema_draft_04_test_LDADD = -lcmocka ../../src/libjso.a
jso_schema_draft_06_test_LDADD = -lcmocka ../../src/libjso.a
