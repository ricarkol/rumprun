// This file is a part of the IncludeOS unikernel - www.includeos.org
//
// Copyright 2015 Oslo and Akershus University College of Applied Sciences
// and Alfred Bratterud
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sqlite/sqlite3.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>

long sysconf2(int name) {
  printf("sysconf(%d) called, not supported\n", name);
  errno = ENOTSUP;
  return -1;
}
uid_t geteuid() {
  printf("geteuid() called, not supported\n");
  return 0;
}
int utime(const char *filename, const struct utimbuf *times)
{
  printf("utime(%s, %p) called, not supported\n", filename, times);
  errno = ENOTSUP;
  return -1;
}

int utimes(const char *filename, const struct timeval *times)
{
  printf("utimes(%s, %p) called, not supported\n", filename, times);
  errno = ENOTSUP;
  return -1;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}
static int sqlite_exec(sqlite3* db, const char* message)
{
  char* error_message = NULL;
  int rc = sqlite3_exec(db, message, callback, 0, &error_message);
  if (rc) {
    assert(error_message);
    fprintf(stderr, "SQL error: %s\n", error_message);
    sqlite3_free(error_message);
    return -1;
  }
  return 0;
}

int tcp_h(sqlite3* db)
{
  char query1[] = "select "
    "    l_returnflag, "
    "    l_linestatus, "
    "    sum(l_quantity) as sum_qty, "
    "    sum(l_extendedprice) as sum_base_price, "
    "    sum(l_extendedprice * (1 - l_discount)) as sum_disc_price, "
    "    sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge, "
    "    avg(l_quantity) as avg_qty, "
    "    avg(l_extendedprice) as avg_price, "
    "    avg(l_discount) as avg_disc, "
    "    count(*) as count_order "
    "from "
    "    lineitem "
    "where "
    "    l_shipdate <= datetime('1998-12-01', '-108 day') "
    "group by "
    "    l_returnflag, "
    "    l_linestatus "
    "order by "
    "    l_returnflag, "
    "    l_linestatus;";

  char query2[] = "select"
    " 	s_acctbal, "
    " 	s_name, "
    " 	n_name, "
    " 	p_partkey, "
    " 	p_mfgr, "
    " 	s_address, "
    " 	s_phone, "
    " 	s_comment "
    " from "
    " 	part, "
    " 	supplier, "
    " 	partsupp, "
    " 	nation, "
    " 	region "
    " where "
    " 	p_partkey = ps_partkey "
    " 	and s_suppkey = ps_suppkey "
    " 	and p_size = 30 "
    " 	and p_type like '\%STEEL' "
    " 	and s_nationkey = n_nationkey "
    " 	and n_regionkey = r_regionkey "
    " 	and r_name = 'ASIA' "
    " 	and ps_supplycost = ( "
    " 		select "
    " 			min(ps_supplycost) "
    " 		from "
    " 			partsupp, "
    " 			supplier, "
    " 			nation, "
    " 			region "
    " 		where "
    " 			p_partkey = ps_partkey "
    " 			and s_suppkey = ps_suppkey "
    " 			and s_nationkey = n_nationkey "
    " 			and n_regionkey = r_regionkey "
    " 			and r_name = 'ASIA' "
    " 	) "
    " order by "
    " 	s_acctbal desc, "
    " 	n_name, "
    " 	s_name, "
    " 	p_partkey "
    " limit 100; "
    "";

  char query3[] = "select"
    " 	l_orderkey, "
    " 	sum(l_extendedprice * (1 - l_discount)) as revenue, "
    " 	o_orderdate, "
    " 	o_shippriority "
    " from "
    " 	customer, "
    " 	orders, "
    " 	lineitem "
    " where "
    " 	c_mktsegment = 'AUTOMOBILE' "
    " 	and c_custkey = o_custkey "
    " 	and l_orderkey = o_orderkey "
    " 	and o_orderdate < datetime('1995-03-13') "
    " 	and l_shipdate > datetime('1995-03-13') "
    " group by "
    " 	l_orderkey, "
    " 	o_orderdate, "
    " 	o_shippriority "
    " order by "
    " 	revenue desc, "
    " 	o_orderdate "
    " limit 10; "
    "";

  char query4[] = "select"
    " 	o_orderpriority, "
    " 	count(*) as order_count "
    " from "
    " 	orders "
    " where "
    " 	o_orderdate >= datetime('1995-01-01') "
    " 	and o_orderdate < datetime('1995-01-01', '+3 month') "
    " 	and exists ( "
    " 		select "
    " 			* "
    " 		from "
    " 			lineitem "
    " 		where "
    " 			l_orderkey = o_orderkey "
    " 			and l_commitdate < l_receiptdate "
    " 	) "
    " group by "
    " 	o_orderpriority "
    " order by "
    " 	o_orderpriority; "
    "";

  char query5_[] = "select"
    " 	n_name, "
    " 	sum(l_extendedprice * (1 - l_discount)) as revenue "
    " from "
    " 	customer, "
    " 	orders, "
    " 	lineitem, "
    " 	supplier, "
    " 	nation, "
    " 	region "
    " where "
    " 	c_custkey = o_custkey "
    " 	and l_orderkey = o_orderkey "
    " 	and l_suppkey = s_suppkey "
    " 	and c_nationkey = s_nationkey "
    " 	and s_nationkey = n_nationkey "
    " 	and n_regionkey = r_regionkey "
    " 	and r_name = 'MIDDLE EAST' "
    " 	and o_orderdate >= datetime('1994-01-01') "
    " 	and o_orderdate < datetime('1994-01-01', '+1 year') "
    " group by "
    " 	n_name "
    " order by "
    " 	revenue desc; "
    "";

  char query5[] = " \
SELECT N_NAME, SUM(L_EXTENDEDPRICE*(1-L_DISCOUNT)) AS REVENUE \
FROM CUSTOMER, ORDERS, LINEITEM, SUPPLIER, NATION, REGION \
WHERE C_CUSTKEY = O_CUSTKEY AND L_ORDERKEY = O_ORDERKEY AND L_SUPPKEY = S_SUPPKEY \
AND C_NATIONKEY = S_NATIONKEY AND S_NATIONKEY = N_NATIONKEY AND N_REGIONKEY = R_REGIONKEY \
AND R_NAME = 'ASIA' AND O_ORDERDATE >= '1994-01-01'  \
AND O_ORDERDATE < '1995-01-01' \
GROUP BY N_NAME \
ORDER BY REVENUE DESC \
  ";

  char query6[] = "select"
    " 	sum(l_extendedprice * l_discount) as revenue "
    " from "
    " 	lineitem "
    " where "
    " 	l_shipdate >= datetime('1994-01-01') "
    " 	and l_shipdate < datetime('1994-01-01', '+1 year') "
    " 	and l_discount between 0.06 - 0.01 and 0.06 + 0.01 "
    " 	and l_quantity < 24; "
    "";

  // copied from https://github.com/enricobacis/jupiter-py/blob/master/tpch/queries/

  char query7[] = " \
SELECT SUPP_NATION, CUST_NATION, L_YEAR, SUM(VOLUME) AS REVENUE \
FROM ( SELECT N1.N_NAME AS SUPP_NATION, N2.N_NAME AS CUST_NATION, L_SHIPDATE AS L_YEAR, \
 L_EXTENDEDPRICE*(1-L_DISCOUNT) AS VOLUME \
 FROM SUPPLIER, LINEITEM, ORDERS, CUSTOMER, NATION N1, NATION N2 \
 WHERE S_SUPPKEY = L_SUPPKEY AND O_ORDERKEY = L_ORDERKEY AND C_CUSTKEY = O_CUSTKEY \
 AND S_NATIONKEY = N1.N_NATIONKEY AND C_NATIONKEY = N2.N_NATIONKEY AND  ((N1.N_NAME = 'FRANCE' AND N2.N_NAME = 'GERMANY') OR \
 (N1.N_NAME = 'GERMANY' AND N2.N_NAME = 'FRANCE')) AND \
 L_SHIPDATE BETWEEN '1995-01-01' AND '1996-12-31' ) AS SHIPPING \
GROUP BY SUPP_NATION, CUST_NATION, L_YEAR \
ORDER BY SUPP_NATION, CUST_NATION, L_YEAR \
";

 char query8[] = " \
SELECT O_YEAR, SUM(CASE WHEN NATION = 'BRAZIL' THEN VOLUME ELSE 0 END)/SUM(VOLUME) AS MKT_SHARE \
FROM (SELECT strftime('%Y', O_ORDERDATE) AS O_YEAR, L_EXTENDEDPRICE*(1-L_DISCOUNT) AS VOLUME, N2.N_NAME AS NATION \
 FROM PART, SUPPLIER, LINEITEM, ORDERS, CUSTOMER, NATION N1, NATION N2, REGION \
 WHERE P_PARTKEY = L_PARTKEY AND S_SUPPKEY = L_SUPPKEY AND L_ORDERKEY = O_ORDERKEY \
 AND O_CUSTKEY = C_CUSTKEY AND C_NATIONKEY = N1.N_NATIONKEY AND \
 N1.N_REGIONKEY = R_REGIONKEY AND R_NAME = 'AMERICA' AND S_NATIONKEY = N2.N_NATIONKEY \
 AND O_ORDERDATE BETWEEN '1995-01-01' AND '1996-12-31' AND P_TYPE= 'ECONOMY ANODIZED STEEL') AS ALL_NATIONS \
GROUP BY O_YEAR \
ORDER BY O_YEAR \
";

  char query9[] = " \
SELECT NATION, O_YEAR, SUM(AMOUNT) AS SUM_PROFIT \
FROM (SELECT N_NAME AS NATION, O_ORDERDATE AS O_YEAR, \
 L_EXTENDEDPRICE*(1-L_DISCOUNT)-PS_SUPPLYCOST*L_QUANTITY AS AMOUNT \
 FROM PART, SUPPLIER, LINEITEM, PARTSUPP, ORDERS, NATION \
 WHERE S_SUPPKEY = L_SUPPKEY AND PS_SUPPKEY= L_SUPPKEY AND PS_PARTKEY = L_PARTKEY AND \
 P_PARTKEY= L_PARTKEY AND O_ORDERKEY = L_ORDERKEY AND S_NATIONKEY = N_NATIONKEY AND \
 P_NAME LIKE '%%green%%') AS PROFIT \
GROUP BY NATION, O_YEAR \
ORDER BY NATION, O_YEAR DESC \
";

  char query10[] = " \
SELECT C_CUSTKEY, C_NAME, SUM(L_EXTENDEDPRICE*(1-L_DISCOUNT)) AS REVENUE, C_ACCTBAL, \
N_NAME, C_ADDRESS, C_PHONE, C_COMMENT \
FROM CUSTOMER, ORDERS, LINEITEM, NATION \
WHERE C_CUSTKEY = O_CUSTKEY AND L_ORDERKEY = O_ORDERKEY AND O_ORDERDATE>= '1993-10-01' AND \
O_ORDERDATE < '1994-02-01' AND \
L_RETURNFLAG = 'R' AND C_NATIONKEY = N_NATIONKEY \
GROUP BY C_CUSTKEY, C_NAME, C_ACCTBAL, C_PHONE, N_NAME, C_ADDRESS, C_COMMENT \
ORDER BY REVENUE DESC \
LIMIT 20 \
";

  char query11[] = "select"
    " 	ps_partkey, "
    " 	sum(ps_supplycost * ps_availqty) as value "
    " from "
    " 	partsupp, "
    " 	supplier, "
    " 	nation "
    " where "
    " 	ps_suppkey = s_suppkey "
    " 	and s_nationkey = n_nationkey "
    " 	and n_name = 'MOZAMBIQUE' "
    " group by "
    " 	ps_partkey having "
    " 		sum(ps_supplycost * ps_availqty) > ( "
    " 			select "
    " 				sum(ps_supplycost * ps_availqty) * 0.0001000000 "
    " 			from "
    " 				partsupp, "
    " 				supplier, "
    " 				nation "
    " 			where "
    " 				ps_suppkey = s_suppkey "
    " 				and s_nationkey = n_nationkey "
    " 				and n_name = 'MOZAMBIQUE' "
    " 		) "
    " order by "
    " 	value desc; "
    "";

  char query11_[] = " \
SELECT PS_PARTKEY, SUM(PS_SUPPLYCOST*PS_AVAILQTY) AS VAL \
FROM PARTSUPP, SUPPLIER, NATION \
WHERE PS_SUPPKEY = S_SUPPKEY AND S_NATIONKEY = N_NATIONKEY AND N_NAME = 'GERMANY' \
GROUP BY PS_PARTKEY \
HAVING SUM(PS_SUPPLYCOST*PS_AVAILQTY) > (SELECT SUM(PS_SUPPLYCOST*PS_AVAILQTY) * 0.0001000000 \
 FROM PARTSUPP, SUPPLIER, NATION \
 WHERE PS_SUPPKEY = S_SUPPKEY AND S_NATIONKEY = N_NATIONKEY AND N_NAME = 'GERMANY') \
ORDER BY VAL DESC  \
";

  char query12[] = " \
SELECT L_SHIPMODE, \
SUM(CASE WHEN O_ORDERPRIORITY = '1-URGENT' OR O_ORDERPRIORITY = '2-HIGH' THEN 1 ELSE 0 END) AS HIGH_LINE_COUNT, \
SUM(CASE WHEN O_ORDERPRIORITY <> '1-URGENT' AND O_ORDERPRIORITY <> '2-HIGH' THEN 1 ELSE 0 END ) AS LOW_LINE_COUNT \
FROM ORDERS, LINEITEM \
WHERE O_ORDERKEY = L_ORDERKEY AND L_SHIPMODE IN ('MAIL','SHIP') \
AND L_COMMITDATE < L_RECEIPTDATE AND L_SHIPDATE < L_COMMITDATE AND L_RECEIPTDATE >= '1994-01-01' \
AND L_RECEIPTDATE < '1995-10-01' \
GROUP BY L_SHIPMODE \
ORDER BY L_SHIPMODE \
";

  char query13[] = "select"
    " 	c_count, "
    " 	count(*) as custdist "
    " from "
    " 	( "
    " 		select "
    " 			c_custkey, "
    " 			count(o_orderkey) as c_count "
    " 		from "
    " 			customer left outer join orders on "
    " 				c_custkey = o_custkey "
    " 				and o_comment not like '%pending%deposits%' "
    " 		group by "
    " 			c_custkey "
    " 	) c_orders "
    " group by "
    " 	c_count "
    " order by "
    " 	custdist desc, "
    " 	c_count desc; "
    "";

  char query14[] = " \
SELECT 100.00* SUM(CASE WHEN P_TYPE LIKE 'PROMO%%' THEN L_EXTENDEDPRICE*(1-L_DISCOUNT) \
ELSE 0 END) / SUM(L_EXTENDEDPRICE*(1-L_DISCOUNT)) AS PROMO_REVENUE \
FROM LINEITEM, PART \
WHERE L_PARTKEY = P_PARTKEY AND L_SHIPDATE >= '1995-09-01' AND L_SHIPDATE < '1995-10-01' \
";

  char query15[] = "select"
    " 		l_suppkey, "
    " 		sum(l_extendedprice * (1 - l_discount)) "
    " 	from "
    " 		lineitem "
    " 	where "
    " 		l_shipdate >= '1997-07-01' "
    " 		and l_shipdate < datetime('1997-07-01', '+3 month') "
    " 	group by "
    " 		l_suppkey; "
    "  "
    "  "
    " select "
    " 	s_suppkey, "
    " 	s_name, "
    " 	s_address, "
    " 	s_phone, "
    " 	total_revenue "
    " from "
    " 	supplier, "
    " 	revenue0 "
    " where "
    " 	s_suppkey = supplier_no "
    " 	and total_revenue = ( "
    " 		select "
    " 			max(total_revenue) "
    " 		from "
    " 			revenue0 "
    " 	) "
    " order by "
    " 	s_suppkey; "
    "  "
    " drop view revenue0; "
    "";

  char query15_[] = " \
SELECT S_SUPPKEY, S_NAME, S_ADDRESS, S_PHONE, TOTAL_REVENUE \
FROM SUPPLIER, REVENUE0 \
WHERE S_SUPPKEY = SUPPLIER_NO AND TOTAL_REVENUE = (SELECT MAX(TOTAL_REVENUE) FROM REVENUE0) \
ORDER BY S_SUPPKEY \
";

  char query16[] = " \
SELECT P_BRAND, P_TYPE, P_SIZE, COUNT(DISTINCT PS_SUPPKEY) AS SUPPLIER_CNT \
FROM PARTSUPP, PART \
WHERE P_PARTKEY = PS_PARTKEY AND P_BRAND <> 'Brand#45' AND P_TYPE NOT LIKE 'MEDIUM POLISHED%%' \
AND P_SIZE IN (49, 14, 23, 45, 19, 3, 36, 9) AND PS_SUPPKEY NOT IN (SELECT S_SUPPKEY FROM SUPPLIER \
 WHERE S_COMMENT LIKE '%%Customer%%Complaints%%') \
GROUP BY P_BRAND, P_TYPE, P_SIZE \
ORDER BY SUPPLIER_CNT DESC, P_BRAND, P_TYPE, P_SIZE \
";

  char query17[] = " \
SELECT SUM(L_EXTENDEDPRICE)/7.0 AS AVG_YEARLY FROM LINEITEM, PART \
WHERE P_PARTKEY = L_PARTKEY AND P_BRAND = 'Brand#23' AND P_CONTAINER = 'MED BOX' \
AND L_QUANTITY < (SELECT 0.2*AVG(L_QUANTITY) FROM LINEITEM WHERE L_PARTKEY = P_PARTKEY) \
";

  char query18[] = " \
SELECT C_NAME, C_CUSTKEY, O_ORDERKEY, O_ORDERDATE, O_TOTALPRICE, SUM(L_QUANTITY) \
FROM CUSTOMER, ORDERS, LINEITEM \
WHERE O_ORDERKEY IN (SELECT L_ORDERKEY FROM LINEITEM GROUP BY L_ORDERKEY HAVING \
 SUM(L_QUANTITY) > 300) AND C_CUSTKEY = O_CUSTKEY AND O_ORDERKEY = L_ORDERKEY \
GROUP BY C_NAME, C_CUSTKEY, O_ORDERKEY, O_ORDERDATE, O_TOTALPRICE \
ORDER BY O_TOTALPRICE DESC, O_ORDERDATE \
LIMIT 100 \
";

  char query19[] = " \
SELECT SUM(L_EXTENDEDPRICE* (1 - L_DISCOUNT)) AS REVENUE \
FROM LINEITEM, PART \
WHERE (P_PARTKEY = L_PARTKEY AND P_BRAND = 'Brand#12' AND P_CONTAINER IN ('SM CASE', 'SM BOX', 'SM PACK', 'SM PKG') AND L_QUANTITY >= 1 AND L_QUANTITY <= 1 + 10 AND P_SIZE BETWEEN 1 AND 5 \
AND L_SHIPMODE IN ('AIR', 'AIR REG') AND L_SHIPINSTRUCT = 'DELIVER IN PERSON') \
OR (P_PARTKEY = L_PARTKEY AND P_BRAND ='Brand#23' AND P_CONTAINER IN ('MED BAG', 'MED BOX', 'MED PKG', 'MED PACK') AND L_QUANTITY >=10 AND L_QUANTITY <=10 + 10 AND P_SIZE BETWEEN 1 AND 10  \
AND L_SHIPMODE IN ('AIR', 'AIR REG') AND L_SHIPINSTRUCT = 'DELIVER IN PERSON') \
OR (P_PARTKEY = L_PARTKEY AND P_BRAND = 'Brand#34' AND P_CONTAINER IN ( 'LG CASE', 'LG BOX', 'LG PACK', 'LG PKG') AND L_QUANTITY >=20 AND L_QUANTITY <= 20 + 10 AND P_SIZE BETWEEN 1 AND 15 \
AND L_SHIPMODE IN ('AIR', 'AIR REG') AND L_SHIPINSTRUCT = 'DELIVER IN PERSON') \
";

  char query20[] = " \
SELECT S_NAME, S_ADDRESS FROM SUPPLIER, NATION \
WHERE S_SUPPKEY IN (SELECT PS_SUPPKEY FROM PARTSUPP \
 WHERE PS_PARTKEY in (SELECT P_PARTKEY FROM PART WHERE P_NAME like 'forest%%') AND \
 PS_AVAILQTY > (SELECT 0.5*sum(L_QUANTITY) FROM LINEITEM WHERE L_PARTKEY = PS_PARTKEY AND \
  L_SUPPKEY = PS_SUPPKEY AND L_SHIPDATE >= '1994-01-01' AND \
  L_SHIPDATE < '1995-01-01') AND S_NATIONKEY = N_NATIONKEY AND N_NAME = 'CANADA') \
ORDER BY S_NAME \
";

  char query21[] = " \
SELECT S_NAME, COUNT(*) AS NUMWAIT \
FROM SUPPLIER, LINEITEM L1, ORDERS, NATION WHERE S_SUPPKEY = L1.L_SUPPKEY AND \
O_ORDERKEY = L1.L_ORDERKEY AND O_ORDERSTATUS = 'F' AND L1.L_RECEIPTDATE> L1.L_COMMITDATE \
AND EXISTS (SELECT * FROM LINEITEM L2 WHERE L2.L_ORDERKEY = L1.L_ORDERKEY \
 AND L2.L_SUPPKEY <> L1.L_SUPPKEY) AND \
NOT EXISTS (SELECT * FROM LINEITEM L3 WHERE L3.L_ORDERKEY = L1.L_ORDERKEY AND \
 L3.L_SUPPKEY <> L1.L_SUPPKEY AND L3.L_RECEIPTDATE > L3.L_COMMITDATE) AND \
S_NATIONKEY = N_NATIONKEY AND N_NAME = 'SAUDI ARABIA' \
GROUP BY S_NAME \
ORDER BY NUMWAIT DESC, S_NAME \
LIMIT 100 \
";

  char query22[] = " \
SELECT CNTRYCODE, COUNT(*) AS NUMCUST, SUM(C_ACCTBAL) AS TOTACCTBAL \
FROM (SELECT SUBSTR(C_PHONE,1,2) AS CNTRYCODE, C_ACCTBAL \
 FROM CUSTOMER WHERE SUBSTR(C_PHONE,1,2) IN ('13', '31', '23', '29', '30', '18', '17') AND \
 C_ACCTBAL > (SELECT AVG(C_ACCTBAL) FROM CUSTOMER WHERE C_ACCTBAL > 0.00 AND \
  SUBSTR(C_PHONE,1,2) IN ('13', '31', '23', '29', '30', '18', '17')) AND \
 NOT EXISTS ( SELECT * FROM ORDERS WHERE O_CUSTKEY = C_CUSTKEY)) AS CUSTSALE \
GROUP BY CNTRYCODE \
ORDER BY CNTRYCODE \
";
  int rc;

  // Some tests are commented out because they are trying to create temp files
  // (not allowed in includeos at the moment).  This is a possible reason for
  // the temp files: "2.7. Materializations Of Views And Subqueries" from
  // https://www.sqlite.org/tempfiles.html

#define SQLITE_MED_SIZE 1

  //printf("1\n");
  //rc = sqlite_exec(db, query1);
  //assert(rc == 0);
  printf("2\n");
  rc = sqlite_exec(db, query2);
  assert(rc == 0);
#ifndef SQLITE_MED_SIZE
  printf("3\n");
  rc = sqlite_exec(db, query3);
  assert(rc == 0);
#endif
  printf("4\n");
  rc = sqlite_exec(db, query4);
  assert(rc == 0);
#ifndef SQLITE_MED_SIZE
  printf("5\n");
  rc = sqlite_exec(db, query5);
  assert(rc == 0);
#endif
  printf("6\n");
  rc = sqlite_exec(db, query6);
  assert(rc == 0);
  printf("7\n");
  rc = sqlite_exec(db, query7);
  assert(rc == 0);
  //printf("8\n");
  //rc = sqlite_exec(db, query8);
  //assert(rc == 0);
  //printf("9\n");
  //rc = sqlite_exec(db, query9);
  //assert(rc == 0);
  //printf("10\n");
  //rc = sqlite_exec(db, query10);
  //assert(rc == 0);
  printf("11\n");
  rc = sqlite_exec(db, query11);
  assert(rc == 0);
  printf("12\n");
  rc = sqlite_exec(db, query12);
  assert(rc == 0);
#ifndef SQLITE_MED_SIZE
  printf("13\n");
  rc = sqlite_exec(db, query13);
  assert(rc == 0);
#endif
  printf("14\n");
  rc = sqlite_exec(db, query14);
  assert(rc == 0);
  //printf("15\n");
  //rc = sqlite_exec(db, query15);
  //assert(rc == 0);
  printf("16\n");
  rc = sqlite_exec(db, query16);
  assert(rc == 0);
#ifndef SQLITE_MED_SIZE
  printf("17\n");
  rc = sqlite_exec(db, query17);
  assert(rc == 0);
#endif
  printf("18\n");
  rc = sqlite_exec(db, query18);
  assert(rc == 0);
  printf("19\n");
  rc = sqlite_exec(db, query19);
  assert(rc == 0);
#ifndef SQLITE_MED_SIZE
  printf("20\n");
  rc = sqlite_exec(db, query20);
  assert(rc == 0);
#endif
  printf("21\n");
  rc = sqlite_exec(db, query21);
  assert(rc == 0);
  //printf("22\n");
  //rc = sqlite_exec(db, query22);
  //assert(rc == 0);
}


/* returns differences in times -
   1 second is the minimum to avoid divide by zero errors */
time_t diff_time(t1,t0)
time_t t1;
time_t t0;
{
   return((t1-=t0)?t1:1);
}

int main()
{
  time_t t1, t2, elapsed;
  sqlite3* db;
  int rc;

  rc = sqlite3_open_v2("/db/company/my.db", &db, SQLITE_OPEN_READONLY, "unix-none");
  assert(rc == 0);

  rc = sqlite_exec(db, "PRAGMA journal_mode=OFF;");
  assert(rc == 0);

  time(&t1);
  tcp_h(db);
  time(&t2);

  elapsed=diff_time(t2, t1);
  printf("%d seconds total\n",elapsed);

  return 0;
}
