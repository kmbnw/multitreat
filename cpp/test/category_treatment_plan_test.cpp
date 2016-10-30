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

    void CategoryTreatmentPlanTest::test_build_treatment_stdev_zero() {
        std::vector<float> all_same(20, 25.0);
        CategoryTreatmentPlan<std::string> plan;

        for (const auto& response: all_same) {
            plan.add("X1", response);
            plan.add("X2", response);
        }

        std::unordered_map<std::string, float> treated;
        plan.build(treated, "NA");

        CPPUNIT_ASSERT_DOUBLES_EQUAL(25.0, treated["X1"], _tolerance);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(25.0, treated["X2"], _tolerance);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(25.0, treated["NA"], _tolerance);
    }


    // happy path testing
    void CategoryTreatmentPlanTest::test_build_treatment() {
        std::vector<float> title_a(_target.begin(), _target.begin() + 4);
        std::vector<float> title_b(_target.begin() + 4, _target.end());

        std::vector<float> emp_a(_target.begin(), _target.begin() + 2);
        std::vector<float> emp_b(_target.begin() + 2, _target.end());

        CategoryTreatmentPlan<std::string> plan;
        for (const auto& response: title_a) {
            _plan.add("Title A", response);
        }

        for (const auto& response: title_b) {
            _plan.add("Title B", response);
        }

        for (const auto& response: emp_a) {
            _plan.add("Employer A", response);
        }

        for (const auto& response: emp_b) {
            _plan.add("Employer B", response);
        }

        std::unordered_map<std::string, float> title_treated;
        std::unordered_map<std::string, float> emp_treated;

        _plan.build(title_treated, "NA");
        _plan.build(emp_treated, "NA");

        float na_val = 108.333335f;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(na_val, title_treated["NA"], _tolerance);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(65.9761123f, title_treated["Title A"], _tolerance);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(161.652862f, title_treated["Title B"], _tolerance);
    }
}

int main(int argc, char **argv) {
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );
    runner.run();
    return 0;
}
