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

#ifndef KMBNW_MULTITREAT_H
#define KMBNW_MULTITREAT_H

namespace multitreat {
    class CategoryTreatmentPlan {
        public:
            CategoryTreatmentPlan();
            void add_pair(std::string category, float target);
            void build_treatment(std::map<std::string, float> &treatment);

        private:
            // the mapping from a category to the numeric targets
            std::map<std::string, std::vector<float> > _group_targets;

            void fill_group_stats(
                std::map<std::string, float> &means,
                std::map<std::string, float> &std_devs,
                std::map<std::string, uint>  &counts);
    };
}
#endif //KMBNW_MULTITREAT_H
