#include "test_common.h"
#include "tests.h"
#include "../src/dict.h"
#include "../src/memory.h"

void test_dict_init() {
    BEGIN_TEST();

    Dict dict;
    dict_init(&dict);

    ASSERT(dict.count == 0, "Expected zero count");
    ASSERT(dict.capacity == 0, "Expected zero capacity");
    ASSERT(!dict.entries, "Expected NULL entries");

    END_TEST();
}

void test_dict_should_not_get_from_empty() {
    BEGIN_TEST();

    Dict dict;
    dict_init(&dict);

    Val val = MK_NIL_VAL;
    ObjStr* key = alloc_str_no_gc("key", 3);
    bool exists = dict_get(&dict, key, &val);

    ASSERT(!exists, "Expected no matching entry in empty dict");

    END_TEST();
}

void test_dict_should_put_and_get_single() {
    BEGIN_TEST();

    Dict dict;
    dict_init(&dict);

    Val val = MK_NUM_VAL(1234);
    ObjStr* key = alloc_str_no_gc("key", 3);
    dict_put(&dict, key, val);

    Val result;
    bool exists = dict_get(&dict, key, &result);

    ASSERT(exists, "Expected added entry to be found");
    ASSERT(UNWRAP_NUM(val) == UNWRAP_NUM(result), "Expected retrieved value to match inserted value");

    END_TEST();
}

void test_dict_should_not_find_after_delete() {
    BEGIN_TEST();

    Dict dict;
    dict_init(&dict);

    Val val = MK_NUM_VAL(1234);
    ObjStr* key = alloc_str_no_gc("key", 3);
    dict_put(&dict, key, val);

    bool exists = dict_del(&dict, key);
    Val result;
    bool exists_after_delete = dict_get(&dict, key, &result);

    ASSERT(exists, "Expected entry to be found during deletion");
    ASSERT(!exists_after_delete, "Expected entry to not be found after deletion");

    END_TEST();
}

static void test_two_put_get(ObjStr* key, Val val, ObjStr* other_key, Val other_val) {
    Dict dict;
    dict_init(&dict);

    dict_put(&dict, key, val);
    dict_put(&dict, other_key, other_val);

    Val result;
    bool exists = dict_get(&dict, key, &result);
    Val other_result;
    bool other_exists = dict_get(&dict, other_key, &other_result);

    ASSERT(exists, "Expected first added entry to be found");
    ASSERT(exists, "Expected second added entry to be found");
    ASSERT(UNWRAP_NUM(val) == UNWRAP_NUM(result), "Expected first retrieved value to match inserted value");
    ASSERT(UNWRAP_NUM(other_val) == UNWRAP_NUM(other_result), "Expected second retrieved value to match inserted value");
}

void test_dict_should_put_and_get_multiple_distinct() {
    BEGIN_TEST();

    Val val = MK_NUM_VAL(1234);
    ObjStr* key = alloc_str_no_gc("key", 3);
    Val other_val = MK_NUM_VAL(4321);
    ObjStr* other_key = alloc_str_no_gc("other_key", 9);

    test_two_put_get(key, val, other_key, other_val);

    END_TEST();
}

void test_dict_should_put_and_get_multiple_conflicting() {
    BEGIN_TEST();

    Val val = MK_NUM_VAL(1234);
    ObjStr* key = alloc_str_no_gc("key", 3);
    Val other_val = MK_NUM_VAL(4321);
    ObjStr* other_key = alloc_str_no_gc("other_key", 9);

    // simulate conflict by ensuring equal hash
    key->hash = 0;
    other_key->hash = 0;

    test_two_put_get(key, val, other_key, other_val);

    END_TEST();
}

void run_all_test_dict() {
    BEGIN_SUITE();

    test_dict_init();
    test_dict_should_not_get_from_empty();
    test_dict_should_put_and_get_single();
    test_dict_should_not_find_after_delete();
    test_dict_should_put_and_get_multiple_distinct();
    test_dict_should_put_and_get_multiple_conflicting();

    END_SUITE();
}
