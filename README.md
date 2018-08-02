This program is a course project for ECE180 Systems Software. Thanks Prof.Gessner, Richard.K and my teammate Vladi Itovi.

#Available commands for database:
(1) quit. ------------------------------------------ Exit program.          
(2) help. ------------------------------------------ List all the available commands so far.
(3) version. --------------------------------------- Show the current version number.
(4) create database {DBname}. ---------------------- Create database named DBname
(5) show databases. -------------------------------- Show all the databases available 
(6) use database {DBname}. ------------------------- Set database DBname as current active databases 
(7) drop database {DBname}. ------------------------ Delete database named DBname
(8) create table {TBname}. ------------------------- Create table named TBname
(9) show tables. ----------------------------------- Show all table names in the current active databases
(10)describe {TBname}. ----------------------------- Output a table where each row describes a field in the requested table and each column describes field, type, null, key(primary or not),default value,extra(auto_increment or not) of the table
(11)drop table {TBname} ---------------------------- Delete table TBname
(12)insert record command. Eg: insert into test(name,amount) values ("JiangLiang",100),("HuaLi",200)
(13)select record command. Eg: "select * from test" will list all records in table test
(14)delete record command. Eg: "delete * from test" will delete all records in the table test.
