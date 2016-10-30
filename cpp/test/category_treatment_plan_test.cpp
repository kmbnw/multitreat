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

#include <iostream>
#include <unordered_map>
#include <vector>
#include "multitreat.h"
#include "category_treatment_plan_test.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

CPPUNIT_TEST_SUITE_REGISTRATION(multitreat::CategoryTreatmentPlanTest);

namespace multitreat {

    void CategoryTreatmentPlanTest::setUp() {
    }

    void CategoryTreatmentPlanTest::tearDown() {
    }

    void CategoryTreatmentPlanTest::test_build_stdev_zero() {
        float response = 25.0f;
        CategoryTreatmentPlan<std::string> plan;

        for (size_t i = 0; i < 20; ++i) {
            plan.add("X1", response);
            plan.add("X2", response);
        }

        std::unordered_map<std::string, float> treated;
        plan.build(treated, "NA");

        CPPUNIT_ASSERT_DOUBLES_EQUAL(response, treated["X1"], _tolerance);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(response, treated["X2"], _tolerance);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(response, treated["NA"], _tolerance);
    }

    void CategoryTreatmentPlanTest::test_build_one_item() {
        float response = 523.0f;
        CategoryTreatmentPlan<std::string> plan;

        plan.add("X1", response);
        plan.add("X2", response);

        std::unordered_map<std::string, float> treated;
        plan.build(treated, "NA");

        CPPUNIT_ASSERT(3 == treated.size());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(response, treated["X1"], _tolerance);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(response, treated["X2"], _tolerance);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(response, treated["NA"], _tolerance);
        CPPUNIT_ASSERT(3 == treated.size());
    }

    void CategoryTreatmentPlanTest::test_build_empty() {
        CategoryTreatmentPlan<std::string> plan;

        std::unordered_map<std::string, float> treated;
        plan.build(treated, "NA");

        CPPUNIT_ASSERT(0 == treated.size());
    }

    // happy path testing
    void CategoryTreatmentPlanTest::test_build() {
        std::vector<float> target { 25, 50, 75, 100, 100, 300 };

        std::vector<std::string> titles { "A", "A", "A", "A", "B", "B" };
        std::vector<std::string> emps { "Fake Inc.", "Fake Inc.", "Evil Inc.", "Evil Inc.", "Evil Inc.", "Evil Inc." };

        CategoryTreatmentPlan<std::string> title_plan;
        CategoryTreatmentPlan<std::string> emp_plan;

        for (size_t i = 0; i < target.size(); ++i) {
            title_plan.add(titles[i], target[i]);
            emp_plan.add(emps[i], target[i]);
        }

        std::unordered_map<std::string, float> title_treated;
        std::unordered_map<std::string, float> emp_treated;

        title_plan.build(title_treated, "NA");
        emp_plan.build(emp_treated, "NA");

        CPPUNIT_ASSERT(3 == title_treated.size());
        CPPUNIT_ASSERT(3 == emp_treated.size());

        float na_val = 108.333335f;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(na_val, title_treated["NA"], _tolerance);

        float title_a_expected = 63.7023429870605f;
        float title_b_expected = 153.389831542969f;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(title_a_expected, title_treated["A"], _tolerance);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(title_b_expected, title_treated["B"], _tolerance);

        float emp_a_expected = 38.6267242431641f;
        float emp_b_expected = 135.911819458008f;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(emp_a_expected, emp_treated["Fake Inc."], _tolerance);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(emp_b_expected, emp_treated["Evil Inc."], _tolerance);
    }
}

int main(int argc, char **argv) {
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );
    runner.run();
    return 0;
}
