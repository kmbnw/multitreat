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
using System;
using System.Threading;
using System.Threading.Tasks;
using System.Collections.Generic;
using System.Linq;

using NUnit.Framework;

using kmbnw.Multitreat;

namespace kmbnw.MultitreatTest
{
    [TestFixture]
    public class CategoryTreatmentPlanTest
    {
        private const float _tolerance = (float) 1e-6;
        [Test]
        public void TestBuild()
        {
            var target = new[] { 25, 50, 75, 100, 100, 300 }.Select(x => (float)x).ToArray();

            var titles = new[] { "A", "A", "A", "A", "B", "B" };
            var emps = new[] { "Fake Inc.", "Fake Inc.", "Evil Inc.", "Evil Inc.", "Evil Inc.", "Evil Inc." };

            var titlePlan = new CategoryTreatmentPlan<string>();
            var empPlan = new CategoryTreatmentPlan<string>();
            for (int idx = 0; idx < target.Length; idx++)
            {
                titlePlan.Add(titles[idx], target[idx]);
                empPlan.Add(emps[idx], target[idx]);
            }

            var titleTreat = titlePlan.Build("NA");
            var empTreat = empPlan.Build("NA");

            float naValue = 108.333335f;
            Assert.AreEqual(naValue, titleTreat["NA"], _tolerance);
            Assert.AreEqual(63.7023429f, titleTreat["A"], _tolerance);
            Assert.AreEqual(153.3898315f, titleTreat["B"], _tolerance);

            Assert.AreEqual(naValue, empTreat["NA"], _tolerance);
            Assert.AreEqual(135.9118194f, empTreat["Evil Inc."], _tolerance);
            Assert.AreEqual(38.6267242f, empTreat["Fake Inc."], _tolerance);
        }

        [Test]
        public void TestBuildStdevZero() {
            float response = 25.0f;
            var plan = new CategoryTreatmentPlan<string>();

            for (int i = 0; i < 20; ++i)
            {
                plan.Add("X1", response);
                plan.Add("X2", response);
            }

            var treated = plan.Build("NA");

            Assert.AreEqual(response, treated["X1"], _tolerance);
            Assert.AreEqual(response, treated["X2"], _tolerance);
            Assert.AreEqual(response, treated["NA"], _tolerance);
        }

        [Test]
        public void TestBuildOneItem() {
            float response = 523.0f;
            var plan = new CategoryTreatmentPlan<string>();

            plan.Add("X1", response);
            plan.Add("X2", response);

            var treated = plan.Build("NA");

            Assert.AreEqual(3, treated.Count);
            Assert.AreEqual(response, treated["X1"], _tolerance);
            Assert.AreEqual(response, treated["X2"], _tolerance);
            Assert.AreEqual(response, treated["NA"], _tolerance);
        }

        [Test]
        public void TestBuildEmpty() {
            var plan = new CategoryTreatmentPlan<string>();

            var treated = plan.Build("NA");

            Assert.AreEqual(0, treated.Count);
        }
    }
}
