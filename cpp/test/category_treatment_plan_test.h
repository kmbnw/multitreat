/*
 * Copyright 2016 Krysta M Bouzek
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <vector>
#include <cppunit/extensions/HelperMacros.h>

#ifndef KMBNW_CAT_PLAN_TEST_H
#define KMBNW_CAT_PLAN_TEST_H

namespace multitreat {
    class CategoryTreatmentPlanTest : public CppUnit::TestFixture {
        CPPUNIT_TEST_SUITE(CategoryTreatmentPlanTest);
        CPPUNIT_TEST(test_build_treatment);
        CPPUNIT_TEST(test_build_treatment_stdev_zero);
        CPPUNIT_TEST_SUITE_END();

        private:
            std::vector<float> _target = { 25.0, 50.0, 75.0, 100.0, 100.0, 300.0 };

            // for most of this kind of work we don't need super tight tolerance
            // so use the NA fill value
            float const _tolerance = 1e-6;

        public:
            void setUp();
            void tearDown();
            void test_build_treatment();
            void test_build_treatment_stdev_zero();
    };
}
#endif
