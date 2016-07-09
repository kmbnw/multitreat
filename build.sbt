name         := "multitreat"
version      := "0.1a"
organization := "net.kmbnw"

scalaVersion := "2.10.5"

libraryDependencies += "org.apache.spark" %% "spark-core" % "1.6.2"
libraryDependencies += "org.apache.spark" %% "spark-sql" % "1.6.2"
libraryDependencies += "org.apache.spark" %% "spark-hive" % "1.6.2"
libraryDependencies += "com.holdenkarau" %% "spark-testing-base" % "1.6.1_0.3.3"

resolvers += Resolver.mavenLocal

javaOptions ++= Seq("-Xms512M", "-Xmx2048M", "-XX:MaxPermSize=2048M", "-XX:+CMSClassUnloadingEnabled")
parallelExecution in Test := false
