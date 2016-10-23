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
    void CategoryTreatmentPlanTest::test_build_treatment() {
        std::vector<float> target = { 25.0, 50.0, 75.0, 100.0, 100.0, 300.0 };

        std::vector<float> title_a(target.begin(), target.begin() + 4);
        std::vector<float> title_b(target.begin() + 4, target.end());

        std::vector<float> emp_a(target.begin(), target.begin() + 2);
        std::vector<float> emp_b(target.begin() + 2, target.end());

        std::map<std::string, std::vector<float>> title_groups;
        std::map<std::string, std::vector<float>> emp_groups;

        title_groups["Title A"] = title_a;
        title_groups["Title B"] = title_b;
        emp_groups["Employer A"] = emp_a;
        emp_groups["Employer B"] = emp_b;

        multitreat::CategoryTreatmentPlan<std::string> plan;

        std::map<std::string, float> title_treated;
        std::map<std::string, float> emp_treated;

        plan.build_treatment(title_groups, title_treated, "NA");
        plan.build_treatment(emp_groups, emp_treated, "NA");

        std::cout << "Titles: " << std::endl;

        for (auto& kv : title_treated) {
            std::cout << kv.first << ": " << kv.second << std::endl;
        }

        std::cout << std::endl << "Employers: " << std::endl;

        for (auto& kv : emp_treated) {
            std::cout << kv.first << ": " << kv.second << std::endl;
        }
    }

        /*
            Expecting this output:

            Titles:
            NA: 108.333
            Title A: 65.9761
            Title B: 161.653

            Employers:
            Employer A: 43.3426
            Employer B: 136.296
            NA: 108.333

           From this original input:
    {"title": "A", "amount": 25, "employer": "A", "title_catN": 65.97610994, "employer_catN": 43.34262378}
    {"title": "A", "amount": 50, "employer": "A", "title_catN": 65.97610994, "employer_catN": 43.34262378}
    {"title": "A", "amount": 75, "employer": "B", "title_catN": 65.97610994, "employer_catN": 136.2962514}
    {"title": "A", "amount": 100, "employer": "B", "title_catN": 65.97610994, "employer_catN": 136.2962514}
    {"title": "B", "amount": 100, "employer": "B", "title_catN": 161.6528632, "employer_catN": 136.2962514}
    {"title": "B", "amount": 300, "employer": "B", "title_catN": 161.6528632, "employer_catN": 136.2962514}
     * */
}

int main(int argc, char **argv) {
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );
    runner.run();
    return 0;
}
