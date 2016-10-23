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
#include <map>
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

    void CategoryTreatmentPlanTest::test_build_treatment() {

        std::vector<float> title_a(_target.begin(), _target.begin() + 4);
        std::vector<float> title_b(_target.begin() + 4, _target.end());

        std::vector<float> emp_a(_target.begin(), _target.begin() + 2);
        std::vector<float> emp_b(_target.begin() + 2, _target.end());

        std::map<std::string, std::vector<float>> title_groups;
        std::map<std::string, std::vector<float>> emp_groups;

        title_groups["Title A"] = title_a;
        title_groups["Title B"] = title_b;
        emp_groups["Employer A"] = emp_a;
        emp_groups["Employer B"] = emp_b;

        std::map<std::string, float> title_treated;
        std::map<std::string, float> emp_treated;

        _plan.build_treatment(title_groups, title_treated, "NA");
        _plan.build_treatment(emp_groups, emp_treated, "NA");

        CPPUNIT_ASSERT_DOUBLES_EQUAL(108.333335f, title_treated["NA"], _tolerance);
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
