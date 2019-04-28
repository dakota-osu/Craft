#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../src/matrix.h"

#include <CUnit/CUnit.h>
#include "matrix_test.h"

#define EPSILON 0.0001
#define M_PI 3.14159265358979323846

static int float_equals(float a, float b) { return abs(a - b) < EPSILON; }

static void properly_normalizes_non_zero_vector3() {

    float ox, oy, oz;

    ox = 4; 
    oy = 5;
    oz = 7;

    normalize(&ox, &oy, &oz);

    float sqr_length = ox*ox + oy*oy + oz*oz;

    CU_ASSERT(float_equals(sqr_length, 1));
}

static void properly_handles_zero_vector3() {

    float ox, oy, oz;

    ox = oy = oz = 0;

    normalize(&ox, &oy, &oz);

    float sqr_length = ox*ox + oy*oy + oz*oz;

    CU_TEST_FATAL(sqr_length);
}

static void properly_constructs_identity_matrix() {
    float matrix[16];
    mat_identity(matrix);

    // four component vector
    float vector[4];

    vector[0] = 3;
    vector[1] = 4;
    vector[2] = 5;
    vector[3] = 89;

    float result[4];

    mat_vec_multiply(result, matrix, vector);

    CU_ASSERT(float_equals(vector[0], result[0]));
    CU_ASSERT(float_equals(vector[1], result[1]));
    CU_ASSERT(float_equals(vector[2], result[2]));
    CU_ASSERT(float_equals(vector[3], result[3]));
}

static void properly_constructs_translation_matrix() {
    float matrix[16];
    mat_translate(matrix, -4, 5, 90);

    float vector[4];
    vector[0] = 5;
    vector[1] = 6;
    vector[2] = 7;
    // note for translating a point in R3, the last component needs to be 
    // a one. translations aren't really linear transforms, so translating 
    // with a matrix is somewhat of a hack
    vector[3] = 1;

    float result[4];

    mat_vec_multiply(result, matrix, vector);

    CU_ASSERT(float_equals(result[0], 1));
    CU_ASSERT(float_equals(result[1], 11));
    CU_ASSERT(float_equals(result[2], 97));
    CU_ASSERT(float_equals(result[3], vector[3]));
}

static void properly_constructs_rotation_matrix() {
    float matrix[16];
    mat_rotate(matrix, 1, 1, 1, M_PI / 2.0f);

    float result[4];

    float eigen[4];
    eigen[0] = 3;
    eigen[1] = 3;
    eigen[2] = 3;
    eigen[3] = 1;

    mat_vec_multiply(result, matrix, eigen);

    CU_ASSERT(float_equals(eigen[0], result[0]));
    CU_ASSERT(float_equals(eigen[1], result[1]));
    CU_ASSERT(float_equals(eigen[2], result[2]));
    CU_ASSERT(float_equals(eigen[3], result[3]));

    float vector[4];
    vector[0] = 1;
    vector[1] = 0;
    vector[2] = -1;
    vector[3] = 0;

    mat_vec_multiply(result, matrix, eigen);

    CU_ASSERT(float_equals(0, result[0]));
    CU_ASSERT(float_equals(0, result[1]));
    CU_ASSERT(float_equals(1, result[2]));
    CU_ASSERT(float_equals(vector[3], result[3]));
}  

static void matrix_vector_product() {
    // matricies are column major as opposed to row major; 
    // transposed from normal notation
    float matrix[] = {
        1, 0, 0, 0,
        3, 3, 0, 1,
        2, 1, 3, 0,
        4, 1, 0, 1
    };

    float vector[] = {
        3, 4, 1, -9
    };

    float expected[] = {
        -19, 4, 3, -5 
    };

    float result[4];

    mat_vec_multiply(result, matrix, vector);

    CU_ASSERT(float_equals(result[0], expected[0]));
    CU_ASSERT(float_equals(result[1], expected[1]));
    CU_ASSERT(float_equals(result[2], expected[2]));
    CU_ASSERT(float_equals(result[3], expected[3]));
}

static void matrix_matrix_product() {
    float lhs_matrix[] = {
        4, 5, 3, 2,
        1, 2, 3, 1,
        0, 1, 0, 1,
        1, 0, 0, 0
    };

    float rhs_matrix[] = {
        3, 9, 0, 0,
        0, 5, 0, 0,
        1, 3, 9, 0,
        0, 1, 3, 9 
    };

    float expected_matrix[] = {
        21, 33, 36, 15,
        5,  10, 15, 5,
        7, 20, 12, 14,
        10, 5,  3, 4
    };

    float result[16];

    mat_multiply(result, lhs_matrix, rhs_matrix);

    for(int i = 0; i < 16; i++) {
        CU_ASSERT(float_equals(result[i], expected_matrix[i]));
    }
}

static CU_TestInfo normalization_tests[] = {
    {"normalization works for non zero vectors", properly_normalizes_non_zero_vector3},
    {"normalization handles zero vector", properly_handles_zero_vector3},
    CU_TEST_INFO_NULL
};

static CU_TestInfo matrix_transform_tests[] = {
    {"properly constructs the identity matrix", properly_constructs_identity_matrix},
    {"properly constructs translation matricies", properly_constructs_translation_matrix},
    {"properly constructs rotation matrix", properly_constructs_translation_matrix},
    CU_TEST_INFO_NULL
};

static CU_TestInfo matrix_product_tests[] = {
    {"multipling a vector through a matrix", matrix_vector_product},
    {"multipling two matricies", matrix_matrix_product},
    CU_TEST_INFO_NULL
};

static CU_SuiteInfo suites[] = {
    {"normalization suite", NULL, NULL, NULL, NULL, normalization_tests},
    {"matrix transforms suite", NULL, NULL, NULL, NULL, matrix_transform_tests},
    {"matrix product suite", NULL, NULL, NULL, NULL, matrix_product_tests},
    CU_SUITE_INFO_NULL
};

void MatrixTest_AddTests() {
    assert(NULL != CU_get_registry());
    assert(!CU_is_test_running());

    if(CU_register_suites(suites) != CUE_SUCCESS) {
        fprintf(stderr, "suite registration failed - %s\n", CU_get_error_msg());
        exit(EXIT_FAILURE);
    }
}
