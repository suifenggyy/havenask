---
toc: content
order: 2
---
# Hape multi-machine mode

* In the multi-machine mode, the havenask cluster is started based on the scheduling of multiple physical machines.
* If any of the following operations is abnormal, see [Hape FAQ and troubleshooting. md](./problem.md) for troubleshooting.


### I. Prepare the environment and configuration
#### Prepare an environment

* Prepare multiple physical machines
* The latest image is registry.cn-hangzhou.aliyuncs.com/havenask/ha3_runtime:latest. Different versions of the image are different. You can obtain the image version by referring to the [release page](https://github.com/alibaba/havenask/releases). Make sure that each physical machine has used Docker to pull the image of the corresponding version.
* All machines are required to have the same user as the hape execution, and the user has sufficient permissions for his home. We recommend that you do not use the root user.
* Prepare HDFS and ensure that all physical machines can access it.
* Make sure that the machines have been ssh-free and the machines can log in to themselves without ssh. If not, you can get through the following steps:
   * Check whether there is a. ssh/id_rsa.pub file in the user directory of each machine, if not, it can be generated by ssh-keygen.
   * Using ssh-copy-id to Copy id_rsa.pub to Other Machines

#### Modify the data import configuration
* For more information about /ha3_install/hape_conf/remote configurations, see [hape cluster configuration](./config.md). Where global.conf must be modified
   * Common Related
      * dataStoreRoot: Change the path to your HDFS directory.
         * If the hdfs address is not in ip:port mode, you need to set your own hadoopHome and javaHome. These two addresses must be under /home/\<user\> (soft links can be set), and corresponding hadoop and java must be available on all machines
   * Swift Related:
      * AdminIpList& workerIpList
      * data_root_path: Change to your own HDFS path
   * Havenask related:
      * AdminIpList& qrsIpList & searcherIpList
   * bs related:
      * AdminIpList& workerIpList
* Note the preceding configuration modifications:
   * You need to ensure that all admin machines can log on to the worker list without password (the workers of havenask are qrs and searcher)
   * The number of machines in the searcher must be no less than the number of subsequent table shards.
   * all ip addresses can be reused, except for havenask, which cannot duplicate the ip addresses of qrs&qrs, qrs&searcher, searcher, and searcher due to worker port conflicts. For example, the ip address of the swift worker can also be used in the havenask admin
   * If you violate the preceding principles, cluster O&M exceptions occur. For more information, see [FAQ and troubleshooting. md](./problem.md).


### 2. Create a havenask cluster

* Create and enter a hape container
```
./docker/havenask/create_container.sh <container_name> <image>
./<container_name>/sshme
```

* Verify that the hape running environment is not abnormal. Because the multi-level mode does not use the default configuration, all subsequent paths need to specify the configuration path /ha3_install/hape_conf/remote of the distributed cluster.
```
hape validate -c /ha3_install/hape_conf/remote
```

* Create a havenask cluster
```
hape start havenask -c /ha3_install/hape_conf/remote
```


### III. Table management
* There are two types of tables: write-through tables and full tables. You can create one or both of them.
* Create a write-through table
   * -t indicates the table name.
   * -p indicates the number of shards. In multi-machine mode, assuming that there are N tables, where the maximum shard is M, then the shard requirement for all tables must be either 1 or M
   * -s indicates the schema path.
```
hape create table -t in0 -p 2 -s /ha3_install/example/cases/normal/in0_schema.json -c /ha3_install/hape_conf/remote

```
* (Slow startup speed, optional) Create a full meter
   * -t indicates the table name.
   * -p indicates the number of shards. In multi-machine mode, assuming that there are N tables, where the maximum shard is M, then the shard requirement for all tables must be either 1 or M
   * -s indicates the schema path.
   * -f indicates the full file path, which supports files or directories. Distributed requirements must upload the data file and then provide the hdfs path.
```
### Use the hdfs tool to upload the /ha3_install/example/data/test.data first
###
hape create table -t in1 -p 2 -s /ha3_install/example/cases/normal/in0_schema.json  -f hdfs://xxxx/test.data -c /ha3_install/hape_conf/remote
```
* You must start the BS service for the full table and start multiple BS tasks. Generally, it takes 3 to 5 minutes to take effect. Therefore, you can run the following command to view the BS status. For more information, see [Hape Cluster Status](./status.md).
```
hape gs bs -c /ha3_install/hape_conf/remote

### When all processes with full are finished and only inc process is left, execute the following command to find that the corresponding table status is READY
hape gs table -t <table_name> -c /ha3_install/hape_conf/remote
```

* If you want to delete a table, you can do the following
```
hape delete table -t <table-name> -c /ha3_install/hape_conf/remote
```

### 4. View the cluster status
* After the table is created, you can use the following command to check the cluster status and determine whether the cluster is ready for reading and writing. For more information, see [Hape Cluster Status](./status.md).
```
### In the returned result, the cluster is ready when the clusterStatus of sqlClusterInfo is set to READY.
hape gs havenask -c /ha3_install/hape_conf/remote
```


### V. Read and write data

* Run the following command to write data to a table:
```
### Since qrs may be pulled up at the remote end, it is necessary to use hape gs havenask subcommand to obtain the address of qrs first.
/ha3_install/sql_query.py --address http://<qrs-ip>:45800 --query "insert into in0 (createtime,hits,id,title,subject)values(1,2,4, 'test', 'test')"
```

* Query data in a Lindorm wide table
```
/ha3_install/sql_query.py  --address  http://<qrs-ip>:45800 --query "select * from in0"
```

* If you want to view the value of a field of the TEXT type, you must execute the following statement to query the [summary index](../indexes/summary.md) in a union manner:
```
/ha3_install/sql_query.py --address  http://<qrs-ip>:45800 --query "select in0_summary_.id, in0_summary_.title, in0_summary_.subject from in0 inner join in0_summary_ on in0.id=in0_summary_.id"
```


* Swift is required for real-time data push for full table writing. Currently, you recommend use swift c++/java client. python is written only to test tools and is not recommend for production use. you need to call the following command. the meaning of the swift python write tool parameter is:
   * -- zk swift service zk address
   * -- The topic swift topic name, which is generally equal to the table name by default.
   * -- The number of count swift topic shards, which is generally equal to the number of table shards by default.
   * -- The schema address of the schema table.
   * -- file The havenask data file to be written.
```
### View the serviceZk address of the Swift service
hape gs swift

### Write test messages
/ha3_install/swift_writer.py --zk <serviceZk> --topic in1 --count 2 --schema /ha3_install/example/cases/normal/in0_schema.json --file /ha3_install/example/data/rt.data
```

### 6. Cluster cleanup
```
### Clean up the HavenAsk cluster and delete the container and zk
hape delete havenask

### Clean up the Swift cluster and delete the container and zk
hape delete swift
```

### Seven. More
* For more information about cluster subcommands, see [Hape subcommands](./command.md).
* For more O&M scenarios, see [Hape O&M scenarios](./scene.md).
* For more examples based on the standalone mode, see hape/example/README.md