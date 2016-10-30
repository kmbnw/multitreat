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

namespace kmbnw.Multitreat
{
    /// <summary>
    /// Create a new treatment plan whose category type is K.
    ///
    /// K is used as a dictionary key and thus should follow all the
    /// rules necessary for that to work correctly.
    /// </summary>
    public class CategoryTreatmentPlan<K>
    {
        private readonly IDictionary<K, double> _groupMeans = new Dictionary<K, double>();
        private readonly IDictionary<K, ulong> _groupCounts = new Dictionary<K, ulong>();
        private readonly IDictionary<K, double> _groupM2Stdev = new Dictionary<K, double>();
        private readonly ReaderWriterLockSlim _lock = new ReaderWriterLockSlim();

        private ulong _count = 0;
        private double _mean = 0;
        private double _m2Stdev = 0;

        public CategoryTreatmentPlan()
        {  }

        /// <summary>
        /// Add a pair of category to response and update the means and std
        /// deviations using the online update given at
        // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
        /// </summary>
        public void Add(K category, float newValue)
        {
            _lock.EnterWriteLock();

            try
            {
                OnlineUpdate(newValue, ref _count, ref _mean, ref _m2Stdev);

                // and for the group
                double catMean = 0;
                ulong catCount = 0;
                double catM2Stdev = 0;

                // have we seen this category before?
                if (_groupMeans.TryGetValue(category, out catMean))
                {
                    catCount = _groupCounts[category];
                    catM2Stdev = _groupM2Stdev[category];
                }

                OnlineUpdate(newValue, ref catCount, ref catMean, ref catM2Stdev);

                // update internal structures
                _groupMeans[category] = catMean;
                _groupCounts[category] = catCount;
                _groupM2Stdev[category] = catM2Stdev;
            }
            finally
            {
                _lock.ExitWriteLock();
            }
        }

        /// <summary>
        /// Construct a mapping from category to Bayes-adjusted response.
        ///
        /// The output value is described in
        ///  "A Preprocessing Scheme for High Cardinality Categorical Attributes in
        ///  Classification and Prediction Problems", Micci-Barreca, Daniele.
        /// </summary>
        /// <param name="naValue">The key which represents "missing" or "NA".
        /// Will be used for the overall mean and used as a key in the return
        /// value to represent the encoding for missing values.
        /// </param>
        /// <returns>A dictionary from category to re-encoded response.</returns>
        public Dictionary<K, float> Build(K naValue)
        {
            var treatment = new Dictionary<K, float>();
            if (_count < 1)
            {
                // nothing to calculate on
                return treatment;
            }

            _lock.EnterReadLock();

            try
            {
                // sample mean and standard deviation
                float naFill = 1e-6f;
                double stdev = StdDevFromM2(_count, _m2Stdev);

                treatment[naValue] = (float) _mean;

                foreach (var kv in _groupMeans.Where(kv => !kv.Key.Equals(naValue)))
                {
                    var category = kv.Key;
                    var catMean = kv.Value;
                    ulong catCount = _groupCounts[category];
                    double catM2Stdev = _groupM2Stdev[category];
                    double catStdev = StdDevFromM2(catCount, catM2Stdev);

                    // using the simple version of lambda from the paper:
                    // lambda = n / (m + n)
                    // where m = group_sd / sample_sd
                    // there is a fill-in for when only one sample exists of 1e-6
                    ulong n = catCount;

                    // TODO I would like to make lambda user-settable
                    double lambda = naFill;
                    if (_count > 1 && n > 1 && stdev > 0) {
                        double m = catStdev / stdev;
                        lambda = n / (m + n);
                    }

                    // Bayesian formula from the paper
                    double treated = lambda * catMean + (1 - lambda) * _mean;
                    treatment[category] = (float) treated;
                }

                return treatment;
            }
            finally
            {
                _lock.ExitReadLock();
            }
        }

        private static void OnlineUpdate(
                float newValue,
                ref ulong xCount,
                ref double xMean,
                ref double xM2Stdev) {
            xCount += 1;
            double delta = newValue - xMean;
            xMean += delta / xCount;
            xM2Stdev += delta * (newValue - xMean);
        }

        private static double StdDevFromM2(ulong count, double m2Stdev) {
            return count < 2 ? double.NaN : m2Stdev / (count - 1);
        }
    }

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
