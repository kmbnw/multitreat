import org.apache.spark.sql.functions._
import org.apache.spark.sql.DataFrame

def prepareNumeric(targetCol: String, groupCol: String, df: DataFrame) : DataFrame = {
	val sampleMean = df.select(avg(targetCol)).first().getDouble(0)
	val sampleSd = df.select(stddev(targetCol)).first().getDouble(0)

    val targetMeanCol = targetCol + "_mean"

	val dfMeans = df.groupBy(groupCol).agg(
		stddev_pop(targetCol).as(targetCol + "_sd"),
		avg(targetCol).as(targetMeanCol),
		count(targetCol).divide(
			stddev(targetCol).divide(sampleSd)
		    .plus(count(targetCol))).as("lambda")
	)

	val ret = dfMeans.withColumn("S", dfMeans.col("lambda").multiply(
		dfMeans.col(targetMeanCol)).plus(
			dfMeans.col("lambda").minus(1).multiply(-1 * sampleMean))
	)
	return ret
}
