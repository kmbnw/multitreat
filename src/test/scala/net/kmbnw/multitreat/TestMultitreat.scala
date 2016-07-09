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
package net.kmbnw.multitreat

import org.apache.spark.sql.functions._
import org.apache.spark.sql.DataFrame
import com.holdenkarau.spark.testing._


class test extends DataFrameSuiteBase {
  private val tol = 0.0001

  test("designNumericSingleGroup") {
    val sqlCtx = sqlContext
    import sqlCtx.implicits._

    val expected = sqlContext.read.json("src/test/resources/salary_one_group.json")
    val df = expected.drop("title_catN")

    val treatmentPlan = new Multitreat("amount", List("title"))
    val treatments = treatmentPlan.designNumeric(df)
    val treated = treatmentPlan.applyTreatments(df, treatments)

    // apparently these have to be explicitly ordered or the equality check can fail
    assertDataFrameApproximateEquals(
      expected.select("amount", "employer", "title", "title_catN"),
      treated.select("amount", "employer", "title", "title_catN"),
      tol)

    /*val input2 = sc.parallelize(List(4, 5, 6)).toDF
    intercept[org.scalatest.exceptions.TestFailedException] {
        assertDataFrameEquals(input1, input2) // not equal
    }*/
  }
}
